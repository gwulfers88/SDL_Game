/*
	Programmer: George Wulfers
	Class:		Coding 2
	Date:		4/14/2016
	--------------------------
	File:	GameManager.cpp

	Desc: Manages the game and its subsystems.

	TODO:

	You will implement a simple scrolling game, similar to Gradius, Mario Bros, or Contra. This game will use the concepts already covered in class, including several others:
	•	FSM (Finite State Machines) – perform at least 3 different animations for each character
	•	Trees – for enemy pathing logic
	•	File handling – to specify at least two different levels with different sets of enemies and backgrounds
	•	Graphical User Interface using text and images – In game, include user feedback including number of lives and kills (or points)

*/
#include "GameManager.h"
#include "TileMap.h"
#include "mem.h"
#include "LevelFile.h"

#define PI 3.14159

//TODO: create file read and open functions
TTF_Font *font = 0;

float worldWidth = 0;
float worldHeight = 0;

bool levelLoaded = false;

SDL_Texture* StringToTexture(SDL_Renderer* renderer, int8* text, SDL_Color color)
{
	SDL_Surface* surf = TTF_RenderText_Solid(font, text, color);

	if(!surf)
		return 0;

	SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
	
	SDL_FreeSurface(surf);

	if(!tex)
		return 0;
	
	return tex;
}

void DrawConsole(SDL_Renderer* renderer, SDL_Texture* text)
{
	SDL_Rect rect;
	rect.x = 5;
	rect.y = 5;
	
	if(!text)
		return;

	SDL_QueryTexture(text, 0, 0, &rect.w, &rect.h);

	SDL_Rect consoleRect;
	consoleRect.x = 0;
	consoleRect.y = 0;
	consoleRect.w = SCREEN_WIDTH;
	consoleRect.h = rect.h + 10;

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, &consoleRect);
	
	SDL_RenderCopy(renderer, text, 0, &rect);
	
	SDL_DestroyTexture(text);
}

struct Object
{
	int8 name[10];			// 10
	int8 type[10];			// 10
	int8 filename[30];		// 30
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

	for(int i = 0; i < 2; i++)
	{
		bg[i] = 0;
	}
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

	if(!(font = TTF_OpenFont("alphaslab.ttf", 18)))
		return false;

