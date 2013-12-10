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
float splatRadius;
float temperatureImpulse;
float densityImpulse;
float dissipation;
vec2f grid_res;
vec2f inv_grid_res;
vec2f inv_res;

RenderTarget *velocityCurrent;
RenderTarget *velocityTemp;
RenderTarget *reactionCurrent;
RenderTarget *reactionTemp;
RenderTarget *rTemp;
RenderTarget *vTemp1;
RenderTarget *vTemp2;
RenderTarget *iTemp1;
RenderTarget *iTemp2;
RenderTarget *iTemp3;
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
RenderTarget *boundary;


/* Functions foar loading geometry */
VertexArray*	createLine(const vec3f &p0, const vec3f &p1);
Geometry		*loadFullscreenQuad();
Geometry		*boundary_list[5];	/* */
VertexArray		*boundary_va[5];
vec2f			offset_list[4];

vec2f			splatPos;

/* Function for cameracontrols */
void cameraControls();

vec2f prev_pos;
vec2f camera_rotation;

// stuffs
void	copyTexture(RenderTarget *from, RenderTarget *to);
void	advectVelocity();
void	applyBouyancy();
void	renderVelocityBoundary();
void	advectTemperature();
void	advectDensity();
void	advectReaction();
void	diffusion();
void	splatTemperature(float f, vec2f position);
void	splatDensity(float f, vec2f position);
void	splatReaction();
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
	splatRadius = 10.0f;
	densityImpulse = 0.148;
	temperatureImpulse = 0.04;
	dissipation = 0.99995;
	timeStep = 0.09f;
	//x = 800;
	//y = 600;
	world = SceneGraph::createWorld(0);
	Platform::getDisplaySize(&x, &y);
	//Platform::setDisplay(true, x, y);
	fov = 1.2f;
	aspect = 4.0f / 3.0f;
	near_far.x = 0.1f;
	near_far.y = 100.0f;
	inv_res.x = 1.f /  (f32)x;
	inv_res.y = 1.f / (f32)y;
	grid_res.x = x;
	grid_res.y = y;
	inv_grid_res.x = 1.0f / grid_res.x;
	inv_grid_res.y = 1.0f / grid_res.y;

	camera = SceneGraph::createCamera(0);
	camera->setPerspectiveProjection(fov, aspect, near_far.x, near_far.y);
    camera->translate(0,2,0);
	
	world->attachChild(camera);
	world->setActiveCamera(camera);

	velocityTemp = SceneGraph::createRenderTarget("VelocityTempRT", grid_res.x, grid_res.y, 1, false, false,TEXTURE_FILTER_BILINEAR, true);
	pressureTemp = SceneGraph::createRenderTarget("PressureTempRT", grid_res.x, grid_res.y, 1, false, false,TEXTURE_FILTER_BILINEAR, true);
	densityTemp = SceneGraph::createRenderTarget("DensityTempRT", grid_res.x, grid_res.y, 1, false, false,TEXTURE_FILTER_BILINEAR, true);
	temperatureTemp = SceneGraph::createRenderTarget("TemperatureTempRT", grid_res.x, grid_res.y, 1, false, false,TEXTURE_FILTER_BILINEAR, true);
	reactionTemp = SceneGraph::createRenderTarget("ReactionTempRT", grid_res.x, grid_res.y, 1, false, false,TEXTURE_FILTER_BILINEAR, true);

	velocityCurrent = SceneGraph::createRenderTarget("VelocityCurrentRT", grid_res.x, grid_res.y, 1, false, false,TEXTURE_FILTER_BILINEAR, true);
	pressureCurrent = SceneGraph::createRenderTarget("PressureCurrentRT", grid_res.x, grid_res.y, 1, false, false,TEXTURE_FILTER_BILINEAR, true);
	densityCurrent = SceneGraph::createRenderTarget("DensityCurrentRT", grid_res.x, grid_res.y, 1, false, false,TEXTURE_FILTER_BILINEAR, true);
	temperatureCurrent = SceneGraph::createRenderTarget("TemperatureCurrentRT", grid_res.x, grid_res.y, 1, false, false,TEXTURE_FILTER_BILINEAR, true);
	reactionCurrent = SceneGraph::createRenderTarget("ReactionCurrentRT", grid_res.x, grid_res.y, 1, false, false,TEXTURE_FILTER_BILINEAR, true);

	divergence = SceneGraph::createRenderTarget("DivergencetRT", grid_res.x, grid_res.y, 1, false, false,TEXTURE_FILTER_BILINEAR);
	boundary = SceneGraph::createRenderTarget("Border", grid_res.x,grid_res.y,1,false,false,TEXTURE_FILTER_NEAREST, true);
	
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
	
	
	boundary_va[0] = createLine(vec3f(-0.9999f, -0.9999f, 0.0f), vec3f(-0.9999f, 0.9999f, 0.0f));
	boundary_va[1] = createLine(vec3f(-0.9999f, 0.9999f, 0.0f), vec3f(0.9999f, 0.9999f, 0.0f));
	boundary_va[2] = createLine(vec3f(0.9999f, 0.9999f, 0.0f), vec3f(0.9999f, -0.9999f, 0.0f));
	boundary_va[3] = createLine(vec3f(0.9999f, -0.9999f, 0.0f), vec3f(-0.9999f, -0.9999f, 0.0f));
	//boundary_va[4] = createLine(vec3f(-0.9999f, 0.9999f, 0.0f), vec3f(0.9999f, -0.9999f,0.0f));
	
	
	boundary_list[0] = SceneGraph::createGeometry("boundary1", boundary_va[0], false);
	boundary_list[1] = SceneGraph::createGeometry("boundary2", boundary_va[1], false);
	boundary_list[2] = SceneGraph::createGeometry("boundary3", boundary_va[2], false);
	boundary_list[3] = SceneGraph::createGeometry("boundary4", boundary_va[3], false);
	//boundary_list[4] = SceneGraph::createGeometry("boundary5", boundary_va[4], false);
	/*offset_list[0] = vec2f(1.0f, 0.0f);
	offset_list[1] = vec2f(0.0f, -1.0f);
	offset_list[2] = vec2f(-1.0f, 0.0f);
	offset_list[3] = vec2f(0.0f, 1.0f);
	*/
	splatPos = vec2f(200.0f, 30.0f);
	
	randomShader->setValue("color", vec4f(0.5f, 0.5f, 0.0f, 1.0f));
	Renderer::setRenderTarget(velocityCurrent);
	Renderer::render(*fullScreenQuad, randomShader);
	
	randomShader->setValue("color", vec4f(0.5f, 0.0f, 0.0f, 1.0f));
	Renderer::setRenderTarget(pressureCurrent);
	Renderer::render(*fullScreenQuad, randomShader);

	randomShader->setValue("color", vec4f(1.0f,0.0f,0.0f,1.0f));
	Renderer::setRenderTarget(boundary);
	for(int i = 0; i <4; i++){
		Renderer::render(*boundary_list[i], randomShader);
	}
	
	/*randomShader->setValue("color", vec4f(0.5f, 0.5f, 0.0f, 1.0f));
	Renderer::setRenderTarget(densityCurrent);
	Renderer::render(*fullScreenQuad, randomShader);

	Renderer::setRenderTarget(temperatureCurrent);
	Renderer::render(*fullScreenQuad, randomShader);*/
	
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
	bool *mouse = Platform::getMouseButtonState();
	if(mouse[MouseButtonLeft]){
         vec2f pos = Platform::getMousePosition();
		 pos.y = y-pos.y;
         splatPos = pos;
	}
	if(mouse[MouseButtonRight]){
		vec2f pos2 = Platform::getMousePosition();
		pos2.y = y-pos2.y;
		splatTemperature(0.05, pos2);
		splatDensity(0.5, pos2);
	
	}

	advectShader->setValue("invRes", inv_grid_res);
	advectShader->setValue("timeStep", timeStep);
	
	advectVelocity();
	advectTemperature();
	advectDensity();
	//advectReaction();
	applyBouyancy();
	//diffusion(); //jacobi for velocity
	splatTemperature(temperatureImpulse, splatPos);
	splatDensity(densityImpulse, splatPos);
	splatReaction();
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
	copyShader->setValue("invRes", inv_grid_res);
	Renderer::setRenderTarget(to);
	Renderer::render(*fullScreenQuad, copyShader);
}

