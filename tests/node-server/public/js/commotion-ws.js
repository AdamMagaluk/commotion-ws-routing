(function() {
  
  /*
  {
      //Protocol message type used by backend service
      "mt" : "XXXX",

      // Destination Address
      "dst" : "10.1.1.1",
      
      // Source Address
      "src" : "10.1.1.2",
      
      // Data specific to each message type    
      "d" : {
        // Data specific to each message type    
      }
      
      // Message Types
      0 : Register client
      1 : Client dissconnect
      2 : Foward Message
      3 : Request Topology
      4 : Topology Update
      
      Register Client
      - Cleint sends this message when socket opens. Sends a list of applications
      to the server that it should register against.
      d :  {
        p : ["test-app","app2"]
      }
      
      - When server recieves this message it fowards message to all connected clients
      on its server and fowards message to all 1 hop neighbors.
    
     Client Dissconects
     - When client dissconects a message is sent to all other clients connected.
     d : {}
    
     Foward Message
     - Used for sending messages to clients on the connected network
     d : {
        p : "test-app"
        m : "app-msg-name"
        d : {} // app msg specific data
     }
    
    Request Topology
    - Requsts 
    
  }
*/

    CommotionSocket = function(url, protocols,opencb,proxyHost, proxyPort, headers) {
        
        /**
         * commotion-ws protocol message types
         */
        var MSG_TYPES = {
            REGISTER_CLIENT : 0,
            CLIENT_DISCONNECT : 1,
            FORWARD_MSG : 2,
            REQUEST_TOPOLOGY : 3,
            TOPOLOGY_UPDATE : 4
        };
        
        this.topologychangecb=function(){}
        
        
        //Callback map to handle all messages for applications.
        this.callbackmap = {};
        
        //
        this.tmpCallBack = null;
        
        //Only allow commotion-ws protocol.
        this.ws = new WebSocket(url,"commotion-ws",proxyHost,proxyPort,headers);
        
        var _this = this;
        // On connection
        this.ws.onopen = function(ret){
            
            // Now announce client appliaction protocols to server.
            _this._send_formated_msg(MSG_TYPES.REGISTER_CLIENT,"0.0.0.0",{"p" : protocols});
        
            _this._send_formated_msg(MSG_TYPES.REQUEST_TOPOLOGY);
        
            // If connect callback was given callback.
            if(typeof opencb == "function") opencb(ret);
            
        }
        
        // Handle all websocket data
        this.ws.onmessage = function(e){
            var json =  eval('(' + e.data + ')');
            if(json.mt == null || typeof json.mt == undefined){
                console.warn("ws.onmessage - Missing message type.");
                return;
            }
            switch(json.mt){
                case MSG_TYPES.TOPOLOGY_UPDATE: return _this.topologychangecb(json);
            }
            
//            if(typeof _this.callbackmap[json.mt] == "function" )
//                _this.callbackmap.json.mt(json);
        };
        
        this._send_formated_msg = function(mt,dst,data){
            if(dst === null) dst = "0.0.0.0";
            if(data === null) data = {};
            
            //Format message to send
            var text = JSON.stringify({"mt" : mt, "dst" : dst, "d" : data});
            this.ws.send(text);     
        }
    };  
    
    /**
     * Register message callbacks
     */
    CommotionSocket.prototype.on = function(msgtypes,cb){
        if(typeof msgtypes != "object"){
            msgtypes = [msgtypes];
        }
        
        var _this = this;
        $(msgtypes).each(function(){
            _this.callbackmap.this = cb;
        })
    }
    
    CommotionSocket.prototype.onclose = function(cb){
        this.ws.onclose = cb;
    }
    
    CommotionSocket.prototype.ontopologychange = function(cb){
        this.topologychangecb = cb;
    }
    
    
})();