package bgu.spl.net.srv;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;


public class ConnectionsImpl<T> implements Connections<T> {

    private Map<Integer, ConnectionHandler<T>> connectionHandlers; //The clients
    private Map<String, List<Integer>> topicSubscribers; //The topics with a list of clients that are registered to them.

    public ConnectionsImpl(){
        connectionHandlers = new HashMap<>(); //TODO: ?סנכרון
        topicSubscribers = new HashMap<>();
    }
    
    /**
    * Sends a message T to client represented by the given connectionId.
    * @return True if the message was sent successfully.
     */
    @Override
    public boolean send(int connectionId, T msg){
        ConnectionHandler<T> handler = connectionHandlers.get(connectionId);
        if (handler != null) {
            handler.send(msg);  //Call the send() method of the ConnectionHandler.
            return true;
        }
        return false;
    }

    /**
    * Sends a message T to clients subscribed to channel.
     */
    @Override
    public void send(String channel, T msg){ 
        List<Integer> subscribers = topicSubscribers.get(channel);
        if (subscribers != null) {
            for (int connectionId : subscribers) {
                send(connectionId, msg);  //Use the send() method according to the specific client.
            }
        }
    }


    /**
    * Removes an active client connectionId from the map.
     */
    @Override
    public void disconnect(int connectionId){
        connectionHandlers.remove(connectionId); //Remove from the clients list.

        for (List<Integer> subscribers : topicSubscribers.values()) { // Remove the client from all the channels they are subscribed to.
            subscribers.remove(Integer.valueOf(connectionId));
        }

    }



}
