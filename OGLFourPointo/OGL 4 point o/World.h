#ifndef _WORLD
#define _WORLD

#include <list>
#include "Body.h"
#include "CurrentState.h"

using namespace std;

class World
{
private:
	list<Body*> world;
	list<Body*>::const_iterator iter;
	CurrentState state;

public:
	~World();
	int TotalBodies();
	void DrawBodies();
	void CreateBodies();
	void DestroyBodies();
};

#endif