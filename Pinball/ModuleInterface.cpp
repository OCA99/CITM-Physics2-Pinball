
#include "Application.h"

#include "ModuleTextures.h"
#include "ModuleRender.h"
#include "ModuleInterface.h"
#include "ModuleSceneIntro.h"

#include<string.h>

ModuleInterface::ModuleInterface(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

ModuleInterface::~ModuleInterface()
{

}

bool ModuleInterface::Start() 
{
	// Load fonts
	mainFont = LoadFont("pinball/ClearFont.png", " ABCDEFGHIJKLMNOPQRSTUVWXYZ", 1);
	numberFont = LoadFont("pinball/BlueFont.png", "0123456789", 1);

	// black rectangle
	blackRect.x = 0;
	blackRect.y = 278;
	blackRect.w = SCREEN_WIDTH;
	blackRect.h = 12;
	

	return true;
}

update_status ModuleInterface::PostUpdate() 
{
	char scoreChar[DYNAMIC_TEXT_LEN + 1] = { "000000" };
	if (score != 0) {
	
		IntToString(scoreChar, score);
	}

	//black rectangle blit
	App->renderer->DrawQuad(blackRect, 0, 0, 0, 255, true);

	//text blit
	BlitText(4, 278, mainFont, text);
	BlitText(140, 278, numberFont, scoreChar);

	if (showWindow) {
		curWinTime--;
		if (curWinTime <= 0) {
			showWindow = false;
			App->scene_intro->lifes = 2;
		}
		App->renderer->DrawQuad(SDL_Rect({ 45,104,70,50 }), 0, 0, 0);
		BlitText(55, 137, numberFont, scoreChar);
		BlitText(59, 106, mainFont, "GAME");
		BlitText(54, 122, mainFont, "OVER");
	}

	return UPDATE_CONTINUE;

}

void ModuleInterface::IntToString(char* buffer, int k) {

	for (int i = 0; i < DYNAMIC_TEXT_LEN; i++) {
		buffer[i] = '0';
	}

	buffer[DYNAMIC_TEXT_LEN] = 0;

	int i = DYNAMIC_TEXT_LEN - 1;
	while (k != 0) {
		if (i < 0) break;
		buffer[i--] += k % 10;
		k /= 10;
	}
}

// Load new texture from file path
int ModuleInterface::LoadFont(const char* texture_path, const char* characters, uint rows)
{
	int id = -1;

	if (texture_path == nullptr || characters == nullptr || rows == 0)
	{
		LOG("Could not load font");
		return id;
	}

	SDL_Texture* tex = App->textures->Load(texture_path);

	if (tex == nullptr || strlen(characters) >= MAX_FONT_CHARS)
	{
		LOG("Could not load font at %s with characters '%s'", texture_path, characters);
		return id;
	}

	id = 0;
	for (; id < MAX_FONTS; ++id)
		if (fonts[id].texture == nullptr)
			break;

	if (id == MAX_FONTS)
	{
		LOG("Cannot load font %s. Array is full (max %d).", texture_path, MAX_FONTS);
		return id;
	}

	Font& font = fonts[id];

	font.texture = tex;
	font.rows = rows;

	// TODO 1: Finish storing font data

	// totalLength ---	length of the lookup table
	// table ---------  All characters displayed in the same order as the texture
	// columns -------  Amount of chars per row of the texture
	// char_w --------	Width of each character
	// char_h --------	Height of each character

	strcpy_s(fonts[id].table, MAX_FONT_CHARS, characters);
	font.totalLength = strlen(characters);
	font.columns = fonts[id].totalLength / rows;

	uint tex_w, tex_h;
	App->textures->GetTextureSize(tex, tex_w, tex_h);
	font.char_w = tex_w / font.columns;
	font.char_h = tex_h / font.rows;

	LOG("Successfully loaded BMP font from %s", texture_path);

	return id;
}

void ModuleInterface::UnLoadFont(int font_id)
{
	if (font_id >= 0 && font_id < MAX_FONTS && fonts[font_id].texture != nullptr)
	{
		App->textures->Unload(fonts[font_id].texture);
		fonts[font_id].texture = nullptr;
		LOG("Successfully Unloaded BMP font_id %d", font_id);
	}
}

void ModuleInterface::BlitText(int x, int y, int font_id, const char* text) const
{
	if (text == nullptr || font_id < 0 || font_id >= MAX_FONTS || fonts[font_id].texture == nullptr)
	{
		//LOG("Unable to render text with bmp font id %d", font_id);
		return;
	}

	const Font* font = &fonts[font_id];
	SDL_Rect spriteRect;
	uint len = strlen(text);

	spriteRect.w = font->char_w;
	spriteRect.h = font->char_h;

	for (uint i = 0; i < len; ++i)
	{
		// TODO 2: Find the character in the table and its position in the texture, then Blit
		uint charIndex = 0;

		// Find the location of the current character in the lookup table
		for (uint j = 0; j < font->totalLength; ++j)
		{
			if (font->table[j] == text[i])
			{
				charIndex = j;
				break;
			}
		}

		// Retrieve the position of the current character in the sprite
		spriteRect.x = spriteRect.w * (charIndex % font->columns);
		spriteRect.y = spriteRect.h * (charIndex / font->columns);

		App->renderer->Blit(font->texture, x, y, &spriteRect, 0.0f);

		// Advance the position where we blit the next character
		x += spriteRect.w;
	}
}

void ModuleInterface::ShowGameOver()
{
	showWindow = true;
	curWinTime = windowTime;
}