	//Init Renderer
	renderer = SDL_CreateRenderer((*window).GetWindow(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	
	if (renderer == 0)
		return false;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	return true;
}

typedef struct State
{
	int TotalSize;
	void* GameMemBlock;

	HANDLE recordHandle;
	int inputRecordIndex;
	
	HANDLE playHandle;
	int inputPlayIndex;
};

State state;

void BeginRecordingInput(State *state, int inputRecordingIndex)
{
	state->inputRecordIndex = inputRecordingIndex;
	int8 *filename = "recording.gw";
	state->recordHandle = CreateFileA(filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);

	DWORD bytesToWrite = (DWORD)state->TotalSize;
	assert(state->TotalSize == bytesToWrite);
	DWORD bytesWritten = 0;
	WriteFile(state->recordHandle, state->GameMemBlock, bytesToWrite, &bytesWritten, 0);
}

void EndRecordingInput(State* state)
{
	CloseHandle(state->recordHandle);
	state->inputRecordIndex = 0;
}

void ClearInputPlayback()
{
	DeleteFile("recording.gw");
}

void BeginInputPlayback(State *state, int inputPlaybackIndex)
{
	state->inputPlayIndex = inputPlaybackIndex;
	int8 *filename = "recording.gw";
	state->playHandle = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

	DWORD bytesToRead = (DWORD)state->TotalSize;
	assert(state->TotalSize == bytesToRead);
	DWORD bytesWritten = 0;
	ReadFile(state->playHandle, state->GameMemBlock, bytesToRead, &bytesWritten, 0);
}

void EndInputPlayback(State* state)
{
	CloseHandle(state->playHandle);
	state->inputPlayIndex = 0;
}

void RecordInput(State *state, Controller* input)
{
	DWORD bytesWritten = 0;
	WriteFile(state->recordHandle, input, sizeof(*input), &bytesWritten, 0);
}

void PlaybackInput(State *state, Controller* input)
{
	DWORD bytesRead = 0;
	if(ReadFile(state->playHandle, input, sizeof(*input), &bytesRead, 0))
	{
		if(bytesRead == 0)
		{
			int playingIndex = state->inputPlayIndex;
			EndInputPlayback(state);
			BeginInputPlayback(state, playingIndex);
		}
	}
}

//Process events from Window and SDL. (Keyboard, Gamepad, joystick, and mouse inputs).
void GameManager::SDLProcEvent(SDL_Event& evnt)
{
	switch (evnt.type)
	{
	case SDL_QUIT:
		Exit();
		
		break;

	case SDL_TEXTINPUT:
		if(console.isConsoleActive)
		{
			COM_strcat(console.input, evnt.text.text);
		}
		break;

	case SDL_KEYDOWN:
		if(evnt.key.keysym.sym == SDLK_BACKSPACE)
		{
			if(console.isConsoleActive)
			{
				uint32 size = COM_strlen(console.input);
				if(size > 0)
				{
					console.input[size - 1] = 0;
				}
			}
		}

		if(evnt.key.keysym.sym == SDLK_SLASH)
			console.isConsoleActive = true;
		
		if(console.isConsoleActive)
		{
			//CUT
			if(evnt.key.keysym.mod & KMOD_LCTRL && evnt.key.keysym.sym == SDLK_x)
			{
				uint32 size = COM_strlen(console.input);

				if(size > 0)
				{
					if(!SDL_SetClipboardText(console.input))
					{
						//success
						ZeroMemory(console.input, sizeof(console.input));
					}
				}
			}//COPY
			else if(evnt.key.keysym.mod & KMOD_LCTRL && evnt.key.keysym.sym == SDLK_c)
			{
				uint32 size = COM_strlen(console.input);

				if(size > 0)
				{
					if(!SDL_SetClipboardText(console.input))
					{

					}
				}
			}//PASTE
			else if(evnt.key.keysym.mod & KMOD_LCTRL && evnt.key.keysym.sym == SDLK_v)
			{
				COM_strcat(console.input, SDL_GetClipboardText());
			}
		}
		
		if(evnt.key.keysym.sym == SDLK_RETURN && console.isConsoleActive)
		{
			console.isConsoleActive = false;
			COM_strcat(console.input, "", '\0');
			COM_convertLower(console.input);
			commandParser(console.input);
			ZeroMemory(console.input, sizeof(console.input));

			if(checkCommand("/show_debug"))
			{
				if(checkParam("all", 1))
				{
					showDebug(&console, SHOW_DEBUG_GRID ^ SHOW_DEBUG_COLISION ^ SHOW_DEBUG_LAYERS ^ SHOW_DEBUG_PATH);
				}
				else if(checkParam("grid", 1))
				{
					showDebug(&console, SHOW_DEBUG_GRID);
				}
				else if(checkParam("col", 1))
				{
					showDebug(&console, SHOW_DEBUG_COLISION);
				}
				else if(checkParam("path", 1) || checkParam("node", 1))
				{
					showDebug(&console, SHOW_DEBUG_PATH);
				}
				else if(checkParam("layers", 1))
				{
					console.layerFlags = 0;

					if(checkParam("\0", 2) || checkParam("", 2))
					{
						showDebug(&console, SHOW_DEBUG_LAYERS);
						setLayers(&console, DEBUG_LAYERS_PLAYER);
					}
					else if(checkParam("1", 2))
					{
						setLayers(&console, DEBUG_LAYERS_ONE);
					}
					else if(checkParam("2", 2))
					{
						setLayers(&console, DEBUG_LAYERS_TWO);
					}
					else if(checkParam("3", 2))
					{
						setLayers(&console, DEBUG_LAYERS_THREE);
					}
					else if(checkParam("4", 2))
					{
						setLayers(&console, DEBUG_LAYERS_FOUR);
					}
					else
					{
						showDebug(&console, SHOW_DEBUG_LAYERS);
					}
				}
				else if(checkParam("cmd", 1))
				{
					showDebug(&console, SHOW_DEBUG_CMD);
				}
			}
			else if(checkCommand("/load_level"))
			{
				if(checkParam("show", 1))
				{
					glfPrintAll();
				}
				else
				{
					GLF* file = glfGetFile(getParam(1));

					if(file)
					{
						COM_printf("loading level %s\n", file->filename);
						if(levelLoaded)
						{
							UnloadContent();
						}
						LoadContent(file->filename);
					}
					else
					{
						COM_printf("Level file does not exist!\n");
					}
				}
			}
			else if(checkCommand("/unload_level"))
			{
				if(levelLoaded)
				{
					UnloadContent();
				}
			}
			else if(checkCommand("/begin_playback"))
			{
				if(levelLoaded)
				{
					if(state.inputPlayIndex == 0)
					{
						BeginInputPlayback(&state, 1);
					}
				}
			}
			else if(checkCommand("/stop_playback"))
			{
				if(levelLoaded)
				{
					if(state.inputPlayIndex)
					{
						EndInputPlayback(&state);
						ZeroMemory(&input, sizeof(input));
					}
				}
			}
			else if(checkCommand("/begin_recording"))
			{
				if(levelLoaded)
				{
					if(state.inputRecordIndex == 0)
					{
						BeginRecordingInput(&state, 1);
					}
				}
			}
			else if(checkCommand("/stop_recording"))
			{
				if(levelLoaded)
				{
					if(state.inputRecordIndex)
					{
						EndRecordingInput(&state);
					}
				}
			}
			else if(checkCommand("/exit"))
			{
				if(MessageBox(0, "Are you sure you would like to exit?", "WARNING", MB_OKCANCEL) == IDOK)
					Exit();
			}
			else if(checkCommand("/mem_check"))
			{
				if(checkParam("low", 1))
				{
					MemCheck();
				}
				else if(checkParam("high", 1))
				{
					MemCheckHigh();
				}
				else if(checkParam("info", 1))
				{
					MemInfo();
				}
				else if(checkParam("\0", 1))
				{
					MemInfo();
					MemCheck();
					MemCheckHigh();
				}
			}
			else if(checkCommand("/help"))
			{
				system("cls");
				ShowWindow( GetConsoleWindow(), SW_SHOW );
				COM_print("Commands: /[command] [parameter]\n");
				COM_print("show_debug\t(grid, layers, col, all, cmd)\tShows visual debug info.\n");
				COM_print("mem_check\t(low, high, info)\t\tChecks low memory integrity.\n");
				COM_print("load_level\t(filename)\t\tLoads level file into memory\n");
				COM_print("unload_level\t\t\tUnloads level file from memory\n");
				COM_print("begin_playback\t\t\tload recorded file and plays it back.\n");
				COM_print("stop_playback\t\t\tunloads recorded file.\n");
				COM_print("begin_recording\t\t\trecords game state into file.\n");
				COM_print("stop_recording\t\t\tstops recording.\n");
				COM_print("exit\t\t\texit program.\n");
				COM_print("\nPress Enter to Continue...\n");
			}
		}
		else if(evnt.key.keysym.sym == SDLK_RETURN && !console.isConsoleActive)
			console.isConsoleActive = true;

		if(!console.isConsoleActive)
		{
			if(evnt.key.keysym.sym == SDLK_ESCAPE && !evnt.key.repeat)
				input.back.isDown = true;

			if(evnt.key.keysym.sym == SDLK_SPACE )//&& !evnt.key.repeat)
				input.start.isDown = true;

			if(evnt.key.keysym.sym == SDLK_q && !evnt.key.repeat)
				input.leftBumper.isDown = true;

			if(evnt.key.keysym.sym == SDLK_e && !evnt.key.repeat)
				input.rightBumper.isDown = true;

			if(evnt.key.keysym.sym == SDLK_w || evnt.key.keysym.sym == SDLK_UP && !evnt.key.repeat)
				input.actionUp.isDown = true;

			if(evnt.key.keysym.sym == SDLK_a || evnt.key.keysym.sym == SDLK_LEFT && !evnt.key.repeat)
				input.actionLeft.isDown = true;
			
			if(evnt.key.keysym.sym == SDLK_s || evnt.key.keysym.sym == SDLK_DOWN && !evnt.key.repeat)
				input.actionDown.isDown = true;
			
			if(evnt.key.keysym.sym == SDLK_d || evnt.key.keysym.sym == SDLK_RIGHT && !evnt.key.repeat)
				input.actionRight.isDown = true;

			//Record and play game state
			if(evnt.key.keysym.sym == SDLK_l)
			{
				if(state.inputRecordIndex == 0)
				{
					BeginRecordingInput(&state, 1);
				}
				else
				{
					EndRecordingInput(&state);
					BeginInputPlayback(&state, 1);
				}
			}

			//stop playing game state
			if(evnt.key.keysym.sym == SDLK_k)
			{
				if(state.inputPlayIndex)
				{
					EndInputPlayback(&state);
					ZeroMemory(&input, sizeof(input));
				}
			}

			if(evnt.key.keysym.sym == SDLK_1 && !evnt.key.repeat)
				input.numOne.isDown = true;

			if(evnt.key.keysym.sym == SDLK_2 && !evnt.key.repeat)
				input.numTwo.isDown = true;
		}
		break;

	case SDL_KEYUP:

		if(!console.isConsoleActive)
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
		if(evnt.button.button == SDL_BUTTON_RIGHT && evnt.button.clicks == 1)
			input.mouse.rightButton.isDown = true;
		
		if(evnt.button.button == SDL_BUTTON_LEFT && evnt.button.clicks == 1)
			input.mouse.leftButton.isDown = true;

		if(evnt.button.button == SDL_BUTTON_MIDDLE && evnt.button.clicks == 1)
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

TileMap tileMap;

void CreatePathNode(SList<PathNode*>* path, vec2 pos)
{
	PathNode* node = (PathNode*)MemAllocName(sizeof(node), "pnode");
	node->pos = pos;
	path->Insert(node);
}

void GameManager::loadAnimations(Player* player, char* animFile)
{
	ifstream file;
	file.open(animFile);

	if(file.fail())
		return;

	char content[MAX_PATH];
	ZeroMemory(content, sizeof(content));

	file.getline(content, sizeof(content));

	char imageFile[MAX_PATH];
	string state = " ";

	file >> imageFile;

	player->texture = LoadTexture(imageFile);

	while(!file.eof())
	{
		AnimInfo* anim = (AnimInfo*)MemAllocName(sizeof(AnimInfo), "p_anim");

		file >>	state
			 >>	anim->maxFrame
			 >>	anim->frameRate
			 >>	anim->clipPos.x
			 >>	anim->clipPos.y
			 >>	anim->clipDims.x
			 >>	anim->clipDims.y;

		player->anim[state] = anim;
	}

	file.close();
}

//Loads content that will be used for this game
void GameManager::LoadContent(int8* filename)
{
	//LOAD LEVEL ONE INFO
	COM_print("\nLOAD LEVEL FILE START\n");

	ZeroMemory(&tileMap, sizeof(tileMap));

	ifstream levelFile;
	levelFile.open(filename);

	if(levelFile.fail())
		return;

	char type[8] = {0};
	char imagefile[30] = {0};
	char levelDataFile[30] = {0};
	char data[256] = {0};

	int y = 0;
	int layer = 0;
	int numEnemies = 0;

	vec2 playerPos = {0};
	uint32 enemyIndex = 0;
	
	//Read jump header info
	levelFile.getline(data, sizeof(data));
	levelFile	>> tileMap.width
				>> tileMap.height
				>> tileMap.tileWidth
				>> tileMap.tileHeight
				>> type
				>> numEnemies;

	levelFile	>> imagefile;
	levelFile	>> levelDataFile;
	levelFile	>> layer;

	vec2 *enemyPos = new vec2[numEnemies];

	worldWidth = tileMap.width * tileMap.tileWidth;
	worldHeight = tileMap.height * tileMap.tileHeight;

	while(!levelFile.eof())
	{
		if( y == tileMap.height)
		{
			y = 0;
			layer++;
			tileMap.numLayers = layer;
		}

		ZeroMemory(data, sizeof(data));
		levelFile >> data;
		
		if(data[0] == '#')
		{
			levelFile.getline(data, sizeof(data));
			continue;
		}

		int x = 0;

		while(x != tileMap.width)
		{	
			Entity *entity = nullptr;

			switch(data[x])
			{
			case '0':
				entity = (Entity*)MemAllocName(sizeof(Entity), "floorL");
				new (entity) Entity;
				entity->dims = Vec2((real32)tileMap.tileWidth, (real32)tileMap.tileHeight);
				entity->pos = Vec2((real32)x * entity->dims.x, (real32)y * entity->dims.y);
				COM_strncpy(entity->type, type, 8);
				entity->layer = layer;
				entity->texture = LoadTexture(imagefile);
				entity->CalculateMidpoint();
				entity->colRect.x = entity->pos.x;
				entity->colRect.y = entity->pos.y;
				entity->colRect.w = entity->dims.x;
				entity->colRect.h = entity->center.y;
				entity->tileID = 0;
				entities.Insert(entity);

				CreatePathNode(&path, Vec2(entity->pos.x + (entity->dims.x * 0.5f), entity->pos.y - (tileMap.tileHeight * 0.5f)));
				break;

			case '1':
				entity = (Entity*)MemAllocName(sizeof(Entity), "floorM");
				new (entity) Entity;
				entity->dims = Vec2((real32)tileMap.tileWidth, (real32)tileMap.tileHeight);
				entity->pos = Vec2((real32)x * entity->dims.x, (real32)y * entity->dims.y);
				COM_strncpy(entity->type, type, 8);
				entity->layer = layer;
				entity->texture = LoadTexture(imagefile);
				entity->CalculateMidpoint();
				entity->colRect.x = entity->pos.x;
				entity->colRect.y = entity->pos.y;
				entity->colRect.w = entity->dims.x;
				entity->colRect.h = entity->center.y;
				entity->tileID = 1;
				entities.Insert(entity);
				
				CreatePathNode(&path, Vec2(entity->pos.x + (entity->dims.x * 0.5f), entity->pos.y - (tileMap.tileHeight * 0.5f)));

				break;

			case '2':
				entity = (Entity*)MemAllocName(sizeof(Entity), "floorR");
				new (entity) Entity;
				entity->dims = Vec2((real32)tileMap.tileWidth, (real32)tileMap.tileHeight);
				entity->pos = Vec2((real32)x * entity->dims.x, (real32)y * entity->dims.y);
				COM_strncpy(entity->type, type, 8);
				entity->layer = layer;
				entity->texture = LoadTexture(imagefile);
				entity->CalculateMidpoint();
				entity->colRect.x = entity->pos.x;
				entity->colRect.y = entity->pos.y;
				entity->colRect.w = entity->dims.x;
				entity->colRect.h = entity->center.y;
				entity->tileID = 2;
				entities.Insert(entity);

				CreatePathNode(&path, Vec2(entity->pos.x + (entity->dims.x * 0.5f), entity->pos.y - (tileMap.tileHeight * 0.5f)));
				break;

			case '3':
				entity = (Entity*)MemAllocName(sizeof(Entity), "floor");
				new (entity) Entity;
				entity->dims = Vec2((real32)tileMap.tileWidth, (real32)tileMap.tileHeight);
				entity->pos = Vec2((real32)x * entity->dims.x, (real32)y * entity->dims.y);
				COM_strncpy(entity->type, type, 8);
				entity->layer = layer;
				entity->texture = LoadTexture(imagefile);
				entity->CalculateMidpoint();
				entity->colRect.x = entity->pos.x;
				entity->colRect.y = entity->pos.y;
				entity->colRect.w = entity->dims.x;
				entity->colRect.h = entity->center.y;
				entity->tileID = 3;
				entities.Insert(entity);
				break;

			case '4':
				entity = (Entity*)MemAllocName(sizeof(Entity), "bush");
				new (entity) Entity;
				entity->dims = Vec2((real32)tileMap.tileWidth, (real32)tileMap.tileHeight);
				entity->pos = Vec2((real32)x * entity->dims.x, (real32)y * entity->dims.y);
				COM_strncpy(entity->type, "bush", 8);
				entity->layer = layer;
				entity->texture = LoadTexture(imagefile);
				entity->CalculateMidpoint();
				
				entity->tileID = 4;
				entities.Insert(entity);
				break;

			case '5':
				entity = (Entity*)MemAllocName(sizeof(Entity), "bush");
				new (entity) Entity;
				entity->dims = Vec2((real32)tileMap.tileWidth, (real32)tileMap.tileHeight);
				entity->pos = Vec2((real32)x * entity->dims.x, (real32)y * entity->dims.y);
				COM_strncpy(entity->type, "bush", 8);
				entity->layer = layer;
				entity->texture = LoadTexture(imagefile);
				entity->CalculateMidpoint();
				
				entity->tileID = 5;
				entities.Insert(entity);
				break;

			case '6':
				entity = (Entity*)MemAllocName(sizeof(Entity), "grass");
				new (entity) Entity;
				entity->dims = Vec2((real32)tileMap.tileWidth, (real32)tileMap.tileHeight);
				entity->pos = Vec2((real32)x * entity->dims.x, (real32)y * entity->dims.y);
				COM_strncpy(entity->type, "grass", 8);
				entity->layer = layer;
				entity->texture = LoadTexture(imagefile);
				entity->CalculateMidpoint();
				
				entity->tileID = 6;
				entities.Insert(entity);
				break;

			case '7':
				entity = (Entity*)MemAllocName(sizeof(Entity), "waterT");
				new (entity) Entity;
				entity->dims = Vec2((real32)tileMap.tileWidth, (real32)tileMap.tileHeight);
				entity->pos = Vec2((real32)x * entity->dims.x, (real32)y * entity->dims.y);
				COM_strncpy(entity->type, "waterT", 8);
				entity->layer = layer;
				entity->texture = LoadTexture(imagefile);
				entity->CalculateMidpoint();
				
				entity->tileID = 7;
				entities.Insert(entity);
				break;

			case '8':
				entity = (Entity*)MemAllocName(sizeof(Entity), "block");
				new (entity) Entity;
				entity->dims = Vec2((real32)tileMap.tileWidth, (real32)tileMap.tileHeight);
				entity->pos = Vec2((real32)x * entity->dims.x, (real32)y * entity->dims.y);
				COM_strncpy(entity->type, "block", 8);
				entity->layer = layer;
				entity->texture = LoadTexture(imagefile);	//TODO: should we use and lookup Texture ID?
				entity->CalculateMidpoint();
				entity->tileID = 8;
				entities.Insert(entity);
				break;

			case '9':
				entity = (Entity*)MemAllocName(sizeof(Entity), "waterF");
				new (entity) Entity;
				entity->dims = Vec2((real32)tileMap.tileWidth, (real32)tileMap.tileHeight);
				entity->pos = Vec2((real32)x * entity->dims.x, (real32)y * entity->dims.y);
				COM_strncpy(entity->type, "waterF", 8);
				entity->layer = layer;
				entity->texture = LoadTexture(imagefile);
				entity->CalculateMidpoint();
				
				entity->tileID = 9;
				entities.Insert(entity);
				break;

			case 'a':
				entity = (Entity*)MemAllocName(sizeof(Entity), "flower");
				new (entity) Entity;
				entity->dims = Vec2((real32)tileMap.tileWidth, (real32)tileMap.tileHeight);
				entity->pos = Vec2((real32)x * entity->dims.x, (real32)y * entity->dims.y);
				COM_strncpy(entity->type, "flower", 8);
				entity->layer = layer;
				entity->texture = LoadTexture(imagefile);
				entity->CalculateMidpoint();
				
				entity->tileID = 10;
				entities.Insert(entity);
				break;

			case 'b':
				entity = (Entity*)MemAllocName(sizeof(Entity), "spike");
				new (entity) Entity;
				entity->dims = Vec2((real32)tileMap.tileWidth, (real32)tileMap.tileHeight);
				entity->pos = Vec2((real32)x * entity->dims.x, (real32)y * entity->dims.y);
				COM_strncpy(entity->type, "spike", 8);
				entity->layer = layer;
				entity->texture = LoadTexture(imagefile);
				entity->CalculateMidpoint();
				entity->colRect.x = entity->pos.x;
				entity->colRect.y = entity->pos.y + entity->center.y;
				entity->colRect.w = entity->dims.x;
				entity->colRect.h = entity->dims.y;
				entity->tileID = 11;
				entities.Insert(entity);
				break;

			case 'c':
				entity = (Entity*)MemAllocName(sizeof(Entity), "flower");
				new (entity) Entity;
				entity->dims = Vec2((real32)tileMap.tileWidth, (real32)tileMap.tileHeight);
				entity->pos = Vec2((real32)x * entity->dims.x, (real32)y * entity->dims.y);
				COM_strncpy(entity->type, "flower", 8);
				entity->layer = layer;
				entity->texture = LoadTexture(imagefile);
				entity->CalculateMidpoint();
				
				entity->tileID = 12;
				entities.Insert(entity);
				break;

			case 'd':
				entity = (Entity*)MemAllocName(sizeof(Entity), "grass");
				new (entity) Entity;
				entity->dims = Vec2((real32)tileMap.tileWidth, (real32)tileMap.tileHeight);
				entity->pos = Vec2((real32)x * entity->dims.x, (real32)y * entity->dims.y);
				COM_strncpy(entity->type, "grass", 8);
				entity->layer = layer;
				entity->texture = LoadTexture(imagefile);
				entity->CalculateMidpoint();
				
				entity->tileID = 13;
				entities.Insert(entity);
				break;

			case 'p':
				playerPos = Vec2((real32)x, (real32)y);
				break;

			case 'n':
				if(enemyIndex < numEnemies)
				{
					enemyPos[enemyIndex++] = Vec2((real32)x * tileMap.tileWidth, (real32)y * tileMap.tileHeight);
				}
				break;

			default:
				break;
			}

			x++;
		}

		y++;

		if(levelFile.fail())
			continue;
	}

	levelFile.close();

	/// Caluculate connections
	for( int i = 0; i < path.GetCount(); i++)
	{
		PathNode* n1 = path.GetByIndex(i);

		for( int j = i + 1; j < path.GetCount(); j++)
		{
			PathNode* n2 = path.GetByIndex(j);

			if(Distance(n1->pos, n2->pos) <= tileMap.tileWidth)
			{
				n1->next = n2;
				n2->prev = n1;
			}
		}
	}

	for( int i = 0; i < path.GetCount(); i++)
	{
		PathNode* n1 = path.GetByIndex(i);
		if(!n1->next && !n1->prev)
		{
			path.RemoveItem(i--);
		}
	}

	COM_print("\nLOAD LEVEL FILE END\n");

	/// CONFIG FILE LOADING

	ifstream file;
	file.open(levelDataFile);

	if(file.fail())
		return;
	
	int8 content[MAX_PATH];
	int bgIndex = 0;

	COM_print("\nLOAD LEVEL DATA FILE START\n");

	enemyIndex = 0;

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
		COM_strncpy(obj.name, content, 10);
		file >> obj.type;
		file >> obj.layer;
		file >> obj.filename;
		file >> obj.pos.x;
		file >> obj.pos.y;
		file >> obj.dims.x;
		file >> obj.dims.y;
		
		//Allocate Sprite into memory block and name that allocation what ever the sprite's name is.
		Entity* temp = 0;

		//if it is player type
		if(!COM_strcmp(obj.type, "player"))
		{
			temp = (Player*)MemAllocName(sizeof(Player), obj.name);
			player = (Player*)temp;	//Isolate player
			new(player) Player;	//Initialize the class constructor.
			temp->pos = Vec2(playerPos.x * tileMap.tileWidth, playerPos.y * tileMap.tileHeight);
			loadAnimations(player, obj.filename);
		}
		else if(!COM_strcmp(obj.type, "enemy") && enemyIndex < numEnemies)
		{
			PathNode* target = 0;
			for(int i = 0; i < path.GetCount(); i++)
			{
				if(Distance(enemyPos[enemyIndex], path.GetByIndex(i)->pos) <= tileMap.tileWidth)
				{
					target = path.GetByIndex(i);
					break;
				}
			}

			temp = (Enemy*)MemAllocName(sizeof(Enemy), obj.name);
			new(temp) Enemy;	//Initialize the class constructor.
			Enemy* e = (Enemy*)temp;
			e->target = target;
			temp->pos = Vec2(enemyPos[enemyIndex].x, enemyPos[enemyIndex].y);
			temp->texture = LoadTexture(obj.filename);
			enemyIndex++;
		}
		else
		{
			temp = (Entity*)MemAllocName(sizeof(Entity), obj.name);
			new(temp) Entity;	//Initialize the class constructor.
			temp->pos = Vec2(obj.pos.x, obj.pos.y);
			temp->texture = LoadTexture(obj.filename);
		}
		
		//
		temp->layer = obj.layer;
		COM_strncpy(temp->type, obj.type, 8);	//Copy only the first 8 bytes of the string.
		
		//if BG type
		if(!COM_strcmp(obj.type, "bg"))
		{
			bg[bgIndex] = temp;
			temp->dims = Vec2(tileMap.width * tileMap.tileWidth/2, tileMap.height * tileMap.tileHeight);	//Fill screen

			if(bgIndex)
			{
				bg[bgIndex]->pos = Vec2(tileMap.width * tileMap.tileWidth/2, 0);
			}

			bgIndex++;
		}
		else
		{
			temp->dims = Vec2((real32)(tileMap.tileWidth * 0.75f), (real32)(tileMap.tileHeight * 0.75f));	//Otherwise just set regular size
		}

		temp->CalculateMidpoint();

		entities.Insert(temp);	//insert into list
		
		if(file.fail())
			continue;
	}
	
	if(enemyPos)
	{
		delete[] enemyPos;
	}

	COM_print("\nLOAD CONFIG FILE END\n");

	file.close();	//Close file

	levelLoaded = true;
}

void GameManager::UnloadContent()
{
	if(levelLoaded)
	{
		player->anim.clear();

		uint32 highMark = CalculateHighMark();
		FreeToHighMark(highMark);

		uint32 lowMark = CalculateLowMark();
		FreeToLowMark(lowMark);
		
		entities.Clear();
		resourceManager.Clear();
		path.Clear();
		player = 0;
		
		levelLoaded = false;
	}
	
}

DWORD elapsed = 0;

//Starts the application
void GameManager::Run()
{
	// Init systems
	if(!Init())
		return;

	glfInit();
	glfFindFiles();

	MemInfo();
	MemCheck();
	MemCheckHigh();

	// Run app
	isRunning = true;

	console.isConsoleActive = false;
	console.showDebug = SHOW_DEBUG_NONE;
	console.layerFlags = DEBUG_LAYERS_PLAYER;
	*console.input = 0;

	SDL_StartTextInput();

	ZeroMemory(&state, sizeof(State));
	
	state.GameMemBlock = GetMemBlock();
	state.TotalSize = GetTotalSize();

	int monitorHz = 60;
	float gameUpdateHz = (monitorHz / 2.0f);
	float targetSecondsPerFrame = 1.0f / gameUpdateHz;
	float deltaTime = targetSecondsPerFrame;

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

		DWORD start = GetTickCount();
		
		if(state.inputRecordIndex)
		{
			RecordInput(&state, &input);
		}

		if(state.inputPlayIndex)
		{
			PlaybackInput(&state, &input);
		}

		Update(deltaTime);	//Update logic
		Render();	//Render logic

		DWORD end = GetTickCount();
		elapsed = ((end - start));
		
		//cout << "MS: " << (float)elapsed/1000 << endl;
	}
}

//TODO: create a world structure that holds level data
real32 gravity = -2.5f;
bool isResetNeeded = false;
bool isReloadNeeded = false;

vec2 safeArea = {0};

// Update logic procedures
void GameManager::Update(real32 dt)
{
	GUIUpdateButton(&showInfo, &input.mouse);
	//GUIUpdateButton(&memReset, &input.mouse);
	//GUIUpdateButton(&reloadContent, &input.mouse);

	if(isResetNeeded)
	{
		if(levelLoaded)
		{
			UnloadContent();
		}

		if(isReloadNeeded)
		{
			//LoadContent();

			isResetNeeded = false;
			isReloadNeeded = false;
		}
	}

	if(entities.GetCount() > 0)
	{
		safeArea = Vec2(1 * tileMap.tileWidth, 6 * tileMap.tileWidth);

#if _DEBUG
		//Show Debug Toggle (DEBUG ONLY)
		if(console.showDebug != SHOW_DEBUG_NONE && console.showDebug ^ SHOW_DEBUG_CMD)
		{
			if(input.mouse.leftButton.isDown)
			{
				player->pos.x = input.mouse.pos.x - player->center.x + 16;
				player->pos.y = input.mouse.pos.y - player->center.y;
			}
		}

		if(console.showDebug & SHOW_DEBUG_CMD)
		{
			ShowWindow( GetConsoleWindow(), SW_SHOW );
		}
		else
		{
			ShowWindow( GetConsoleWindow(), SW_HIDE );
		}
#endif
		player->dir = Vec2(0, 0);

		if(player->isAlive)
		{
			//player movement
			if(input.actionLeft.isDown)
			{
				player->dir.x = -1;
			}
			else if(input.actionRight.isDown)
			{
				player->dir.x = 1;
			}
		}

		//Jump code
		if(input.actionUp.isDown && player->isGrounded && !player->isJumping)
		{
			player->isJumping = true;
		}

		if(player->isJumping)
		{
			player->jumpTime = 1.0f;
			player->state = "jump";
			player->isJumping = false;
		}

		if(player->jumpTime > 0)
		{
			player->isGrounded = false;
			player->dir.y -= 2.0f * PI *sin(player->jumpTime);
			player->jumpTime -= dt;
		}
		
		player->dir.y -= gravity;

		player->Move(dt);
		player->Update(dt);

		//Update background on scroll
		for(int i = 0; i < 2; i++)
		{
			if(bg[i]->pos.x + bg[i]->dims.x < 0)
			{
				bg[i]->pos.x += 2.0f * bg[i]->dims.x;
			}
			else if(bg[i]->pos.x > tileMap.width * tileMap.tileWidth/2)
			{
				bg[i]->pos.x -= 2.0f * bg[i]->dims.x;
			}
		}

		for(uint32 i = 0; i < entities.GetCount(); i++)
		{
			//HANDLE SCROLLING
			if(player->dir.x > 0)	//PLAYER RIGHT
			{	
				if(i < path.GetCount())
				{
					path.GetByIndex(i)->pos.x -= player->speed * dt;
				}

				entities.GetByIndex(i)->pos.x -= player->speed * dt;
			}
			else if(player->dir.x < 0)	//PLAYER LEFT
			{
				if(i < path.GetCount())
				{
					path.GetByIndex(i)->pos.x += player->speed * dt;
				}

				entities.GetByIndex(i)->pos.x += player->speed * dt;
			}

			entities.GetByIndex(i)->Update(dt);

			for(int j = i + 1; j < entities.GetCount(); j++)
			{
				if(!COM_strcmp(entities.GetByIndex(i)->type, "bg") || !COM_strcmp(entities.GetByIndex(j)->type, "bg"))
				{
					continue;
				}

				Entity* obj = (Entity*)entities.GetByIndex(i);
				Entity* obj2 = (Entity*)entities.GetByIndex(j);

				//TODO(George): Perform Object Culling for faster performance.
				if(obj->layer == obj2->layer)
				{
					//Collision Testing
					if(obj->CollisionAABB(obj2))
					{
						obj->HandleCollision(obj2);
					}
					else if(obj2->CollisionAABB(obj))
					{
						obj2->HandleCollision(obj);
					}
				}
				else if(!COM_strcmp(obj->type, "floor") || !COM_strcmp(obj->type, "spike"))
				{
					if(Distance(player->pos, obj->pos) <= player->dims.y + 40)
					{
						if(player->pos.y + player->center.y < obj->pos.y)
						{
							player->layer = obj->layer;
						}
					}

					if(!COM_strcmp(obj2->type, "enemy"))
					{
						if(Distance(obj2->pos, obj->pos) <= obj2->dims.y + 40)
						{
							if(obj2->pos.y + obj2->center.y < obj->pos.y)
							{
								obj2->layer = obj->layer;
							}
						}
					}
				}
			}
		}
	}
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

		return false;
	}
};

bool showGuiInfo = false;

void ShowInfo(void)
{
	showGuiInfo = !showGuiInfo;
}

void FreeContent(void)
{
	isResetNeeded = true;
}

void ReloadContent(void)
{
	if(!isResetNeeded)
	{
		isResetNeeded = true;
	}

	isReloadNeeded = true;
}

void GameManager::OnGUI(void)
{
	//TODO: create GUI elements (text, buttons, panels, windows, etc.)
	SDL_Color color;
	color.a = 255;
	color.r = 255;
	color.g = 50;
	color.b = 50;

	uint32 xOffset = SCREEN_WIDTH - 150;
	uint32 yOffset = 10;

	CreateGUIButton(&showInfo, xOffset, yOffset, StringToTexture(renderer, "show info", color), ShowInfo);
	DrawGUIButton(renderer, &showInfo);

	yOffset += showInfo.border.h + 10;

	CreateGUIButton(&memReset, xOffset, yOffset, StringToTexture(renderer, "free content", color), FreeContent);
	//DrawGUIButton(renderer, &memReset);
	
	yOffset += memReset.border.h + 10;

	CreateGUIButton(&reloadContent, xOffset, yOffset, StringToTexture(renderer, "reload content", color), ReloadContent);
	//DrawGUIButton(renderer, &reloadContent);

	GUIText txtState;
	
	color.a = 255;
	color.r = 255;
	color.g = 255;
	color.b = 255;

	txtState.pos = Vec2(10, SCREEN_HEIGHT - 25);

	if(state.inputRecordIndex)
	{
		txtState.texture = StringToTexture(renderer, "state: RECORDING...", color);
	}
	else if(state.inputPlayIndex)
	{
		txtState.texture = StringToTexture(renderer, "state: PLAYBACK...", color);
	}
	else if(!state.inputPlayIndex && !state.inputRecordIndex)
	{
		txtState.texture = StringToTexture(renderer, "state: NORMAL...", color);
	}

	DrawGUIText(renderer, &txtState);

	if(!levelLoaded)
	{
		color.a = 255;
		color.r = 255;
		color.g = 50;
		color.b = 50;
		GUIText text = {0};
	
		int8 data[256] = "use the command line to load levels and unload levels.";

		text.pos = Vec2(100, 100);
		text.texture = StringToTexture(renderer, data, color);
		DrawGUIText(renderer, &text);

		float offsety = text.pos.y + text.dims.y;
		ZeroMemory(data, sizeof(data));
		COM_strcpy(data, "Load levels: /load_level [filename] (ex. /load_level level1.glf)");
		text.pos = Vec2(100, offsety);
		text.texture = StringToTexture(renderer, data, color);
		DrawGUIText(renderer, &text);

		offsety = text.pos.y + text.dims.y;
		ZeroMemory(data, sizeof(data));
		COM_strcpy(data, "unload current level: /unload_level");
		text.pos = Vec2(100, offsety);
		text.texture = StringToTexture(renderer, data, color);
		DrawGUIText(renderer, &text);

		offsety = text.pos.y + text.dims.y;
		ZeroMemory(data, sizeof(data));
		COM_strcpy(data, "list available levels: /load_level show");
		text.pos = Vec2(100, offsety);
		text.texture = StringToTexture(renderer, data, color);
		DrawGUIText(renderer, &text);

		offsety = text.pos.y + text.dims.y;
		ZeroMemory(data, sizeof(data));
		COM_strcpy(data, "A, D: move player left or right");
		text.pos = Vec2(100, offsety);
		text.texture = StringToTexture(renderer, data, color);
		DrawGUIText(renderer, &text);

		offsety = text.pos.y + text.dims.y;
		ZeroMemory(data, sizeof(data));
		COM_strcpy(data, "SPACE: Jump");
		text.pos = Vec2(100, offsety);
		text.texture = StringToTexture(renderer, data, color);
		DrawGUIText(renderer, &text);

		offsety = text.pos.y + text.dims.y;
		ZeroMemory(data, sizeof(data));
		COM_strcpy(data, "Enter, '/': enables commadn line (use /help for a list of commands)");
		text.pos = Vec2(100, offsety);
		text.texture = StringToTexture(renderer, data, color);
		DrawGUIText(renderer, &text);
	}
	else
	{
		color.a = 255;
		color.r = 0;
		color.g = 0;
		color.b = 0;
		GUIText lives = {0};
	
		int8 data[256] = "Lives: ";
		int8 buffer[16] = {0};

		if(player)
		{
			COM_strcat(data, itoa(player->lives, buffer, 10));
		}
		lives.pos = Vec2(10, 10);
		lives.texture = StringToTexture(renderer, data, color);
		DrawGUIText(renderer, &lives);
	}

	if(showGuiInfo)
	{
		color.a = 255;
		color.r = 255;
		color.g = 50;
		color.b = 50;
		GUIText text;
	
		int8 data[256] = "Current layer: ";
		int8 buffer[16] = {0};

		vec2 tilePos = WorldPosToTilePos(input.mouse.pos, &tileMap);
		uint32 TileID = GetTileID(tilePos, &tileMap);

		if(player)
		{
			COM_strcat(data, itoa(player->layer, buffer, 10));
			text.texture = StringToTexture(renderer, data, color);
			text.pos = Vec2(10, 10);
			DrawGUIText(renderer, &text);
		}

		ZeroMemory(data, sizeof(data));

		COM_strcat(data, "Game Mem Used: ");
		COM_strcat(data, itoa(MemGetUsedSize(), buffer, 10));
		COM_strcat(data, " bytes");
		text.texture = StringToTexture(renderer, data, color);
		text.pos = Vec2(10, 30);
		DrawGUIText(renderer, &text);

		ZeroMemory(data, sizeof(data));

		COM_strcat(data, "Low Mem Size: ");
		COM_strcat(data, itoa(MemGetSizeLow(), buffer, 10));
		COM_strcat(data, " bytes");
		text.texture = StringToTexture(renderer, data, color);
		text.pos = Vec2(10, 50);
		DrawGUIText(renderer, &text);

		ZeroMemory(data, sizeof(data));

		COM_strcat(data, "Hi Mem Size: ");
		COM_strcat(data, itoa(MemGetSizeHigh(), buffer, 10));
		COM_strcat(data, " bytes");
		text.texture = StringToTexture(renderer, data, color);
		text.pos = Vec2(10, 70);
		DrawGUIText(renderer, &text);

		ZeroMemory(data, sizeof(data));

		COM_strcat(data, "Tile Pos: ");
		COM_strcat(data, itoa((int)tilePos.x, buffer, 10));
		COM_strcat(data, ", ");
		COM_strcat(data, itoa((int)tilePos.y, buffer, 10));
		text.texture = StringToTexture(renderer, data, color);
		text.pos = Vec2(SCREEN_WIDTH / 2, 10);
		DrawGUIText(renderer, &text);

		ZeroMemory(data, sizeof(data));

		COM_strcat(data, "Tile ID: ");
		COM_strcat(data, itoa(TileID, buffer, 10));
		text.texture = StringToTexture(renderer, data, color);
		text.pos = Vec2(SCREEN_WIDTH / 2, 30);
		DrawGUIText(renderer, &text);

		ZeroMemory(data, sizeof(data));

		COM_strcat(data, "Mouse Pos: ");
		COM_strcat(data, itoa(input.mouse.pos.x, buffer, 10));
		COM_strcat(data, ", ");
		COM_strcat(data, itoa(input.mouse.pos.y, buffer, 10));
		text.texture = StringToTexture(renderer, data, color);
		text.pos = Vec2(SCREEN_WIDTH / 2, 50);
		DrawGUIText(renderer, &text);

		ZeroMemory(data, sizeof(data));

		COM_strcat(data, "MS: ");
		COM_strcat(data, itoa(elapsed, buffer, 10));
		text.texture = StringToTexture(renderer, data, color);
		text.pos = Vec2(SCREEN_WIDTH / 2, 70);
		DrawGUIText(renderer, &text);
	}
}

//Render procedure
void GameManager::Render()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
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
		
