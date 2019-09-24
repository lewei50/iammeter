const tls = require('tls');
const fs = require('fs');
var path = require('path');


const options = {
    key: fs.readFileSync('_server.key'),
    cert: fs.readFileSync('_server.crt'),
    rejectUnauthorized: false,
};

const server = tls.createServer(options, (socket) => {
    console.log('server connected',
        socket.authorized ? 'authorized' : 'unauthorized');


    socket.setEncoding('utf8');

    socket.on('data', (data) => {
        console.log(data);
    });

    socket.on('error', function(err) {
        console.log(err);
    });

});
process.on('SIGINT', function() {
    console.log('server closed.');
    setTimeout(function() {
        process.exit();
    }, 100);
});
server.listen(8000, () => {
    console.log('server bound');
});