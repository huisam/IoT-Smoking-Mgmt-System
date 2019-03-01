// Send data to cloud

var LED = process.argv[2];

console.log('LED : ' + LED);

var webSocketUrl = ""; // write your SocketUrl, id, Token
var device_id = "";
var device_token = "";

var WebSocket = require('ws');
var isWebSocketReady = false;
var ws = null;

var puts = require('sys').puts;

function getTimeMillis(){
    return parseInt(Date.now().toString());
}

function start() {
    //Create the WebSocket connect
    isWebSocketReady = false;
    ws = new WebSocket(webSocketUrl); // Connection
    ws.on('open', function() { // message type 'open' or 'messgae' or 'close'
        console.log("WebSocket connection is open ....");
        register(); // register function call
        sendSensorValueToArtikCloud();
    });
    ws.on('message', function(data) {
       console.log("Received message: " + data + '\n');
       handleRcvMsg(data); // Handling Received message
    });
    ws.on('close', function() {
        console.log("WebSocket connection is closed ....");
        exitClosePins();
    });
}

function handleRcvMsg(msg){ // Called when message received
    var msgObj = JSON.parse(msg); // Parsing message
    if (msgObj.type != "action") return; //Early return -->Only Handle Message

    var actions = msgObj.data.actions; // get action
    var actionName = actions[0].name; // get action's name
    console.log("The received action is " + actionName); // print action name
    var newState;

    if (actionName.toLowerCase() == "receiveok") { 
                console.log('ByeBye');
        process.exit(0);
    }
    else {
        console.log('Do nothing since receiving unrecognized action ' + actionName);
        return;
    }
}
// Websocket base step
// 1) register
// 2) transaction

function register(){ // When 'open' : Register device to cloud
    console.log("Registering device on the WebSocket connection");
    try{
        var registerMessage = '{"type":"register", "sdid":"'+device_id+'", "Authorization":"bearer '+device_token+'", "cid":"'+getTimeMillis()+'"}'; // json type message format : Type , Source Device ID , Token , Time stamp
        console.log('Sending register message ' + registerMessage + '\n');
        ws.send(registerMessage, {mask: true}); // send to cloud
        isWebSocketReady = true;
    }
    catch (e) {
        console.error('Failed to register messages. Error in registering message: '
        + e.toString());
    }
}

function sendSensorValueToArtikCloud(){ // Called every 5s
    try{
        ts = ', "ts": '+getTimeMillis();
        var data = {
            "LED": LED,
        };
        var payload = '{"sdid":"'+device_id+'"'+ts+', \"data": '+JSON.stringify(data)+', \"cid":"'+getTimeMillis()+'"}';
        console.log('Sending payload ' + payload + '\n');
        ws.send(payload, {mask: true});
    } catch (e) {
        console.error('Error in sending a message: ' + e.toString() +'\n');
    }
}

function exitClosePins() {
    console.log('Exit and destroy all pins!');
    process.exit();
}

start();
