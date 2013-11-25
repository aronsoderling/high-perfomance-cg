#include "RenderChimp.h"
#include "SceneParser.h"
#include "Geometry.h"

/*---------------------------------------------------------------------------*/

Geometry::Geometry(
		const char		*nname
	) : Transformable(nname, NODE_GEOMETRY), Uniform()
{
	vertexArray = 0;
	shaderProgram = 0;
}

/*---------------------------------------------------------------------------*/

Geometry::Geometry(
		const char		*nname,
		VertexArray		*vertex_array,
		bool			instantiate_resources
	) : Transformable(nname, NODE_GEOMETRY), Uniform()
{
	shaderProgram = 0;
	vertexArray = vertex_array;
}

/*---------------------------------------------------------------------------*/

Geometry::Geometry(
		const char		*nname,
		const char		*filename
	) : Transformable(nname, NODE_GEOMETRY), Uniform()
{
	char fnbuf[FILENAME_MAX];
	const char *fnprefix = RC_RESOURCE_PATH;

	vertexArray = 0;
	shaderProgram = 0;

	if (strlen(filename) + strlen(fnprefix) >= (FILENAME_MAX - 1)) {
		REPORT_WARNING("Scene filename too long (%s)", filename);
		broken = true;
		return;
	}

	strcpy(fnbuf, fnprefix);
	strcat(fnbuf, filename);

	ParseScene(this, fnbuf);
}

/*---------------------------------------------------------------------------*/

Geometry::~Geometry()
{
	if (vertexArray)
		SceneGraph::dereferenceResource(vertexArray);
	if (shaderProgram)
		SceneGraph::dereferenceResource(shaderProgram);
}

/*---------------------------------------------------------------------------*/

void Geometry::render()
{
	drawSelf();
}

/*---------------------------------------------------------------------------*/

void Geometry::setVertexArray(
		VertexArray		*vertex_array,

		bool			instantiate_resource
	)
{
	if (vertexArray)
		SceneGraph::dereferenceResource(vertexArray);

	if (instantiate_resource) {
		vertexArray = (VertexArray *) vertex_array->instantiate();
	} else {
		vertexArray = vertex_array;
	}
}

/*---------------------------------------------------------------------------*/

void Geometry::setShaderProgram(
		ShaderProgram	*shader_program,

		bool			instantiate_resource
	)
{
	if (shaderProgram)
		SceneGraph::dereferenceResource(shaderProgram);
	if (instantiate_resource)
		shaderProgram = (ShaderProgram *) shader_program->instantiate();
	else
		shaderProgram = shader_program;
}

/*---------------------------------------------------------------------------*/

VertexArray *Geometry::getVertexArray() const
{
	return vertexArray;
}

/*---------------------------------------------------------------------------*/

ShaderProgram *Geometry::getShaderProgram() const
{
	return shaderProgram;
}

/*---------------------------------------------------------------------------*/

void Geometry::drawSelf()
{
	Renderer::render(*this);
}

/*---------------------------------------------------------------------------*/

Node *Geometry::duplicateSelf()
{
	Geometry *dup;

	dup = (Geometry *) SceneGraph::createGeometry(name);

	dup->setVertexArray(vertexArray, true);

	if (shaderProgram)
		dup->setShaderProgram((ShaderProgram *) shaderProgram->instantiate());

	dup->R = R;
	dup->T = T;
	dup->S = S;

	copyDataTo(dup);

	return dup;
}

/*---------------------------------------------------------------------------*/

void Geometry::getLocalBoundingBox(
		AABox			*target
	)
{
	AABox box;

	if (!vertexArray) {
		AABox b;
		b.minCorner = 0.0f;
		b.maxCorner = 0.0f;
		REPORT_WARNING("Geometry \"%s\" has no vertex array attached", name);
		*target = b;
		return;
	}

	vertexArray->getBoundingBox(target);
}

/*---------------------------------------------------------------------------*/

/* LOWPRIO: Should we store this in the geometry node? */
void Geometry::getWorldBoundingBox(
		AABox			*target
	)
{
	u32 i;
	mat4f w;
	AABox box;
	AABox wbox;

	if (!vertexArray) {
		AABox b;
		b.minCorner = 0.0f;
		b.maxCorner = 0.0f;
		REPORT_WARNING("Geometry \"%s\" has no vertex array attached", name);
		*target = b;
		return;
	}

	vertexArray->getBoundingBox(&box);

	wbox.minCorner = fMax;
	wbox.maxCorner = -fMax;

	w = getWorldMatrix();

	for (i = 0; i < 8; i++) {
		vec3f wpt;
		vec3f pt = box.getCornerPoint(i);
		wpt = w.affineMul(pt);
		wbox.include(wpt);
	}

	*target = wbox;
}

/*---------------------------------------------------------------------------*/

void Geometry::logInfo()
{
	Console::log("-=-=-=-");

	logNodeInfo();
	logTransformableInfo();

	Console::log("Vertex array: %s", vertexArray->getIdentifier());
	Console::log("Runtime data:");
	logData();
	Console::log(" ");
	Console::log("-=-=-=-");
}

/*---------------------------------------------------------------------------*/

#if 0
static const char *CMDHELP_geo_set_vertexarray = {
	"/camera_perspective <Geometry> <fov> <aspect> <near> <far>"
};

bool CMD_camera_perspective(
		u32		n_args,
		char	(*args)[RC_COMMAND_MAX_ARG_LEN]
	)
{
	Camera *cam;
	f32 f[4];

	if (!Command::checkArgCount(5, n_args))
		return false;

	if (!(cam = fetchCamera(args[0])))
		return false;

	if (!parseFloats(4, f, &args[1]))
		return false;

	cam->setPerspectiveProjection(f[0], f[1], f[2], f[3]);

	return true;
}
#endif

/*---------------------------------------------------------------------------*/

void Geometry::registerCommands()
{
//	Command::add("camera_perspective", CMDHELP_camera_perspective, CMD_camera_perspective);
}

/*---------------------------------------------------------------------------*/
