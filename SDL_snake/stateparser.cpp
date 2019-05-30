//
//  StateParser.cpp
//  SDL Game Programming Book
//
//  Created by shaun mitchell on 24/02/2013.
//  Copyright (c) 2013 shaun mitchell. All rights reserved.
//

#include "stateparser.h"
#include "TextureManager.h"
#include "game.h"
#include "gameobjectfactory.h"

using namespace std;

bool StateParser::parseState(const char *stateFile, string stateID, vector<GameObject *> *pObjects, std::vector<std::string>*pTextureIDs)
{
	// create the XML document
	TiXmlDocument xmlDoc;
	// load the state file

	if (!xmlDoc.LoadFile(stateFile))
	{
		cerr << xmlDoc.ErrorDesc() << "\n";
		return false;
	}
	// get the root element
	TiXmlElement* pRoot = xmlDoc.RootElement();
	// pre declare the states root node
	TiXmlElement* pStateRoot = 0;
	// get this states root node and assign it to pStateRoot
	for (TiXmlElement* e = pRoot->FirstChildElement(); e != NULL; e =
		e->NextSiblingElement())
	{
		if (e->Value() == stateID)
		{
			pStateRoot = e;
			break; //clw note：Alien Attack新增
		}
	}
	// pre declare the texture root
	TiXmlElement* pTextureRoot = 0;

	// get the root of the texture elements
	for (TiXmlElement* e = pStateRoot->FirstChildElement(); e !=
		NULL; e = e->NextSiblingElement())
	{
		if (e->Value() == string("TEXTURES"))
		{
			pTextureRoot = e;
		}
	}

	// now parse the textures   //clw note!!
	parseTextures(pTextureRoot, pTextureIDs);

	// pre declare the object root node
	TiXmlElement* pObjectRoot = 0;
	// get the root node and assign it to pObjectRoot
	for (TiXmlElement* e = pStateRoot->FirstChildElement(); e !=
		NULL; e = e->NextSiblingElement())
	{
		if (e->Value() == string("OBJECTS"))
		{
			pObjectRoot = e;
		}
	}
	// now parse the objects    //clw note!!加载texture之后
	parseObjects(pObjectRoot, pObjects);
	return true;
}

void StateParser::parseTextures(TiXmlElement* pStateRoot,
	std::vector<std::string> *pTextureIDs)
{
	for (TiXmlElement* e = pStateRoot->FirstChildElement(); e !=
		NULL; e = e->NextSiblingElement())
	{
		string filenameAttribute = e->Attribute("filename");
		string idAttribute = e->Attribute("ID");
		pTextureIDs->push_back(idAttribute); // push into list
		
		//clw note 20180916：加载图片到TextureManager类的m_textureMap对象
		TheTextureManager::Instance()->load(filenameAttribute,idAttribute, TheGame::Instance()->GetRenderer());   
	}
}

void StateParser::parseObjects(TiXmlElement *pStateRoot, std::vector<GameObject *> *pObjects)
{
    for(TiXmlElement* e = pStateRoot->FirstChildElement(); e != NULL; e = e->NextSiblingElement())
    {
		//clw note:这里是从test.xml中读取各object属性的地方，
        int x, y, width, height, numFrames, callbackID, animSpeed;
        string textureID;
		e->Attribute("x", &x);
        e->Attribute("y", &y);
        e->Attribute("width",&width);
        e->Attribute("height", &height);
        e->Attribute("numFrames", &numFrames);
        e->Attribute("callbackID", &callbackID);
        e->Attribute("animSpeed", &animSpeed);
        
        textureID = e->Attribute("textureID");
        //int x, int y, int width, int height, std::string textureID, int numFrames, void()
         GameObject* pGameObject = TheGameObjectFactory::Instance()->create(e->Attribute("type"));
		//clw note 20180916：debug到这里要保证pGameObject非空；如果为空检查之前可能没有对类型register；
        pGameObject->load(std::unique_ptr<LoaderParams>(new LoaderParams(x, y, width, height, textureID, numFrames, callbackID, animSpeed)));
        pObjects->push_back(pGameObject);
    }
}