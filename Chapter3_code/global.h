#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <SDL.h>

//说明：通常应该尽量避免在项目中使用全局变量。
//这里只是暂时将一些对象写成全局变量的形式，保证源代码尽可能简单。
 
extern const int g_moveVelocity;

// 窗口宽高
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

// 地图宽高
extern const int MAP_WIDTH;
extern const int MAP_HEIGHT;

extern SDL_Window *g_pWindow;
extern SDL_Renderer *g_pRenderer;
extern SDL_Texture *g_pMapTexture;  
extern SDL_Texture *g_pSpriteSheetTexture;


#endif   /* defined(__GLOBAL_H__)*/