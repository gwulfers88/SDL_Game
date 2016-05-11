//Using SDL and standard IO
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>


//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


//The window we'll be rendering to
static SDL_Window*   g_window = NULL;
static SDL_Renderer* g_renderer = NULL;
static SDL_Texture*  g_texture = NULL;
static SDL_Rect      g_offset = {0,0,0,0};



//bool Init(int w, int h, int x, int y);
//SDL_Texture* LoadMedia(std::string filename);
//void Close();


/*
	Starts up SDL, creates window, binds render object to window
	w		- width of window
	h		- height of window
	x		- x-location of window on screen (default 0)
	y		- y-location of window on screen (default 0)
	return true on success, false otherwise
*/
bool Init(int w, int h, int x, int y)
{
	//Initialize SDL
	if( SDL_Init( SDL_INIT_EVERYTHING) < 0 )
		return false;

	//initialize png support, may also add IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF
	if ( IMG_Init( IMG_INIT_PNG ) == 0 )
		return false;

	//create window
	g_window = SDL_CreateWindow( "SDL Startup", x, y, w, h, SDL_WINDOW_SHOWN );
	if( g_window == NULL )
		return false;

	//create renderer, bind to window
	g_renderer = SDL_CreateRenderer( g_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if ( g_renderer == NULL)
		return false;

	return true;
}


/*
	Loads media image, binds to render object
	filename - filename (including directory location) if image to be loaded
	returns loaded texture on success, NULL otherwise
*/
SDL_Texture* LoadMedia(std::string filename)
{
	//load image
	SDL_Surface* surf = IMG_Load( filename.c_str() );

	//verify it was loaded
	if ( surf == NULL )
	{
		exit(404);	//shuts down program if image not found
		return NULL;
	}
	if ( g_renderer == NULL )
	{
		exit(101);
		return NULL;
	}

	//bind image to renderer
	g_texture = SDL_CreateTextureFromSurface( g_renderer, surf );
	SDL_FreeSurface( surf );

	return g_texture;
}
 

/*
	destroys all textures loaded, closes window/renderer, quits sdl
*/
void Close()
{
	//Deallocate texture(s)
	SDL_DestroyTexture( g_texture );
	g_texture = NULL;

	//Destroy renderer
	SDL_DestroyRenderer( g_renderer );
	g_renderer = NULL;

	//Destroy window
	SDL_DestroyWindow( g_window );
	g_window = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !Init(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0) )
		return -1;
	
	//load texture
	g_texture = LoadMedia("images/hello_world.bmp");
	
	//set pos, size of image for rendering purposes
	g_offset.x = 0;
	g_offset.y = 0;
	SDL_QueryTexture(g_texture, NULL, NULL, &g_offset.w, &g_offset.h);

	//render loop
	//only have it run for 100 frames, normally have it run indefinitely
	int i=0;
	while (i<100)	
	{
		//game logic update
		g_offset.x++;

		//clear screen
		SDL_RenderClear( g_renderer );

		//populate with images
		SDL_RenderCopy( g_renderer, g_texture, NULL, &g_offset);

		//update screen
		SDL_RenderPresent( g_renderer );

		//Wait 17ms ~ 60fps
		SDL_Delay( 17 );

		i++;
	}

	//Free resources and close SDL
	Close();
	
	return 0;
}