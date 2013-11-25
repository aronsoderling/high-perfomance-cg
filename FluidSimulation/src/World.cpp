#include <cstdio>
#include "RenderChimp.h"
#include "Node.h"
#include "Transformable.h"
#include "Camera.h"
#include "World.h"

/*---------------------------------------------------------------------------*/

World::World(
		const char	*nname
	) : Node(nname, NODE_WORLD)
{
	activeCamera = 0;
}

/*---------------------------------------------------------------------------*/

World::~World()
{
}

/*---------------------------------------------------------------------------*/

void World::drawSelf()
{
	if (activeCamera)
		Renderer::setCameraMatrices(activeCamera);
}

/*---------------------------------------------------------------------------*/

void World::setActiveCamera(
		Camera *camera
	)
{
	activeCamera = camera;
}

/*---------------------------------------------------------------------------*/

Camera *World::getActiveCamera()
{
	return activeCamera;
}

/*---------------------------------------------------------------------------*/

Node *World::duplicateSelf()
{
	World *c = SceneGraph::createWorld("WorldCopy");

	c->setActiveCamera(activeCamera);

	return c;
}

/*---------------------------------------------------------------------------*/

/* WARNING: Untested function! */
void World::getKNNLights(
		Light		**light_list,
		u32			*n_lights_found,

		Sphere		*search_volume,
		u32			kNN

	)
{
	u32 i, n_ch;
	f32 m;

	ASSERT(kNN > 0);
	ASSERT(search_volume);
	ASSERT(n_lights_found);
	ASSERT(light_list);

	n_ch = getChildrenCount();

	*n_lights_found = 0;

	m = 0.0f;

	for (i = 0; i < n_ch; i++) {
		Node *child = getChild(i);

		getKNNLightsRec(light_list, search_volume, kNN,
			n_lights_found, &m, child);
	}
}

/*---------------------------------------------------------------------------*/

void World::getKNNLightsRec(
		Light		**light_list,

		Sphere		*search_volume,
		u32			kNN,

		u32			*n,
		f32			*max_dist,
		Node		*curr
	)
{
	/* LOWPRIO: Optimize! */
	u32 i;
	u32 n_ch = curr->getChildrenCount();

	if (curr->getNodeType() == NODE_LIGHT) {
		Light *light = (Light *) curr;

		if (search_volume->intersect(&light->falloff)) {

			vec3f distvec = light->getWorldPosition() - search_volume->origin;
			f32 dist = distvec.length2();

			if (*n == kNN) {

				if (dist < *max_dist) {
					u32 j;
					i32 m = -1;

					*max_dist = dist;

					for (j = 0; j < kNN; j++) {
						distvec = light_list[j]->getWorldPosition() - search_volume->origin;
						dist = distvec.length2();

						if (dist > *max_dist) {
							/* This has to happen at least once */
							*max_dist = dist;
							m = (i32) j;
						}
					}

					ASSERT(m >= 0);

					light_list[m] = light;
				}

			} else {

				if (dist > *max_dist) {
					*max_dist = dist;
				}

				light_list[*n] = light;
				(*n)++;
			}

		}
	}

	for (i = 0; i < n_ch; i++) {
		Node *child = curr->getChild(i);

		getKNNLightsRec(light_list, search_volume, kNN, n, max_dist, child);
	}


}

/*---------------------------------------------------------------------------*/

void World::logInfo()
{
	Console::log("-=-=-=-");

	logNodeInfo();

	Console::log("Active camera: %s", activeCamera ? activeCamera->getName() : "None");

	Console::log("-=-=-=-");
}

/*---------------------------------------------------------------------------*/

