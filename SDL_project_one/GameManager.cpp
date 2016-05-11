/*
	Programmer: George Wulfers
	Class:		Coding 2
	Date:		4/14/2016
	--------------------------
	File:	GameManager.cpp

	Desc: Manages the game and its subsystems.
*/
#include "GameManager.h"
#include <SDL/SDL_ttf.h>
#include "mem.h"
#include "console.h"

// HOMEWORK: Make player change animations with key presses, multiple animations.
//TODO: Fix player struct and animations.

ifstream levelFile;
TTF_Font *font = 0;

bool loadFromRenderedText(SDL_Renderer* renderer, int8* text, SDL_Color color)
{
	SDL_Surface* surf = TTF_RenderText_Solid(font, text, color);

	if(!surf)
		return false;

	SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
	
	SDL_FreeSurface(surf);

	if(!tex)
		return false;
	
	SDL_Rect rect;
	rect.x = 5;
	rect.y = 5;
	
	SDL_QueryTexture(tex, 0, 0, &rect.w, &rect.h);

	SDL_Rect consoleRect;
	consoleRect.x = 0;
	consoleRect.y = 0;
	consoleRect.w = SCREEN_WIDTH;
	consoleRect.h = rect.h + 10;

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, &consoleRect);
	
	SDL_RenderCopy(renderer, tex, 0, &rect);
	
	SDL_DestroyTexture(tex);

	return true;
}

struct Object
{
	int8 name[10];			// 10
	int8 type[10];			// 10
	int8 filename[20];		// 20
	vec2 pos;				// 8
	vec2 dims;				// 8
	int32 layer;			// 4
};							// 60 bytes

// Constructor
GameManager::GameManager()
{
	isRunning = false;
	
	window = 0;
	player = 0;
	renderer = 0;

	ZeroMemory(&input, sizeof(Controller));
}

// Destructor
GameManager::~GameManager()
{
	Cleanup();
}

