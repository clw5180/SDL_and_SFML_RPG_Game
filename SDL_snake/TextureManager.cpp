//=============================================================================
#include "TextureManager.h"
#include "Game.h"
#include "Log.h"

//=============================================================================
std::map<std::string, SDL_Texture*> m_textureMap;
TextureManager* TextureManager::pInstance = NULL;

//=============================================================================

//void TextureManager::CleanUp() 
//{
//	//clw note 20180824 
//	//教程里的思路是这样的，把image和文字分开
//	//Free loaded images
//	//gTextTexture.free();
//
//	//Free global font
//	//TTF_CloseFont(gFont);
//	//gFont = NULL;
//
//	if(m_textureMap.size() <= 0) 
//		return;
//
//	for(auto& Iterator : m_textureMap)
//	{
//		//clw note 20180913：对于一个map对象，(*it).first会得到key，(*it).second会得到value
//		SDL_Texture* TheTexture = (SDL_Texture*)Iterator.second;
//		if(TheTexture) 
//		{
//			delete TheTexture;
//			TheTexture = NULL;
//		}
//	}
//
//	m_textureMap.clear();
//}
//====================================================

bool TextureManager::load(string fileName, string id, SDL_Renderer* pRenderer)
{
	return AddImage(pRenderer, id, fileName);
}


bool TextureManager::AddImage(SDL_Renderer* Renderer, std::string ID, std::string Filename) 
{
	if (ID == "")
	{
		Log("Bad ID passed");
		return false;
	}
		
	if (Renderer == NULL) 
	{
		Log("Bad SDL renderer passed");
		return false;
	}

	SDL_Surface* TempSurface = IMG_Load(Filename.c_str());
	if (TempSurface == NULL) 
	{
		Log("Unable to load image : %s : %s", Filename.c_str(), IMG_GetError());
		return false;
	}

	SDL_Texture* pTexture;
	// Convert SDL surface to a texture
	if ((pTexture = SDL_CreateTextureFromSurface(Renderer, TempSurface)) == NULL)
	{
		Log("Unable to create SDL Texture : %s : %s", Filename.c_str(), IMG_GetError());
		return false;
	}

	m_textureMap[ID] = pTexture;

	// Grab dimensions   
	// clw note: SDL_QueryTexture的作用：查询texture的属性
	//SDL_QueryTexture(m_SDLTexture, NULL, NULL, &m_Width, &m_Height);
	//Log("Texture Dimensions: %s : %d %d", Filename.c_str(), Width, Height);

	SDL_FreeSurface(TempSurface);

	//Log("Able to LoadFromImageFile Texture: %s", ID.c_str());
	return true;
}

bool TextureManager::AddText(SDL_Renderer* Renderer, std::string ID, std::string Filename)
{
	if (ID == "")
		return false;

	//clw note 20180824：调整字体的样式 ———— 通过TTF_OpenFont的第1个参数；
	//					 调整字体的大小 ———— 通过TTF_OpenFont的第2个参数；
	//Open the font  
	m_pFont = TTF_OpenFont(Filename.c_str(), 28);
	if (m_pFont == NULL)
	{
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}

	//Render text
	SDL_Color textColor = { 0xFF, 0xFF, 0xFF };
	string str = "score : 0";
	if (LoadFromRenderedText(Renderer, ID, str, textColor) == false)
	{
		Log("Unable to LoadFromRenderedText Texture: %s", ID.c_str());
		return false;
	}

	return true;
}

