#include "App.h"
#include <iostream>

App::App()
{
	Init("Maze Generator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 1000);
	currentScene = new Maze(20, 20);
}


App::~App()
{
}

void App::Init(const char* title, int xPos, int yPos, int width, int height) {
	if(SDL_Init(SDL_INIT_EVERYTHING)==0){
		std::cout << "Subsystems Initialised!..." << std::endl;
		window = SDL_CreateWindow(title, xPos, yPos, width, height, 0);
		if (window) {
			std::cout << "Window Created!..." << std::endl;
		}
		renderer = SDL_CreateRenderer(window, -1, 0);
		if (renderer) {
			std::cout << "Renderer Created!..." << std::endl;
		}
		isRunning = true;
	}
	else isRunning = false;
}

void App::HandleEvents(){
	SDL_Event event;
	SDL_PollEvent(&event);
	switch (event.type) {
	case SDL_QUIT:
		isRunning = false;
		break;
	}
}

void App::Update(){
	currentScene->Update();
}

void App::Render(){
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	currentScene->Render(renderer);
	SDL_RenderPresent(renderer);
}

void App::Clean(){
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	std::cout << "Cleaned!..." << std::endl;
}

