package bgu.spl.net.impl.stomp;
import bgu.spl.net.srv.Server;

public class StompServer {
    public static void main(String[] args) {

        if (args.length == 0) {
            System.out.println("No arguments provided. Please specify port num and 'tpc' or 'reactor'.");
            return;
        }

        String pattern = args[1].toLowerCase();
        int portId = Integer.parseInt(args[0]);

        switch (pattern) {
            case "tpc":
                Server.threadPerClient(
                        portId, //port
                        () -> new StompProtocol<Frame>(), //protocol factory
                        FrameMessageEncoderDecoder::new //message encoder decoder factory
                ).serve();
                break;

            case "reactor":
                Server.reactor(
                        Runtime.getRuntime().availableProcessors(), //Threads num
                        portId, //port
                        () -> new StompProtocol<Frame>(), //protocol factory
                        FrameMessageEncoderDecoder::new //message encoder decoder factory
                ).serve();
            break;
        }
    }
}
