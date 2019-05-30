/*This source code copyrighted by Lazy Foo' Productions (2004-2019)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, and strings

#include "dot.h"
#include "constant.h"
#include "texturemanager.h"
#include <SDL_ttf.h>

//clw note：全局变量在这里首次定义，因为init等一些子函数都要使用

//The window we'll be rendering to
SDL_Window* g_pWindow = NULL; 
//The window renderer
SDL_Renderer* g_pRenderer = NULL;
//Globally used font
TTF_Font *g_pFont = NULL;   //clw modify 20190122

//Rendered texture 
CTextureManager gTextTexture;    //clw modify 20190122
CTextureManager gDotTexture; //Scene textures


//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//Box collision detector
bool checkCollision( SDL_Rect a, SDL_Rect b );


bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		g_pWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (g_pWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (g_pRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(g_pRenderer, 0xFF, 0xFF, 0xFF, 0xFF); 

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}

				//Initialize SDL_ttf

				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load press texture
	//if( !gDotTexture.loadFromFile( "27_collision_detection/dot.bmp" ) )
	if (!gDotTexture.loadFromFile("dot.bmp")) //clw note
	{
		printf("Failed to load dot texture!\n");
		success = false;
	}

	/************************************************************************/
	/* clw modify 20190122
	/************************************************************************/

	//Open the font
	//g_pFont = TTF_OpenFont("16_true_type_fonts/lazy.ttf", 28);

	//g_pFont = TTF_OpenFont("simhei.ttf", 28); //clw note：设置字体样式和大小
	g_pFont = TTF_OpenFont("JDFGUYIN.TTF", 28); 
	if (g_pFont == NULL)
	{
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}
	else
	{
		//Render text
		SDL_Color textColor = { 66, 24, 0 }; //clw note：设置字体颜色
		//if (!gTextTexture.loadFromRenderedText("The dot and the wall", textColor))

		const std::string myStr = "Collision Detection：碰撞检测"; 
		//clw note：输出中文是一个难题，详见下面的loadFromRenderedText方法内部
		if (!gTextTexture.loadFromRenderedText(myStr, textColor)) //clw note：设置字体内容
		{
			printf("Failed to render text texture!\n");
			success = false;
		}
	}
	/************************************************************************/

	return success;
}

void close()
{
	//Free loaded images
	gDotTexture.free();


	gTextTexture.free();

	//Free global font
	TTF_CloseFont(g_pFont);
	g_pFont = NULL;

	//Destroy window	
	SDL_DestroyRenderer(g_pRenderer);
	SDL_DestroyWindow(g_pWindow);
	g_pWindow = NULL;
	g_pRenderer = NULL;

	//Quit SDL subsystems

	TTF_Quit();  //clw modify 20190122

	IMG_Quit();
	SDL_Quit();
}



//int main( int argc, char* args[] )
int main(int argc, char* args[])
{
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//The dot that will be moving around on the screen
			CDot dot;

			//Set the wall
			SDL_Rect wall;
			wall.x = 300;
			wall.y = 40;
			wall.w = 40;
			wall.h = 400;
			
			//While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}

					//Handle input for the dot
					dot.handleEvent( e );
				}

				//Move the dot and check collision
				dot.move( wall );

				//Clear screen
				SDL_SetRenderDrawColor( g_pRenderer, 0xc5, 0xd2, 0xb3, 0xFF ); //clw note:设置一个灰色
				SDL_RenderClear( g_pRenderer );

				//Render wall
				SDL_SetRenderDrawColor( g_pRenderer, 0x00, 0x00, 0x00, 0xFF );		
				SDL_RenderDrawRect( g_pRenderer, &wall );
			

				//Render dot
				dot.render();

				//Render current frame

				gTextTexture.render((SCREEN_WIDTH - gTextTexture.getWidth()) / 2, 0);

				//Update screen
				SDL_RenderPresent( g_pRenderer );
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}

