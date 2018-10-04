#include <libs\uws\uWS.h>
#include room.h

Room::getNumOfPlayers() {
  return numOfPlayers;//will change
}

Room::getMaxNumOfPlayers() {
  return maxNumOfPlayers;//will change
}

Room::addPlayer(uWS::WebSocket<uWS::CLIENT> websocket) {
  std::unordered_map<std::string, auto> player = {
    {"websocket", websocket}
  };
  numOfPlayers += 1;
  players.push_back(player);
}

Room::isFull() {
  return (getNumOfPlayers() <= getMaxNumOfPlayers());
}
