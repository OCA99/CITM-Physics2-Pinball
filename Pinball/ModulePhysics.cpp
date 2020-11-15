#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "math.h"
#include "ModuleInterface.h"

#ifdef _DEBUG
#pragma comment( lib, "Box2D/libx86/Debug/Box2D.lib" )
#else
#pragma comment( lib, "Box2D/libx86/Release/Box2D.lib" )
#endif

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	world = NULL;
	mouseJoint = NULL;
	debug = false;

	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));
	world->SetContactListener(this);
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	// needed to create joints like mouse joint
	b2BodyDef bd;
	ground = world->CreateBody(&bd);

	return true;
}

// 
update_status ModulePhysics::PreUpdate()
{
	world->Step(1.0f / 60.0f, 6, 2);

	for (b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		if (c->GetFixtureA()->IsSensor() && c->IsTouching())
		{
			PhysBody* pb1 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			PhysBody* pb2 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			if (pb1 && pb2 && pb1->listener)
				pb1->listener->OnCollision(pb1, pb2);
		}
	}

	return UPDATE_CONTINUE;
}

PhysBody* ModulePhysics::CreateCircle(int x, int y, int radius)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = radius;

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangle(int x, int y, int width, int height)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width * 0.5f;
	pbody->height = height * 0.5f;

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangleSensor(int x, int y, int width, int height)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.isSensor = true;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width;
	pbody->height = height;

	return pbody;
}

PhysBody* ModulePhysics::CreateChain(int x, int y, int* points, int size)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for (uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]) - PIXEL_TO_METERS(x);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]) - PIXEL_TO_METERS(y);
	}

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;

	b->CreateFixture(&fixture);

	delete p;

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = 0;

	return pbody;
}

PhysBody* ModulePhysics::CreateStaticChain(int x, int y, int* points, int size)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for (uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]) - PIXEL_TO_METERS(x);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]) - PIXEL_TO_METERS(y);
	}

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;

	b->CreateFixture(&fixture);

	delete p;

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = 0;

	return pbody;
}

// 
update_status ModulePhysics::PostUpdate()
{
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if (!debug)
		return UPDATE_CONTINUE;

	b2Body* selectedBody = nullptr;
	b2Vec2 selectedVector;
	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch (f->GetType())
			{
				// Draw circles ------------------------------------------------
			case b2Shape::e_circle:
			{
				b2CircleShape* shape = (b2CircleShape*)f->GetShape();
				b2Vec2 pos = f->GetBody()->GetPosition();
				App->renderer->DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), METERS_TO_PIXELS(shape->m_radius), 255, 0, 0);
			}
			break;

			// Draw polygons ------------------------------------------------
			case b2Shape::e_polygon:
			{
				b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
				int32 count = polygonShape->GetVertexCount();
				b2Vec2 prev, v;

				for (int32 i = 0; i < count; ++i)
				{
					v = b->GetWorldPoint(polygonShape->GetVertex(i));
					if (i > 0)
						App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);

					prev = v;
				}

				v = b->GetWorldPoint(polygonShape->GetVertex(0));
				App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);
			}
			break;

			// Draw chains contour -------------------------------------------
			case b2Shape::e_chain:
			{
				b2ChainShape* shape = (b2ChainShape*)f->GetShape();
				b2Vec2 prev, v;

				for (int32 i = 0; i < shape->m_count; ++i)
				{
					v = b->GetWorldPoint(shape->m_vertices[i]);
					if (i > 0)
					{
						int prevx = METERS_TO_PIXELS(prev.x);
						int prevy = METERS_TO_PIXELS(prev.y);
						App->renderer->DrawLine(prevx, prevy, METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 0, 255, 0, 255);
					}
					prev = v;
				}

				v = b->GetWorldPoint(shape->m_vertices[0]);
				App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
			}
			break;

			// Draw a single segment(edge) ----------------------------------
			case b2Shape::e_edge:
			{
				b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
				b2Vec2 v1, v2;

				v1 = b->GetWorldPoint(shape->m_vertex0);
				v1 = b->GetWorldPoint(shape->m_vertex1);
				App->renderer->DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), 100, 100, 255);
			}
			break;
			}

			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN) {
				b2Vec2 vec;
				vec.x = PIXEL_TO_METERS(App->input->GetMouseX());
				vec.y = PIXEL_TO_METERS(App->input->GetMouseY());

				if (f->TestPoint(vec) && selectedBody == nullptr) {
					selectedBody = f->GetBody();
					selectedVector = vec;
				}
			}
		}
	}

	if (selectedBody != nullptr)
	{
		b2MouseJointDef def;
		def.bodyA = ground;
		def.bodyB = selectedBody;
		def.target = selectedVector;
		def.dampingRatio = 0.5f;
		def.frequencyHz = 2.0f;
		def.maxForce = 100.0f * selectedBody->GetMass();
		mouseJoint = (b2MouseJoint*)world->CreateJoint(&def);
		selectedBody = nullptr;
	}

	if (mouseJoint != nullptr)
	{
		selectedVector.x = PIXEL_TO_METERS(App->input->GetMouseX());
		selectedVector.y = PIXEL_TO_METERS(App->input->GetMouseY());
		mouseJoint->SetTarget(selectedVector);
		App->renderer->DrawLine(METERS_TO_PIXELS(mouseJoint->GetAnchorA().x), METERS_TO_PIXELS(mouseJoint->GetAnchorA().y), METERS_TO_PIXELS(mouseJoint->GetAnchorB().x), METERS_TO_PIXELS(mouseJoint->GetAnchorB().y), 250, 0, 0, 255);
	}

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && mouseJoint != nullptr)
	{
		world->DestroyJoint(mouseJoint);
		mouseJoint = nullptr;
	}

	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	// Delete the whole physics world!
	delete world;

	return true;
}

