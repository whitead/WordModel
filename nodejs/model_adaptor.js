var net = require('net');
var sys = require('sys');
var packet = require('./packet');

function ModelAdaptor(host, port, prediction_callback) {
    this.socket = new net.Socket();
    this.socket.on('error', function(er) {
	if (er.errno === process.ECONNREFUSED) {
            sys.log('ECONNREFUSED: connection refused to '
		    + host
		    + ':'
		    + port);
	} else {
            sys.log(er);
	}
    });
    this.socket.connect(port, host)
    this.socket.on('data', function(data) {
	do {
	    var p = packet.decode_packet(data);
	    if(p.type == packet.PACKET_TYPE.PREDICTION)
		prediction_callback(p.string);
	    data = p.remaining;
	} while(p.remaining.length > 0);
    });
}

ModelAdaptor.prototype.get_prediction = function(str) {
    var p = packet.encode_packet(packet.PACKET_TYPE.PREDICT, str, packet.create_buffer());
    this.socket.write(p);
};

ModelAdaptor.prototype.close = function() {
    this.socket.unref();
};

module.exports = ModelAdaptor;