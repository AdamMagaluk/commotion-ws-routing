(function() {
  
    CommotionSocket = function(url, protocols, proxyHost, proxyPort, headers) {
        if (!protocols) {
            protocols = ["commotion-ws"];
        } else if (typeof protocols == "string") {
            protocols = ["commotion-ws",protocols];
        }else if (typeof protocols == "array"){
            protocols.push("commotion-ws");
        }
        this.ws = new WebSocket(url,protocols,proxyHost,proxyPort,headers);
    };
  
  
})();