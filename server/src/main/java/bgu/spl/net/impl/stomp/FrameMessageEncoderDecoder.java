package bgu.spl.net.impl.stomp;

import java.nio.charset.StandardCharsets;
import java.util.HashMap;
import java.util.Map;

import bgu.spl.net.api.MessageEncoderDecoder;

public class FrameMessageEncoderDecoder<T> implements MessageEncoderDecoder<Frame> {

    private StringBuilder buffer = new StringBuilder();

    @Override
    public Frame decodeNextByte(byte nextByte){

        if (nextByte == '\u0000') {
            String frameString = buffer.toString().trim();
            buffer.setLength(0); // Reset the buffer
            return parseFrame(frameString); // Parse the complete frame and return it
        } else {
            buffer.append((char) nextByte);
            return null; // Return null since the frame is still incomplete
        }

    }

    @Override
    public byte[] encode(Frame message){
        // Convert the Frame object to a string and encode it as UTF-8 bytes
        return message.toString().getBytes(StandardCharsets.UTF_8); 
    }


     private Frame parseFrame(String frameString) {
        
        String[] lines = frameString.split("\n");
        String command = lines[0]; // Extract the command from the first line

        // Parse the headers, which are key-value pairs following the command
        Map<String, String> headers = new HashMap<>();
        int i = 1;
        while (i < lines.length && !lines[i].isEmpty()) {
            String[] headerParts = lines[i].split(":", 2);
            if (headerParts.length == 2) {
                headers.put(headerParts[0], headerParts[1]);
            }
            i++;
        }

        // Parse the body of the frame, which starts after the empty line
        StringBuilder body = new StringBuilder();
        for (int j = i + 1; j < lines.length; j++) {
            body.append(lines[j]).append("\n");
        }

        // Return the parsed Frame
        return new Frame(command, headers, body.toString().trim());
    }

}
