var ws_server = "ws://"+window.location.hostname+":7681";

$(document).ready(function(){
    
    // Create a application specific data
    var appData = {
        clients : []
    };
    
    var cws = new CommotionSocket(ws_server,["chat-server","other-app"],function(){
        $("#connected").removeClass("red");
        $("#connected").addClass("green");
        $(".showOnConnect").show();
    });
    
    cws.onclose(function(){
        $("#connected").removeClass("green");
        $("#connected").addClass("red");
        $(".showOnConnect").hide();
    });
    
    cws.ontopologychange(function(msg){
       
        var cliientList = $("#clients ul"); 
        $(cliientList).html("");
        appData.clients=[];
        appData.activeClient = -1;
        var i=0;
        $(cliientList).append('<li id="'+(i++)+'">All Clients</li>');
        cws.forclients(function(client){
            appData.clients.push(client);
            $(cliientList).append('<li id="'+(i++)+'">'+client.hostname+"-"+client.id+'</li>');
        });
          
    });
        
    $($("#clients li")).live("click",function(id){
        var idx = $(this).attr('id');
        appData.activeClient = idx;
        $(this).addClass("active");
        $(this).siblings().removeClass("active");
    });
    
    cws.on(["norm","error"],function(ret){
        var idx=0;
        cws.forclients(function(client){
          if(ret.src.address.equals(client.address)){
                $('.chatwindow').append('<li class='+ret.message_type+'><b>'+client.hostname+'</b> - '+ret.data+'</li>');
                return;
          }
          idx++;
        });
    })

    $("#send-msg").click(function(){
        if(appData.activeClient == 0) {
            cws.broadcast($("#message-type").val(),$(msg).val());
        }else{
            cws.send(appData.clients[appData.activeClient].address,$("#message-type").val(),$(msg).val());
        }
        
        $(msg).val("");
    });

})
