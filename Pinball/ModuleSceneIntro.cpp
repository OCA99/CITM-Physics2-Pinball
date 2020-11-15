#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModuleInterface.h"
#include "Application.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	background = App->textures->Load("pinball/level.png");
	objectsTexture = App->textures->Load("pinball/sprites.png");



	ballRect = SDL_Rect({165, 103, 8, 8 });
	leftPikaRect = SDL_Rect({263, 117, 13, 14});
	rightPikaRect = SDL_Rect({276, 117, 13, 14});
	activeComboRect = SDL_Rect({ 191, 0, 6, 6 });

	pikaAnimation.PushBack(SDL_Rect({ 84, 270, 16, 16 }));
	pikaAnimation.PushBack(SDL_Rect({ 100, 270, 16, 16 }));
	pikaAnimation.PushBack(SDL_Rect({ 116, 270, 16, 16 }));
	pikaAnimation.loop = false;
	pikaAnimation.speed = 0.3f;

	CreateWalls();
	CreateBall();
	
	deathSensor = App->physics->CreateRectangleSensor(192/2, 290, 192, 14);
	deathSensor->type = COLLIDER_TYPE::DEATH;

	mainSong = App->audio->LoadFx("audio/music/Main_Theme.ogg");
	App->audio->PlayFx(mainSong,1);

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");
	
	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{

	if (waitingForBallReset) {
		waitingForBallReset = false;
		ResetBall(ballWaitingForReset);
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		CreateBallInMousePos();
	}

	p2List_item<PhysBody*>* ball_single = balls.getFirst();
	while (ball_single != NULL)
	{
		p2List_item<PhysBody*>* next = ball_single->next;
		if (ball_single->data->waitingForDelete) {
			if (ball_single->data->body != nullptr)
				App->physics->world->DestroyBody(ball_single->data->body);
			if (ball_single->data->body2 != nullptr)
				App->physics->world->DestroyBody(ball_single->data->body2);
			delete ball_single->data;
			balls.del(ball_single);
		}
		else
		{
			b2Vec2 vel = ball_single->data->body->GetLinearVelocity();
			float speed = vel.Normalize();
			if (speed > maxSpeed) {
				ball_single->data->body->SetLinearVelocity(maxSpeed * vel);
			}
		}

		ball_single = next;
	}



	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB) {

	PhysBody* c;

	if (bodyA == nullptr || bodyB == nullptr)
		return;

	if (bodyB->type == COLLIDER_TYPE::BALL) {
		c = bodyA;
		bodyA = bodyB;
		bodyB = c;
	}

	if (bodyA->type == COLLIDER_TYPE::BALL) {
		if (bodyB->type == COLLIDER_TYPE::DEATH) {
			if (bodyA->resettable) {
				waitingForBallReset = true;
				ballWaitingForReset = bodyA;
				CheckLife();
				App->audio->PlayFx(2, 0);
			}
			else {
				bodyA->waitingForDelete = true;
			}
		}

		if (bodyB->type == COLLIDER_TYPE::PIKA) {
			App->ui->score += 100;
			App->audio->PlayFx(8, 0);

		}
		if (bodyB->type == COLLIDER_TYPE::DIGLET) {
			App->ui->score += 50;
			App->audio->PlayFx(6, 0);
		}
		if (bodyB->type == COLLIDER_TYPE::TRIANGLE) {
			App->ui->score += 20;
			App->audio->PlayFx(7, 0);

		}

		if (bodyB->type == COLLIDER_TYPE::COMBO1) {
			comb1active = !comb1active;

			if (comb1active && comb2active && comb3active) {
				lifes++;
				if (lifes > 3) lifes = 3;
				comb1active = comb2active = comb3active = false;
			}
		}

		if (bodyB->type == COLLIDER_TYPE::COMBO2) {
			comb2active = !comb2active;

			if (comb1active && comb2active && comb3active) {
				lifes++;
				if (lifes > 3) lifes = 3;
				comb1active = comb2active = comb3active = false;
			}
		}

		if (bodyB->type == COLLIDER_TYPE::COMBO3) {
			comb3active = !comb3active;

			if (comb1active && comb2active && comb3active) {
				lifes++;
				if (lifes > 3) lifes = 3;
				comb1active = comb2active = comb3active = false;
			}
		}
	}
	
}

