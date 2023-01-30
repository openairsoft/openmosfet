import { renderWifiStatus } from '../views/wifi-status';

const TRY_LIMIT = 20;

export async function request(url, options, retryCount) {
  if (retryCount === undefined) retryCount = 1;
  if (options === undefined) options = {};
  try {
    return await fetch(url, options).then((response) => response.json());
  } catch (err) {
    retryCount += 1;
    if (retryCount > TRY_LIMIT) {
      renderWifiStatus('interrupted');
      throw new Error('Request try limit reached');
    } else {
      // wait for 1 sec
      await new Promise((resolve) => {
        setTimeout(resolve, 1000);
      });

      return request(url, options, retryCount);
    }
  }
}
