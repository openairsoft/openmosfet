import { request } from '../utils/request';
import { renderWifiStatus } from './wifi-status';

export function checkUpdates() {
  return request('https://api.github.com/repos/openairsoft/openmosfet/releases/latest');
}

export function renderUpdate(latestUpdate, version) {
  renderWifiStatus('connected');
  console.log('[LATEST_VERSION]', latestUpdate.tag_name);

  document.querySelectorAll('[data=newversion]').forEach((el) => { el.textContent = `v${latestUpdate.tag_name}`; });
  document.querySelectorAll('[data=updateurl]').forEach((el) => { el.href = latestUpdate.html_url; });

  if (latestUpdate.tag_name.localeCompare(version) > 0) {
    document.querySelectorAll('.is_uptodate').forEach((el) => { el.style.display = 'none'; });
    document.querySelectorAll('.is_updating').forEach((el) => { el.style.display = 'none'; });
    document.querySelectorAll('.is_updateavailable').forEach((el) => { el.style.display = 'block'; });
  } else {
    document.querySelectorAll('.is_uptodate').forEach((el) => { el.style.display = 'block'; });
    document.querySelectorAll('.is_updating').forEach((el) => { el.style.display = 'none'; });
    document.querySelectorAll('.is_updateavailable').forEach((el) => { el.style.display = 'none'; });
    document.querySelectorAll('[data=updateurl]').forEach((el) => { el.href = latestUpdate.html_url.replace(latestUpdate.tag_name, version); });
  }
}
export async function performUpdate() {
  document.querySelectorAll('.is_uptodate').forEach((el) => { el.style.display = 'none'; });
  document.querySelectorAll('.is_updating').forEach((el) => { el.style.display = 'block'; });
  document.querySelectorAll('.is_updateavailable').forEach((el) => { el.style.display = 'none'; });

  try {
    document.querySelector('#UPDATE').classList.add('is_updating');
    await request('/api/core/update/', {
      method: 'POST',
    });
  } catch (error) {
    renderWifiStatus('interrupted');
    console.log('[CONFIG] Cannot request update');
  } finally {
    document.getElementById('loader').classList.remove('hide');
    setTimeout(() => {
      document.location.reload();
    }, 600);
  }
}
