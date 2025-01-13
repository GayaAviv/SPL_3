package bgu.spl.net.impl.stomp;
import bgu.spl.net.api.MessagingProtocol;
import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;


public class StompProtocol implements StompMessagingProtocol<T> {

    private boolean shouldTerminate = false;

    public void start(int connectionId, Connections<T> connections){

    }
    

    public T process(T msg){

    }
 
    public boolean shouldTerminate(){
        return shouldTerminate;
    }

}
