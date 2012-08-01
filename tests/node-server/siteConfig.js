var settings = {
	sessionSecret : 'sessionSecret'
	, port: process.env.PORT || 8080
	, uri: 'http://localhost:8080' // Without trailing /
	, debug: (process.env.NODE_ENV !== 'production')
};

if (process.env.NODE_ENV == 'production') {
	settings.uri = 'http://yourname.no.de';
	settings.port = process.env.PORT || 80; 
}
module.exports = settings;