void PhysBody::GetPosition(int& x, int& y) const
{
	b2Vec2 pos = body->GetPosition();
	x = METERS_TO_PIXELS(pos.x) - (width);
	y = METERS_TO_PIXELS(pos.y) - (height);
}

float PhysBody::GetRotation() const
{
	return RADTODEG * body->GetAngle();
}

bool PhysBody::Contains(int x, int y) const
{
	b2Vec2 p(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	const b2Fixture* fixture = body->GetFixtureList();

	while (fixture != NULL)
	{
		if (fixture->GetShape()->TestPoint(body->GetTransform(), p) == true)
			return true;
		fixture = fixture->GetNext();
	}

	return false;
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
	int ret = -1;

	b2RayCastInput input;
	b2RayCastOutput output;

	input.p1.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
	input.p2.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));
	input.maxFraction = 1.0f;

	const b2Fixture* fixture = body->GetFixtureList();

	while (fixture != NULL)
	{
		if (fixture->GetShape()->RayCast(&output, input, body->GetTransform(), 0) == true)
		{
			// do we want the normal ?

			float fx = x2 - x1;
			float fy = y2 - y1;
			float dist = sqrtf((fx * fx) + (fy * fy));

			normal_x = output.normal.x;
			normal_y = output.normal.y;

			return output.fraction * dist;
		}
		fixture = fixture->GetNext();
	}

	return ret;
}

void ModulePhysics::BeginContact(b2Contact* contact)
{
	PhysBody* physA = (PhysBody*)contact->GetFixtureA()->GetBody()->GetUserData();
	PhysBody* physB = (PhysBody*)contact->GetFixtureB()->GetBody()->GetUserData();

	if (physA && physA->listener != NULL)
		physA->listener->OnCollision(physA, physB);

	if (physB && physB->listener != NULL)
		physB->listener->OnCollision(physB, physA);
}

