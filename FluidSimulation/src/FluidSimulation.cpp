#include "program.h"

#if defined EDAN35

#include "RenderChimp.h"
#include <iostream>
#include <GL/glew.h>
#include "glut.h"


World *world;
Camera *camera;
Geometry *fullScreenQuad;
ShaderProgram *advectShader;
ShaderProgram *jacobiShader;
ShaderProgram *divergenceShader;
ShaderProgram *subtractGradientShader;
ShaderProgram *randomShader;
ShaderProgram *splatShader;
ShaderProgram *visualizeShader;
ShaderProgram *boundaryShader;

float timeStep;
float currentFrameTime;
float lastFrameTime;
vec2f inv_res;

RenderTarget *velocityCurrent;
RenderTarget *velocityTemp;
RenderTarget *vTemp1;
RenderTarget *vTemp2;
RenderTarget *vTemp3;
RenderTarget *divergence;
RenderTarget *pressureCurrent;
RenderTarget *pressureTemp;
RenderTarget *pTemp[21];
RenderTarget *density;
RenderTarget *vTempb1[4];
RenderTarget *vTempb2[4];
RenderTarget *vTempb3[4];
RenderTarget *pTempb1[21*4];


vec4f dissipation;

/* Functions foar loading geometry */
VertexArray*	createLine(const vec3f &p0, const vec3f &p1);
Geometry		*loadFullscreenQuad();
Geometry		*boundary[4];	/* */
VertexArray		*boundary_va[4];
vec2f			offset_list[4];

/* Function for cameracontrols */
void cameraControls();

vec2f prev_pos;
vec2f camera_rotation;

/*	
	setup world and quad
	initiate buffers and shaders
*/
void RCInit()
{
	RenderState *state;
	u32 x, y;
	vec2f near_far;
	vec2f res;
	f32 fov, aspect;

	world = SceneGraph::createWorld(0);
	Platform::getDisplaySize(&x, &y);

	fov = 1.2f;
	aspect = 4.0f / 3.0f;
	near_far.x = 0.1f;
	near_far.y = 100.0f;
	inv_res.x = 1.f /  (f32)x;
	inv_res.y = 1.f / (f32)y;

	dissipation = vec4f(1.0, 1.0, 1.0, 1.0);

	camera = SceneGraph::createCamera(0);
	camera->setPerspectiveProjection(fov, aspect, near_far.x, near_far.y);
    camera->translate(0,2,0);
	
	world->attachChild(camera);
	world->setActiveCamera(camera);

	velocityTemp = SceneGraph::createRenderTarget("VelocityTempRT", x, y, 1, false, false,TEXTURE_FILTER_BILINEAR);
	velocityCurrent = SceneGraph::createRenderTarget("VelocityCurrentRT", x, y, 1, false, false,TEXTURE_FILTER_BILINEAR);
	divergence = SceneGraph::createRenderTarget("DivergencetRT", x, y, 1, false, false,TEXTURE_FILTER_BILINEAR);
	pressureTemp = SceneGraph::createRenderTarget("PressureTempRT", x, y, 1, false, false,TEXTURE_FILTER_BILINEAR);
	pressureCurrent = SceneGraph::createRenderTarget("PressureCurrentRT", x, y, 1, false, false,TEXTURE_FILTER_BILINEAR);
	density = SceneGraph::createRenderTarget("DensityRT", x, y, 1, false, false,TEXTURE_FILTER_BILINEAR);
	
	//setup shaders
	advectShader = SceneGraph::createShaderProgram("AdvectSP", 0, "FluidVertex.vs", "Advect.fs", 0);
	jacobiShader = SceneGraph::createShaderProgram("JacobiSP", 0, "FluidVertex.vs", "Jacobi.fs", 0);
	divergenceShader = SceneGraph::createShaderProgram("DivergenceSP", 0, "FluidVertex.vs", "Divergence.fs", 0);
	subtractGradientShader = SceneGraph::createShaderProgram("SubtractGradientSP", 0, "FluidVertex.vs", "SubtractGradient.fs", 0);
	splatShader = SceneGraph::createShaderProgram("SplatSP", 0, "FluidVertex.vs", "Splat.fs", 0);
	randomShader = SceneGraph::createShaderProgram("RandomSP", 0, "FluidVertex.vs", "Random.fs", 0);
	visualizeShader = SceneGraph::createShaderProgram("VisualizeSP", 0, "FluidVertex.vs", "Visualize.fs", 0);
	boundaryShader = SceneGraph::createShaderProgram("BoundarySP", 0, "FluidVertex.vs", "Boundary.fs", 0);
	fullScreenQuad = loadFullscreenQuad();
	
	boundary_va[0] = createLine(vec3f(-0.9f, -0.9f, 0.0f), vec3f(-0.9f, 0.9f, 0.0f));
	boundary_va[1] = createLine(vec3f(-0.9f, 0.9f, 0.0f), vec3f(0.9f, 0.9f, 0.0f));
	boundary_va[2] = createLine(vec3f(0.9f, 0.9f, 0.0f), vec3f(0.9f, -0.9f, 0.0f));
	boundary_va[3] = createLine(vec3f(0.9f, -0.9f, 0.0f), vec3f(-0.9f, -0.9f, 0.0f));
	boundary[0] = SceneGraph::createGeometry("boundary1", boundary_va[0], false);
	boundary[1] = SceneGraph::createGeometry("boundary2", boundary_va[1], false);
	boundary[2] = SceneGraph::createGeometry("boundary3", boundary_va[2], false);
	boundary[3] = SceneGraph::createGeometry("boundary4", boundary_va[3], false);
	offset_list[0] = vec2f(1.0f, 0.0f);
	offset_list[1] = vec2f(0.0f, 1.0f);
	offset_list[2] = vec2f(x-1.0f, 0.0f);
	offset_list[3] = vec2f(0.0f, y-1.0f);

	
	
	randomShader->setValue("invRes", inv_res);

	Renderer::setRenderTarget(velocityCurrent);
	Renderer::clearColor(vec4f(0.f,0.f,0.f,0.f));
	Renderer::clearDepth(1.0f);
	Renderer::render(*fullScreenQuad, randomShader);
	Renderer::setRenderTarget(pressureCurrent);
	Renderer::clearColor(vec4f(0.f,0.f,0.f,0.f));
	Renderer::clearDepth(1.0f);
	Renderer::render(*fullScreenQuad, randomShader);
	
	prev_pos = vec2f(0.0f, 0.0f);
	camera_rotation = vec2f(0.0f, 0.0f);
}

