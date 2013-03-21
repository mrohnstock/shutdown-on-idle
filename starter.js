var SOI  = require('./soi');
var http = require('http');
var url  = require('url');
var s    = new SOI.SOI();

http.createServer(function (req, res) {
    var parts  = url.parse(req.url, true);

    if (req.url === '/version')
    {
        res.writeHead(200, {'Content-Type': 'text/plain'});
        res.end(s.version());
    }

    if (req.url === '/shutdown')
    {
        res.writeHead(200, {'Content-Type': 'text/plain'});
        res.end(s.shutdown());
    }

    if (req.url === '/')
    {
        res.writeHead(200, {'Content-Type': 'text/html'});
        res.write('<html>\n');
        res.write('<head>\n');
        res.write('<script src="//ajax.googleapis.com/ajax/libs/jquery/1.9.1/jquery.min.js"></script>\n');
        res.write('<script src="https://raw.github.com/twitter/bootstrap/eb24718add4dd36fe92fdbdb79e6ff4ce5919300/docs/assets/js/bootstrap.min.js"></script>\n');
        res.write('<link href="https://raw.github.com/twitter/bootstrap/eb24718add4dd36fe92fdbdb79e6ff4ce5919300/docs/assets/css/bootstrap.css" rel="stylesheet" />\n');
        res.write('</body>\n');
        res.write('<body>\n');
        res.write(s.version() + '<br />\n\n<br />');
        res.write(s.processlist() + '<br />\n');
        res.write('<a class="btn btn-danger" href="shutdown">Shutdown</a>\n');
        res.write('</body>\n');
        res.end("</html>");
    }

    res.writeHead(200, {'Content-Type': 'text/plain'});
    res.end(s.add(parts.query['add']));

}).listen(12345, '0.0.0.0');
console.log('Server running at http://0.0.0.0:12345/');