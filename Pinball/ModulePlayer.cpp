#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModuleRender.h"
#include "ModuleInput.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	leftFlipperPosition = p2Point<int>(50, 253);
	rightFlipperPosition = p2Point<int>(86, 253);

	leftFlipperPivot = p2Point<int>(4, 4);
	rightFlipperPivot = p2Point<int>(19, 4);

	leftFlipperRect = SDL_Rect({ 77, 245, 24, 9 });
	rightFlipperRect = SDL_Rect({ 279, 245, 24, 9 });
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	texture = App->textures->Load("pinball/sprites.png");

	leftFlipperBody = App->physics->CreateLeftFlipper(leftFlipperPivot);
	rightFlipperBody = App->physics->CreateRightFlipper(rightFlipperPivot);

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

update_status ModulePlayer::PreUpdate()
{
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
	{
		leftFlipperBody->body->ApplyTorque(-65.0f, true);
	}

	return UPDATE_CONTINUE;
}

// Update: draw background
update_status ModulePlayer::Update()
{
	return UPDATE_CONTINUE;
}

update_status ModulePlayer::PostUpdate()
{
	App->renderer->Blit(texture, leftFlipperPosition.x, leftFlipperPosition.y, &leftFlipperRect, 0.0f, leftFlipperBody->body->GetAngle(), leftFlipperPivot.x, leftFlipperPivot.y);
	App->renderer->Blit(texture, rightFlipperPosition.x, rightFlipperPosition.y, &rightFlipperRect, 0.0f, rightFlipperBody->body->GetAngle(), rightFlipperPivot.x, rightFlipperPivot.y);

	return UPDATE_CONTINUE;
}



