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


    //Handle functions

    private void handleConnect(Frame msg) {

        ConnectionsImpl<Frame> connectionsImpl = (ConnectionsImpl<Frame>) connections;

        Map<String, String> headers = msg.getHeaders();

        String version = headers.get("accept-version");
        String username = headers.get("login");
        String password = headers.get("passcode");

        if(!connectionsImpl.getActiveClientUser().containsKey(connectionId)){ //In case the client is already logged in with the same user or another
            sendErrorFrame(msg,"The client is already logged in, log out before trying again");
        }

        else if(!connectionsImpl.getActiveUsers().contains(username)){ //In case the user is already logged in from another client
            sendErrorFrame(msg,"User already logged in");
        }

        else if (connectionsImpl.getUsers().containsKey(username)){ //In case the userName alredy exist
            String realPassword = connectionsImpl.getUsers().get(username);
            if (!realPassword.equals(password)){  //Check password
                sendErrorFrame(msg,"Wrong password");
            }
            else{ //Connection was successful
                connectionsImpl.addConnectionUser(connectionId,username);
                sendConnectedFrame(version);
            }
        }

        else{ //In case the user is new and the Connection was successful
            connectionsImpl.addConnectionUser(connectionId,username);
            connectionsImpl.getUsers().put(username,password);
            sendConnectedFrame(version);
        }        
            
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

    private void handleSend(Frame msg) {
        // TODO Auto-generated method stub
        throw new UnsupportedOperationException("Unimplemented method 'handleConnect'");
    }

    //FramSend functions

    private void sendErrorFrame(Frame msg, String string) {
        // TODO Auto-generated method stub
        throw new UnsupportedOperationException("Unimplemented method 'sendErrorFrame'");
    }

    private void sendConnectedFrame(String version) {
        HashMap<String,String> headerHashMap =  new HashMap<String,String>();
        headerHashMap.put("version", version);
        Frame connectedFrame = new Frame("CONNECTED",headerHashMap,"");
        connections.send(connectionId, connectedFrame);
    }


}
