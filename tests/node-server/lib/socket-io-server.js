module.exports = function Server(expressInstance) {
    var parseCookie = require('connect').utils.parseCookie;
    var io = require('socket.io').listen(expressInstance);
    
    io.configure(function () {
        io.set('log level', 2);
    });

    io.sockets.on('connection', function(socket) {

    });

    io.sockets.on('error', function(){
        console.log(arguments);
    });

    return io;
};
