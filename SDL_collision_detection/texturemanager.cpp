#include "texturemanager.h"
#include "common.h"
#include "global.h"
#include <SDL_ttf.h> //clw modify 20190122

CTextureManager::CTextureManager()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}


CTextureManager::~CTextureManager()
{
	//Deallocate
	free();
}

bool CTextureManager::loadFromFile(std::string path)
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0xFF, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(g_pRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//SDL_SetTextureBlendMode(newTexture, SDL_BLENDMODE_NONE); //clw note:混合模式，一般用不到
		//SDL_SetTextureColorMod(newTexture, 0, 0, 255);
		//SDL_SetTextureAlphaMod(newTexture, 255 / 2);  //clw note: 设置纹理的透明度。set alpha value multiplied into render copy operations.                                                                             srcA = srcA * (alpha / 255)
		

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}


bool CTextureManager::loadFromRenderedText(const std::string& textureText/*clw note：这里做成引用可否？*/, SDL_Color textColor)
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	//clw modify 20190124 用于显示中文字体
	std::vector<Uint16> unicodeUnit = getUnicode(textureText);
	int arraySize = unicodeUnit.size();
	Uint16* perOne = new Uint16[arraySize + 1];
	for (int i = 0; i < arraySize; i++)
		perOne[i] = unicodeUnit[i];
	perOne[arraySize] = 0;

	SDL_Surface *textSurface = TTF_RenderUNICODE_Solid(g_pFont, perOne, textColor); 
	delete[] perOne;

	if (textSurface != NULL)
	{
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(g_pRenderer, textSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}
	else
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}

	//Return success
	return mTexture != NULL;
}

void CTextureManager::free()
{
	//Free texture if it exists
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void CTextureManager::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	//Modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void CTextureManager::setBlendMode(SDL_BlendMode blending)
{
	//Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void CTextureManager::setAlpha(Uint8 alpha)
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void CTextureManager::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx(g_pRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int CTextureManager::getWidth()
{
	return mWidth;
}

int CTextureManager::getHeight()
{
	return mHeight;
}