bool TextureManager::LoadFromRenderedText(SDL_Renderer* Renderer, std::string ID, std::string textureText, SDL_Color textColor)
{
	//GetTextureFromList rid of preexisting texture
	//free(); 

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(m_pFont, textureText.c_str(), textColor);
	if (textSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}

	SDL_Texture* pTexture;
	//Create texture from surface pixels
	if ((pTexture = SDL_CreateTextureFromSurface(Renderer, textSurface)) == NULL)
	{
		printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	m_textureMap[ID] = pTexture;

	//GetTextureFromList rid of old surface
	SDL_FreeSurface(textSurface);
	

	//Return success
	return true;
}


void TextureManager::draw(string id, int x, int y, int
	width, int height, SDL_Renderer* pRenderer,
	SDL_RendererFlip flip)
{
	SDL_Rect srcRect;
	SDL_Rect destRect;
	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = destRect.w = width;
	srcRect.h = destRect.h = height;
	destRect.x = x;
	destRect.y = y;
	SDL_RenderCopyEx(pRenderer, m_textureMap[id], &srcRect,
		&destRect, 0, 0, flip);
}

void TextureManager::drawClip(string id, int x, int y, int diameter, SDL_Renderer* pRenderer, SDL_Rect *clip /*src_clip*/, SDL_RendererFlip flip)
{
	SDL_Rect destRect;
	destRect.x = x;
	destRect.y = y;
	if (clip != nullptr)
	{
		//clw note 20180820：
		//定义snake为32*32的圆，窗口之前定义为1024*768，相当于横着可以放32个圆，竖着24个；
		destRect.w = diameter;
		destRect.h = diameter;
	}
	
	SDL_RenderCopy(pRenderer, m_textureMap[id], clip, &destRect);
	//SDL_RenderCopyEx(pRenderer, m_textureMap[id], clip, &destRect, 0, 0, flip);
}

void TextureManager::drawFrame(string id, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer
	*pRenderer, SDL_RendererFlip flip)
{
	SDL_Rect srcRect;
	SDL_Rect destRect;
	srcRect.x = width * currentFrame;
	srcRect.y = height * (currentRow - 1);
	srcRect.w = destRect.w = width;
	srcRect.h = destRect.h = height;
	destRect.x = x;
	destRect.y = y;
	SDL_RenderCopyEx(pRenderer, m_textureMap[id], &srcRect,
		&destRect, 0, 0, flip);
}

//clw modify 20190201
void TextureManager::drawFrameEx(string id, int src_width, int src_height,
	int dest_x, int dest_y, int dest_width, int dest_height, int currentRow, int currentFrame, 
	SDL_Renderer* pRenderer, SDL_RendererFlip flip)
{
	SDL_Rect srcRect;
	SDL_Rect destRect;
	srcRect.x = src_width * currentFrame;
	srcRect.y = src_height * (currentRow - 1);
	srcRect.w = src_width;
	srcRect.h = src_height;
	destRect.x = dest_x;
	destRect.y = dest_y;
	destRect.w = dest_width;
	destRect.h = dest_height;
	SDL_RenderCopyEx(pRenderer, m_textureMap[id], &srcRect, &destRect, 0, 0, flip);
}

//----------------------------------------------------------------------------
/*
* Draw an SDL_Texture to an SDL_Renderer at position x, y, preserving
* the texture's width and height and taking a clip of the texture if desired
* If a clip is passed, the clip's width and height will be used instead of the texture's
* @param tex The source texture we want to draw
* @param rend The renderer we want to draw too
* @param x The x coordinate to draw too
* @param y The y coordinate to draw too
* @param clip The sub-section of the texture to draw (clipping rect)
*		default of nullptr draws the entire texture
*/

//clw note 20180819：有函数声明(原型)时,默认参数可以放在函数声明或者定义中，但只能放在二者之一 
void TextureManager::RenderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int diameter, SDL_Rect *clip = nullptr)
{

	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	if (clip != nullptr)
	{
		//clw note 20180820：
		//定义snake为32*32的圆，窗口之前定义为1024*768，相当于横着可以放32个圆，竖着24个；
		dst.w = diameter;
		dst.h = diameter;
	}
	else
	{
		SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
	}
	SDL_RenderCopy(ren, tex, clip, &dst);
}

void TextureManager::clearFromTextureMap(std::string id)
{
	m_textureMap.erase(id);
}

//============================================================================
//void TextureManager::free()
//{
//	//Free texture if it exists
//	if (m_SDLTexture != NULL)
//	{
//		SDL_DestroyTexture(m_SDLTexture);
//		m_SDLTexture = NULL;
//		m_Width = 0;
//		m_Height = 0;
//	}
//}