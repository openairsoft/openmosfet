const colors = require('colors'),
      express = require('express'),
      path = require('path'),
      port = 3000;

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
app.get('/cfg.json', (req, res) => {
  console.log('[GET /cfg.json] Config requested'.request);
  res.json(config);
  console.log(JSON.stringify(config).dataSent+"\n\n\n");
})

// To improve
app.patch('/api/core', (req, res) => {
  console.log('[PATCH /api/core] Update from github requested'.request);
  console.log('This feature is not available in local development mode\n\n\n'.warn);
  res.json(config)
})

// Update config with new data (no validation) and return it
app.post('/api/config', (req, res) => {
  console.log('[POST /api/config] Form submitted'.request);
  console.log('There is no validation in local development mode,\nthe request body is saved and returned '.warn);
  console.log(JSON.stringify(req.body).dataReceived);
  config = req.body;
  setTimeout(()=>{
    res.json(config);
  }, 500);
  console.log(JSON.stringify(config).dataSent+"\n\n\n");
})

app.listen(port, () => {
  console.log(`Local development server is running at http://localhost:${port}\n`)
})