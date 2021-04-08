build: bin folders green

folders:
	mkdir -p src/Green/Files/Logs
	mkdir -p src/Orange/Files/Logs
	mkdir -p src/Blue/Files/Logs
	mkdir -p src/PinkNode/Files/Logs

bin:
	mkdir -p bin

green: src/Green/OrangeAgent.cpp src/Green/OrangeAgent.h src/Green/GreenNode.h src/Green/GreenNode.cpp src/Green/main.cpp

	g++ -Wall -Wextra -std=c++11 -o \
	bin/testGreenNode \
	src/Green/main.cpp \
	src/Green/OrangeAgent.cpp \
	src/Green/GreenNode.cpp \
	src/Green/LinkLayer.cpp \
	src/Green/PackagesDispatcher.cpp \
	src/Green/Transmitter.cpp \
	src/Green/DgramListener.cpp \
	src/Green/TransmittersController.cpp \
	src/Green/DgramTalker.cpp \
	src/Green/Log.cpp \
	src/Green/BlueAgent.cpp \
	src/Green/BlueProducer.cpp \
	src/Green/BlueConsumer.cpp \
	src/Green/Visitor.cpp \
	src/Green/StatusTable.cpp \
	src/Green/RespondHeartbitRequest.cpp \
	src/Green/Heartbeat.cpp \
	src/Green/HeartbitStateReviewer.cpp \
	src/Green/fromOtherNodesMiniDispatcher.cpp \
	src/common/* \
	src/Green/Payloads/*.cpp \
	src/Green/NetworkLayer/*.cpp \
	src/Green/PinkAgent.cpp \
	src/Green/PinkProducer.cpp \
	src/Green/PinkConsumer.cpp \
	src/Green/TcpSocketWrapper.cpp \
	src/Green/NeighborsTableDeliver.cpp \
	-pthread

run: build
	/bin/program

clean:
	rm -rf bin
