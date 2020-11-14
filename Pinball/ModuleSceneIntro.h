#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();
	SDL_Texture* background;
	SDL_Texture* ballTexture;
	int font = -1;

	const char* text = { "HELLO WORLD" };

	SDL_Rect backRect;
	SDL_Rect ballRect;

	//physbodies

	PhysBody* outerWallB;
	PhysBody* upperMoonB;
	PhysBody* leftLB;
	PhysBody* rightLB;
	PhysBody* rightMediumWallB;
	PhysBody* rightBarB;
	PhysBody* diggletRB;
	PhysBody* diggletLB;
	PhysBody* leftSmallWallB;
	PhysBody* rightTinyWallB;
	PhysBody* leftTinyWallB;
	PhysBody* starWallB;
	PhysBody* rightTriangleB;
	PhysBody* leftTriangleB;
	PhysBody* leftTriangleBounceB;
	PhysBody* rightTriangleBounceB;

	float maxSpeed = 10.0f;

	p2List<PhysBody*> balls;

	bool Start();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	void CreateWalls();
	void CreateBallInMousePos();

public:

};
