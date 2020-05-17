#include "game.h"
#include <iostream>
#include "SDL.h"
#include <chrono>

std::vector<std::thread> threads;

Game::Game(std::size_t Screen_Width, std::size_t Screen_Height, std::size_t grid_width, std::size_t grid_height)
    : snake(grid_width, grid_height),
      engine(dev()),
       //[AH]: populating the unique pointer with an instance of the Renderer
      renderer(new Renderer(Screen_Width, Screen_Height, grid_width, grid_height)),
      random_w(0, static_cast<int>(grid_width)),
      random_h(0, static_cast<int>(grid_height)) {
  PlaceFood();
}
Game::~Game()
{
  //[AH]: joining back the treads to before closing the program
  std::for_each(threads.begin(), threads.end(), [](std::thread &t) {
        t.join();
    });
}
void Game::Run(std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;
   //[AH]: starting the controller thread
  controller.Simulate();
  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    //[AH] removed the control polling and changing it to poll for recieving input from the control thread
    HandleControl(running,controller.GetControl());
    Update();
    //[AH]: changing the render to adapt with the new unique pointer for the render
    renderer->Render(snake, food);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer->UpdateWindowTitle(score, frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

void Game::PlaceFood() {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (!snake.SnakeCell(x, y)) {
      food.x = x;
      food.y = y;
      return;
    }
  }
}

void Game::Update() {
  if (!snake.alive) return;

  snake.Update();

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  // Check if there's food over here
  if (food.x == new_x && food.y == new_y) {
    score++;
    PlaceFood();
    // Grow snake and increase speed.
    snake.GrowBody();
    snake.speed += 0.02;
  }
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.size; }
/*
  [AH]: Adding function to change the direction of the snake after recieving the signal from the controlled thread 
*/
void Game::HandleControl(bool & running, Snake::Direction input) {
  Snake::Direction opposite;
  if (input == Snake::Direction::kQuit)
  {
    running = false;
  }else if (input == Snake::Direction::kNon)
  {
    // No input recieved yet continue
  }
  else
  {
    switch (input) 
    {
      case Snake::Direction::kUp:
        opposite = Snake::Direction::kDown;
        break;

      case Snake::Direction::kDown:
        opposite = Snake::Direction::kUp;
        break;

      case Snake::Direction::kLeft:
        opposite = Snake::Direction::kRight;
        break;

      case Snake::Direction::kRight:
        opposite = Snake::Direction::kLeft;
        break;
    }
    if (snake.direction != opposite || snake.size == 1) snake.direction = input;
  }
  return;
}