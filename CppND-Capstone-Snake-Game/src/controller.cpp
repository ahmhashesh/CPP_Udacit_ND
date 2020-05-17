#include "controller.h"
#include <iostream>
#include "SDL.h"
#include "snake.h"

//[AH]: queue recieve message definition
Snake::Direction MessageQueue::receive()
{
  Snake::Direction msg;
  std::unique_lock<std::mutex> uLock(_mux);
  
 if(cond_v.wait_for(uLock,std::chrono::milliseconds(20),[this] { return !_queue.empty();}))// pass unique lock to condition variable
  {
    // remove last vector element from queue
    msg = std::move(_queue.back());
    _queue.pop_back();
    _queue.clear();
  } else 
  {
    msg = Snake::Direction::kNon;
  }
  return msg;
}
//[AH]: queue send message definition
void MessageQueue::send(Snake::Direction &&msg)
{
  std::lock_guard<std::mutex> uLock(_mux);
  // add vector to queue
  _queue.push_back(std::move(msg));
  cond_v.notify_one(); // notify client after pushing new control into vector
}
// [AH]: creating the thread 
void Controller::Simulate()  {
  kill = false;
  threads.emplace_back(std::thread(&Controller::SimThread,this));
}
// [AH]: the main control thread loop
void Controller::SimThread()
{
  while (kill == false)
  {
    HandleInput();
  }

}
//[AH]: modified the handle to addapt to the new design of multiple thread communication
void Controller::HandleInput() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      //running = false;
      SendControl(Snake::Direction::kQuit);
      kill= true;
    } else if (e.type == SDL_KEYDOWN) {
      switch (e.key.keysym.sym) {
        case SDLK_UP:
          SendControl(Snake::Direction::kUp);
          break;

        case SDLK_DOWN:
          SendControl(Snake::Direction::kDown);
          break;

        case SDLK_LEFT:
          SendControl(Snake::Direction::kLeft);
          break;

        case SDLK_RIGHT:
          SendControl(Snake::Direction::kRight);
          break;
      }
    }
  }
}
// [AH]: adding definition for the get control value method
Snake::Direction Controller::GetControl()
{
  return Queue.receive();
}
// [AH]: adding definition for the send control value method
void Controller::SendControl(Snake::Direction input)
{
  Queue.send(std::move(input));
}