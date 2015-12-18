// based on https://github.com/voodootikigod/node-serialport/blob/master/examples/readdata.js
var com = require("serialport");

// note, Teensys all use the same serial number, so your operating system will apply some
// magic to generate a name for the serial ports.  On OSX, the Teensys are named with a
// number that increases when the port is opened/closed.  Example:
//
// $ ls -l /dev/tty.usbmodem*
// crw-rw-rw-  1 root  wheel   31,   8 Oct  8 12:32 /dev/tty.usbmodem1168911
// crw-rw-rw-  1 root  wheel   31,  38 Oct  8 17:15 /dev/tty.usbmodem19
// crw-rw-rw-  1 root  wheel   31,  54 Oct  9 12:56 /dev/tty.usbmodem27
// crw-rw-rw-  1 root  wheel   31,  58 Oct  9 13:00 /dev/tty.usbmodem29
// crw-rw-rw-  1 root  wheel   31,  62 Oct  9 13:04 /dev/tty.usbmodem31
// crw-rw-rw-  1 root  wheel   31,  66 Oct  9 13:04 /dev/tty.usbmodem33
// 
// Note that the ones ending in 19, 27, 29, 31, 33 are all the same Teensy.
// Closing the port properly will delete that device, but terminating the connection
// improperly will not.  Hence, hard-coding the device name is not recommended.
//
// Doing it anyway.  Use the highest-numbered device of the format /dev/tty.usbmodemXX
var portNumber = "/dev/tty.usbmodem36";
var s = new com.SerialPort(portNumber, {
    baudrate: 9600,
    parser: com.parsers.readline('\r\n')
});

s.on('open',function() {
    console.log('Port ' + portNumber + ' open');
    console.log("Enter the command 's.close()' to close this port");
    s.write("l"); // after response, should output the string "wasd"
});

list = function(callback){
    callback = callback || function (err, ports){};
    if (process.platform !== 'darwin'){
        s.list(function(err, ports){
            out = [];
            ports.forEach(function(port){
                out.push(port.comName);
            });
            callback(null, out);
        });
        return;
    }
    child_process.exec('ls /dev/tty.*', function(err, stdout, stderr){
        if (err) return callback(err);
        if (stderr !== "") return callback(stderr);
        return callback(null, stdout.split("\n").slice(0,-1));
    });
};

s.on('data', function(data) {
    console.log(data);
});

