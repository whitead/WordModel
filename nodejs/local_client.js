var http = require('http');
var fs = require('fs');

function handler (req, res) {
  fs.readFile(__dirname + '/local_client.html',
  function (err, data) {
    if (err) {
      res.writeHead(500);
      return res.end('Error loading local_client.html');
    }
    res.writeHead(200);
    res.end(data);
  });
}

http.createServer(handler).listen(8888, '127.0.0.1');

console.log('Server running at http://127.0.0.1:8888/');