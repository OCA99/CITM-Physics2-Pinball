
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

	textures = App->textures->Load("pinball/sprites.png");
	ballR = SDL_Rect({ 165, 95, 7, 7 });

	// black rectangle
	blackRect.x = 0;
	blackRect.y = 278;
	blackRect.w = SCREEN_WIDTH;
	blackRect.h = 12;
	

	return true;
}

update_status ModuleInterface::PostUpdate() 
{

	DrawLifes();

	if (!showWindow) 
	{
		IntToString(scoreChar, score);
	}
	
	if (score >= highScore)
	{
		highScore = score;
		IntToString(maxScoreChar, highScore);
	}

	IntToString(lastScoreChar, lastScore);

	//black rectangle blit
	App->renderer->DrawQuad(blackRect, 0, 0, 0, 255, true);

	//text blit
	BlitText(4, 279, mainFont, text);
	BlitText(140, 279, numberFont, scoreChar);

	BlitText(4, 291, mainFont, "HIGH SCORE");
	BlitText(140, 291, numberFont, maxScoreChar);

	BlitText(4, 303, mainFont, "LAST SCORE");
	BlitText(140, 303, numberFont, lastScoreChar);

	if (showWindow) {

		curWinTime--;
		if (curWinTime <= 0) {
			showWindow = false;
			App->scene_intro->lifes = 2;
			lastScore = score;
			score = 0;
		}

		App->renderer->DrawQuad(SDL_Rect({ 41,99,79,59 }), 0, 0, 0);
		App->renderer->DrawQuad(SDL_Rect({ 42,100,77,57 }), 255, 255, 255);
		App->renderer->DrawQuad(SDL_Rect({ 45,104,70,50 }), 0, 0, 0);
		BlitText(55, 137, numberFont, scoreChar);
		BlitText(59, 106, mainFont, "GAME");
		BlitText(59, 122, mainFont, "OVER");
		
	}

	


	return UPDATE_CONTINUE;

}

void ModuleInterface::DrawLifes() {
	if (App->scene_intro->lifes == 2) 
	{
		App->renderer->Blit(textures, 180, 230,&ballR);
	}
	if (App->scene_intro->lifes >= 1)
	{
		App->renderer->Blit(textures, 180, 240, &ballR);
	}
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
