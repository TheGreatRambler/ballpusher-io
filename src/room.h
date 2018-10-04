#pragma once

class Room {
  private:
    double getPercentFull();
    int getNumOfPlayers();
    int numOfPlayers = 0;
    int maxNumOfPlayers = 30;
    std::vector<std::unordered_map<std::string, auto>> players;
  public:
    bool isFull();
    void addPlayer();
}
