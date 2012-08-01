var ws_server = "ws://localhost:7681";

var topology = {
    '10.1.1.1' : {}
};

var connected = false;

$(document).ready(function(){
    
    
    var cws = new CommotionSocket(ws_server,"chat-server");
    
//    // Write your code in the same way as for native WebSocket:
//    var ws = new WebSocket(ws_server,"commotion-ws");
//    ws.onopen = function() {
//        connected=true;
//        $("#connected").removeClass("red");
//        $("#connected").addClass("green");
//    };
//    ws.onmessage = function(e) {
//        alert(e.data);
//    };
//    ws.onclose = function() {
//        connected=false;
//        $("#connected").addClass("red");
//        $("#connected").removeClass("green");
//    };
//    
//    $("#send-msg").click(function(){
//        if(!connected)return;
//        var msg = $('#msg').val(); 
//        ws.send(msg);
//        console.log("Sent:" + msg.length);
//        
//        $('#msg').val("");
//    });

})
