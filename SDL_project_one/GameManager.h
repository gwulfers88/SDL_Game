/*
	Programmer: George Wulfers
	Class:		Coding 2
	Date:		4/14/2016
	--------------------------
	File:	GameManager.h

	Desc: Manages the game and its subsystems.
*/

#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "common.h"
#include "console.h"
#include "Window.h"
#include "SList.h"
#include "Player.h"
#include "Enemy.h"
#include "Input.h"
#include "GUI.h"
#include <queue>

using namespace Input;
using namespace GUI;

#define RESOURCE_SENTINEL 0x8b05
// Basic structure that manages the resources added to this game.
// This structure will allow us to store one file in memory and share
// it between entities without loading a new file of the same one if one exists already.
struct ResourceManager
{
	int8 filename[30];		//Do we really need 30 bytes for each filename?
	uint32 id;				//unique ID for the current texture in memory.
	SDL_Texture* texture;	//Texture pointer in memory.
};

//Game manager class
class GameManager
{
public :
	GameManager(void);
	~GameManager(void);

	bool Init(void);
	
	void Run(void);
	void Update(real32 dt);
	void Render(void);

private:
	//Loads the file from disk into memory
	SDL_Texture* LoadTexture(int8* filename);
	vec2 DimFromTexture(SDL_Texture* texture);

	void SDLProcEvent(SDL_Event& evnt);
	void LoadContent(void);
	void Cleanup(void);
	void Exit(void);
	void OnGUI(void);
	bool isRunning;

	Window* window;
	SDL_Renderer* renderer;
	
	SList<Entity*> entities;
	SList<ResourceManager*> resourceManager;	//TODO(George): Hash table? or linked list?

	Player* player;
	Enemy* enemy;

	Controller input;

	GUIButton showInfo;
	GUIButton memReset;
	GUIButton reloadContent;

	Console console;
};

#endif