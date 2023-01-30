import { request } from '../utils/request';

function debounce(method, delay) {
  clearTimeout(method._tId);
  method._tId = setTimeout(() => {
    method();
  }, delay);
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

export function renderSetup() {
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
        const componentsState = await request('/api/components/state');
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
