#include <libs\uws\uWS.h>
// this will be filled out later

int main(int argc, char *argv[]) {
	// start
	uWS::Hub websocketHub;
	
	websocketHub.onMessage([](uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode opCode) {
        ws->send(message, length, opCode);
    });

    if (websocketHub.listen(argv[1])) { // argv[1] is port
        websocketHub.run();
    }
	
	//std::vector<std::unordered_map<std::string, auto>> rooms;// auto for now
}
