#pragma once
#include "SDL.h"
#include "Maze.h"

const int FPS = 60;
const int frameDelay = 1000 / FPS;
class App
{
public:
	App();
	~App();
	void Init(const char* title, int xPos, int yPos, int width, int height);
	void HandleEvents();
	void Update();
	void Render();
	void Clean();

	bool running() { return isRunning; };
	Scene *currentScene;
private:
	bool isRunning;
	SDL_Window *window;
	SDL_Renderer *renderer;
};

