
#include "program.h"

#if defined ASSIGNMENT_1

/*
	EDA221 Introduction to Computer Graphics, 2011

	Assignment 1 - Solar System
*/

#include "RenderChimp.h"
#include "Command.h"

World	*world;				/* Scene graph root node */
Camera	*camera;			/* Scene camera */
Group	*camera_pivot;		/* A group is an empty, transformable node */
Light	*light;				/* A light is a point in the scene which can be used for lighting calculations */

Geometry		*sun;		/* Sun node */

vec3f			sun_ambient(1.0f, 1.0f, 1.0f);	/* Ambient color for the sun (never gets dark!) */

ShaderProgram	*sun_shader;		/* To add textures we need a shader program (vertex + fragment shader) */
Texture			*sun_texture;		/* Sun texture */

float			sun_spin = 0.4f;	/* Angular velocity (spin) of the sun (radians/s) */


/* variables used for mouse-camera control */
vec2f	camera_rotation(0.0f, 0.0f),	/* current rotation of camera */
		mouse_prev_pos(0.0f, 0.0f);		/* previous mouse position */

void RCInit()
{
	/* Create world node */
	world = SceneGraph::createWorld("solar_system");

	/* TODO: Create camera */
	camera = /* ... */
	/* Set camera perspective projection (field-of-view, aspect ratio, near-plane. far-plane) */
	camera->setPerspectiveProjection(1.2f, 4.0f / 3.0f, 1.0f, 1000.0f);
	/* TODO: Translate the camera to (0, 1, 6) */
    /* ... */
	/* Make this camera the active camera */
	world->setActiveCamera(camera);


	/* Create camera pivot group */
	camera_pivot = SceneGraph::createGroup("camera_pivot");
	/* Attach camera */
	camera_pivot->attachChild(camera);
	/* Attach to world */
	world->attachChild(camera_pivot);

	/* TODO: Create light source */
    /* ... */
	/* TODO: Set the light color to (1, 1, 1) */
    /* ... */
	/* TODO: Translate the light to (10, 10, 10) */
    /* ... */
	/* TODO: Attach the light to the world */
    /* ... */

	/* Create sun geometry from a sphere object-file and attach it to world */
	sun = SceneGraph::createGeometry("sun", "scenes/sphere.obj");
	world->attachChild(sun);

	/* TODO: Build & run (F5 in Visual Studio) */

	/* Create a shader, set its attributes and attach to sun (uncomment when reaching this point) */
	//sun_shader = SceneGraph::createShaderProgram("sun_shader", "LambertTexture", 0);
	//sun_shader->setVector("LightPosition", light->getWorldPosition().vec, 3, UNIFORM_FLOAT32);
	//sun_shader->setVector("LightColor", light->getColor().vec, 3, UNIFORM_FLOAT32);
	//sun_shader->setVector("Ambient", sun_ambient.vec, 3, UNIFORM_FLOAT32);
    //sun->setShaderProgram(sun_shader);
    
	/* Create a texture from file and assign to the sun (uncomment when reaching this point) */
	//sun_texture = SceneGraph::createTexture("sun_texture", "textures/sunmap.jpg", true, TEXTURE_FILTER_TRILINEAR, TEXTURE_WRAP_REPEAT);
	//sun->setTexture("DiffuseTex", sun_texture);

    
	/* TODO: Build & run (F5 in Visual Studio) */


	/* TODO: Create rest of the solar system... */


	/* Set clear color and depth */
	Renderer::setClearColor(vec4f(0.1f, 0.1f, 0.1f, 1.0f));
	Renderer::setClearDepth(1.0f);

    /* Enable XZ-grid and world axes */
    Command::run("/grid 1");
    Command::run("/axis_origin 1");
}

u32 RCUpdate()
{
	/* Apply rotation to the sun */
	sun->rotateY( sun_spin * Platform::getFrameTimeStep() );

    
	/* TODO: Perform animations for the rest of the solar system */

    
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

	/* Perform camera rotations (pivot style) */
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

	/* tell RenderChimp to render the scenegraph */
	world->renderAll();


	return 0;
}

void RCDestroy()
{
	/* release memory allocated by the scenegraph */
	SceneGraph::deleteNode(world);
}

#endif /* ASSIGNMENT_1 */
