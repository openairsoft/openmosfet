const TRY_LIMIT = 20;

window.addEventListener('DOMContentLoaded', async () => {
  if (!window.fetch || !('content' in document.createElement('template'))) {
    alert('Browser not supported. Please update');
  }

  let version;
  let config;
  let latestUpdate;

  // Remove grid gap if only one child exist
  document.querySelectorAll('.form-group :only-child').forEach((el) => {
    el.parentElement.style.gap = 0;
  });

  // Current and future DOM listener
  document.querySelector('body').addEventListener('click', (evt) => {
    const el = evt.target;

    if (Array.from(document.querySelectorAll('.collapse > a')).includes(el)) {
      const collapseContent = el.parentElement.querySelector(':scope > div');

      if (el.parentElement.classList.toggle('open')) {
        collapseContent.style.maxHeight = `${collapseContent.scrollHeight}px`;
      } else {
        collapseContent.style.maxHeight = '0px';
      }
    }

    if (Array.from(document.querySelectorAll('.button-group button')).includes(el)) {
      el.parentElement.querySelectorAll('button').forEach((_el) => _el.classList.remove('active'));
      el.classList.add('active');
    }
  }, true);

  const $config = document.querySelector('#config');

  $config.addEventListener('submit', async (e) => {
    e.preventDefault();

    console.log('Saving...');

    // Process all inputs except firemodes
    const newConfig = {};

    $config.querySelectorAll('.container>div:not(#SHOOTMODE)').forEach((module) => {
      Object.assign(newConfig, extractInputs(module));
    });

    // Process firemodes

    newConfig.fireModes = [];

    $config.querySelectorAll('#SHOOTMODE [data-firemode-index]').forEach((module) => {
      const modeindex = parseInt(module.getAttribute('data-firemode-index'), 10);

      const mode = extractInputs(module);
      mode.motorPower /= 100;

      module.querySelectorAll('[data=burstMode] button').forEach((modeBtn, index) => {
        if (modeBtn.matches('.active')) {
          mode.burstMode = index;
        }
      });

      newConfig.fireModes[modeindex] = mode;
    });

    document.getElementById('loader').classList.remove('hide');

    let updatedConfig;
    try {
      updatedConfig = await _fetch('/api/config', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(newConfig),
      });
    } catch (error) {
      setWifiStatus('interrupted');
      console.log('[CONFIG] Cannot send config');
    }

    if (updatedConfig) {
      // Convert to string to allow simple object comparison
      const sorting = Object.keys(newConfig).sort();
      if (JSON.stringify(newConfig, sorting) === JSON.stringify(updatedConfig, sorting)) {
        console.log('[CONFIG] Update sucessfull');
      } else {
        console.log('[CONFIG] Data sended not equal to data received:');
        console.log('-->', newConfig);
        console.log('<--', updatedConfig);
        alert('Your OpenMosfet returned a different value from what you wanted to save. You should check every value you typed to detect any changes.');
      }

      renderConfig(updatedConfig);
    }

    document.getElementById('loader').classList.add('hide');
  });

  document.querySelector('#updatefromgithub').addEventListener('click', async () => {
    document.querySelectorAll('.is_uptodate').forEach((el) => { el.style.display = 'none'; });
    document.querySelectorAll('.is_updating').forEach((el) => { el.style.display = 'block'; });
    document.querySelectorAll('.is_updateavailable').forEach((el) => { el.style.display = 'none'; });

    try {
      document.querySelector('#UPDATE').classList.add('is_updating');
      await _fetch('/api/core/update/', {
        method: 'POST',
      });
    } catch (error) {
      setWifiStatus('interrupted');
      console.log('[CONFIG] Cannot request update');
    } finally {
      document.getElementById('loader').classList.remove('hide');
      setTimeout(() => {
        document.location.reload();
      }, 600);
    }
  });

  setWifiStatus('interrupted');

  try {
    version = await getVersion();
  } catch (error) {
    console.log('[VERSION] Cannot fetch config');
    return;
  }

  setWifiStatus('only_local');

  console.log('[VERSION]', version);
  document.getElementById('version').textContent = `v${version}`;

  try {
    config = await getConfig();
  } catch (error) {
    setWifiStatus('interrupted');
    console.log('[CONFIG] Cannot fetch config');
    return;
  }

  document.getElementById('loader').classList.add('hide');
  setWifiStatus('only_local');

  renderConfig(config);

  if (config.enableSetupScreen) {
    renderSetup();
  }

  try {
    latestUpdate = await checkUpdates();
  } catch (error) {
    console.log('[LATEST_UPDATE] Cannot fetch latest update');
  }

  if (latestUpdate) {
    renderUpdate(latestUpdate, version);
  }
});

function renderUpdate(latestUpdate, version) {
  setWifiStatus('connected');
  console.log('[LATEST_VERSION]', latestUpdate.tag_name);

  document.querySelectorAll('[data=newversion]').forEach((el) => { el.textContent = `v${latestUpdate.tag_name}`; });
  document.querySelectorAll('[data=newversionurl]').forEach((el) => { el.href = latestUpdate.html_url; });

  if (latestUpdate.tag_name !== version) {
    document.querySelectorAll('.is_uptodate').forEach((el) => { el.style.display = 'none'; });
    document.querySelectorAll('.is_updating').forEach((el) => { el.style.display = 'none'; });
    document.querySelectorAll('.is_updateavailable').forEach((el) => { el.style.display = 'block'; });
  } else {
    document.querySelectorAll('.is_uptodate').forEach((el) => { el.style.display = 'block'; });
    document.querySelectorAll('.is_updating').forEach((el) => { el.style.display = 'none'; });
    document.querySelectorAll('.is_updateavailable').forEach((el) => { el.style.display = 'none'; });
  }
}

