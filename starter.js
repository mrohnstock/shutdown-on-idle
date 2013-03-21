var SOI  = require('./soi');
var http = require('http');
var url  = require('url');
var s    = new SOI.SOI();

http.createServer(function (req, res) {
    var parts  = url.parse(req.url, true);

    if (req.url === '/version')
    {
        res.writeHead(200, {'Content-Type': 'text/plan'});
        res.end(s.version());
    }

    res.end(s.add(parts.query['add']));
}).listen(12345, '127.0.0.1');
console.log('Server running at http://127.0.0.1:12345/');