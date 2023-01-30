import colors from 'colors';
import express from 'express';
import path from 'path';
import livereload from "livereload";
import { fileURLToPath } from 'url';
import fs from 'fs';
import { watch } from 'rollup';
import { nodeResolve } from '@rollup/plugin-node-resolve';

const __filename = fileURLToPath(import.meta.url);

const __dirname = path.dirname(__filename);

let config = JSON.parse(fs.readFileSync(path.join(__dirname, './defaultConf.json'), { encoding: 'utf-8' }));

const port = 3000;

const VERSION = '1.11.0-beta';

const PATHS = {
  tmp: path.join(__dirname, '../../tmp/'),
  src: path.join(__dirname, '../../src/'),
  build: path.join(__dirname, '../../build/'),
}

colors.setTheme({
  request: ['bgCyan', 'black'],
  dataReceived: 'grey',
  dataSent: 'green',
  warn: 'yellow',
  error: 'red'
});

const liveReloadServer = livereload.createServer({
  exts: ['html', 'css'],
});
liveReloadServer.watch([PATHS.src, PATHS.tmp]);
const watcher = watch({
  input: path.join(PATHS.src, 'js/main.js'),
  output: {
    file: path.join(PATHS.tmp, 'bundle.js'),
    sourcemap: "inline"
  },
  plugins: [
    nodeResolve(),
  ],
});

watcher.on('event', (event) => {
  if(event.code === 'ERROR') {
    console.error(event.error);
  }

  if(event.code === 'BUNDLE_END' && event.output[0].startsWith(PATHS.tmp)) {
    liveReloadServer.refresh(event.output[0]);
  }

  if (event.result) {

  	event.result.close();
  }
});


const app = express()

app.use(express.json())

// Map src folder to server root
app.get('/', (req, res) => {
  let content = fs.readFileSync(path.join(PATHS.src, 'index.html'), 'utf8');
  if(content.match(/<\/\s*body>/)) {
    content = content.replace(/<\/\s*body>/, /* html */`
    <script>
      document.write('<script src="http://'
      + (location.host || 'localhost').split(':')[0]
      + ':35729/livereload.js"></'
      + 'script>')
    </script>
    <script src="bundle.js" type="module"></script>
    </body>`);
  }
  res.send(content);
})
app.use('/', express.static(PATHS.src));
app.use('/build', express.static(PATHS.build));
app.use('/', express.static(PATHS.tmp));

// Return the config
app.get(['/api/config', '/cfg.json'], (req, res) => {
  setTimeout(()=>{
    console.log('[GET /api/config] Config requested'.request);
    res.json(config);
    console.log(JSON.stringify(config).dataSent+"\n\n\n");
  }, 500);
})

// Update config with new data (no validation) and return it
app.post('/api/config', (req, res) => {
  console.log('[POST /api/config] Form submitted'.request);
  console.log('There is no validation in local development mode,\nthe request body is saved and returned '.warn);
  console.log(JSON.stringify(req.body).dataReceived);
  config = {
    ...config,
    ...req.body
  };
  setTimeout(()=>{
    console.log(JSON.stringify(config).dataSent+"\n\n\n");
    res.json(config);
  }, 500);
})


// Return the config
app.get('/api/core/version', (req, res) => {
  setTimeout(()=>{
    console.log('[GET /api/core/version] Version requested'.request);
    res.json(VERSION);
    console.log(JSON.stringify(VERSION).dataSent+"\n\n\n");
  }, 500);
})

// To improve
app.post('/api/core/update', (req, res) => {
  console.log('[POST /api/core/update] Update from github requested'.request);
  console.log('This feature is not available in local development mode\n\n\n'.warn);
  console.log('Delay of 5 seconds before response\n\n\n'.warn);

  setTimeout(()=>{
    res.json(config);
  }, 5000);
})

app.put('/api/core/update', (req, res) => {
  console.log('[PUT /api/core/update] Update from .bin file'.request);
  console.log('This feature is not available in local development mode\n\n\n'.warn);
  res.json(config);
})


app.get('/api/components/state', (req, res) => {
  setTimeout(()=>{
    console.log('[GET /api/components/state] Components state'.request);
    let ComponentsState = {
      trigger: Boolean((Date.now()/1000).toFixed()%2) // Random value
    }
    res.json(ComponentsState);
    console.log(JSON.stringify(ComponentsState).dataSent+"\n\n\n");
  }, 500);
})


app.listen(port, () => {
  console.log(`Local development server is running at http://localhost:${port}\n`)
})
