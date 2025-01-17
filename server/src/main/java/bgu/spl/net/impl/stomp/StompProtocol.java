package bgu.spl.net.impl.stomp;
import java.util.HashMap;
import java.util.Map;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.ConnectionsImpl;


public class StompProtocol<T> implements StompMessagingProtocol<Frame> {

    //Fileds
    private boolean shouldTerminate = false;
    private int connectionId;
    private Connections<Frame> connections;

    //Methods

    /**
    * Initiate the protocol with the active connections structure of the
    * server and saves the owner client’s connection id.
     */
    @Override
    public void start(int connectionId, Connections<Frame> connections){

        this.connectionId = connectionId;
        this.connections = connections;
    }
            
            
    @Override
    public void process(Frame msg){

        String command = msg.getCommand();

        switch (command) {
            case "CONNECT":
                handleConnect(msg);
                break;
    
            case "SEND":
                handleSend(msg);
                break;
    
            case "SUBSCRIBE":
                handleSubscribe(msg);
                break;
    
            case "UNSUBSCRIBE":
                handleUnsubscribe(msg);
                break;
    
            case "DISCONNECT":
                handleDisconnect(msg);
                break;
        }
    }
  
    @Override
    public boolean shouldTerminate(){
        return shouldTerminate;
    }


    //Utility functions

    private void handleConnect(Frame msg) {

        Map<String, String> headers = msg.getHeaders();

        String host = headers.get("host");
        String username = headers.get("login");
        String password = headers.get("passcode");

        if(!connections.getConnectionHandlers().containsKey(connectionId)){
            sendErrorFrame(Frame,"The client is already logged in, log out before trying again");
        }

        

    }

    private void handleSend(Frame msg) {
        // TODO Auto-generated method stub
        throw new UnsupportedOperationException("Unimplemented method 'handleConnect'");
    }

    private void handleSubscribe(Frame msg) {
        // TODO Auto-generated method stub
        throw new UnsupportedOperationException("Unimplemented method 'handleConnect'");
    }

    private void handleUnsubscribe(Frame msg) {
        // TODO Auto-generated method stub
        throw new UnsupportedOperationException("Unimplemented method 'handleConnect'");
    }

    private void handleDisconnect(Frame msg) {
        // TODO Auto-generated method stub
        throw new UnsupportedOperationException("Unimplemented method 'handleConnect'");
    }

    private void sendConnectedFrame() {
        HashMap<String,String> headerHashMap =  new HashMap<String,String>();
        headerHashMap.put("version", "1.2");
        Frame connectedFrame = new Frame("CONNECTED",headerHashMap,"");
        connections.send(connectionId, connectedFrame);
    }


}
