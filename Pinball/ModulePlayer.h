#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"
#include "ModuleTextures.h"
#include "ModulePhysics.h"

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

private:
	PhysBody* leftFlipperBody;
	PhysBody* rightFlipperBody;
	PhysBody* plunge;
	SDL_Rect leftFlipperRect;
	SDL_Rect rightFlipperRect;


	SDL_Texture* texture;

	p2Point<int> leftFlipperPosition;
	p2Point<int> rightFlipperPosition;
	p2Point<int> leftFlipperPivot;
	p2Point<int> rightFlipperPivot;
};