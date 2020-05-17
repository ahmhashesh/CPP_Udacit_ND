#ifndef GAME_H
#define GAME_H

#include <random>
#include <thread>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <vector>
#include <algorithm>

#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"

class Game {
 public:
  Game(std::size_t Screen_Width, std::size_t Screen_Height, std::size_t grid_width, std::size_t grid_height);
  ~Game();
  void Run( std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize() const;
  
 private:
  Snake snake;
  Controller controller;
  std::unique_ptr<Renderer> renderer;
  SDL_Point food;
  
  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  int score{0};

  void PlaceFood();
  void Update();
  void HandleControl(bool & running,Snake::Direction input);
};

#endif