PhysBody* ModulePhysics::CreateRightFlipper(p2Point<int>& pivot) {
	b2BodyDef flipperBodyDef;
	flipperBodyDef.type = b2_dynamicBody;
	flipperBodyDef.position.Set(PIXEL_TO_METERS(0), PIXEL_TO_METERS(0));

	b2Body* flipperBody = world->CreateBody(&flipperBodyDef);

	b2PolygonShape flipperShape;

	int coords[10] = {
		86, 257,
		108, 254,
		109, 256,
		108, 260,
		89, 258
	};

	b2Vec2 rightTriggerVec[5];
	for (int i = 0; i < 5; i++)
	{
		rightTriggerVec[i].x = PIXEL_TO_METERS(coords[i * 2]);
		rightTriggerVec[i].y = PIXEL_TO_METERS(coords[i * 2 + 1]);
	}

	flipperShape.Set(rightTriggerVec, 5);

	b2FixtureDef flipperFixtureDef;
	flipperFixtureDef.shape = &flipperShape;
	flipperFixtureDef.density = 10;
	flipperFixtureDef.filter.categoryBits = 0x0001;
	flipperFixtureDef.filter.maskBits = 0x0002;
	flipperBody->CreateFixture(&flipperFixtureDef);

	b2Vec2 centerRectangle = flipperBody->GetWorldCenter();
	centerRectangle += (b2Vec2(PIXEL_TO_METERS(7), PIXEL_TO_METERS(0)));

	b2BodyDef pivotBodyDef;
	pivotBodyDef.type = b2_staticBody;
	pivotBodyDef.position.Set(centerRectangle.x, centerRectangle.y);

	b2Body* pivotBody = world->CreateBody(&pivotBodyDef);

	b2CircleShape pivotCircle;
	pivotCircle.m_radius = PIXEL_TO_METERS(0.5f);
	b2FixtureDef pivotFixtureDef;
	pivotFixtureDef.shape = &pivotCircle;
	pivotFixtureDef.filter.categoryBits = 0x0001;
	pivotFixtureDef.filter.maskBits = 0x0002;
	pivotBody->CreateFixture(&pivotFixtureDef);

	b2RevoluteJointDef revJointDef;
	revJointDef.Initialize(flipperBody, pivotBody, centerRectangle);
	revJointDef.upperAngle = 0.5f;
	revJointDef.lowerAngle = -0.5f;
	revJointDef.enableLimit = true;
	revJointDef.maxMotorTorque = 10.0f;
	revJointDef.motorSpeed = 0.0f;
	revJointDef.enableMotor = true;
	b2Joint* joint = world->CreateJoint(&revJointDef);

	flipperBody->SetBullet(true);

	PhysBody* rightFlipper = new PhysBody();
	rightFlipper->body = flipperBody;
	rightFlipper->body2 = pivotBody;
	rightFlipper->joint = joint;
	flipperBody->SetUserData(rightFlipper);

	return rightFlipper;
}

