var net = require("net");

var server = net.createServer(function(socket) {
    socket.on('data', function(data) {
        console.log(data.toString());
    })
})

server.listen(8000, function() {
    console.log("Server bound on http://127.0.0.1:8000/");
})