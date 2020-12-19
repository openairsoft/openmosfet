const colors = require('colors'),
      express = require('express'),
      path = require('path'),
      port = 3000;

const VERSION = '0.2.2-alpha';

let config = require('./defaultConf.json');

colors.setTheme({
  request: ['bgCyan', 'black'],
  dataReceived: 'grey',
  dataSent: 'green',
  warn: 'yellow',
  error: 'red'
});

const app = express()
app.use(express.json())

// Map src folder to server root
app.use('/', express.static(path.join(__dirname, '../../src/')));

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
  config = req.body;
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
  console.log('Delay of 30 seconds before response\n\n\n'.warn);

  setTimeout(()=>{
    res.json(config);
  }, 5000);
})

app.put('/api/core/update', (req, res) => {
  console.log('[PUT /api/core/update] Update from .bin file'.request);
  console.log('This feature is not available in local development mode\n\n\n'.warn);
  res.json(config);
})

app.listen(port, () => {
  console.log(`Local development server is running at http://localhost:${port}\n`)
})