/*
	render the quad using the different shaders in the correct order
	all computation done in the shaders
*/
u32 RCUpdate()
{
	cameraControls();
	//currentFrameTime = Platform::getFrameTime();
	timeStep = 10.125f;
	//lastFrameTime = currentFrameTime;

	
	printf ("timeStep: %f \n", timeStep);

	
	//Advect velocity
	advectShader->setValue("timeStep", timeStep);
	advectShader->setTexture("velocityTexture", velocityCurrent->getTexture(0));
	advectShader->setTexture("xTexture", velocityCurrent->getTexture(0));
	advectShader->setValue("invRes", inv_res);

	Renderer::setRenderTarget(velocityTemp);
	Renderer::render(*fullScreenQuad, advectShader);

	vTemp1 = velocityCurrent;
	velocityCurrent = velocityTemp;
	velocityTemp = vTemp1;
	
	
	/*
	//Advect density
	advectShader->setValue("timeStep", timeStep);
	advectShader->setTexture("velocityTexture", velocityCurrent->getTexture(0));
	advectShader->setTexture("xTexture", density->getTexture(0));
	advectShader->setValue("invRes", inv_res);

	Renderer::setRenderTarget(density);
	Renderer::clearColor(vec4f(0.f,0.f,0.f,0.f));
	Renderer::clearDepth(1.0f);
	Renderer::render(*fullScreenQuad, advectShader);
	*/
	/*
	//Compute jacobi iterations, diffusion
	jacobiShader->setValue("iBeta", 0.25f);
	jacobiShader->setValue("invRes", inv_res);

	Renderer::setRenderTarget(velocityTemp);

	for(int i=0; i<=20; i++){
		Renderer::clearColor(vec4f(0.f,0.f,0.f,0.f));
		Renderer::clearDepth(1.0f);
		jacobiShader->setTexture("x", velocityCurrent->getTexture(0));
		jacobiShader->setTexture("b", velocityCurrent->getTexture(0));
		Renderer::render(*fullScreenQuad, jacobiShader);
		vTemp = velocityCurrent;
		velocityCurrent = velocityTemp;
		velocityTemp = vTemp;
	}
	*/
	
	bool *mouse = Platform::getMouseButtonState();
	if(mouse[MouseButtonLeft]){
		vec2f pos = Platform::getMousePosition();

	
		splatShader->setValue("radius", 30.0f);
		splatShader->setValue("invRes", inv_res);
		splatShader->setValue("pos", pos);
		splatShader->setTexture("x", velocityCurrent->getTexture(0));
	

		Renderer::setRenderTarget(velocityTemp);
		Renderer::render(*fullScreenQuad, splatShader);
		
		vTemp2 = velocityCurrent;
		velocityCurrent = velocityTemp;
		velocityTemp = vTemp2;	
	}
	
	
	//compute divergence
	divergenceShader->setTexture("w", velocityCurrent->getTexture(0));
	divergenceShader->setValue("invRes", inv_res);

	Renderer::setRenderTarget(divergence);
	Renderer::render(*fullScreenQuad, divergenceShader);
	
	
	//jocobi for pressure
	jacobiShader->setValue("iBeta", 0.25f);
	jacobiShader->setValue("invRes", inv_res);

	Renderer::setRenderTarget(pressureTemp);
	for(int i=0; i<=20; i++){
		jacobiShader->setTexture("x", pressureCurrent->getTexture(0));
		jacobiShader->setTexture("b", divergence->getTexture(0));
		Renderer::render(*fullScreenQuad, jacobiShader);
		pTemp[i] = pressureCurrent;
		pressureCurrent = pressureTemp;
		pressureTemp = pTemp[i];
	}
	
	//compute gradient subtraction
	subtractGradientShader->setValue("invRes", inv_res);
	subtractGradientShader->setTexture("w", velocityCurrent->getTexture(0));
	subtractGradientShader->setTexture("p", pressureCurrent->getTexture(0));

	Renderer::setRenderTarget(velocityTemp);
	Renderer::clearColor(vec4f(0.f,0.f,0.f,0.f));
	Renderer::clearDepth(1.0f);
	Renderer::render(*fullScreenQuad, subtractGradientShader);

	vTemp3 = velocityCurrent;
	velocityCurrent = velocityTemp;
	velocityTemp = vTemp3;

	boundaryShader->setValue("invRes", inv_res);
	boundaryShader->setValue("scale", -1.0f);

	Renderer::setRenderTarget(velocityTemp);
	
	for(int i = 0; i <4; i++){
		boundaryShader->setTexture("x", velocityCurrent->getTexture(0));
		boundaryShader->setValue("offset", offset_list[i]);
		Renderer::render(*boundary[i], boundaryShader);

		vTempb3[i] = velocityCurrent;
		velocityCurrent = velocityTemp;
		velocityTemp = vTempb3[i];
	}
	/*
	visualizeShader->setTexture("visualizeTexture", density->getTexture(0));
	visualizeShader->setValue("invRes", inv_res);
	Renderer::setRenderTarget(0);
	Renderer::clearColor(vec4f(0.f,0.f,0.f,0.f));
	Renderer::clearDepth(1.0f);
	Renderer::render(*fullScreenQuad, visualizeShader);
	*/

	Renderer::setRenderTarget(0);
	Renderer::clearColor(vec4f(0.f,0.f,0.f,0.f));
	Renderer::clearDepth(1.0f);
	for(int i = 0; i <4; i++){
		Renderer::render(*boundary[i], randomShader);
	}
	return 0;
}