void advectVelocity(){
	advectShader->setValue("dissipation", 1.0f);
	advectShader->setValue("k", 0.0f);
	advectShader->setValue("baseColor", vec4f(0.5f,0.5f,0.0f, 1.0f));
	advectShader->setTexture("v", velocityCurrent->getTexture(0));
	advectShader->setTexture("x", velocityCurrent->getTexture(0));
	advectShader->setTexture("b", boundary->getTexture(0));

	Renderer::setRenderTarget(velocityTemp);
	Renderer::render(*fullScreenQuad, advectShader);
	
	//renderVelocityBoundary();
	
	vTemp1 = velocityCurrent;
	velocityCurrent = velocityTemp;
	velocityTemp = vTemp1;
}

void renderVelocityBoundary(){
	copyTexture(velocityTemp, velocityCurrent);
	
	boundaryShader->setValue("invRes", inv_grid_res);
	boundaryShader->setValue("scale", -1.0f);
	Renderer::setRenderTarget(velocityTemp);

	// boundary conditions
	for(int i = 0; i <4; i++){
		boundaryShader->setTexture("x", velocityCurrent->getTexture(0));
		boundaryShader->setValue("offset", offset_list[i]);

		Renderer::render(*boundary_list[i], boundaryShader);
	}
}

void advectTemperature(){
	advectShader->setValue("baseColor", vec4f(0.0f,0.0f,0.0f, 1.0f));
	advectShader->setValue("dissipation",dissipation);
	advectShader->setValue("k", 0.0f);
	advectShader->setTexture("v", velocityCurrent->getTexture(0));
	advectShader->setTexture("x", temperatureCurrent->getTexture(0));
	advectShader->setTexture("b", boundary->getTexture(0));

	Renderer::setRenderTarget(temperatureTemp);
	Renderer::render(*fullScreenQuad, advectShader);

	tTemp = temperatureCurrent;
	temperatureCurrent = temperatureTemp;
	temperatureTemp = tTemp;
}

