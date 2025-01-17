package bgu.spl.net.api;

import bgu.spl.net.srv.Connections;

public interface MessagingProtocol<T> {
    
    void start(int connectionId, Connections<T> connections);
    /**
     * process the given message 
     * @param msg the received message
     * Handle the msg property without return
     */
    void process(T msg);
 
    /**
     * @return true if the connection should be terminated
     */
    boolean shouldTerminate();
 
}