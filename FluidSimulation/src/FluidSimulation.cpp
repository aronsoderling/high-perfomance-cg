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
ShaderProgram *copyShader;
ShaderProgram *boundaryShader;
ShaderProgram *resetFloatShader;
ShaderProgram *bouyancyShader;

u32 x, y;
float timeStep;
float currentFrameTime;
float lastFrameTime;
vec2f inv_res;

RenderTarget *velocityCurrent;
RenderTarget *velocityTemp;
RenderTarget *vTemp1;
RenderTarget *vTemp2;
RenderTarget *iTemp1;
RenderTarget *iTemp2;
RenderTarget *vTempl[21];
RenderTarget *divergence;
RenderTarget *pressureCurrent;
RenderTarget *pressureTemp;
RenderTarget *pTemp[41];
RenderTarget *density;
RenderTarget *temperatureCurrent;	
RenderTarget *temperatureTemp;
RenderTarget *tTemp;
RenderTarget *densityCurrent;	
RenderTarget *densityTemp;
RenderTarget *dTemp;
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

vec2f			splatPos;

/* Function for cameracontrols */
void cameraControls();

vec2f prev_pos;
vec2f camera_rotation;

// stuffs
void	copyTexture(RenderTarget *from, RenderTarget *to);
void	advectVelocity();
void	renderVelocityBoundary();
void	advectTemperature();
void	advectDensity();
void	applyBouyancy();
void	diffusion();
void	splatTemperature();
void	splatDensity();
void	computeDivergence();
void	jacobiForPressure();
void	renderPressureBoundary();
void	computeGradientSubtraction();
void	printBoundaries();
void	visualize();

