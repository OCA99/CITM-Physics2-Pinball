#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"
#include "Box2D/Box2D/Box2D.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y -7.0f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

enum BODY_INDEX
{
	BALL,
	WALL,
	TRIGGER,
	BUMPER,
	PLUNGE,

	NOT_DEFINED
};
enum PHYSIC_BODY_TYPE
{
	_BALL,
	_WALL,
	_TRIGGER,
	_BUMPER,
	_PLUNGE,
	_DEAD_SENSOR,
	_REBOUNCER1,
	_REBOUNCER2,
	_REBOUNCER3,

	_LEVEL_CHANGE,

	_NOT_DEFINED
};

class PhysBody
{
public:
	PhysBody() : listener(NULL), body(NULL)
	{}

	void GetPosition(int& x, int& y) const;
	float GetRotation() const;
	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;

public:
	int width, height;
	b2Body* body;
	b2Body* body2;
	b2Joint* joint;
	Module* listener;

	PHYSIC_BODY_TYPE bodyType = _NOT_DEFINED;
};

// Module --------------------------------------
class ModulePhysics : public Module, public b2ContactListener // TODO
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	PhysBody* CreateRightFlipper(p2Point<int>& pivot);
	PhysBody* CreateLeftFlipper(p2Point<int>& pivot);

	PhysBody* CreateCircle(int x, int y, int radius);
	PhysBody* CreateRectangle(int x, int y, int width, int height);
	PhysBody* CreateRectangleSensor(int x, int y, int width, int height);
	PhysBody* CreateChain(int x, int y, int* points, int size);

	// b2ContactListener ---
	void BeginContact(b2Contact* contact);

private:

	bool debug = false;
	b2World* world;
	b2MouseJoint* mouse_joint;
	b2Body* ground;
};