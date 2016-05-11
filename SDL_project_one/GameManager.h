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
#include <queue>

#define MAX_ENTITIES 4

//Button for controller.
struct Button
{
	bool isDown;
};

//Mouse struct
struct MouseInput
{
	Button leftButton;
	Button rightButton;
	Button middleButton;
	vec2 pos;
};

// Controller for input processing.
struct Controller
{
	MouseInput mouse;

	Button numOne;
	Button numTwo;

	Button start;
	Button back;
	Button rightBumper;
	Button leftBumper;

	Button actionUp;
	Button actionDown;
	Button actionRight;
	Button actionLeft;
};

#define RESOURCE_SENTINEL 0x8b05
// Basic structure that manages the resources added to this game.
// This structure will allow us to store one file in memory and share
// it between entities without loading a new file of the same one if one exists already.
struct ResourceManager
{
	int8 filename[20];		//Do we really need 20 bytes for each filename?
	uint32 id;				//unique ID for the current texture in memory.
	SDL_Texture* texture;	//Texture pointer in memory.
};

//Game manager class
class GameManager
{
public :
	GameManager();
	~GameManager();

	bool Init();
	
	void Run();
	void Update();
	void Render();

private:
	//Loads the file from disk into memory
	SDL_Texture* LoadTexture(int8* filename);
	vec2 DimFromTexture(SDL_Texture* texture);

	bool Collision(Entity* A, Entity* B);
	void SDLProcEvent(SDL_Event& evnt);
	void LoadContent();
	void Cleanup();
	void Exit();

	bool isRunning;

	Window* window;
	SDL_Renderer* renderer;
	
	SList<Entity*> entities;
	SList<ResourceManager*> resourceManager;	//TODO(George): Hash table? or linked list?

	Player* player;
	
	Controller input;
	Console console;
};

#endif