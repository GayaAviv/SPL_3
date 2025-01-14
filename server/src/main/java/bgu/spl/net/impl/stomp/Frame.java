package bgu.spl.net.impl.stomp;

import java.util.HashMap;
import java.util.Map;


public class Frame {

    private String command;
    private Map<String, String> headers;
    private String body;

    public Frame(String command, Map<String, String> headers, String body) {
        this.command = command;
        this.headers = headers != null ? new HashMap<>(headers) : new HashMap<>();
        this.body = body;
    }

    public Frame() {
        this.headers = new HashMap<>();
    }

    public String getCommand() {
        return command;
    }

    public void setCommand(String command) {
        this.command = command;
    }

    public Map<String, String> getHeaders() {
        return headers;
    }

    public void addHeader(String key, String value) {
        headers.put(key, value);
    }

    public void removeHeader(String key) {
        headers.remove(key);
    }

    public String getBody() {
        return body;
    }

    public void setBody(String body) {
        this.body = body;
    }

    @Override
    public String toString() {
        StringBuilder frame = new StringBuilder(command + "\n");
        headers.forEach((key, value) -> frame.append(key).append(":").append(value).append("\n"));
        frame.append("\n").append(body != null ? body : "").append("\u0000");
        return frame.toString();
    }

}
