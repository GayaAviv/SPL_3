package bgu.spl.net.srv;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;


import bgu.spl.net.impl.stomp.Subscriber;


public class ConnectionsImpl<T> implements Connections<T> {

    private Map<Integer, ConnectionHandler<T>> connectionHandlers; //The client IDs with their respective connection handlers
    private Map<String, List<Subscriber>> topicSubscribers; //The topics with a list of subscribers

    private Map<Integer,String> activeClientUser; //The currently logged-in user for each clientId
    private List<String> activeUsers; //The currently logged-in users (across all clients)

    private Map<String,String> users; //The usernames and their corresponding passwords

    public ConnectionsImpl(){
        connectionHandlers = new HashMap<>();
        topicSubscribers = new HashMap<>();
        activeClientUser = new HashMap<>();
        activeUsers = new LinkedList<>();
        users = new HashMap<>();
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
        List<Subscriber> subscribers = topicSubscribers.get(channel);
        if (subscribers != null) {
            for (Subscriber subscriber : subscribers) {
                int connectionId = subscriber.getConnectionId();
                send(connectionId, msg);  // Use the send() method for each client (connectionId).
            }
        }
    }


    /**
    * Removes an active client connectionId from the map.
     */
    @Override
    public void disconnect(int connectionId){
        connectionHandlers.remove(connectionId); //Remove from the client-connectionId data.
        for (List<Subscriber> subscribers : topicSubscribers.values()) { // Remove the client from all the channels they are subscribed to.
            for (Subscriber s : subscribers) {
                if(s.getConnectionId() == connectionId){
                    subscribers.remove(s);
                    break;
                }
            }
        }

        String userName = activeClientUser.remove(connectionId); //Remove from connectionId-userName data.
        activeUsers.remove(userName); //Remove from the list of active users
    }

    
    public void addConnectionUser(int connectionId, String username) {
        activeClientUser.put(connectionId, username);
        activeUsers.add(username);
    }

    public void addConnectionHandler(int connectionID, ConnectionHandler<T> connectionHandler) {
        connectionHandlers.put(connectionID, connectionHandler);
    }

    public boolean addSubscriber(String topic, int connectionId, String subscriptionId) {
        int subscribeID = Integer.parseInt(subscriptionId);

        if(topicSubscribers.containsKey(topic)){ //In case topic already exist
            List<Subscriber> subscribers = topicSubscribers.get(topic);
            Subscriber newSub = new Subscriber(connectionId, subscribeID);
            return subscribers.add(newSub);
        }

        else{ //In case of new topic
            Subscriber newSub = new Subscriber(connectionId, subscribeID);
            List<Subscriber> topicSub = new LinkedList<>();
            topicSub.add(newSub);
            topicSubscribers.put(topic, topicSub);
            return true;
        }
    }

    
    public boolean removeSubscriber(int connectionId, String subscriptionId) {
        int subscribeID = Integer.parseInt(subscriptionId);
        
        boolean found = false;
        for (List<Subscriber> subscribers : topicSubscribers.values()) { 
            for (Subscriber s : subscribers) {
                if(s.getConnectionId() == connectionId && s.getSubscribeId() == subscribeID){
                    subscribers.remove(s);
                    found = true;
                    break;
                }
            }

            if(found){
                return found;
            }
        }

        return found;
    }
       
    public int isSubscribe(String topic, int connectionId) {
        List<Subscriber> subscribers = topicSubscribers.get(topic);
        for (Subscriber s : subscribers){
            if(s.getConnectionId() == connectionId)
                return s.getSubscribeId();
        }
        return -1;
    }

    public Map<Integer,ConnectionHandler<T>> getConnectionHandlers(){
        return connectionHandlers;
    }

    public Map<String,String> getUsers(){
        return users;
    }

    public Map<Integer,String> getActiveClientUser(){
        return activeClientUser;
    }

    public List<String> getActiveUsers() {
       return activeUsers;
    }
}
