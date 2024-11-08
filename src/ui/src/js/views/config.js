import { postConfig } from '../utils/config';
import { extractInputs, fillInputs, map } from '../utils/inputs';
import { renderWifiStatus } from './wifi-status';

export function renderConfig(config) {
  // Auto-fill input name matching with keys
  fillInputs(document.querySelector('#config'), config);

  const template = document.querySelector('#shootmode_template');
  const container = document.querySelector('#shootmode_container');

  container.innerHTML = '';
  config.fireModes.forEach((mode, index) => {
    const clone = document.importNode(template.content, true);

    fillInputs(clone, {...mode, motorPower: map(mode.motorPower, 0, 255, 0, 100) });
    clone.children[0].setAttribute('data-firemode-index', index);
    clone.querySelector('[data=firemode_index]').textContent = index + 1;
    clone.querySelectorAll('[data=burstMode] button')[mode.burstMode].classList.add('active');

    container.appendChild(clone);
  });

  if (!config.enableSetupScreen) {
    document.querySelector('#setup').classList.add('hide');
  }
}

export async function updateConfig(e) {
  e.preventDefault();

  console.log('Saving...');

  // Process all inputs except firemodes
  const newConfig = {};

  const $config = document.querySelector('#config');

  $config.querySelectorAll('.container>div:not(#SHOOTMODE)').forEach((module) => {
    Object.assign(newConfig, extractInputs(module));
  });

  // Process firemodes

  newConfig.fireModes = [];

  $config.querySelectorAll('#SHOOTMODE [data-firemode-index]').forEach((module) => {
    const modeindex = parseInt(module.getAttribute('data-firemode-index'), 10);

    const mode = extractInputs(module);
    mode.motorPower = map(mode.motorPower, 0, 100, 0, 255)

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
    updatedConfig = await postConfig(newConfig);
  } catch (error) {
    renderWifiStatus('interrupted');
    console.log('[CONFIG] Cannot send config');
  } finally {
    document.getElementById('loader').classList.add('hide');
  }

  if (updatedConfig) {
    // Convert to string to allow simple object comparison
    const sorting = Object.keys(newConfig).sort();
    if (JSON.stringify(newConfig, sorting) === JSON.stringify(updatedConfig, sorting)) {
      console.log('[CONFIG] Update sucessful');
    } else {
      console.log('[CONFIG] Data sended not equal to data received:');
      console.log('-->', newConfig);
      console.log('<--', updatedConfig);
      alert('Your OpenMosfet returned differents values from what you sent. This is unexpected.');
    }

    renderConfig(updatedConfig);
  }
}
