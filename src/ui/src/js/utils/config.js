import { request } from './request';

export function getConfig() {
  return request('/api/config');
}

export function postConfig(config) {
  return request('/api/config', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
    },
    body: JSON.stringify(config),
  });
}

export function getVersion() {
  return request('/api/core/version');
}
