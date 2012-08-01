// Fetch the site configuration
var siteConf = require('./lib/getConfig');
process.title = siteConf.uri.replace(/http:\/\/(www)?/, '');

process.addListener('uncaughtException', function (err, stack) {
	console.log('Caught exception: '+err+'\n'+err.stack);
	console.log('\u0007'); // Terminal bell
});

var express = require('express');

var app = module.exports = express.createServer();
app.listen(siteConf.port, null);

// Add auto reload for CSS/JS/templates when in development
app.configure('development', function(){
});

// Settings
app.configure(function() {
	app.set('view engine', 'ejs');
	app.set('views', __dirname+'/views');
});

// Middleware
app.configure(function() {
app.use(express.bodyParser({uploadDir:'./uploads'}));
	app.use(express.cookieParser());
    	app.use(express.logger({format: ':response-time ms - :date - :req[x-real-ip] - :method :url :user-agent / :referrer'}));
        app.use(express.static(__dirname + '/public'));
});


// Show all errors and keep search engines out using robots.txt
app.configure('development', function(){
	app.use(express.errorHandler({
		'dumpExceptions': true
		, 'showStack': true
	}));
	app.all('/robots.txt', function(req,res) {
		res.send('User-agent: *\nDisallow: /', {'Content-Type': 'text/plain'});
	});
});

// Suppress errors, allow all search engines
app.configure('production', function(){
	app.use(express.errorHandler());
	app.all('/robots.txt', function(req,res) {
		res.send('User-agent: *', {'Content-Type': 'text/plain'});
	});
});

// Error handling
app.error(function(err, req, res, next){
    // Log the error to Airbreak if available, good for backtracking.
    console.log(err);

    if (err instanceof NotFound) {
        res.render('errors/404');
    } else {
        res.render('errors/500');
    }
});
function NotFound(msg){
    this.name = 'NotFound';
    Error.call(this, msg);
    Error.captureStackTrace(this, arguments.callee);
}

// Setup web routes.
require('./routes')(app,siteConf);
console.log('Running in '+(process.env.NODE_ENV || 'development')+' mode @ '+siteConf.uri);
