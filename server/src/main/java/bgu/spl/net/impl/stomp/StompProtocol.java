package bgu.spl.net.impl.stomp;
import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;


public class StompProtocol<T> implements StompMessagingProtocol<Frame> {

    //Fileds
    private boolean shouldTerminate = false;


    //Methods

    @Override
    public void start(int connectionId, Connections<Frame> connections){

    }
    
    @Override
    public void process(Frame msg){

    }

    @Override
    public boolean shouldTerminate(){
        return shouldTerminate;
    }

}
