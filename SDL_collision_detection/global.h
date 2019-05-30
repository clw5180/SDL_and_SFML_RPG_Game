#pragma once

#include <SDL_image.h>
#include <SDL_ttf.h> //clw modify 20190122

//The window we'll be rendering to
extern SDL_Window *g_pWindow;

//The window renderer
extern SDL_Renderer *g_pRenderer;

//Globally used font
extern TTF_Font *g_pFont;   //clw modify 20190122
						 
