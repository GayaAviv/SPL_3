package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.MessageEncoderDecoder;

public class FrameMessageEncoderDecoder implements MessageEncoderDecoder<Frame> {


    public Frame decodeNextByte(byte nextByte){

        return new Frame();

    }

    /**
     * encodes the given message to bytes array
     *
     * @param message the message to encode
     * @return the encoded bytes
     */
    public byte[] encode(Frame message){
        return (message + "\n").getBytes(); //uses utf8 by default

    }

}
