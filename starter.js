var SOI     = require('./soi');
var fs      = require('fs');
var mime    = require('mime');
var twig    = require('twig');
var express = require('express');
var path    = require('path');
var s       = new SOI.SOI();
var app     = express();

app.use('/resources', express.static(__dirname + '/resources'));

app.get('/', function(req, res)
{
    res.render('index.twig', { soi: s });
});

app.get('/about', function(req, res)
{
    res.render('about.twig', { soi: s });
});

app.get('/graph', function(req, res)
{
    res.render('graph.twig', { soi: s });
});

app.get('/shutdown', function(req, res)
{
    res.end(s.shutdown());
});

app.get('/add', function(req, res)
{
    if (req.query.name)
    {
        s.add(req.query.name, req.query.description, req.query.binary, req.query.logfile);
        res.render('index.twig', { message : "Process '" + req.query.name + "' added successfully!", type : "success", soi : s });
    }
});
app.listen(12345);
console.log('Server running at port 12345');