//Initializes the systems used in this application
bool GameManager::Init()
{
	InitCommandline();

	//Init SDL
	if( SDL_Init( SDL_INIT_EVERYTHING) < 0 )
		return false;

	//Init IMG
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

	if(TTF_Init() == -1)
		return false;

	window = (Window*)MemAllocName(sizeof(Window), "window");	//Allocate our window into our memory space.

	// Init window
	if(!(*window).Init_Window("WulfEngine- SDL",
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN))
		return false;

	if(!(font = TTF_OpenFont("lazy.ttf", 24)))
		return false;

	//Init Renderer
	renderer = SDL_CreateRenderer((*window).GetWindow(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	
	if (renderer == 0)
		return false;

	return true;
}

int8 console[256] = "";
bool isConsoleActive = false;
//TODO: Where to put this?
#define SHOW_DEBUG_NONE		0x00000000
#define SHOW_DEBUG_GRID		0x00000001
#define SHOW_DEBUG_COLISION 0x00000010
#define SHOW_DEBUG_LAYERS	0x00000100
uint32 ShowDebug = SHOW_DEBUG_NONE;

//Process events from Window and SDL. (Keyboard, Gamepad, joystick, and mouse inputs).
void GameManager::SDLProcEvent(SDL_Event& evnt)
{
	switch (evnt.type)
	{
	case SDL_QUIT:
		Exit();
		
		break;

	case SDL_TEXTINPUT:
		if(isConsoleActive)
		{
			COM_strcat(console, evnt.text.text);
		}
		break;

	case SDL_KEYDOWN:
		if(evnt.key.keysym.sym == SDLK_BACKSPACE)
		{
			if(isConsoleActive)
			{
				uint32 size = COM_strlen(console);
				if(size > 0)
				{
					console[size - 1] = 0;
				}
			}
		}

		if(evnt.key.keysym.sym == SDLK_SLASH)
			isConsoleActive = true;

		//TODO: Clean console command parser a bit.
		if(evnt.key.keysym.sym == SDLK_RETURN && isConsoleActive)
		{
			isConsoleActive = false;
			COM_strcat(console, "", '\0');
			COM_convertLower(console);
			commandParser(console);
			ZeroMemory(console, sizeof(console));

			//TODO: Change check function to separately check commands and multiple parameters
			if(checkCommand("/show_debug"))
			{
				if(checkParam("all", 1))
				{
					ShowDebug ^= SHOW_DEBUG_GRID ^ SHOW_DEBUG_COLISION ^ SHOW_DEBUG_LAYERS;
				}
				else if(checkParam("grid", 1))
				{
					ShowDebug ^= SHOW_DEBUG_GRID;
				}
				else if(checkParam("col", 1))
				{
					ShowDebug ^= SHOW_DEBUG_COLISION;
				}
				else if(checkParam("layers", 1))
				{
					ShowDebug ^= SHOW_DEBUG_LAYERS;
				}
			}
			else if(checkCommand("/exit"))
			{
				if(MessageBox(0, "Are you sure you would like to exit?", "WARNING", MB_OKCANCEL) == IDOK)
					Exit();
			}
			else if(checkCommand("/help"))
			{
				system("cls");
				ShowWindow( GetConsoleWindow(), SW_SHOW );
				COM_print("Commands: /[command] [parameter]\n");
				COM_print("show_debug\t(grid, layers, col)\tShows visual debug info.\n");
				COM_print("mem_check\t(low, high)\t\tChecks low memory integrity.\n");
				COM_print("exit\t\t\texit program.\n");
				COM_print("\nPress Enter to Continue...\n");
				getchar();
				ShowWindow( GetConsoleWindow(), SW_HIDE );
			}
			else if(checkCommand("/mem_check"))
			{
				if(checkParam("low", 1))
				{
					system("cls");
					ShowWindow( GetConsoleWindow(), SW_SHOW );
					MemCheck();
					COM_print("\nPress Enter to Continue...\n");
					getchar();
					ShowWindow( GetConsoleWindow(), SW_HIDE );
				}
				else if(checkParam("high", 1))
				{
					MessageBox(0, "This has not yet been implemnted.", "ATTENTION!", MB_OK);
				}
			}
		}
		else if(evnt.key.keysym.sym == SDLK_RETURN && !isConsoleActive)
			isConsoleActive = true;

		if(!isConsoleActive)
		{
			if(evnt.key.keysym.sym == SDLK_ESCAPE)
				input.back.isDown = true;

			if(evnt.key.keysym.sym == SDLK_SPACE)
				input.start.isDown = true;

			if(evnt.key.keysym.sym == SDLK_q)
				input.leftBumper.isDown = true;

			if(evnt.key.keysym.sym == SDLK_e)
				input.rightBumper.isDown = true;

			if(evnt.key.keysym.sym == SDLK_w || evnt.key.keysym.sym == SDLK_UP)
				input.actionUp.isDown = true;

			if(evnt.key.keysym.sym == SDLK_a || evnt.key.keysym.sym == SDLK_LEFT)
				input.actionLeft.isDown = true;
			
			if(evnt.key.keysym.sym == SDLK_s || evnt.key.keysym.sym == SDLK_DOWN)
				input.actionDown.isDown = true;
			
			if(evnt.key.keysym.sym == SDLK_d || evnt.key.keysym.sym == SDLK_RIGHT)
				input.actionRight.isDown = true;

			if(evnt.key.keysym.sym == SDLK_1)
				input.numOne.isDown = true;

			if(evnt.key.keysym.sym == SDLK_2)
				input.numTwo.isDown = true;
		}
		break;

	case SDL_KEYUP:

		if(!isConsoleActive)
		{
			if(evnt.key.keysym.sym == SDLK_ESCAPE)
				input.back.isDown = false;
			
			if(evnt.key.keysym.sym == SDLK_SPACE)
				input.start.isDown = false;

			if(evnt.key.keysym.sym == SDLK_q)
				input.leftBumper.isDown = false;

			if(evnt.key.keysym.sym == SDLK_e)
				input.rightBumper.isDown = false;

			if(evnt.key.keysym.sym == SDLK_w || evnt.key.keysym.sym == SDLK_UP)
				input.actionUp.isDown = false;

			if(evnt.key.keysym.sym == SDLK_a || evnt.key.keysym.sym == SDLK_LEFT)
				input.actionLeft.isDown = false;
			
			if(evnt.key.keysym.sym == SDLK_s || evnt.key.keysym.sym == SDLK_DOWN)
				input.actionDown.isDown = false;
			
			if(evnt.key.keysym.sym == SDLK_d || evnt.key.keysym.sym == SDLK_RIGHT)
				input.actionRight.isDown = false;

			if(evnt.key.keysym.sym == SDLK_1)
				input.numOne.isDown = false;

			if(evnt.key.keysym.sym == SDLK_2)
				input.numTwo.isDown = false;
		}
		break;

	case SDL_MOUSEBUTTONDOWN:
		if(evnt.button.button == SDL_BUTTON_RIGHT)
			input.mouse.rightButton.isDown = true;
		
		if(evnt.button.button == SDL_BUTTON_LEFT)
			input.mouse.leftButton.isDown = true;

		if(evnt.button.button == SDL_BUTTON_MIDDLE)
			input.mouse.middleButton.isDown = true;

		break;

	case SDL_MOUSEBUTTONUP:
		if(evnt.button.button == SDL_BUTTON_RIGHT)
			input.mouse.rightButton.isDown = false;
		
		if(evnt.button.button == SDL_BUTTON_LEFT)
			input.mouse.leftButton.isDown = false;

		if(evnt.button.button == SDL_BUTTON_MIDDLE)
			input.mouse.middleButton.isDown = false;

		break;

	case SDL_MOUSEMOTION:
		input.mouse.pos.x = (real32)evnt.motion.x;
		input.mouse.pos.y = (real32)evnt.motion.y;
		
		uint32 tileX = ((int32)input.mouse.pos.x / 128);
		uint32 tileY = ((int32)input.mouse.pos.y / 128);
		uint32 tileID = (tileY * 8) + tileX;
		
		uint32 dX = (tileID % 8);
		uint32 dY = tileID / 8;

		COM_printf("tileID: %d\n", tileID);
		COM_printf("derriv: %d, %d\n", dX, dY);
		COM_printf("actual: %d, %d\n", tileX, tileY);
		COM_printf("MouseD: %.02f, %.02f\n", input.mouse.pos.x, input.mouse.pos.y);

		break;
	}
}

//Loads the file from disk into memory
SDL_Texture* GameManager::LoadTexture(int8* filename)
{
	// TODO(George): Process filename string? sanitize it?
	COM_printf("checking for file: %s in memory.\n", filename);
 
	//Check to see if we have a file loaded with this name
	for( uint32 i = 0; i < resourceManager.GetCount(); i++)
	{
		ResourceManager* tmp = resourceManager.GetByIndex(i);

		//COM_strcmp return 0 if both strings match.
		if(!COM_strcmp(tmp->filename, filename))
		{
			COM_printf("\tfound file: %s in memory(ID: %d).\n", filename, tmp->id);
			return tmp->texture;	//Return current loaded texture
		}
	}
	
	COM_printf("Loading file: %s into memory.\n", filename);

	// Load a new texture into memory
	if(SDL_Surface* surf = IMG_Load(filename))
	{
		SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
		SDL_FreeSurface(surf);

		if(texture)
		{
			//Allocate a new resource management item into our memory space
			ResourceManager* temp = (ResourceManager*)MemHighAllocName(sizeof(ResourceManager), "res");
			
			// Copy the filename to the resource manager item's filename to keep track of it.
			COM_strcpy(temp->filename, filename);
			temp->id = RESOURCE_SENTINEL + (resourceManager.GetCount() + 1);	//ID
			temp->texture = texture;	//Store texture ptr.

			resourceManager.Insert(temp);	//Insert into list.

			COM_printf("\tnew file: %s\n\tID: %d\n", filename, temp->id);

			return texture;	// return texture for use.
		}
	}

	return 0;	//We should not be able to get here. Lets hope!
}

//returns the dimensions from the specified texture (Packaged in a vector 2 struct)
vec2 GameManager::DimFromTexture(SDL_Texture* texture)
{
	vec2 dims;

	if(texture)
	{
		int w = 0, h = 0;

		SDL_QueryTexture(texture, 0, 0, &w, &h);

		dims.x = (real32)w;
		dims.y = (real32)h;
	}

	return dims;
}

//Loads content that will be used for this game
void GameManager::LoadContent()
{
	//LOAD LEVEL ONE INFO
	levelFile.open("level1.txt");

	if(levelFile.fail())
		return;

	char type[8] = {0};
	char lEdge[20] = {0};
	char rEdge[20] = {0};
	char mid[20] = {0};
	char block[20] = {0};
	char bush[20] = {0};

	int layer = 0;

	vec2 playerPos = {0};

	int tileW = 0;
	int tileH = 0;

	char data[256] = {0};

	int y = 0;

	//Read jump header info
	levelFile.getline(data, sizeof(data));
	levelFile >> tileW >> tileH >> type;
	levelFile >> lEdge >> rEdge >> mid >> block >> bush;
	levelFile >> layer;

	while(!levelFile.eof())
	{
		if( y == 6)
		{
			y = 0;
			layer++;
		}

		ZeroMemory(data, sizeof(data));
		levelFile >> data;
		
		if(data[0] == '#')
		{
			levelFile.getline(data, sizeof(data));
			continue;
		}

		int x = 0;

		while(x != 8)
		{	
			Entity *entity = nullptr;

			switch(data[x])
			{
			case 'e':
				break;

			case 'b':
				entity = (Entity*)MemAllocName(sizeof(Entity), "floorB");
				new (entity) Entity;
				entity->dims = Vec2(tileW, tileH);
				entity->pos = Vec2(x * entity->dims.x, y * entity->dims.y);
				COM_strncpy(entity->type, "block", 8);
				entity->layer = layer;
				entity->texture = LoadTexture(block);	//TODO: should we use and lookup Texture ID?
				entity->CalculateMidpoint();
				entities.Insert(entity);
				break;

			case 'u':
				entity = (Entity*)MemAllocName(sizeof(Entity), "bush");
				new (entity) Entity;
				entity->dims = Vec2(tileW, tileH);
				entity->pos = Vec2(x * entity->dims.x, y * entity->dims.y);
				COM_strncpy(entity->type, "bush", 8);
				entity->layer = layer;
				entity->texture = LoadTexture(bush);
				entity->CalculateMidpoint();
				entities.Insert(entity);
				break;

			case 'l':
				entity = (Entity*)MemAllocName(sizeof(Entity), "floorL");
				new (entity) Entity;
				entity->dims = Vec2(tileW, tileH);
				entity->pos = Vec2(x * entity->dims.x, y * entity->dims.y);
				COM_strncpy(entity->type, type, 8);
				entity->layer = layer;
				entity->texture = LoadTexture(lEdge);
				entity->CalculateMidpoint();
				entities.Insert(entity);
				break;

			case 'm':
				entity = (Entity*)MemAllocName(sizeof(Entity), "floorM");
				new (entity) Entity;
				entity->dims = Vec2(tileW, tileH);
				entity->pos = Vec2(x * entity->dims.x, y * entity->dims.y);
				COM_strncpy(entity->type, type, 8);
				entity->layer = layer;
				entity->texture = LoadTexture(mid);
				entity->CalculateMidpoint();
				entities.Insert(entity);
				break;

			case 'r':
				entity = (Entity*)MemAllocName(sizeof(Entity), "floorR");
				new (entity) Entity;
				entity->dims = Vec2(tileW, tileH);
				entity->pos = Vec2(x * entity->dims.x, y * entity->dims.y);
				COM_strncpy(entity->type, type, 8);
				entity->layer = layer;
				entity->texture = LoadTexture(rEdge);
				entity->CalculateMidpoint();
				entities.Insert(entity);
				break;

			case 'p':
				playerPos = Vec2(x, y);
				break;
			}

			x++;
		}

		y++;

		if(levelFile.fail())
			continue;
	}

	levelFile.close();

	/// CONFIG FILE LOADING

	ifstream file;
	file.open("config.txt");

	if(file.fail())
		return;
	
	int8 content[40];
	
	cout << "\nFILE CONTENT" << endl;

	while(!file.eof())
	{
		file >> content;

		//Ignore comments
		if(content[0] == '#')
		{
			file.getline(content, sizeof(content));
			continue;
		}

		Object obj;
		ZeroMemory(&obj, sizeof(Object));	//Zero out struct

		// Fillout struct
		COM_strcpy(obj.name, content);
		file >> obj.type;
		file >> obj.layer;
		file >> obj.filename;
		file >> obj.pos.x;
		file >> obj.pos.y;
		file >> obj.dims.x;
		file >> obj.dims.y;
		
		//Allocate Sprite into memory block and name that allocation what ever the sprite's name is.
		Entity* temp;

		//if it is player type
		if(!COM_strcmp(obj.type, "player"))
		{
			temp = (Player*)MemAllocName(sizeof(Player), obj.name);
			new(temp) Player;	//Initialize the class constructor.
			player = (Player*)temp;	//Isolate player
			temp->pos = Vec2(playerPos.x * obj.dims.x, playerPos.y * obj.dims.x);
			player->speed = 5;
		}
		else
		{
			temp = (Entity*)MemAllocName(sizeof(Entity), obj.name);
			new(temp) Entity;	//Initialize the class constructor.
			temp->pos = Vec2(obj.pos.x, obj.pos.y);
		}
		
		//
		temp->layer = obj.layer;
		COM_strncpy(temp->type, obj.type, 8);	//Copy only the first 8 bytes of the string.
		temp->texture = LoadTexture(obj.filename);
		
		//if BG type
		if(!COM_strcmp(obj.type, "bg"))
		{
			temp->dims = Vec2(SCREEN_WIDTH, SCREEN_HEIGHT);	//Fill screen
		}
		else
		{
			temp->dims = Vec2(obj.dims.x, obj.dims.y);	//Otherwise just set regular size
		}

		temp->CalculateMidpoint();

		entities.Insert(temp);	//insert into list
		
		if(file.fail())
			continue;
	}
	
	cout << "FILE CONTENT\n" << endl;

	file.close();	//Close file
}

//Starts the application
void GameManager::Run()
{
	// Init systems
	if(!Init())
		return;

	//Load game content
	LoadContent();

	MemCheck();

	// Run app
	isRunning = true;

	COM_print("\nCONTROLS:\n\tMOVE: WASD keys or Arrow Keys.\n\tJUMP: Space bar.\n\tDrop from platform: S + E.");
	COM_print("\n\tDEBUG start: 2 key.\n\tDEBUG end: 1 keys.\n");

	SDL_StartTextInput();
	
	//ShowWindow( GetConsoleWindow(), SW_HIDE );

	//Game loop
	while(isRunning)
	{
		SDL_Event evnt;	//SDL event handle

		//Event processing loop
		while(SDL_PollEvent(&evnt))
		{
			SDLProcEvent(evnt);
		}

		//Exit app
		if(input.back.isDown)
		{
			Exit();
		}

		DWORD start = GetTickCount();	//

		Update();	//Update logic
		Render();	//Render logic

		DWORD end = GetTickCount();
		DWORD elapsed = ((end - start));

		//cout << "MS: " << (float)elapsed/1000 << endl;
	}
}

//AABB Collision detection.
bool GameManager::Collision(Entity* A, Entity* B)
{
	if((A->pos.x + 32 < B->pos.x + B->dims.x &&
		A->pos.x + A->dims.x - 32 > B->pos.x &&
		A->pos.y < B->pos.y + B->center.y &&
		A->dims.y + A->pos.y > B->pos.y))
	{
		return true;
	}

	return false;
}

//TODO: create a world structure that holds level data
real32 gravity = -2.f;

// Update logic procedures
void GameManager::Update()
{
	player->animate = false;

	vec2 dir;
	dir.x = 0;
	dir.y = 0;

	//player movement
	if(input.actionLeft.isDown)
	{
		player->animate = true;
		dir.x = -1;
		//player->flip = SDL_FLIP_NONE;
		player->facingDir = 0;
	}
	else if(input.actionRight.isDown)
	{
		player->animate = true;
		dir.x = 1;
		//player->flip = SDL_FLIP_HORIZONTAL; we can flip our sprite if we dont have the correct animation on file.
		player->facingDir = 1;
	}
	
#if _DEBUG
	//Show Debug Toggle (DEBUG ONLY)
	
#endif

	player->isCrouching = input.actionDown.isDown;
	
	if(input.actionDown.isDown && input.rightBumper.isDown)
	{
		player->layer++;
	}

	if(input.start.isDown && !player->isJumping && player->isGrounded)
	{
		player->isJumping = true;
		player->isGrounded = false;
	}

	int oldfacingDir = player->facingDir;

	if(player->isCrouching)
	{
		if(player->facingDir == 1)
			player->facingDir = 9;
		else if(player->facingDir == 0)
			player->facingDir = 8;
	}
	else
		player->facingDir = oldfacingDir;

	if(player->isJumping)
	{
		dir.y = -30;
		player->isJumping = false;
	}

	dir.y -= gravity;

	//Collision testing
	for(int i = 0; i < entities.GetCount(); i++)
	{
		if(!COM_strcmp(entities.GetByIndex(i)->type, "bg") || !COM_strcmp(entities.GetByIndex(i)->type, "player"))
			i++;

		Entity* obj = entities.GetByIndex(i);

		//TODO(George): Perform Object Culling for faster performance.
		if(obj->layer == player->layer && !COM_strcmp(obj->type, "floor"))
		{
			if(Collision(player, obj))
			{
				if( player->pos.y > obj->pos.y + obj->center.y &&
					player->pos.y + player->center.y > obj->pos.y + obj->dims.y)			//player Top
				{
					real32 colOffsetY = (obj->pos.y + obj->dims.y) - (player->pos.y);		//Calculate how far in we went to go back by that much.
					player->pos.y += colOffsetY - 2;										//Go back
				}
				else if(player->pos.y + player->center.y < obj->pos.y &&
						player->pos.y + player->dims.y < obj->pos.y + obj->center.y)		//player Bottom
				{
					real32 colOffsetY = (player->pos.y + player->dims.y) - (obj->pos.y);	//Calculate how far in we went to go back by that much.
					player->pos.y -= colOffsetY - 2;										//Go back
					player->isGrounded = true;
				}
				else if(player->pos.x + player->dims.x < obj->pos.x + obj->center.x &&
					player->pos.x + player->center.x < obj->pos.x)							//player Right
				{
					real32 colOffsetX = (player->pos.x + player->dims.x) - (obj->pos.x);	//Calculate how far in we went to go back by that much.
					player->pos.x -= colOffsetX;											//Go back
				}
				else if(player->pos.x > obj->pos.x + obj->center.x)							//player Left
				{
					real32 colOffsetX = (obj->pos.x + obj->dims.x) - (player->pos.x);		//Calculate how far in we went to go back by that much.
					player->pos.x += colOffsetX;											//Go back
				}
			}
		}
		else if(!COM_strcmp(obj->type, "floor"))
		{
			if(Distance(player->pos, obj->pos) <= player->dims.y + 40)
			{
				if(player->pos.y < obj->pos.y)
				{
					player->layer = obj->layer;
				}
			}
		}
	}
	
	player->Move(dir);	//Move player
	player->Update();
}

struct RenderObject
{
	Entity* obj;
	bool operator < (const RenderObject& a) const
	{
		if(obj->layer > a.obj->layer)
		{
			return true;
		}
		else if( obj->layer == a.obj->layer)
		{
			return obj->pos.y > a.obj->pos.y;
		}

		return false;
	}
};

//Render procedure
void GameManager::Render()
{
	SDL_SetRenderDrawColor(renderer, 50, 100, 200, 255);
	SDL_RenderClear(renderer);	//Cear the buffer
	
	priority_queue<RenderObject> q;

	//Draw here!
	for(uint32 i = 0; i < entities.GetCount(); i++)
	{
		RenderObject renderObj;
		renderObj.obj = entities.GetByIndex(i);

		if(!COM_strcmp(renderObj.obj->type, "bg"))
			renderObj.obj->Draw(renderer);
		else
		{
			q.push(renderObj);
		}
	}

	while(!q.empty())
	{
		RenderObject obj = q.top();
		q.pop();
		
		if(ShowDebug & SHOW_DEBUG_LAYERS)
		{
			if(obj.obj->layer == player->layer)
			{
				obj.obj->Draw(renderer);
			}
		}
		else
		{
			obj.obj->Draw(renderer);		
		}
	}

	if(ShowDebug & SHOW_DEBUG_GRID)
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		for(int y = 0; y < 6; y++)
		{
			for(int x = 0; x < 8; x++)
			{
				SDL_Rect rect;
				rect.x = x * 128;
				rect.y = y * 128;
				rect.w = 128;
				rect.h = 128;

				SDL_RenderDrawRect(renderer, &rect);
			}
		}
	}
	
	if(ShowDebug & SHOW_DEBUG_COLISION)
	{
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		for(int i = 0; i < entities.GetCount(); i++)
		{
			if(!COM_strcmp(entities.GetByIndex(i)->type, "floor"))
			{
				SDL_Rect colRect;
				colRect.x = entities.GetByIndex(i)->pos.x;
				colRect.y = entities.GetByIndex(i)->pos.y;
				colRect.w = entities.GetByIndex(i)->dims.x;
				colRect.h = entities.GetByIndex(i)->center.y;

				SDL_RenderDrawRect(renderer, &colRect);
			}
		}
	}

	if(isConsoleActive)
	{
		SDL_Color color;
		color.a = 255;
		color.r = 255;
		color.g = 255;
		color.b = 255;

		uint32 size = COM_strlen(console);

		if(size > 0)
		{
			loadFromRenderedText(renderer, console, color);
		}
		else
		{
			loadFromRenderedText(renderer, " ", color);
		}
	}

	SDL_RenderPresent(renderer);	//Send to screen and draw scene
}

// Cleanup procedure (frees used memory)
void GameManager::Cleanup()
{
	SDL_StopTextInput();

	TTF_CloseFont(font);
	font = 0;

	//free renderer
	if(renderer)
	{
		SDL_DestroyRenderer(renderer);
	}
	
	//Frees the entire memory block
	FreeMemBlock();

	TTF_Quit();
	IMG_Quit();	//Stop using IMG
	SDL_Quit();	//Stop using SDL
}

// Exit application
void GameManager::Exit()
{
	isRunning = false;
}