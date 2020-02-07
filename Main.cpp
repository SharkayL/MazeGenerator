#define SDL_MAIN_HANDLED
#include "App.h"

int main() {
	App *app = new App();
	Uint32 frameStart;
	int frameTime;
	while (app->running())
	{
		frameStart = SDL_GetTicks();
		app->HandleEvents();
		app->Update();
		app->Render();
		frameTime = SDL_GetTicks() - frameStart;
		if (frameDelay > frameTime) {
			SDL_Delay(frameDelay - frameTime);
		}
	}
	app->Clean();

	return 0;
}