/*	
	setup world and quad
	initiate buffers and shaders
*/
void RCInit()
{
	RenderState *state;
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

	velocityTemp = SceneGraph::createRenderTarget("VelocityTempRT", x, y, 1, false, false,TEXTURE_FILTER_BILINEAR, true);
	pressureTemp = SceneGraph::createRenderTarget("PressureTempRT", x, y, 1, false, false,TEXTURE_FILTER_BILINEAR, true);
	densityTemp = SceneGraph::createRenderTarget("DensityTempRT", x, y, 1, false, false,TEXTURE_FILTER_BILINEAR, true);
	temperatureTemp = SceneGraph::createRenderTarget("TemperatureTempRT", x, y, 1, false, false,TEXTURE_FILTER_BILINEAR, true);

	velocityCurrent = SceneGraph::createRenderTarget("VelocityCurrentRT", x, y, 1, false, false,TEXTURE_FILTER_BILINEAR, true);
	pressureCurrent = SceneGraph::createRenderTarget("PressureCurrentRT", x, y, 1, false, false,TEXTURE_FILTER_BILINEAR, true);
	densityCurrent = SceneGraph::createRenderTarget("DensityCurrentRT", x, y, 1, false, false,TEXTURE_FILTER_BILINEAR, true);
	temperatureCurrent = SceneGraph::createRenderTarget("TemperatureCurrentRT", x, y, 1, false, false,TEXTURE_FILTER_BILINEAR, true);

	divergence = SceneGraph::createRenderTarget("DivergencetRT", x, y, 1, false, false,TEXTURE_FILTER_BILINEAR);
	
	//setup shaders
	advectShader = SceneGraph::createShaderProgram("AdvectSP", 0, "FluidVertex.vs", "Advect.fs", 0);
	jacobiShader = SceneGraph::createShaderProgram("JacobiSP", 0, "FluidVertex.vs", "Jacobi.fs", 0);
	divergenceShader = SceneGraph::createShaderProgram("DivergenceSP", 0, "FluidVertex.vs", "Divergence.fs", 0);
	subtractGradientShader = SceneGraph::createShaderProgram("SubtractGradientSP", 0, "FluidVertex.vs", "SubtractGradient.fs", 0);
	splatShader = SceneGraph::createShaderProgram("SplatSP", 0, "FluidVertex.vs", "Splat.fs", 0);
	randomShader = SceneGraph::createShaderProgram("RandomSP", 0, "FluidVertex.vs", "Random.fs", 0);
	visualizeShader = SceneGraph::createShaderProgram("VisualizeSP", 0, "FluidVertex.vs", "Visualize.fs", 0);
	copyShader = SceneGraph::createShaderProgram("CopySP", 0, "FluidVertex.vs", "Copy.fs", 0);
	boundaryShader = SceneGraph::createShaderProgram("BoundarySP", 0, "FluidVertex.vs", "Boundary.fs", 0);
	resetFloatShader = SceneGraph::createShaderProgram("ResetFloatSP", 0, "FluidVertex.vs", "resetFloat.fs", 0);
	bouyancyShader = SceneGraph::createShaderProgram("BouyancySP", 0, "FluidVertex.vs", "Bouyancy.fs", 0);
	fullScreenQuad = loadFullscreenQuad();
	
	boundary_va[0] = createLine(vec3f(-0.999f, -0.999f, 0.0f), vec3f(-0.999f, 0.999f, 0.0f));
	boundary_va[1] = createLine(vec3f(-0.999f, 0.999f, 0.0f), vec3f(0.999f, 0.999f, 0.0f));
	boundary_va[2] = createLine(vec3f(0.999f, 0.999f, 0.0f), vec3f(0.999f, -0.999f, 0.0f));
	boundary_va[3] = createLine(vec3f(0.999f, -0.999f, 0.0f), vec3f(-0.999f, -0.999f, 0.0f));
	boundary[0] = SceneGraph::createGeometry("boundary1", boundary_va[0], false);
	boundary[1] = SceneGraph::createGeometry("boundary2", boundary_va[1], false);
	boundary[2] = SceneGraph::createGeometry("boundary3", boundary_va[2], false);
	boundary[3] = SceneGraph::createGeometry("boundary4", boundary_va[3], false);
	offset_list[0] = vec2f(1.0f, 0.0f);
	offset_list[1] = vec2f(0.0f, -1.0f);
	offset_list[2] = vec2f(-1.0f, 0.0f);
	offset_list[3] = vec2f(0.0f, 1.0f);

	splatPos = vec2f(200.0f, 30.0f);
	
	randomShader->setValue("color", vec4f(0.5f, 0.5f, 0.0f, 1.0f));
	Renderer::setRenderTarget(velocityCurrent);
	Renderer::render(*fullScreenQuad, randomShader);
	
	randomShader->setValue("color", vec4f(0.5f, 0.0f, 0.0f, 1.0f));
	Renderer::setRenderTarget(pressureCurrent);
	Renderer::render(*fullScreenQuad, randomShader);
	
	randomShader->setValue("color", vec4f(0.5f, 0.5f, 0.0f, 1.0f));
	Renderer::setRenderTarget(densityCurrent);
	Renderer::render(*fullScreenQuad, randomShader);

	Renderer::setRenderTarget(temperatureCurrent);
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
	timeStep = 0.1f;
	bool *mouse = Platform::getMouseButtonState();
	if(mouse[MouseButtonLeft]){
         vec2f pos = Platform::getMousePosition();
         printf("Position: %f, %f", pos.x, pos.y);
		 pos.y = y-pos.y;
         splatPos = pos;
	}
	
	advectVelocity();
	advectTemperature();
	advectDensity();
	applyBouyancy();
	//diffusion(); //jacobi for velocity
	splatTemperature();
	splatDensity();
	computeDivergence();
	jacobiForPressure();
	computeGradientSubtraction();
	printBoundaries();
	visualize();

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

void copyTexture(RenderTarget *from, RenderTarget *to){
	copyShader->setTexture("copyTexture", from->getTexture(0));
	copyShader->setValue("invRes", inv_res);
	Renderer::setRenderTarget(to);
	Renderer::render(*fullScreenQuad, copyShader);
}

void advectVelocity(){
	advectShader->setValue("timeStep", timeStep);
	advectShader->setValue("dissipation", 1.0f);
	advectShader->setTexture("velocityTexture", velocityCurrent->getTexture(0));
	advectShader->setTexture("xTexture", velocityCurrent->getTexture(0));
	advectShader->setValue("invRes", inv_res);

	Renderer::setRenderTarget(velocityTemp);
	Renderer::render(*fullScreenQuad, advectShader);
	
	renderVelocityBoundary();
	
	vTemp1 = velocityCurrent;
	velocityCurrent = velocityTemp;
	velocityTemp = vTemp1;
}

void renderVelocityBoundary(){
	copyTexture(velocityTemp, velocityCurrent);
	
	boundaryShader->setValue("invRes", inv_res);
	boundaryShader->setValue("scale", -1.0f);
	Renderer::setRenderTarget(velocityTemp);

	// boundary conditions
	for(int i = 0; i <4; i++){
		boundaryShader->setTexture("x", velocityCurrent->getTexture(0));
		boundaryShader->setValue("offset", offset_list[i]);

		Renderer::render(*boundary[i], boundaryShader);
	}
}

void advectTemperature(){
	advectShader->setValue("dissipation",0.9994f);
	advectShader->setTexture("velocityTexture", velocityCurrent->getTexture(0));
	advectShader->setTexture("xTexture", temperatureCurrent->getTexture(0));
	advectShader->setValue("invRes", inv_res);

	Renderer::setRenderTarget(temperatureTemp);
	Renderer::render(*fullScreenQuad, advectShader);

	tTemp = temperatureCurrent;
	temperatureCurrent = temperatureTemp;
	temperatureTemp = tTemp;
}

void advectDensity(){
	advectShader->setValue("dissipation",0.9994f);
	advectShader->setTexture("xTexture", densityCurrent->getTexture(0));
	advectShader->setValue("invRes", inv_res);

	Renderer::setRenderTarget(densityTemp);
	Renderer::render(*fullScreenQuad, advectShader);

	dTemp = densityCurrent;
	densityCurrent = densityTemp;
	densityTemp = dTemp;
}

void applyBouyancy(){
	//Apply bouyancy
	bouyancyShader->setTexture("t", temperatureCurrent->getTexture(0));
	bouyancyShader->setTexture("d", densityCurrent->getTexture(0));
	bouyancyShader->setTexture("v", velocityCurrent->getTexture(0));
	bouyancyShader->setValue("invRes", inv_res);
	bouyancyShader->setValue("timeStep", timeStep);

	Renderer::setRenderTarget(velocityTemp);
	Renderer::render(*fullScreenQuad, bouyancyShader);

	vTemp1 = velocityCurrent;
	velocityCurrent = velocityTemp;
	velocityTemp = vTemp1;
}

void diffusion(){
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
		vTempl[i] = velocityCurrent;
		velocityCurrent = velocityTemp;
		velocityTemp = vTempl[i];
	}
}

