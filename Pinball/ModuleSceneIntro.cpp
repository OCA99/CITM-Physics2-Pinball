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

	backRect.w = backRect.h = 0;
	backRect.w = SCREEN_WIDTH;
	backRect.h = SCREEN_HEIGHT;
	

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

