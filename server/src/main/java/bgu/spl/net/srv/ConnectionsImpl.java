package bgu.spl.net.srv;

import java.util.LinkedList;
import java.util.List;


public class ConnectionsImpl<T> implements Connections<T> {

    private List<ConnectionHandler<T>> connectionHandlers;

    public ConnectionsImpl(){
        connectionHandlers = new LinkedList<>();
    }
    
    public boolean send(int connectionId, T msg){

        return true;

    }

    public void send(String channel, T msg){

    }

    public void disconnect(int connectionId){

    }

}
