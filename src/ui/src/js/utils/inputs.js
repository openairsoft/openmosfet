export function fillInputs(el, data) {
  Object.entries(data).forEach(([key, val]) => {
    if (typeof val === 'object') return;
    el.querySelectorAll(`[name="${key}"]`).forEach((input) => {
      if (input.matches('[type="checkbox"]')) {
        input.checked = val;
      } else {
        if(input.type === 'number') {
          const { step } = input;
          const precision = (step ?? 1).toString().split(".")[1]?.length ?? 0;
          input.value = Number(val).toFixed(precision);
        } else {
          input.value = val;
        }
      }
    });
  });
}

export function extractInputs(el) {
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
