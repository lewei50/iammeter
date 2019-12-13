var http = require('http');

var server = http.createServer(function(req, res) {
    var body = [];
    req.on('data', (chunk) => {
        body.push(chunk);
    }).on('end', () => {
        body = Buffer.concat(body).toString();
        res.writeHead(200, { 'Content-Type': 'text/json' });
        res.end('{"Successful":true,"Message":"Upload successful."}\n');
        console.log('Got data:' + body);
    });
});
// server.on('clientError', (err, socket) => {
//     console.log(err);
//     socket.end('HTTP/1.1 400 Bad Request\r\n\r\n');
// });
server.listen(80);
console.log('Server running at http://127.0.0.1:80/');