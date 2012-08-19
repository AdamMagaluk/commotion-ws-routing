(function() {
  
    /*
  {
      //Protocol message type used by backend service
      "mt" : "XXXX",

      // Destination Address
      "dst" : {ip:0xff2222, id:1},
      
      // Source Address
      "src" : {ip:0xff2222, id:1},
      
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
     
     Forward Message
     - Used for sending messages to clients on the connected network
     d : {
        t : "app-msg-name"
        d : {} // app msg specific data
     }
    
  }
*/

    /**
    * commotion-ws protocol message types
    */   
    var MSG_TYPES = {
        REGISTER_CLIENT : 0,
        CLIENT_DISCONNECT : 1,
        FORWARD_MSG : 2,
        REQUEST_TOPOLOGY : 3,
        TOPOLOGY_UPDATE : 4,
        BROADCAST_MSG : 5
    };
    
    
    
    CommotionSocket = function(url, protocols,opencb,proxyHost, proxyPort, headers) {
        
        var _this = this;

        // Latest topololgy from server
        this.topology={};
        
        // Callback for when topology changes
        this.topologychangecb={
            localcallback : function(top){
                // Update topology
                _this.topology = top;
                // call users callback
                _this.topologychangecb.userCallback(_this.topology);
            },
            userCallback : function(){}
        };
        
        
        //Callback map to handle all messages for applications.
        this.callbackmap = {};

        //Only allow commotion-ws protocol.
        this.ws = new WebSocket(url,"commotion-ws",proxyHost,proxyPort,headers);
        
        // On connection
        this.ws.onopen = function(ret){

            //  Give server a bit of time to settle, not neccessary but ill leave it.
            setTimeout(function(){
                // Now announce client appliactions to server.
                _this._send_formated_msg(MSG_TYPES.REGISTER_CLIENT,undefined,{
                    "p" : protocols
                });
            },100);
            
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
                case MSG_TYPES.TOPOLOGY_UPDATE:
                    return _this.topologychangecb.localcallback(json.d);
                case MSG_TYPES.FORWARD_MSG:
                case MSG_TYPES.BROADCAST_MSG:
                    return _this._handle_forward_msg(json);
            }
        };
        
        this._send_formated_msg = function(mt,dst,data){
            var dest;
            if(dst !== undefined && "ip" in dst && "id" in dst){
                dest={
                    ip:Number(dst.ip),
                    id:Number(dst.id)
                };
            }else{
                dest={
                    ip:0,
                    id:0
                };
            }
            
            if(data === null) data = {};
            
            //Format message to send
            var text = JSON.stringify({
                "mt" : mt, 
                "dst" : dest, 
                "d" : data
            });
            this.ws.send(text);     
        }
        
        this.find_client = function(addr,cb){
            this.forclients(function(c){
                if(c.addr === addr.addr && c.id === addr.id){
                    cb(c);
                }
            })
            return null;
        }
        
        this._handle_forward_msg = function(msg){ 
            
            if(msg !== undefined && "d" in msg && "dst" in msg && "src" in msg){
                var data = msg.d;
                var _this = this;
                
                this.find_client(msg.src,function(source){
                    
                    if(source == null){
                        console.warn("Received message from a client not in local topology\n");
                        return;
                    }
                    
                    if(data !== undefined && "t" in data){
                        var messageType = data.t;
                        var ret = {
                            src : source,
                            message_type : messageType,
                            data : data.d
                        };
                        if(_this.callbackmap[messageType] !== undefined && typeof _this.callbackmap[messageType] == "function"){
                            _this.callbackmap[messageType](ret);
                        }
                    }else{
                        console.error("_handle_forward_msg - Data undefined or no data.d")
                    }   
                });
            }else{
                console.error("_handle_forward_msg - Does not have d or dst")
            }
        }
    };  
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    /**
     * Register application message callbacks
     */
    CommotionSocket.prototype.on = function(msgtypes,cb){
        if(typeof msgtypes != "object"){
            msgtypes = [msgtypes];
        }
        
        var _this = this;
        $(msgtypes).each(function(){
            _this.callbackmap[this] = cb;
        })
    }
    
    
    CommotionSocket.prototype.onclose = function(cb){
        this.ws.onclose = cb;
    }
    
    /**
     * Supply a callback for when the network topology changes
     */
    CommotionSocket.prototype.ontopologychange = function(cb){
        this.topologychangecb.userCallback = cb;
    }
    
    
    /**
     * Send application message to node.
     * Node - Has to be in the format, {ip:"address",id:"id"}
     * Message Type - Specific application message name
     * Message Data - Data
     * 
     * send({ip:"101.1.1.1",id:0},"demomessage","Hello")
     */
    CommotionSocket.prototype.send = function(node,msgtype,msgdata){
        var dest;
        if(node !== undefined && "ip" in node && "id" in node){
            dest={
                ip:Number(node.ip),
                id:Number(node.id)
            };
        }else{
            console.error("CommotionSocket::send - Node paramater not formated correctly.");
        }
        
        var messagetype;
        if(messagetype !== undefined || typeof msgtype !== "string"){
            console.error("CommotionSocket::send - Message type not a string.");
        }else{
            messagetype = msgtype;
        }
        
        var data = {
            t:null,
            d:null
        };
        if(msgdata === undefined){
            data.d = "";
        }
        
        data.d = msgdata;
        data.t = messagetype;
        
        this._send_formated_msg(MSG_TYPES.FORWARD_MSG,dest,data);
        
    }
    
    /**
     * Send application message to node.
     * Node - Has to be in the format, {ip:"address",id:"id"}
     * Message Type - Specific application message name
     * Message Data - Data
     * 
     * send({ip:"101.1.1.1",id:0},"demomessage","Hello")
     */
    CommotionSocket.prototype.broadcast = function(msgtype,msgdata){
        var dest={
                ip:Number(0),
                id:Number(0)
            };
        
        var messagetype;
        if(messagetype !== undefined || typeof msgtype !== "string"){
            console.error("CommotionSocket::send - Message type not a string.");
        }else{
            messagetype = msgtype;
        }
        
        var data = {
            t:null,
            d:null
        };
        if(msgdata === undefined){
            data.d = "";
        }
        
        data.d = msgdata;
        data.t = messagetype;
        
        this._send_formated_msg(MSG_TYPES.BROADCAST_MSG,dest,data);
        
    }
    
    
    
    /**
     * Run supplied callback on all clients within all access points.
     */
    CommotionSocket.prototype.forclients = function(cb){
        $(this.topology.aps).each(function(){
            var ap = this.addr;
            $(this.clients).each(function(){
                this.address = {
                    ip : this.addr,
                    id:this.id
                };
                this.ap_host = ap;
                cb(this);
            });
        });
    }
    
    
    
    
    Object.prototype.equals = function(x)
    {
        var p;
        for(p in this) {
            if(typeof(x[p])=='undefined') {
                return false;
            }
        }

        for(p in this) {
            if (this[p]) {
                switch(typeof(this[p])) {
                    case 'object':
                        if (!this[p].equals(x[p])) {
                            return false;
                        }
                        break;
                    case 'function':
                        if (typeof(x[p])=='undefined' ||
                            (p != 'equals' && this[p].toString() != x[p].toString()))
                            return false;
                        break;
                    default:
                        if (this[p] != x[p]) {
                            return false;
                        }
                }
            } else {
                if (x[p])
                    return false;
            }
        }

        for(p in x) {
            if(typeof(this[p])=='undefined') {
                return false;
            }
        }

        return true;
    }
    
})();