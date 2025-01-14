package bgu.spl.net.impl.stomp;
import bgu.spl.net.api.MessagingProtocol;
import bgu.spl.net.srv.Connections;


public class StompProtocol<T> implements MessagingProtocol<Frame> {

    //Fileds
    private boolean shouldTerminate = false;


    //Methods
    @Override
    public void start(int connectionId, Connections<Frame> connections){

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
