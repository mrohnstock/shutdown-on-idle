var SOI = require('./soi');
var http = require('http');
var s = new SOI.SOI();

http.createServer(function (req, res) {

  res.writeHead(200, {'Content-Type': 'text/plan'});
  res.end(s.version());

}).listen(12345, '127.0.0.1');
console.log('Server running at http://127.0.0.1:12345/');