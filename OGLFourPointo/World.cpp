#include "World.h"
#include "Cube.h"
#include "Grid.h"

World::~World()
{
	for(iter = world.begin(); iter != world.end(); iter++)
	{
		delete *iter;
	}
}

int World::TotalBodies()
{
	return world.size();
}

void World::DrawBodies(GLboolean disableColorWrite)
{
	for(iter = world.begin(); iter != world.end(); iter++)
	{
		(*iter)->Draw(disableColorWrite);
	}
}

void World::DrawFromLightPOV()
{
	for(iter = world.begin(); iter != world.end(); iter++)
	{
		(*iter)->DrawFromLightPOV();
	}
}

void World::CreateBodies()
{ 
	Grid* grid = new Grid();
	Cube* cube = new Cube();

	cube->Create();
	grid->Create();

    world.push_back(grid);
    world.push_back(cube);
    
	
	
	
	
}

void World::DestroyBodies()
{
	for(iter = world.begin(); iter != world.end(); iter++)
	{
		(*iter)->Destroy();
	}
}