function renderConfig(config) {
  console.log('[CONFIG]', config);

  // Auto-fill input name matching with keys
  fillInputs(document.querySelector('#config'), config);

  const template = document.querySelector('#shootmode_template');
  const container = document.querySelector('#shootmode_container');

  container.innerHTML = '';
  config.fireModes.forEach((mode, index) => {
    const clone = document.importNode(template.content, true);

    fillInputs(clone, mode);
    clone.children[0].setAttribute('data-firemode-index', index);
    clone.querySelector('[data=firemode_index]').textContent = index + 1;
    clone.querySelector('[name=motorPower]').value = mode.motorPower * 100;
    clone.querySelectorAll('[data=burstMode] button')[mode.burstMode].classList.add('active');

    container.appendChild(clone);
  });

  if (!config.enableSetupScreen) {
    document.querySelector('#setup').classList.add('hide');
  }
}

function renderSetup() {
  const intervals = {};
  document.querySelector('#setup').classList.remove('hide');
  document.querySelectorAll('#setup .close').forEach((x) => {
    x.addEventListener('click', () => {
      document.removeEventListener('scroll', onSetupScroll);
      document.querySelector('[name=enableSetupScreen]').checked = false;

      document.querySelector('#save').click();

      // Clear all intervals
      Object.values(intervals).forEach(clearInterval);
    });
  });

  document.querySelectorAll('#setup a[data-target]').forEach((x) => {
    x.addEventListener('click', (e) => {
      e.preventDefault();
      document.querySelector(e.target.getAttribute('data-target')).scrollIntoView({
        behavior: 'smooth',
      });
    });
  });

  document.querySelector('#setup .slides').addEventListener('scroll', onSetupScroll);

  function onSetupScroll() {
    debounce(onSetupScrolled, 500);
  }

  function onSetupScrolled() {
    if (isInViewport(document.querySelector('#gearboxmodel'))) {
      intervals.trigger = setInterval(async () => {
        const componentsState = await _fetch('/api/components/state');
        if (componentsState.trigger) {
          document.querySelector('#gearboxmodel').classList.add('active');
        } else {
          document.querySelector('#gearboxmodel').classList.remove('active');
        }
      }, 500);
    } else {
      clearInterval(intervals.trigger);
    }
  }
}

function fillInputs(el, data) {
  Object.entries(data).forEach(([key, val]) => {
    if (typeof val === 'object') return;
    el.querySelectorAll(`[name="${key}"]`).forEach((input) => {
      if (input.matches('[type="checkbox"]')) {
        input.checked = val;
      } else {
        input.value = val;
      }
    });
  });
}

function extractInputs(el) {
  const data = {};

  el.querySelectorAll('[name]').forEach((input) => {
    if (input.matches('[type="checkbox"]')) {
      data[input.name] = input.checked;
    } else {
      data[input.name] = input.value;
      if (input.type === 'number') data[input.name] = Number(data[input.name]);
    }
  });

  return data;
}

function getConfig() {
  return _fetch('/api/config');
}

function getVersion() {
  return _fetch('/api/core/version');
}

function checkUpdates() {
  return _fetch('https://api.github.com/repos/openairsoft/openmosfet/releases/latest');
}

async function _fetch(url, options, retryCount) {
  if (retryCount === undefined) retryCount = 1;
  if (options === undefined) options = {};
  try {
    return await fetch(url, options).then((response) => response.json());
  } catch (err) {
    retryCount += 1;
    if (retryCount > TRY_LIMIT) {
      throw new Error('TRY_LIMIT_REACHED');
    } else {
      // wait for 1 sec
      await new Promise((resolve) => {
        setTimeout(resolve, 1000);
      });

      return _fetch(url, options, retryCount);
    }
  }
}

function setWifiStatus(status) {
  document.querySelectorAll('.is_only_local').forEach((el) => { el.style.display = 'none'; });
  switch (status) {
    case 'connected':
      document.getElementById('wifistatus').style.fill = '#009432';
      break;
    case 'only_local':
      document.getElementById('wifistatus').style.fill = '#f39c12';
      document.querySelectorAll('.is_only_local').forEach((el) => { el.style.display = 'block'; });
      break;
    case 'interrupted':
      document.getElementById('wifistatus').style.fill = '#EA2027';
      break;
    default:
      break;
  }
}

function isInViewport(element) {
  const rect = element.getBoundingClientRect();
  return (
    rect.top >= 0
    && rect.left >= 0
    && rect.bottom <= (window.innerHeight || document.documentElement.clientHeight)
    && rect.right <= (window.innerWidth || document.documentElement.clientWidth)
  );
}

function debounce(method, delay) {
  clearTimeout(method._tId);
  method._tId = setTimeout(() => {
    method();
  }, delay);
}