void splatTemperature(){
	splatShader->setValue("radius",20.0f);
	splatShader->setValue("f",0.03f);
	splatShader->setValue("invRes", inv_res);
	splatShader->setValue("pos", splatPos);
	splatShader->setTexture("x", temperatureCurrent->getTexture(0));
	
	Renderer::setRenderTarget(temperatureTemp);
	Renderer::render(*fullScreenQuad, splatShader);
		
	iTemp1 = temperatureCurrent;
	temperatureCurrent = temperatureTemp;
	temperatureTemp = iTemp1;	
}

void splatDensity(){
	splatShader->setValue("radius",20.0f);
	splatShader->setValue("f",0.8f);
	splatShader->setValue("invRes", inv_res);
	splatShader->setValue("pos", splatPos);
	splatShader->setTexture("x", densityCurrent->getTexture(0));

	Renderer::setRenderTarget(densityTemp);
	Renderer::render(*fullScreenQuad, splatShader);
		
	iTemp2 = densityCurrent;
	densityCurrent = densityTemp;
	densityTemp = iTemp2;
}

void computeDivergence(){
	divergenceShader->setTexture("w", velocityCurrent->getTexture(0));
	divergenceShader->setValue("invRes", inv_res);

	Renderer::setRenderTarget(divergence);
	Renderer::render(*fullScreenQuad, divergenceShader);
}

void jacobiForPressure(){
	jacobiShader->setValue("invRes", inv_res);

	Renderer::setRenderTarget(pressureCurrent);
	Renderer::clearColor(vec4f(0.5f,0.f,0.f,0.f));
	Renderer::clearDepth(1.0f);

	Renderer::setRenderTarget(pressureTemp);
	for(int i=0; i<=40; i++){
		jacobiShader->setTexture("x", pressureCurrent->getTexture(0));
		jacobiShader->setTexture("b", divergence->getTexture(0));
		Renderer::render(*fullScreenQuad, jacobiShader);
		
		//renderPressureBoundary();

		pTemp[i] = pressureCurrent;
		pressureCurrent = pressureTemp;
		pressureTemp = pTemp[i];
	}
}

void renderPressureBoundary(){
	copyTexture(pressureTemp, pressureCurrent);
	
	boundaryShader->setValue("invRes", inv_res);
	boundaryShader->setValue("scale", 1.0f);
	Renderer::setRenderTarget(pressureTemp);

	// boundary conditions
	for(int i = 0; i <4; i++){
		boundaryShader->setTexture("x", pressureCurrent->getTexture(0));
		boundaryShader->setValue("offset", offset_list[i]);

		Renderer::render(*boundary[i], boundaryShader);
	}
}

void computeGradientSubtraction(){
	subtractGradientShader->setValue("invRes", inv_res);
	subtractGradientShader->setTexture("w", velocityCurrent->getTexture(0));
	subtractGradientShader->setTexture("p", pressureCurrent->getTexture(0));

	Renderer::setRenderTarget(velocityTemp);
	Renderer::render(*fullScreenQuad, subtractGradientShader);

	vTemp2 = velocityCurrent;
	velocityCurrent = velocityTemp;
	velocityTemp = vTemp2;
}

void printBoundaries(){
	randomShader->setValue("color", vec4f(0.0f, 0.0f, 1.0f, 1.0f));
	Renderer::setRenderTarget(0);
	Renderer::clearColor(vec4f(0.f,0.f,0.f,0.f));
	Renderer::clearDepth(1.0f);
	for(int i = 0; i <4; i++){
		Renderer::render(*boundary[i], randomShader);
	}
}

void visualize(){
	Renderer::setRenderTarget(0);
	Renderer::clearColor(vec4f(0.f,0.f,0.f,0.f));
	Renderer::clearDepth(1.0f);
	visualizeShader->setTexture("visualizeTexture" ,densityCurrent->getTexture(0));
	visualizeShader->setValue("invRes", inv_res); 
	Renderer::render(*fullScreenQuad, visualizeShader);
}

#endif /* RC_COMPILE_BENCHMARK */

