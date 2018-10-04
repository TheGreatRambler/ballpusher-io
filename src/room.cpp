#include <libs\uws\uWS.h>
#include room.h
#include <helpers/util.h>

Room::getNumOfPlayers() {
  return numOfPlayers;
}

Room::getMaxNumOfPlayers() {
  return maxNumOfPlayers;
}

Room::addPlayer(uWS::WebSocket<uWS::CLIENT> *websocket) {
  std::string initText = roomId;
  sendSocketText(websocket, initText);
  std::unordered_map<std::string, auto> player = {
    {"websocket", websocket} // pointer to websocket
  };
  numOfPlayers += 1;
  players.push_back(player);
}

Room::isFull() {
  return (getNumOfPlayers() <= getMaxNumOfPlayers());
}
