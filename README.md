# Snake Game

A classic Snake Game implemented in C++ as a console-based game.

This project was built to practice C++ programming fundamentals, object-oriented programming, game loop design, keyboard input, collision detection, and basic data structures.

## Features

* Console-based Snake Game
* WASD keyboard controls
* Food spawning at random positions
* Snake growth when food is collected
* Score system
* Wall collision detection
* Self-collision detection
* Game Over state
* Adjustable game speed

## Controls

| Key | Action        |
| --- | ------------- |
| `W` | Move Up       |
| `A` | Move Left     |
| `S` | Move Down     |
| `D` | Move Right    |
| `X` | Exit the game |

The game prevents the snake from immediately moving in the opposite direction.

## How It Works

The game is organized around a `SnakeGame` class that manages the main game state, including:

* Snake head position
* Snake body
* Fruit position
* Current direction
* Score
* Game Over state

The snake body is stored using:

```cpp
vector<Point> tail;
```

Each game iteration follows the basic loop:

```text
Draw
  ↓
Input
  ↓
Logic
  ↓
Collision Check
  ↓
Repeat
```

The game uses keyboard input to update the snake's direction, moves the snake, checks whether food has been collected, updates the body, and then checks for collisions.

## Project Structure

```text
SnakeGame/
│
├── docs/
│   └── screenshots/
│
├── src/
│   └── snake_game.cpp
│
├── .gitignore
└── README.md
```

## Requirements

* Windows
* C++ compiler
* Console/terminal

This project currently uses Windows-specific libraries such as:

```cpp
#include <conio.h>
#include <windows.h>
```

Therefore, the current implementation is intended for Windows.

## Build and Run

Using `g++`:

```bash
g++ src/snake_game.cpp -o SnakeGame.exe
```

Run:

```powershell
.\SnakeGame.exe
```

If you are using Visual Studio, Dev-C++, or another C++ IDE, you can also compile and run the project directly through the IDE.

## Screenshots

Screenshots of the game will be stored in:

```text
docs/screenshots/
```

## What I Practiced

This project helped me practice:

* C++ classes and encapsulation
* `struct`
* `enum`
* `vector`
* Functions and member functions
* Loops and conditional statements
* Keyboard input
* Random number generation
* Collision detection
* Game loop structure
* Git and GitHub workflow

## Future Improvements

Planned improvements include:

* Improve the console interface
* Add difficulty levels
* Add a high-score system
* Improve code organization
* Add pause/restart functionality
* Refactor the game into multiple source/header files
* Experiment with pathfinding and game AI
* Explore autonomous Snake behavior

## Author

**Phan Quoc Huy**

Information Technology Student interested in **Game AI Programming**.

GitHub: `huypq020607-code`