void RCDestroy()
{
	SceneGraph::deleteNode(world);
}


void cameraControls()
{
	bool *keys = Platform::getKeyState();
	bool *mouse = Platform::getMouseButtonState();
	vec2f pos = Platform::getMousePosition() * 0.002f;
	f32 move, strafe, up;
	vec3f view_front, view_right, view_up;



	move = 0.0f;
	strafe = 0.0f;
	up = 0.0f;
	
	float speed = 12.0f;
	if (keys[KeyShift])
		speed = 3.0f;
	speed *= Platform::getFrameTimeStep();

	if (keys[KeyW])
		move += speed;
	if (keys[KeyS])
		move -= speed;
	if (keys[KeyA])
		strafe -= speed;
	if (keys[KeyD])
		strafe += speed;
	if (keys[KeyQ])
		up -= speed;
	if (keys[KeyE])
		up += speed;


	view_front = camera->getWorldFront();
	view_right = camera->getWorldRight();
	view_up = camera->getWorldUp();

	camera->setRotateX(camera_rotation.y);
	camera->rotateY(camera_rotation.x);
	camera->translate(view_front * move);
	camera->translate(view_right * strafe);
	camera->translate(view_up * up);

	prev_pos = pos;
}
/* Loads a fullscreen quad */
Geometry *loadFullscreenQuad()
{
	f32 quad[2 * 6] = {
		-1.0f, -1.0f,
		1.0f, -1.0f,
		-1.0f, 1.0f,

		-1.0f, 1.0f,
		1.0f, -1.0f,
		1.0f, 1.0f
	};

	VertexArray *fsquad;
	fsquad = SceneGraph::createVertexArray("FSQuadVA", quad, sizeof(f32) * 2, 6, TRIANGLES, USAGE_STATIC);
	fsquad->setAttribute("Vertex", 0, 2, ATTRIB_FLOAT32, false);

	return  SceneGraph::createGeometry("FSQuad", fsquad, false);
}

VertexArray* createLine(const vec3f &p0, const vec3f &p1){
	vec3f *va = new vec3f[2];
	va[0] = p0;
	va[1] = p1;
	
	VertexArray *vertex_array = SceneGraph::createVertexArray(0, va, sizeof(vec3f), 
	2, LINES, USAGE_STATIC);
	vertex_array->setAttribute("Vertex", 0, 3, ATTRIB_FLOAT32);
	delete[] va;
	
	return vertex_array;
}
#endif /* RC_COMPILE_BENCHMARK */

