// this file is HIGHLY inspired (if not straight-up stolen) from this file https://github.com/ayushsharma82/ElegantOTA/blob/master/ui/compress.js
// check out the cool projects this person made https://github.com/ayushsharma82/ElegantOTA and https://github.com/ayushsharma82/AsyncElegantOTA
// thank you !

const { gzip } = require('@gfx/zopfli');
const FS = require('fs');
const path = require('path');

const HTML = FS.readFileSync(path.resolve(__dirname, './build/out.html'));

gzip(HTML, { numiterations: 15 }, (err, output) => {
  if (err) {
    return console.error(err);
  }

  const FILE = `const uint32_t UI_HTML_SIZE = ${output.length};
  const uint8_t UI_HTML[] PROGMEM = { ${output} };`;

  FS.writeFileSync(path.resolve(__dirname, '../ui.h'), FILE);
  console.log(`[COMPRESS] Compressed Build Files to ui.h: ${output.length} Bytes`);
});
