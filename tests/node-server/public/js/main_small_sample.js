// Specifiy the websocket server
var ws_server = "ws://"+window.location.hostname+":7681";

$(document).ready(function(){
    
    /**
     * Create the CommotionSocket object with the websocket server and
     * a list of applications to announce to the network.
     */
    var cws = new CommotionSocket(ws_server,["chat-server","other-app"],function(){
        // On connection
        
        setTimeout(function(){
            
            // Example of sending "Hello World" to each client
            cws.forclients(function(client){
                // Send must take the libraries address format which is
                // { 
                //    ip:// 32bit number representaion of ip address, host order
                //    id:// unique id for ip, allows multiple clients with the same ip. 
                // }
                cws.send(client.address,"norm","Hello World");
            });
            
        },2500);

    });
    
    /**
     * Subscribe to specific messages this will allow js to callback only messages
     * that application is listening for.
     */
    // Handle messages "norm" and "error" 
    cws.on(["norm","error"],function(ret){
        // Callback for message
    })
    
    cws.onclose(function(){
        // On dissconnect
    });
    
    cws.ontopologychange(function(ret){
       // Everytime the topology updates
       
        // Use for clients to loop through all clients connected.
        cws.forclients(function(client){
            // Handle each client
        });
    });
    
    
})
