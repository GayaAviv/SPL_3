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
    private ConnectionsImpl<Frame> connections;

    private int msgID;

    //Methods

    /**
    * Initiate the protocol with the active connections structure of the
    * server and saves the owner client’s connection id.
     */
    @Override
    public void start(int connectionId, Connections<Frame> connections){

        this.connectionId = connectionId;
        this.connections = (ConnectionsImpl<Frame>) connections;
        msgID = 0;
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

        Map<String, String> headers = msg.getHeaders();
        String version = headers.get("accept-version");
        String username = headers.get("login");
        String password = headers.get("passcode");

        
        if(connections.getActiveUsers().contains(username)){ //In case the user is already logged in from another client
            sendErrorFrame(msg,"User already logged in");
        }

        else if (connections.getUsers().containsKey(username)){ //In case the userName alredy exist
            String realPassword = connections.getUsers().get(username);
            if (!realPassword.equals(password)){  //Check password
                sendErrorFrame(msg,"Wrong password");
            }
            else{ //Connection was successful
                connections.addConnectionUser(connectionId,username);
                sendConnectedFrame(version);
            }
        }

        else{ //In case the user is new and the Connection was successful
            connections.addConnectionUser(connectionId,username);
            connections.getUsers().put(username,password);
            sendConnectedFrame(version);
        }        
            
    } 

    private void handleSubscribe(Frame msg) {
        
        Map<String, String> headers = msg.getHeaders();
        String topic = headers.get("destination");
        String subscribeId = headers.get("id");
        String receiptId = headers.get("receipt");

        boolean success = connections.addSubscriber(topic, connectionId, subscribeId);
        if(success){
            sendReceiptFrame(receiptId);
        }
        else{
            sendErrorFrame(msg,"Subscribe unsuccessful");
        }
    }
               
    private void handleUnsubscribe(Frame msg) {

        Map<String, String> headers = msg.getHeaders();
        String subscribeId = headers.get("id");
        String receiptId = headers.get("receipt");

        if(connections.removeSubscriber(connectionId, subscribeId)){
            sendReceiptFrame(receiptId);
        }
        
        //TODO : לבדוק אם צריך לשלוח ארור אם לא עשה
    }

    private void handleDisconnect(Frame msg) {

        Map<String, String> headers = msg.getHeaders();
        String receiptId = headers.get("receipt");

        sendReceiptFrame(receiptId);
        shouldTerminate = true;
        connections.disconnect(connectionId);
    }

    private void handleSend(Frame msg) {
        Map<String, String> headers = msg.getHeaders();
        String topic = headers.get("destination");

        int subscribeID = connections.isSubscribe(topic, connectionId);
        if(subscribeID != -1){
            sendMessageFrame(msg, subscribeID); 
        }
        else{
            sendErrorFrame(msg, "You can't send a message to this topic, you need to subscribe first!");
        }
    }



    //FrameSend functions

    private void sendErrorFrame(Frame msg, String errorMsg) {

        Map<String, String> headers = msg.getHeaders();
        String receipt = headers.get("receipt");

        //Build the frame body
        StringBuilder body = new StringBuilder(); 
        body.append("The Message:\n");
        body.append("-----\n");
        body.append(msg.toString()).append("\n");
        body.append("-----\n");

        //Build the headers
        HashMap<String,String> headerHashMap =  new HashMap<String,String>();
        if(receipt != null){
            headerHashMap.put("receipt-id", receipt);
        }
        headerHashMap.put("message", errorMsg);

        Frame errorFrame = new Frame("ERROR",headerHashMap, body.toString());
        if(connections.send(connectionId, errorFrame)){
            shouldTerminate = true;
            connections.disconnect(connectionId);
        }
    }

    private void sendConnectedFrame(String version) {
        HashMap<String,String> headerHashMap =  new HashMap<String,String>();
        headerHashMap.put("version", version);
        Frame connectedFrame = new Frame("CONNECTED",headerHashMap,"");
        connections.send(connectionId, connectedFrame);
    }

    private void sendReceiptFrame(String receiptId) {
        HashMap<String,String> headerHashMap =  new HashMap<String,String>();
        headerHashMap.put("receipt-id", receiptId);
        Frame receiptFrame = new Frame("RECEIPT",headerHashMap,"");
        connections.send(connectionId, receiptFrame);
    }

    private void sendMessageFrame(Frame msg, Integer subscribeID) {

        Map<String, String> headers = msg.getHeaders();
        String topic = headers.get("destination");
        String body = msg.getBody();
        String subscriptionId = subscribeID.toString();

        Integer messageIdInt = msgID++;
        String messageIdString = messageIdInt.toString();

        HashMap<String,String> headerHashMap =  new HashMap<String,String>();
        headerHashMap.put("subscription", subscriptionId);
        headerHashMap.put("message-id", messageIdString);
        headerHashMap.put("destination", topic);

        Frame msgFrame = new Frame("MESSAGE",headerHashMap,body);
        connections.send(topic, msgFrame);

    }




}
