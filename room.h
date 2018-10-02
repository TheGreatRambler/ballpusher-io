class BallRoom {
  private:
    double getPercentFull();
    int getNumOfPlayers();
    std::vector<std::unordered_map<std::string, auto>> players;
  public:
    void addPlayer();
}
