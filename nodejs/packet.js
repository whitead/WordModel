var encoding = 'ascii'; 
var header_length = 4 + 1;
var max_body_length = 512;

PACKET_TYPE = {
    TRAIN : 0,
    PREDICT : 1,
    PREDICTION : 2
};

exports.PACKET_TYPE = PACKET_TYPE;
exports.encoding = encoding;

exports.encode_packet = function (type, string, buf) {
    length = string.length + 1;
    //write zeros
    var i = 0;
    for(; i < 3 - length / 10; i++)
	buf.write('0', i);
    buf.write('' + length, i);
    buf.write('' + type, 4);
    buf.write(string + '\0', header_length);
    return buf.slice(0,header_length + length);
};

exports.decode_packet = function (buf) {
    length = +buf.toString(encoding,0,header_length - 1);
    type = +buf.toString(encoding,header_length - 1, header_length);
    string = buf.toString(encoding,header_length, header_length + length - 1);//skip 1 for C string terminating characeter
    return {type: type, string:string, remaining:buf.slice(header_length + length, buf.length)}
};


exports.create_buffer = function () {
    return new Buffer(header_length + max_body_length, encoding);
};