
#include "program.h"

#if defined ASSIGNMENT_23

/*
	EDA221 Introduction to Computer Graphics, 2011

	Assignment 2 - Parametric Shapes
*/

#include "RenderChimp.h"
#include "Command.h"
#include "ParametricShapes.h"

World	*world;				/* Scene graph root node */
Camera	*camera;			/* Scene camera */
Group	*camera_pivot;		/* A group is an empty, transformable node */
Light	*light;				/* A light is a point in the scene which can be used for lighting calculations */

Geometry		*shape;	/* */
VertexArray		*shape_va;

vec3f			ambient(0.3f, 0.3f, 0.3f);	/* Ambient color for the sun (never gets dark) */

ShaderProgram	*shape_shader;		/* To add textures we need a shader program (vertex + fragment shader) */
Texture			*shape_texture;		/* Sun texture */

float			spin = 0.2f;	/* Angular velocity of the sun (radians/s) */


/* variables used for mouse-camera control */
vec2f	camera_rotation(0.0f, 0.0f),	/* current rotation of camera */
		mouse_prev_pos(0.0f, 0.0f);		/* previous mouse position */

void RCInit()
{
	world = SceneGraph::createWorld("solar_system");

	/* Camera */
	camera = SceneGraph::createCamera("cam1");
	camera->setPerspectiveProjection(1.2f, 4.0f / 3.0f, 1.0f, 1000.0f);
	camera->translate(0.0f, 1.0f, 6.0f);
	world->setActiveCamera(camera);

	/* Create pivot */
	camera_pivot = SceneGraph::createGroup("camera_pivot");
	camera_pivot->attachChild(camera);
	world->attachChild(camera_pivot);

	/* light */
	light = SceneGraph::createLight("light1");
	light->setColor(1.0f, 1.0f, 1.0f);
	light->setTranslate(15.0f, 15.0f, 15.0f);
	world->attachChild(light);	

	/* Create vertex array for circle ring */
	shape_va = createCircleRing(4, 60, 1, 2);
    
    // TODO: implement createSphere(...) in ParametricShapes.cpp
	//shape_va = createSphere(...);
    
    // TODO: implement createTorus(...) in ParametricShapes.cpp
	//shape_va = createTorus(...);

	shape = SceneGraph::createGeometry("saturn_rings", shape_va, false);
	world->attachChild(shape);

	/* Create a shader and set its attributes */
	shape_shader = SceneGraph::createShaderProgram("shader", "Lambert", 0);
	shape_shader->setVector("LightPosition", light->getWorldPosition().vec, 3, UNIFORM_FLOAT32);
	shape_shader->setVector("LightColor", light->getColor().vec, 3, UNIFORM_FLOAT32);
	shape_shader->setVector("Ambient", ambient.vec, 3, UNIFORM_FLOAT32);

	/* Assign shader and texture to the sun (uncomment when reaching this point) */
	shape->setShaderProgram(shape_shader);

	/* Set clear color and depth */
	Renderer::setClearColor(vec4f(0.1f, 0.1f, 0.1f, 1.0f));
	Renderer::setClearDepth(1.0f);
    
    /* Enable XZ-grid and world axes */
    Command::run("/grid 1");
    Command::run("/axis_origin 1");

}

u32 RCUpdate()
{
	shape->rotateY( spin * Platform::getFrameTimeStep() );

	/* Mouse & keyboard controls --> */

	/* Get the current mouse button state */
	bool *mouse = Platform::getMouseButtonState();

	/* Get mouse screen coordinates [-1, 1] */
	vec2f mouse_pos = Platform::getMousePosition();

	/* Translate mouse position and button state to camera rotations */
	if (mouse[RC_MOUSE_LEFT]) {

		/* Absolute position */

		camera_rotation.x = -mouse_pos.x * 2.0f;
		camera_rotation.y = mouse_pos.y * 2.0f;

	} else if (mouse[RC_MOUSE_RIGHT]) {

		/* Relative position */

		vec2f mouse_diff = mouse_pos - mouse_prev_pos;
		camera_rotation.x -= mouse_diff.x * 2.0f;
		camera_rotation.y += mouse_diff.y * 2.0f;
	}

	/* Perform camera rotations */
	camera_pivot->setRotateX(camera_rotation.y);
	camera_pivot->rotateY(camera_rotation.x);

	/* Store previous mouse screen position */
	mouse_prev_pos = mouse_pos;



	/* Get the current key state */
	bool *keys = Platform::getKeyState();

	/* Map keys to bilateral camera movement */
	f32 move = 0.0f, strafe = 0.0f;
	if (keys[RC_KEY_W])	move += 0.1f;
	if (keys[RC_KEY_S])	move -= 0.1f;
	if (keys[RC_KEY_A])	strafe -= 0.1f;
	if (keys[RC_KEY_D])	strafe += 0.1f;

	/* Apply movement to camera */
	camera->translate(camera->getLocalFront() * move);
	camera->translate(camera->getLocalRight() * strafe);

	/* Clear color and depth buffers */
	Renderer::clearColor();
	Renderer::clearDepth();

	/* Tell RenderChimp to render the scenegraph */
	world->renderAll();


	return 0;
}

void RCDestroy()
{
	// release memory allocated by the scenegraph
	SceneGraph::deleteNode(world);
}

#endif /* ASSIGNMENT_2 */

