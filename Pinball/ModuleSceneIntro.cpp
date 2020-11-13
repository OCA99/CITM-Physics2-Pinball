#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
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
	CreateWalls();
	

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
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB) {

}

update_status ModuleSceneIntro::PostUpdate()
{
	App->renderer->Blit(background, 0, 0);
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
	47, 37,
	56, 33,
	66, 29,
	72, 28,
	90, 28,
	96, 29,
	103, 31,
	109, 34,
	116, 38,
	110, 32,
	103, 28,
	97, 25,
	86, 22,
	76, 22,
	69, 23,
	63, 25,
	55, 29,
	49, 33,
	43, 37,
	
	};

	upperMoonB = App->physics->CreateStaticChain(0, 0, upperMoon, 40);

	int leftL[20] = {
	50, 260,
	50, 254,
	53, 251,
	28, 235,
	24, 230,
	24, 207,
	20, 207,
	20, 240,
	30, 248,
	46, 258
	};

	leftLB = App->physics->CreateStaticChain(0, 0, leftL, 20);
	

	int rightL[22] = {
	107, 251,
	110, 255,
	110, 260,
	127, 249,
	140, 239,
	140, 207,
	136, 207,
	136, 231,
	130, 236,
	116, 246,
	109, 250
	};

	rightLB = App->physics->CreateStaticChain(0, 0, rightL, 22);

	int rightMediumWall[78] = {
	136, 115,
	134, 121,
	132, 130,
	129, 137,
	126, 144,
	122, 149,
	116, 143,
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
	116, 108,
	115, 112,
	113, 115,
	110, 121,
	108, 121,
	105, 119,
	105, 112,
	106, 106,
	107, 99,
	108, 92,
	109, 84,
	110, 79,
	110, 49,
	112, 49,
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

	int diggletL[12] = {
	23, 176,
	32, 176,
	35, 180,
	35, 185,
	32, 189,
	23, 189,
	};

	diggletLB = App->physics->CreateStaticChain(0, 0, diggletL, 12);

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
	35, 226,
	35, 209,
	39, 209,
	50, 232,
	50, 236,
	47, 236,
	};

	leftTriangleB = App->physics->CreateStaticChain(0, 0, leftTriangle, 12);

	int rightTriangle[12] = {
	125, 228,
	125, 209,
	121, 209,
	110, 233,
	110, 236,
	113, 236,
	};

	rightTriangleB = App->physics->CreateStaticChain(0, 0, rightTriangle, 12);


}