update_status ModuleSceneIntro::PostUpdate()
{
	App->renderer->Blit(background, 0, 0);
	

	//Render each ball Texture
	p2List_item<PhysBody*>* ball_single = balls.getFirst();
	while (ball_single != NULL)
	{
		int x, y;
		ball_single->data->GetPosition(x, y);

		App->renderer->Blit(objectsTexture, x, y, &ballRect);

		ball_single = ball_single->next;
	}

	App->renderer->Blit(objectsTexture, 8, 247, &leftPikaRect);
	App->renderer->Blit(objectsTexture, 139, 247, &rightPikaRect);
	App->renderer->Blit(objectsTexture, 160, 262, &pikaAnimation.GetCurrentFrame());

	if (comb1active) {
		App->renderer->Blit(objectsTexture, 55, 58, &activeComboRect);
	}
	if (comb2active) {
		App->renderer->Blit(objectsTexture, 77, 47, &activeComboRect);
	}
	if (comb3active) {
		App->renderer->Blit(objectsTexture, 99, 48, &activeComboRect);
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::CreateWalls() {

	int levelOutline[116] = {
	56, 278,
	0, 278,
	0, 0,
	192, 0,
	192, 278,
	173, 278,
	173, 84,
	172, 78,
	170, 71,
	168, 63,
	165, 57,
	163, 52,
	160, 47,
	156, 42,
	152, 37,
	148, 33,
	143, 28,
	137, 24,
	130, 20,
	124, 17,
	118, 15,
	110, 12,
	100, 10,
	88, 10,
	71, 11,
	61, 13,
	54, 15,
	48, 18,
	42, 21,
	38, 24,
	33, 28,
	28, 32,
	25, 32,
	25, 16,
	22, 13,
	18, 12,
	13, 13,
	9, 18,
	9, 101,
	10, 109,
	11, 118,
	13, 129,
	15, 135,
	17, 143,
	19, 150,
	21, 155,
	24, 161,
	28, 166,
	23, 173,
	22, 192,
	15, 195,
	11, 198,
	8, 202,
	7, 206,
	7, 261,
	21, 261,
	21, 255,
	23, 255,
	
	};

	outerWallB = App->physics->CreateStaticChain(0, 0, levelOutline, 116);

	int upperMoon[40] = {
	39, 42,
	46, 39,
	56, 35,
	67, 32,
	81, 31,
	91, 31,
	98, 33,
	105, 35,
	111, 37,
	116, 39,
	111, 33,
	105, 29,
	97, 25,
	89, 23,
	80, 22,
	69, 24,
	61, 26,
	55, 29,
	49, 33,
	43, 37,
	};

	upperMoonB = App->physics->CreateStaticChain(0, 0, upperMoon, 40);

	int leftL[16] = {
		52, 260,
		56, 257,
		55, 253,
		24, 231,
		24, 207,
		20, 207,
		20, 241,
		48, 259
	};

	leftLB = App->physics->CreateStaticChain(0, 0, leftL, 16);
	

	int rightL[14] = {
		104, 257,
		109, 260,
		140, 239,
		140, 207,
		136, 207,
		136, 232,
		106, 253
	};

	rightLB = App->physics->CreateStaticChain(0, 0, rightL, 14);

	int rightMediumWall[78] = {
	136, 115,
	134, 121,
	132, 130,
	129, 137,
	126, 144,
	122, 149,
	116, 142,
	123, 133,
	127, 123,
	130, 111,
	131, 106,
	131, 91,
	130, 85,
	126, 82,
	121, 82,
	117, 85,
	117, 92,
	116, 95,
	116, 103,
	115, 112,
	113, 115,
	111, 119,
	107, 120,
	106, 117,
	106, 112,
	107, 107,
	108, 100,
	109, 94,
	110, 85,
	110, 78,
	110, 49,
	114, 51,
	117, 53,
	122, 58,
	127, 63,
	131, 70,
	134, 77,
	135, 81,
	136, 84,
	
	};

	rightMediumWallB = App->physics->CreateStaticChain(0, 0, rightMediumWall, 78);

	int rightBar[70] = {
	103, 278,
	161, 278,
	161, 84,
	160, 78,
	157, 69,
	155, 64,
	153, 60,
	150, 55,
	147, 50,
	144, 46,
	138, 40,
	141, 48,
	145, 55,
	148, 63,
	150, 73,
	150, 101,
	149, 110,
	148, 118,
	147, 124,
	146, 130,
	144, 136,
	142, 144,
	139, 154,
	136, 160,
	132, 167,
	135, 170,
	137, 175,
	137, 193,
	143, 194,
	147, 197,
	151, 202,
	152, 206,
	152, 261,
	139, 261,
	139, 255,
	};

	rightBarB = App->physics->CreateStaticChain(0, 0, rightBar, 70);

	int diggletR[12] = {
	136, 176,
	128, 176,
	125, 180,
	125, 185,
	128, 189,
	136, 189,
	};

	diggletRB = App->physics->CreateStaticChain(0, 0, diggletR, 12);
	diggletRB->body->GetFixtureList()->SetRestitution(1.5);
	diggletRB->type = COLLIDER_TYPE::DIGLET;

	int diggletL[12] = {
	23, 176,
	32, 176,
	35, 180,
	35, 185,
	32, 189,
	23, 189,
	};

	diggletLB = App->physics->CreateStaticChain(0, 0, diggletL, 12);
	diggletLB->body->GetFixtureList()->SetRestitution(1.5);
	diggletLB->type = COLLIDER_TYPE::DIGLET;

	int leftSmallWall[30] = {
	26, 122,
	28, 132,
	32, 140,
	37, 148,
	40, 151,
	45, 145,
	38, 135,
	33, 123,
	29, 106,
	29, 96,
	31, 90,
	31, 85,
	28, 86,
	24, 96,
	24, 115,
	};

	leftSmallWallB = App->physics->CreateStaticChain(0, 0, leftSmallWall, 30);

	int leftTinyWall[16] = {
	66, 55,
	66, 64,
	68, 64,
	72, 62,
	72, 48,
	71, 47,
	68, 47,
	66, 49,
	};

	leftTinyWallB = App->physics->CreateStaticChain(0, 0, leftTinyWall, 16);

	int rightTinyWall[18] = {
	88, 49,
	88, 57,
	89, 58,
	93, 58,
	94, 57,
	94, 44,
	93, 43,
	89, 43,
	88, 44,
	};

	rightTinyWallB = App->physics->CreateStaticChain(0, 0, rightTinyWall, 18);

	int starWall[30] = {
	43, 109,
	46, 121,
	49, 125,
	55, 127,
	65, 118,
	64, 113,
	59, 107,
	53, 102,
	49, 95,
	47, 89,
	47, 81,
	50, 77,
	50, 66,
	46, 75,
	43, 82,
	};

	starWallB = App->physics->CreateStaticChain(0, 0, starWall, 30);

	int leftTriangle[12] = {
		38, 211,
		40, 224,
		49, 234,
		48, 236,
		36, 228,
		35, 210
	};

	leftTriangleB = App->physics->CreateStaticChain(0, 0, leftTriangle, 12);
	leftTriangleB->type = COLLIDER_TYPE::TRIANGLE;

	int leftTriangleBounce[8] = {
		38, 210,
		39, 224,
		49, 234,
		40, 215
	};

	leftTriangleBounceB = App->physics->CreateStaticChain(0, 0, leftTriangleBounce, 8);
	leftTriangleBounceB->body->GetFixtureList()->SetRestitution(4);
	leftTriangleBounceB->type = COLLIDER_TYPE::TRIANGLE;

	int rightTriangle[12] = {
		111, 232,
		121, 224,
		122, 210,
		124, 210,
		125, 227,
		112, 236
	};

	rightTriangleB = App->physics->CreateStaticChain(0, 0, rightTriangle, 12);
	rightTriangleB->type = COLLIDER_TYPE::TRIANGLE;
	int rightTriangleBounce[8] = {
		111, 232,
		121, 225,
		122, 211,
		113, 228
	};

	rightTriangleBounceB = App->physics->CreateStaticChain(0, 0, rightTriangleBounce, 8);
	rightTriangleBounceB->body->GetFixtureList()->SetRestitution(4);
	rightTriangleBounceB->type = COLLIDER_TYPE::TRIANGLE;

	int leftPika[8] = {
		8, 257,
		21, 257,
		21, 260,
		8, 260
	};

	pikaLeftB = App->physics->CreateStaticChain(0, 0, leftPika, 8);
	pikaLeftB->body->GetFixtureList()->SetRestitution(4);
	pikaLeftB->type = COLLIDER_TYPE::PIKA;

	int rightPika[8] = {
		139, 258,
		152, 258,
		152, 260,
		139, 260
	};

	pikaRightB = App->physics->CreateStaticChain(0, 0, rightPika, 8);
	pikaRightB->body->GetFixtureList()->SetRestitution(4);
	pikaRightB->type = COLLIDER_TYPE::PIKA;

	PhysBody* combo1 = App->physics->CreateRectangleSensor(58, 66, 16, 8);
	combo1->type = COLLIDER_TYPE::COMBO1;

	PhysBody* combo2 = App->physics->CreateRectangleSensor(80, 52, 17, 8);
	combo2->type = COLLIDER_TYPE::COMBO2;

	PhysBody* combo3 = App->physics->CreateRectangleSensor(102, 53, 16, 8);
	combo3->type = COLLIDER_TYPE::COMBO3;
}

void ModuleSceneIntro::CreateBall()
{
	balls.add(App->physics->CreateBall(166, 260, 4.0f));
	balls.getLast()->data->listener = this;
	balls.getLast()->data->type = COLLIDER_TYPE::BALL;
}

void ModuleSceneIntro::ResetBall(PhysBody *ball)
{
	b2Vec2 ballpos;
	ballpos.x = PIXEL_TO_METERS(166.0f);
	ballpos.y = PIXEL_TO_METERS(260.0f);
	ball->body->SetTransform(ballpos,0.0f);
	ball->body->SetLinearVelocity(b2Vec2(0, 0));
}

void ModuleSceneIntro::CreateBallInMousePos()
{
	//if(debug){}
	int x, y;
	float radius = 3.5f;

	x = App->input->GetMouseX();
	y = App->input->GetMouseY();

	balls.add(App->physics->CreateBall(x, y, radius));
	balls.getLast()->data->resettable = false;
	balls.getLast()->data->listener = this;
	balls.getLast()->data->type = COLLIDER_TYPE::BALL;
}

void ModuleSceneIntro::CheckLife()
{
	if (lifes == 0) {
		//Loose window...
		App->ui->ShowGameOver();
	}
	else{ 
		lifes--;
		LOG("LIFES:%d", lifes);
	}

}
