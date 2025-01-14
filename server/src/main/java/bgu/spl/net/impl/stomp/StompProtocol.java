package bgu.spl.net.impl.stomp;
import java.util.HashMap;

import bgu.spl.net.api.MessagingProtocol;
import bgu.spl.net.srv.Connections;


public class StompProtocol<T> implements MessagingProtocol<Frame> {

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

        //Send "CONNECTED" frame
        HashMap<String,String> headerHashMap =  new HashMap<String,String>();
        headerHashMap.put("version", "1.2");
        Frame connectedFrame = new Frame("CONNECTED",headerHashMap,"");
        connections.send(connectionId, connectedFrame);
    }
    
    @Override
    public Frame process(Frame msg){

        return new Frame();
    }

    @Override
    public boolean shouldTerminate(){
        return shouldTerminate;
    }

}
