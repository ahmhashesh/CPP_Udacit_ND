#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <thread>
#include "snake.h"
#include <thread>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <vector>
#include <algorithm>


extern std::vector<std::thread> threads;

class MessageQueue
{
public:
Snake::Direction receive();
void send(Snake::Direction &&msg);
private:
    std::condition_variable cond_v;
    std::mutex _mux;
    std::deque<Snake::Direction> _queue;

};

class Controller {
 public:
  void HandleInput();
  void Simulate();
  void SendControl(Snake::Direction input);
  Snake::Direction GetControl();

 private:
  void SimThread();
  //[AH]: Adding message Queue for receiving the control data from the Controller object
  MessageQueue Queue;
  bool kill;
  
};

#endif