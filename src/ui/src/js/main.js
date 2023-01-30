import { getConfig, getVersion } from './utils/config';
import { renderConfig, updateConfig } from './views/config';
import { renderSetup } from './views/setup';
import { checkUpdates, performUpdate, renderUpdate } from './views/update';
import { renderWifiStatus } from './views/wifi-status';

// Current and future DOM listener
document.body.addEventListener('click', (evt) => {
  const el = evt.target;

  // Collapsable
  if (Array.from(document.querySelectorAll('.collapse > a')).includes(el)) {
    const collapseContent = el.parentElement.querySelector(':scope > div');

    if (el.parentElement.classList.toggle('open')) {
      collapseContent.style.maxHeight = `${collapseContent.scrollHeight}px`;
    } else {
      collapseContent.style.maxHeight = '0px';
    }
  }

  // Button group
  if (Array.from(document.querySelectorAll('.button-group button')).includes(el)) {
    el.parentElement.querySelectorAll('button').forEach((_el) => _el.classList.remove('active'));
    el.classList.add('active');
  }
}, true);

window.addEventListener('DOMContentLoaded', async () => {
  if (!window.fetch || !('content' in document.createElement('template'))) {
    alert('Browser not supported. Please update');
    throw new Error('Browser not supported. Please update');
  }

  let version;
  let config;
  let latestUpdate;

  document.querySelector('#config').addEventListener('submit', updateConfig);
  document.querySelector('#updatefromgithub').addEventListener('click', performUpdate);

  renderWifiStatus('interrupted');

  try {
    [version, config] = await Promise.all([getVersion(), getConfig()]);
    renderWifiStatus('only_local');
  } catch (error) {
    console.log('[VERSION] Cannot fetch config or version');
    throw error;
  } finally {
    document.getElementById('loader').classList.add('hide');
  }

  document.querySelectorAll('[data=version]').forEach((el) => { el.textContent = `v${version}`; });

  renderConfig(config);

  if (config.enableSetupScreen) {
    renderSetup();
  }

  try {
    latestUpdate = await checkUpdates();
  } catch (error) {
    renderWifiStatus('only_local');
    console.log('[LATEST_UPDATE] Cannot fetch latest update');
  }

  if (latestUpdate) {
    renderUpdate(latestUpdate, version);
  }
});
