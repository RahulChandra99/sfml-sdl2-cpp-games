#include <SDL.h>
#include "cleanup.h"
#include "res_path.h"
#include "drawing_functions.h"
#include "SDL_mixer.h"


int main(int argc, char **argv)
{
	//setup SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		cout << "error ini SDL" << endl;
		return 1;
	}

	//setup window
	SDL_Window* window = SDL_CreateWindow("Cyborg Battle", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		640, 352, SDL_WINDOW_SHOWN); // SDL_WINDOW_FULLSCREEN - for fullscreen

	if (window == nullptr)
	{
		SDL_Quit();
		cout << "window error" << endl;
		return 1;
	}

	//setup renderer
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}