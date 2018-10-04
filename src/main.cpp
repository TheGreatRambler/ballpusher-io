#include <libs\uws\uWS.h>
#include <room.h>
// this will be filled out later

int main(int argc, char *argv[]) {
	// start
	uWS::Hub websocketHub;
	std::vector<std::unordered_map<std::string, auto>> rooms;// auto for now
	
	// stuff in arrays in lambdas are variables used by the lambda outside the scope
	websocketHub.onConnection([rooms, Room](uWS::WebSocket<uWS::CLIENT> *ws, uWS::HttpRequest req) {
		if (static_cast<int>(rooms.size()) == 0 || rooms.back().isFull()) {
			Room* newRoom = new Room(); // pointer to room
			newRoom.addPlayer(ws);
			rooms.push_back(newRoom);
		} else {
			Rooms.back().addPlayer(ws); //  add player
		}
	});
	
	websocketHub.onMessage([](uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode opCode) {
		//auto newRoom = [&]() {
		//	// lambda
		//};
		//ws->send(message, length, opCode);
	});
	
	if (websocketHub.listen(argv[1])) { // argv[1] is port
		websocketHub.run();
	}
	
	
}
