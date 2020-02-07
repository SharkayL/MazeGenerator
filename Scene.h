#pragma once
#include "SDL.h"
class Scene
{
public:
	Scene();
	~Scene();
	//virtual void HandleEvents();
	virtual void Update();
	virtual void Render(SDL_Renderer *renderer);
};