void advectDensity(){
	advectShader->setValue("baseColor", vec4f(0.0f,0.0f,0.0f, 1.0f));
	advectShader->setValue("dissipation",dissipation);
	advectShader->setValue("k", 0.0f);
	advectShader->setTexture("v", velocityCurrent->getTexture(0));
	advectShader->setTexture("x", densityCurrent->getTexture(0));
	advectShader->setTexture("b", boundary->getTexture(0));

	Renderer::setRenderTarget(densityTemp);
	Renderer::render(*fullScreenQuad, advectShader);

	dTemp = densityCurrent;
	densityCurrent = densityTemp;
	densityTemp = dTemp;
}

void advectReaction(){
	advectShader->setValue("baseColor", vec4f(0.0f,0.0f,0.0f, 1.0f));
	advectShader->setValue("dissipation",1.0f);
	advectShader->setTexture("v", velocityCurrent->getTexture(0));
	advectShader->setTexture("x", reactionCurrent->getTexture(0));
	advectShader->setTexture("b", boundary->getTexture(0));

	Renderer::setRenderTarget(reactionTemp);
	Renderer::render(*fullScreenQuad, advectShader);

	rTemp = reactionCurrent;
	densityCurrent = reactionTemp;
	reactionTemp = rTemp;
	advectShader->setValue("k", 0.0f);

}

void applyBouyancy(){
	//Apply bouyancy
	bouyancyShader->setTexture("t", temperatureCurrent->getTexture(0));
	bouyancyShader->setTexture("d", densityCurrent->getTexture(0));
	bouyancyShader->setTexture("v", velocityCurrent->getTexture(0));
	bouyancyShader->setValue("invRes", inv_grid_res);
	bouyancyShader->setValue("timeStep", timeStep);

	Renderer::setRenderTarget(velocityTemp);
	Renderer::render(*fullScreenQuad, bouyancyShader);
	
	//renderVelocityBoundary();

	vTemp1 = velocityCurrent;
	velocityCurrent = velocityTemp;
	velocityTemp = vTemp1;
}

