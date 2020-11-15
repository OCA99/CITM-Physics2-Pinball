#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"
#include "Animation.h"

class PhysBody;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();
	SDL_Texture* background;
	SDL_Texture* objectsTexture;


	SDL_Rect backRect;
	SDL_Rect ballRect;
	SDL_Rect leftPikaRect;
	SDL_Rect rightPikaRect;

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
	PhysBody* deathSensor;

	PhysBody* pikaLeftB;
	PhysBody* pikaRightB;

	Animation pikaAnimation;

	float maxSpeed = 10.0f;
	
	int lifes = 2;

	p2List<PhysBody*> balls;

	bool Start();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	void CreateWalls();
	void CreateBall();
	void ResetBall(PhysBody *ball);
	void CreateBallInMousePos();
	void CheckLife();

public:


private:
	bool waitingForBallReset = false;
	PhysBody* ballWaitingForReset = nullptr;
};
