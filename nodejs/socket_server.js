var sys = require('sys');
var ModelAdaptor = require('./model_adaptor');

if(process.argv.length < 4) {
    sys.log('Usage: socket_server <model_server address> <port>');
    return;
}
var model_host = process.argv[2];
var model_port = +process.argv[3];

var io = require('socket.io').listen(80);

io.sockets.on('connection', function (socket) {
    var ma = new ModelAdaptor(model_host, model_port, function(prediction) {
	socket.emit('prediction', prediction);
    });
    socket.on('predict', function (token) { 
	console.log('predict');
	ma.get_prediction(token);
    });
    socket.on('train', function (token) { 
	console.log('training');
	ma.train(token);
    });
    socket.on('disconnect', function () { 
	ma.close();
    });
});

