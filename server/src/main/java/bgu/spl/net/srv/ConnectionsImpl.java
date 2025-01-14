package bgu.spl.net.srv;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;


public class ConnectionsImpl<T> implements Connections<T> {

    private Map<Integer, ConnectionHandler<T>> connectionHandlers; //The clients
    private Map<String, List<Integer>> topicSubscribers; //The topics with a list of clients that register to a spesific topic

    public ConnectionsImpl(){
        connectionHandlers = new HashMap<>(); //TODO: סנכרון
        topicSubscribers = new HashMap<>();
    }
    
    @Override
    public boolean send(int connectionId, T msg){
        return true;
    }

    @Override
    public void send(String channel, T msg){ 

    }

    @Override
    public void disconnect(int connectionId){

    }

}
