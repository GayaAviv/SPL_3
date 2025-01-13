package bgu.spl.net.impl.stomp;

import bgu.spl.net.impl.echo.EchoProtocol;
import bgu.spl.net.impl.echo.LineMessageEncoderDecoder;
import bgu.spl.net.srv.Server;

public class StompServer {
    public static void main(String[] args) {

        if (args.length == 0) {
            System.out.println("No arguments provided. Please specify port num and 'tpc' or 'reactor'.");
            return;
        }

        String pattern = args[1].toLowerCase();
        int portId = Integer.parseInt(args[1]);

        switch (pattern) {
            case "tpc":
                Server.threadPerClient(
                        portId, //port
                        () -> new StompProtocol(), //protocol factory
                        LineMessageEncoderDecoder::new //message encoder decoder factory
                ).serve();
                break;

            case "reactor":
                Server.reactor(
                        Runtime.getRuntime().availableProcessors(), //Threads num
                        portId, //port
                        () -> new StompProtocol(), //protocol factory
                    LineMessageEncoderDecoder::new //message encoder decoder factory
                ).serve();
            break;
        }
    }
}
