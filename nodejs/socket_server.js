var sys = require('sys');
var ModelAdaptor = require('./model_adaptor');

if(process.argv.length < 4) {
    sys.log('Usage: socket_server <model_server address> <port>');
    return;
}
var model_host = process.argv[2];
var model_port = +process.argv[3];

var some_text = "Hello Andrew Hello Andrew Hello Andrew Hello Andrew Hello Andrew";

var model = new ModelAdaptor(model_host, model_port, function(prediction) {
    sys.log("prediction: " + prediction);
});

some_text.split(' ').map(function(token) { model.get_prediction(token + ' ')});
some_text.split(' ').map(function(token) { model.get_prediction(token + ' ')});
//model.close();