void diffusion(){
	//Compute jacobi iterations, diffusion
	jacobiShader->setValue("iBeta", 0.25f);
	jacobiShader->setValue("invRes", inv_grid_res);

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

void splatTemperature(float f, vec2f position){
	splatShader->setValue("radius",splatRadius);
	splatShader->setValue("f",f);
	splatShader->setValue("invRes", inv_grid_res);
	splatShader->setValue("pos", position);
	splatShader->setTexture("x", temperatureCurrent->getTexture(0));
	
	Renderer::setRenderTarget(temperatureTemp);
	Renderer::render(*fullScreenQuad, splatShader);
		
	iTemp1 = temperatureCurrent;
	temperatureCurrent = temperatureTemp;
	temperatureTemp = iTemp1;	
}

void splatDensity(float f, vec2f position){
	splatShader->setValue("radius",splatRadius);
	splatShader->setValue("f",f);
	splatShader->setValue("invRes", inv_grid_res);
	splatShader->setValue("pos", position);
	splatShader->setTexture("x", densityCurrent->getTexture(0));

	Renderer::setRenderTarget(densityTemp);
	Renderer::render(*fullScreenQuad, splatShader);
		
	iTemp2 = densityCurrent;
	densityCurrent = densityTemp;
	densityTemp = iTemp2;
}

void splatReaction(){
	splatShader->setValue("radius",20.0f);
	splatShader->setValue("f",0.5f);
	splatShader->setValue("invRes", inv_grid_res);
	splatShader->setValue("pos", splatPos);
	splatShader->setTexture("x", reactionCurrent->getTexture(0));

	Renderer::setRenderTarget(reactionTemp);
	Renderer::render(*fullScreenQuad, splatShader);
		
	iTemp3 = reactionCurrent;
	reactionCurrent = reactionTemp;
	reactionTemp = iTemp3;
}

void computeDivergence(){
	divergenceShader->setTexture("w", velocityCurrent->getTexture(0));
	divergenceShader->setTexture("b", boundary->getTexture(0));
	divergenceShader->setValue("invRes", inv_grid_res);

	Renderer::setRenderTarget(divergence);
	Renderer::render(*fullScreenQuad, divergenceShader);
}

void jacobiForPressure(){
	jacobiShader->setValue("invRes", inv_grid_res);
	jacobiShader->setTexture("b", boundary->getTexture(0));

	Renderer::setRenderTarget(pressureCurrent);
	Renderer::clearColor(vec4f(0.5f,0.f,0.f,0.f));
	Renderer::clearDepth(1.0f);

	Renderer::setRenderTarget(pressureTemp);
	for(int i=0; i<=40; i++){
		jacobiShader->setTexture("p", pressureCurrent->getTexture(0));
		jacobiShader->setTexture("d", divergence->getTexture(0));
		Renderer::render(*fullScreenQuad, jacobiShader);
		
		//renderPressureBoundary();

		pTemp[i] = pressureCurrent;
		pressureCurrent = pressureTemp;
		pressureTemp = pTemp[i];
	}
}

void renderPressureBoundary(){
	copyTexture(pressureTemp, pressureCurrent);
	
	boundaryShader->setValue("invRes", inv_grid_res);
	boundaryShader->setValue("scale", 1.0f);
	Renderer::setRenderTarget(pressureTemp);

	// boundary conditions
	for(int i = 0; i <4; i++){
		boundaryShader->setTexture("x", pressureCurrent->getTexture(0));
		boundaryShader->setValue("offset", offset_list[i]);

		Renderer::render(*boundary_list[i], boundaryShader);
	}
}

void computeGradientSubtraction(){
	subtractGradientShader->setValue("invRes", inv_grid_res);
	subtractGradientShader->setTexture("w", velocityCurrent->getTexture(0));
	subtractGradientShader->setTexture("p", pressureCurrent->getTexture(0));
	subtractGradientShader->setTexture("b", boundary->getTexture(0));

	Renderer::setRenderTarget(velocityTemp);
	Renderer::render(*fullScreenQuad, subtractGradientShader);

	//renderVelocityBoundary();

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
		Renderer::render(*boundary_list[i], randomShader);
	}
}

void visualize(){
	Renderer::setRenderTarget(0);
	Renderer::clearColor(vec4f(0.f,0.f,0.f,0.f));
	Renderer::clearDepth(1.0f);
	visualizeShader->setTexture("densityTexture" ,densityCurrent->getTexture(0));
	visualizeShader->setTexture("temperatureTexture" , temperatureCurrent->getTexture(0));
	visualizeShader->setTexture("reactionTexture" , reactionCurrent->getTexture(0));
	visualizeShader->setValue("invRes", inv_grid_res); 
	Renderer::render(*fullScreenQuad, visualizeShader);
}

#endif /* RC_COMPILE_BENCHMARK */

