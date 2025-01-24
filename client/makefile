CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -lpthread
all: StompEMIClient

StompEMIClient: bin/ConnectionHandler.o bin/event.o bin/Frame.o bin/EncoderDecoder.o bin/keyboardInput.o bin/StompProtocol.o bin/CommunicationThread.o bin/StompClient.o
	g++ -o bin/StompEMIClient bin/ConnectionHandler.o bin/event.o bin/Frame.o bin/EncoderDecoder.o bin/keyboardInput.o bin/StompProtocol.o bin/CommunicationThread.o bin/StompClient.o $(LDFLAGS)

bin/ConnectionHandler.o: src/ConnectionHandler.cpp include/ConnectionHandler.h
	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

bin/event.o: src/event.cpp include/event.h
	g++ $(CFLAGS) -o bin/event.o src/event.cpp

bin/Frame.o: src/Frame.cpp include/Frame.h
	g++ $(CFLAGS) -o bin/Frame.o src/Frame.cpp

bin/EncoderDecoder.o: src/EncoderDecoder.cpp include/EncoderDecoder.h
	g++ $(CFLAGS) -o bin/EncoderDecoder.o src/EncoderDecoder.cpp

bin/keyboardInput.o: src/keyboardInput.cpp include/keyboardInput.h
	g++ $(CFLAGS) -o bin/keyboardInput.o src/keyboardInput.cpp

bin/StompProtocol.o: src/StompProtocol.cpp include/StompProtocol.h
	g++ $(CFLAGS) -o bin/StompProtocol.o src/StompProtocol.cpp

bin/CommunicationThread.o: src/CommunicationThread.cpp include/CommunicationThread.h
	g++ $(CFLAGS) -o bin/CommunicationThread.o src/CommunicationThread.cpp

bin/StompClient.o: src/StompClient.cpp include/ConnectionHandler.h include/event.h include/Frame.h include/EncoderDecoder.h include/keyboardInput.h include/StompProtocol.h include/CommunicationThread.h
	g++ $(CFLAGS) -o bin/StompClient.o src/StompClient.cpp

.PHONY: clean
clean:
	rm -f bin/*
	