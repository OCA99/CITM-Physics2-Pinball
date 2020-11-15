#pragma once

#include "Module.h"
#include "Application.h"
#include "SDL\include\SDL_pixels.h"

#define MAX_FONTS 10
#define MAX_FONT_CHARS 256
#define DYNAMIC_TEXT_LEN 6

struct SDL_Texture;

struct Font
{
	// Lookup table. All characters displayed in the same order as the texture
	char table[MAX_FONT_CHARS];

	// The font texture
	SDL_Texture* texture = nullptr;

	// Font setup data
	uint totalLength;
	uint rows, columns;
	uint char_w, char_h;
};

class ModuleInterface : public Module
{
public:

	bool Start();

	// Constructor
	ModuleInterface(Application* app, bool start_enabled = true);

	// Destructor
	~ModuleInterface();

	update_status PostUpdate();

	void IntToString(char* buffer, int k);

	// Loads a font file from a texture
	// Returns a font index from the fonts array
	//Param texturePath	- The path to the texture file
	// Param characters		- The lookup table. All characters displayed in the same order as the texture
	// Param rows			- The amount of character rows in the texture
	int LoadFont(const char* texturePath, const char* characters, uint rows = 1);

	// Removes a font by its index
	// Unloads the texture and removes it from the fonts array
	void UnLoadFont(int fontIndex);

	// Create a surface from text
	void BlitText(int x, int y, int fontIndex, const char* text) const;

	//Show GameOver
	void ShowGameOver();


	//text
	const char* text = { "SCORE" };
	int score = 0;
	int mainFont = -1;
	int numberFont = -1;
	SDL_Rect blackRect;

	//GameOver Window
	bool showWindow = false;
	int windowTime = 350;
	int curWinTime = 0;

private:
	// An array to keep track and store all loaded fonts
	Font fonts[MAX_FONTS];
};
