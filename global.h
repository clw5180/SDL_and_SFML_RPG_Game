#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <SDL.h>

// 定义窗口宽高
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

extern SDL_Window *g_pWindow;
extern SDL_Renderer *g_pRenderer;
extern SDL_Texture *g_pTexture;

#endif   /* defined(__GLOBAL_H__)*/