		if(console.showDebug & SHOW_DEBUG_LAYERS)
		{
			if(console.layerFlags & DEBUG_LAYERS_ONE)
			{
				if(obj.obj->layer == 1)
				{
					obj.obj->Draw(renderer);
				}
			}
			else if(console.layerFlags & DEBUG_LAYERS_TWO)
			{
				if(obj.obj->layer == 2)
				{
					obj.obj->Draw(renderer);
				}
			}
			else if(console.layerFlags & DEBUG_LAYERS_THREE)
			{
				if(obj.obj->layer == 3)
				{
					obj.obj->Draw(renderer);
				}
			}
			else if(console.layerFlags & DEBUG_LAYERS_FOUR)
			{
				if(obj.obj->layer == 4)
				{
					obj.obj->Draw(renderer);
				}
			}
			else
			{
				if(obj.obj->layer == player->layer)
				{
					obj.obj->Draw(renderer);
				}
			}
		}
		else
		{
			obj.obj->Draw(renderer);		
		}
	}

	if(console.showDebug & SHOW_DEBUG_GRID)
	{
		for(int y = 0; y < 6; y++)
		{
			for(int x = 0; x < 8; x++)
			{
				vec2 tilePos = WorldPosToTilePos(input.mouse.pos, &tileMap);
				
				SDL_Rect rect;
				rect.x = x * tileMap.tileWidth;
				rect.y = y * tileMap.tileHeight;
				rect.w = tileMap.tileWidth;
				rect.h = tileMap.tileHeight;

				if(player)
				{
					vec2 center = Vec2(player->pos.x + player->center.x, player->pos.y + player->center.y);
					vec2 p = WorldPosToTilePos(center, &tileMap);
									
					if((int)p.x == x && (int)p.y == y)
					{
						SDL_SetRenderDrawColor(renderer, 0, 255, 0, 50);
						SDL_RenderFillRect(renderer, &rect);
					}
				}

				if((int)tilePos.x == x && (int)tilePos.y == y)
				{
					SDL_SetRenderDrawColor(renderer, 255, 0, 0, 50);
					SDL_RenderFillRect(renderer, &rect);
				}
				else
				{
					SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
					SDL_RenderDrawRect(renderer, &rect);
				}
			}
		}
	}
	
	if(console.showDebug & SHOW_DEBUG_PATH)
	{
		for(int i = 0; i < path.GetCount(); i++)
		{
			PathNode* n = path.GetByIndex(i);
			SDL_Rect rect = {0};
			rect.x = n->pos.x;
			rect.y = n->pos.y;
			rect.w = 10;
			rect.h = 10;

			SDL_SetRenderDrawColor(renderer, 10, 10, 10, 255);
			SDL_RenderFillRect(renderer, &rect);

			if(n->next)
			{
				PathNode* n2 = n->next;

				SDL_SetRenderDrawColor(renderer, 10, 10, 10, 255);
				SDL_RenderDrawLine(renderer, n->pos.x+5, n->pos.y+5, n2->pos.x+5, n2->pos.y+5);
			}
		}
	}

	if(console.showDebug & SHOW_DEBUG_COLISION)
	{
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100);
		for(uint32 i = 0; i < entities.GetCount(); i++)
		{
			SDL_RenderFillRect(renderer, &entities.GetByIndex(i)->colRect);
		}
	}

	OnGUI();

	if(console.isConsoleActive)
	{
		SDL_Color color;
		color.a = 255;
		color.r = 255;
		color.g = 255;
		color.b = 255;

		uint32 size = COM_strlen(console.input);

		if(size > 0)
		{
			DrawConsole(renderer, StringToTexture(renderer, console.input, color));
		}
		else
		{
			DrawConsole(renderer, StringToTexture(renderer, " ", color));
		}
	}

	SDL_RenderPresent(renderer);	//Send to screen and draw scene
}

// Cleanup procedure (frees used memory)
void GameManager::Cleanup()
{
	SDL_StopTextInput();

	ClearInputPlayback();

	if(font)
	{
		TTF_CloseFont(font);
		font = 0;
	}

	//free renderer
	if(renderer)
	{
		SDL_DestroyRenderer(renderer);
	}
	
	if(player)
	{
		player->anim.clear();
	}

	entities.Clear();
	path.Clear();
	resourceManager.Clear();

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