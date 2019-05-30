//==============================================================================
/*
	Texture Bank class for loading multiple textures

	3/18/2014
    SDLTutorials.com
    Tim Jones
*/
//==============================================================================
#ifndef __TEXTUREBANK_H__
#define __TEXTUREBANK_H__

#include <map>
#include <string>
#include <vector>
#include <SDL_image.h>
#include <SDL_ttf.h>

//#include "Log.h"
//#include "Game.h"

using namespace std;

//============================================================
//类名称：    TextureManager
//说明：      纹理管理器类
//            旧版的map数据成员为map<std::string, Texture*>类型;
//日期：      2018-09-14
//修改记录：  弃用Texture类，
//============================================================
class TextureManager 
{
public:
	static TextureManager* Instance()
	{
		if (pInstance == NULL)
		{
			pInstance = new TextureManager();
		}
		return pInstance;
	}

	bool load(string fileName, string id, SDL_Renderer* pRenderer);

	// load image and text    
	bool AddImage(SDL_Renderer* Renderer, std::string ID, std::string Filename);//加载图片到TextureManager中  
	bool AddText(SDL_Renderer* Renderer, std::string ID, std::string Filename);//加载文本到TextureManager中
	bool LoadFromRenderedText(SDL_Renderer* Renderer, std::string ID, std::string textureText, SDL_Color textColor); //addText的子函数，也可以单独使用，如在Game中

	// Free up resources
	//void CleanUp();

	// render
	void draw(string id, int x, int y, int width, int
		height, SDL_Renderer* pRenderer, SDL_RendererFlip flip =
		SDL_FLIP_NONE);
	void drawClip(string id, int x, int y, int diameter, SDL_Renderer* pRenderer, SDL_Rect *clip /*src_clip*/, SDL_RendererFlip flip = SDL_FLIP_NONE);//add by clw，专为含有4个圆的精灵表所写。。
	void drawFrame(string id, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer*
		pRenderer, SDL_RendererFlip flip = SDL_FLIP_NONE); 
	void drawFrameEx(string id, int src_width, int src_height,
		int dest_x, int dest_y, int dest_width, int dest_height,
		int currentRow, int currentFrame, SDL_Renderer*
		pRenderer, SDL_RendererFlip flip = SDL_FLIP_NONE);

	void RenderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int diameter, SDL_Rect *clip /*src_clip*/ );  //clw modify 20180819
	
	void clearFromTextureMap(std::string id);

	//=========================================================
	//Get方法声明
	//=========================================================
	SDL_Texture* GetTextureFromList(std::string ID) 
	{
		if (m_textureMap.find(ID) == m_textureMap.end()) 
			return 0;
		return m_textureMap[ID];
	}

private:
	//=========================================================
	//子程式声明
	//=========================================================
	//void free();  ////GetTextureFromList rid of preexisting texture

	//=========================================================
	//数据成员声明
	//=========================================================
	std::map<std::string, SDL_Texture*> m_textureMap;

	static TextureManager* pInstance;

	TTF_Font* m_pFont = NULL;
};

typedef TextureManager TheTextureManager;

#endif
