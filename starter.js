var SOI     = require('./soi');
var fs      = require('fs');
var mime    = require('mime');
var twig    = require('twig');
var express = require('express');
var path    = require('path');
var s       = new SOI.SOI();
var app     = express();

app.use('/resources', express.static(__dirname + '/resources'));
app.use(express.bodyParser());

app.get('/', function(req, res)
{
    res.render('watch.twig', { soi: s });
});

app.get('/about', function(req, res)
{
    res.render('about.twig', { soi: s });
});

app.get('/system', function(req, res)
{
    res.render('system.twig', { soi: s });
});

app.post('/', function(req, res)
{
    if (req.body.name && req.body.description && req.body.binary && req.body.logfile)
    {
        if (s.add(req.body.name, req.body.description, req.body.binary, req.body.logfile))
        {
            res.render('watch.twig', { message : "Process '" + req.body.name + "' added successfully", type : "success", soi : s });
        }
        res.render('watch.twig', { message : "Process '" + req.body.name + "' already exists", type : "danger", soi : s });
    }
    else if (req.body.shutdown === s.hostname())
    {
        s.shutdown();
        res.render('watch.twig', { message : s.hostname() + " is going down for shutdown - good bye", type : "warning", soi : s });
    }
    res.render('watch.twig', { soi: s });
});
app.listen(12345);
console.log('Server running at port 12345');