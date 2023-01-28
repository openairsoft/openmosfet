import { gzip } from '@gfx/zopfli';
import FS from 'fs';
import path from 'path';
import { fileURLToPath } from 'url';
import {minify} from 'html-minifier';

const __filename = fileURLToPath(import.meta.url);

const __dirname = path.dirname(__filename);

const html = FS.readFileSync(path.resolve(__dirname, '../build/index.rollup.html'), { encoding: 'utf-8' });
console.log(`[COMPRESS] Original size:\t${html.length} Bytes`);

const minified = minify(html, {
  minifyCSS: true,
  collapseWhitespace: true,
  removeComments: true,
  preserveLineBreaks: false,
});

FS.writeFileSync(path.resolve(__dirname, '../build/index.html'),minified, { encoding: 'utf-8' });

console.log(`[COMPRESS] Minified size:\t${minified.length} Bytes`);

gzip(minified, { numiterations: 15 }, (err, output) => {
  if (err) {
    throw err;
  }

  const FILE = `const uint32_t UI_HTML_SIZE = ${output.length};
  const uint8_t UI_HTML[] PROGMEM = { ${output} };`;

  FS.writeFileSync(path.resolve(__dirname, '../../ui.h'), FILE);
  console.log(`[COMPRESS] Compressed size:\t${output.length} Bytes`);
  console.log(`[COMPRESS] Compressed output writed to ui.h`);
});
