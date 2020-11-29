const TRY_LIMIT = 20;

window.addEventListener("DOMContentLoaded", async () => {
  console.log("DOM entièrement chargé et analysé");

  if (!window.fetch || !("content" in document.createElement("template"))) {
    alert('Navigateur non compatible. Merci de mettre à jour');
  }

  // Remove grid gap if only one child exist
  document.querySelectorAll('.form-group :only-child').forEach(el => el.parentElement.style.gap = 0);

  // Current and future DOM listener
  document.querySelector('body').addEventListener('click', function (evt) {
    let el = evt.target;

    if (Array.from(document.querySelectorAll('.collapse > a')).includes(el)) {
      let collapseContent = el.parentElement.querySelector(':scope > div');

      if (el.parentElement.classList.toggle('open')) {
        collapseContent.style.maxHeight = collapseContent.offsetWidth + 'px';
      } else {
        collapseContent.style.maxHeight = '0px';
      }
    }

    if (Array.from(document.querySelectorAll('.button-group button')).includes(el)) {
      el.parentElement.querySelectorAll('button').forEach(el => el.classList.remove('active'))
      el.classList.add('active');
    }
  }, true);


  document.querySelector('#config').addEventListener("submit", async (e) => {
    e.preventDefault();

    console.log('Saving...');

    var newConfig = {};

    document.querySelector('#config').querySelectorAll('.container>div:not(#SHOOTMODE)').forEach(module => ({
      ...newConfig,
      ...extractInputs(module)
    }))

    newConfig.fireModes = [];

    document.querySelector('#config').querySelectorAll('#SHOOTMODE [data-firemode-index]').forEach(module => {
      let modeindex = parseInt(module.getAttribute('data-firemode-index'));

      let mode = extractInputs(module);
      mode.motorPower =  mode.motorPower / 100;

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
          'Content-Type': 'application/json'
        },
        body: JSON.stringify(newConfig)
      })
    } catch (error) {
      setWifiStatus('interrupted');
      console.log('[CONFIG] Cannot send config');
    }

    if(updatedConfig) {
      // Convert to string to allow simple object comparison
      sorting = Object.keys(newConfig).sort()
      if(JSON.stringify(newConfig, sorting) == JSON.stringify(updatedConfig, sorting)){
        console.log('[CONFIG] Update sucessfull');
      }else{
        console.log('[CONFIG] Data sended not equal to data received:');
        console.log('-->', newConfig);
        console.log('<--', updatedConfig);
        alert('Your OpenMosfet returned a different value from what you wanted to save. You should check every value you typed to detect any changes.')
      }
      
      renderConfig(updatedConfig);
    }

    document.getElementById('loader').classList.add('hide');


  });

  // Delay for debugging
  // await new Promise(resolve => setTimeout(resolve, 2000));

  setWifiStatus('interrupted');

  let version, config, latestUpdate;

  try {
    version = await getVersion();
  } catch (error) {
    console.log('[VERSION] Cannot fetch config');
    return;
  }

  setWifiStatus('only_local');
  console.log('[VERSION]', version);
  document.getElementById('version').textContent = 'v' + version;


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


  try {
    latestUpdate = await checkUpdates();
  } catch (error) {
    console.log('[LATEST_UPDATE] Cannot fetch latest update');
  }

  if (latestUpdate) {
    setWifiStatus('connected');
    console.log('[LATEST_UPDATE]', latestUpdate);

    document.querySelectorAll('[data=newversion]').forEach((el) => el.textContent = 'v' + latestUpdate.tag_name);
    document.querySelectorAll('[data=newversionurl]').forEach((el) => el.href = latestUpdate.html_url);

    if (latestUpdate.tag_name != version) {
      document.querySelectorAll('.is_uptodate').forEach((el) => el.style.display = 'none')
      document.querySelectorAll('.is_updateavailable').forEach((el) => el.style.display = 'block')
    } else {
      document.querySelectorAll('.is_uptodate').forEach((el) => el.style.display = 'block')
      document.querySelectorAll('.is_updateavailable').forEach((el) => el.style.display = 'none')
    }
  }

});


function renderConfig(config) {
  console.log('[CONFIG]', config);

  // Auto-fill input name matching with keys
  fillInputs(document.querySelector('#config'), config);

  var template = document.querySelector("#shootmode_template");
  var container = document.querySelector('#shootmode_container');

  container.innerHTML = ""
  config.fireModes.forEach((mode, index) => {
    var clone = document.importNode(template.content, true);

    fillInputs(clone, mode);
    clone.querySelector(':scope > div').setAttribute('data-firemode-index', index);
    clone.querySelector('[data=firemode_index]').textContent = index + 1;
    clone.querySelector('[name=motorPower]').value = mode.motorPower * 100;
    clone.querySelectorAll('[data=burstMode] button')[mode.burstMode].classList.add('active');

    container.appendChild(clone);

  })

}

function fillInputs(el, data) {
  for (const key in data) {
    let val = data[key];
    if (typeof val != 'object') {
      el.querySelectorAll('[name="' + key + '"]').forEach((el) => {
        if (el.matches('[type="checkbox"]')) {
          el.checked = val
        } else {
          el.value = val
        }
      });
    }
  }
}

function extractInputs(el) {
  let data = {};

  el.querySelectorAll('[name]').forEach(input => {
    if (input.matches('[type="checkbox"]')) {
      data[input.name] = input.checked;
    } else {
      
      data[input.name] = input.value
      if(input.type=='number') data[input.name] = Number(data[input.name])
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
  return _fetch('https://api.github.com/repos/simonjamain/openmosfet/releases/latest');
}

async function _fetch(url, options, retry_count) {
  if (retry_count === undefined) retry_count = 1;
  if (options === undefined) options = {};
  try {
    return await fetch(url, options).then((response) => response.json());
  } catch (err) {
    retry_count++
    if (retry_count > TRY_LIMIT) {
      throw new Error('TRY_LIMIT_REACHED');
    } else {
      await new Promise(resolve => setTimeout(resolve, 1000));
      return _fetch(url, options,retry_count);
    }
  }
}

function setWifiStatus(status) {
  switch (status) {
    case 'connected':
      document.getElementById('wifistatus').style.fill = '#009432';
      break;
    case 'only_local':
      document.getElementById('wifistatus').style.fill = '#f39c12';
      break;
    case 'interrupted':
      document.getElementById('wifistatus').style.fill = '#EA2027';
      break;

  }
}