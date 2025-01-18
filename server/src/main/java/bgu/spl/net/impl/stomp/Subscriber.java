package bgu.spl.net.impl.stomp;

public class Subscriber {
    private int connectionId;
    private int subscribeId;

    public Subscriber(int connectionId, int subscribeId){
        this.connectionId = connectionId;
        this.subscribeId = subscribeId;
    }

    public int getConnectionId(){
        return connectionId;
    }

    public int getSubscribeId(){
        return subscribeId;
    }
}