PhysBody* ModulePhysics::CreateLeftFlipper(p2Point<int>& pivot) {
	b2BodyDef flipperBodyDef;
	flipperBodyDef.type = b2_dynamicBody;
	flipperBodyDef.position.Set(PIXEL_TO_METERS(0), PIXEL_TO_METERS(0));

	b2Body* flipperBody = world->CreateBody(&flipperBodyDef);

	b2PolygonShape flipperShape;

	int coords[10] = {
		74, 257,
		53, 254,
		51, 257,
		52, 260,
		71, 258
	};

	b2Vec2 leftFlipperVec[5];
	for (int i = 0; i < 5; i++)
	{
		leftFlipperVec[i].x = PIXEL_TO_METERS(coords[i * 2]);
		leftFlipperVec[i].y = PIXEL_TO_METERS(coords[i * 2 + 1]);
	}

	flipperShape.Set(leftFlipperVec, 5);

	b2FixtureDef flipperFixtureDef;
	flipperFixtureDef.shape = &flipperShape;
	flipperFixtureDef.density = 10;
	flipperFixtureDef.filter.categoryBits = 0x0001;
	flipperFixtureDef.filter.maskBits = 0x0002;
	flipperBody->CreateFixture(&flipperFixtureDef);

	b2Vec2 centerRectangle = flipperBody->GetWorldCenter();
	centerRectangle += (b2Vec2(PIXEL_TO_METERS(-5), 0));

	b2BodyDef pivotBodyDef;
	pivotBodyDef.type = b2_staticBody;
	pivotBodyDef.position.Set(centerRectangle.x, centerRectangle.y);

	b2Body* pivotBody = world->CreateBody(&pivotBodyDef);

	b2CircleShape pivotCircle;
	pivotCircle.m_radius = PIXEL_TO_METERS(0.5f);
	b2FixtureDef pivotFixtureDef;
	pivotFixtureDef.shape = &pivotCircle;
	pivotFixtureDef.filter.categoryBits = 0x0001;
	pivotFixtureDef.filter.maskBits = 0x0002;
	pivotBody->CreateFixture(&pivotFixtureDef);

	b2RevoluteJointDef revJointDef;
	revJointDef.Initialize(flipperBody, pivotBody, centerRectangle);
	revJointDef.upperAngle = 0.5f;
	revJointDef.lowerAngle = -0.5f;
	revJointDef.enableLimit = true;
	revJointDef.maxMotorTorque = 10.0f;
	revJointDef.motorSpeed = 0.0f;
	revJointDef.enableMotor = true;
	b2Joint* joint = world->CreateJoint(&revJointDef);

	flipperBody->SetBullet(true);

	PhysBody* leftFlipper = new PhysBody();
	leftFlipper->body = flipperBody;
	leftFlipper->body2 = pivotBody;
	leftFlipper->joint = joint;
	flipperBody->SetUserData(leftFlipper);

	return leftFlipper;
}

PhysBody* ModulePhysics::CreateBall(int x, int y, float radius)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.bullet = true;

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 3.5f;
	fixture.friction = 0.0f;
	fixture.restitution = 0.0f;
	fixture.filter.categoryBits = 0x0002;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = radius;

	return pbody;

}

PhysBody* ModulePhysics::CreatePlunge()
{
	b2BodyDef bodyA;
	bodyA.type = b2_dynamicBody;
	bodyA.position.Set(PIXEL_TO_METERS(167), PIXEL_TO_METERS(265));

	b2Body* b1 = world->CreateBody(&bodyA);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(8) * 0.5f, PIXEL_TO_METERS(2) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 20.0f;
	fixture.restitution = 0.0f;
	fixture.filter.categoryBits = 0x0003;
	fixture.filter.maskBits = 0x0002;

	b1->CreateFixture(&fixture);

	b2BodyDef bodyB;
	bodyB.type = b2_staticBody;
	bodyB.position.Set(PIXEL_TO_METERS(167), PIXEL_TO_METERS(277));

	b2Body* b2 = world->CreateBody(&bodyB);
	b2PolygonShape box1;
	box1.SetAsBox(PIXEL_TO_METERS(8) * 0.5f, PIXEL_TO_METERS(8) * 0.5f);

	b2FixtureDef fixture2;
	fixture2.shape = &box1;
	fixture2.density = 1.0f;
	fixture2.filter.categoryBits = 0x0004;

	bodyA.bullet = true;
	bodyB.bullet = true;

	b2->CreateFixture(&fixture2);

	b2PrismaticJointDef jointDef;
	jointDef.bodyA = b2;
	jointDef.bodyB = b1;
	jointDef.collideConnected = false;

	jointDef.localAxisA.Set(0, 1);
	jointDef.localAxisA.Normalize();
	jointDef.localAnchorA.Set(0, 0);
	jointDef.localAnchorB.Set(0, 0);

	jointDef.lowerTranslation = -0.2f;
	jointDef.upperTranslation = -0.1f;
	jointDef.enableLimit = true;
	jointDef.maxMotorForce = 200.0f;
	jointDef.motorSpeed = -200.0f;
	jointDef.enableMotor = true;
	world->CreateJoint(&jointDef);

	PhysBody* pbody = new PhysBody();
	pbody->body = b1;
	pbody->body2 = b2;
	b1->SetUserData(pbody);

	return pbody;
}