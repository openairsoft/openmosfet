const express = require('express')
const path = require('path')
const port = 3000

let config = require('./defaultConf.json');

const app = express()
app.use(express.json())
// Map src folder to server root
app.use('/', express.static(path.join(__dirname, '../../src/')));


// Return the config
app.get('/cfg.json', (req, res) => {
  res.json(config)
})

// To improve
app.path('/api/core', (req, res) => {
  res.json(config)
})

// Update config with new data (no validation) and return it
app.post('/api/config', (req, res) => {
  console.log('[POST /api/config] Form submitted');
  config = req.body;
  res.json(config)
})


app.listen(port, () => {
  console.log(`Example app listening at http://localhost:${port}`)
})