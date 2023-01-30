export function renderWifiStatus(status) {
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
