package bgu.spl.net.srv;

import java.util.Map;

public interface Connections<T> {

    boolean send(int connectionId, T msg);

    void send(String channel, T msg);

    void disconnect(int connectionId);

    int getNextID();

    Map<Integer,ConnectionHandler<T>> getConnectionHandlers();
}
