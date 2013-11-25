#include "RenderChimp.h"
#include "DefaultObjects.h"
#include "Command.h"

/*---------------------------------------------------------------------------*/

ResourceList	*SceneGraph::vertexList;
ResourceList	*SceneGraph::renderTargetList;
ResourceList	*SceneGraph::textureList;
ResourceList	*SceneGraph::cubeMapList;
ResourceList	*SceneGraph::shaderProgramList;

NodeList		*SceneGraph::nodeList;

char			SceneGraph::nameBuf[256];

/*---------------------------------------------------------------------------*/

void SceneGraph::init()
{
	printf("  Setting up resource lists\n");
	shaderProgramList = new ResourceList;
	vertexList = new ResourceList;
	renderTargetList = new ResourceList;
	textureList = new ResourceList;
	cubeMapList = new ResourceList;

	printf("  Setting up node list\n");
	nodeList = new NodeList;

	printf("  Initiating default objects\n");
	DefaultObjects::init();

}

/*---------------------------------------------------------------------------*/

void SceneGraph::destroy()
{
	DefaultObjects::destroy();

	delete shaderProgramList;
	delete vertexList;
	delete renderTargetList;
	delete textureList;
	delete cubeMapList;

	delete nodeList;
}

/*---------------------------------------------------------------------------*/

/* Common destroyer */
void SceneGraph::deleteNode(
		Node			*node
	)
{
	Node *child;
	u32 i;

	if (!node) {
		REPORT_WARNING("Trying to delete node, but received a null pointer");
		return;
	}

	child = node->getChild(0);

	for (i = 0; i < node->getChildrenCount(); i++) {
		Node *c = child->next;
		SceneGraph::deleteNode(child);
		child = c;
	}

	nodeList->remove(node);

	delete node;
}

/*---------------------------------------------------------------------------*/

/* World */
World *SceneGraph::createWorld(
		const char			*nname
	)
{
	return createEmptyNode<World>(nname);
}

/*---------------------------------------------------------------------------*/

/* Transformable */

/* Group */
Group *SceneGraph::createGroup(
		const char			*nname
	)
{
	return createEmptyNode<Group>(nname);
}

/*---------------------------------------------------------------------------*/

Group *SceneGraph::createGroup(
		const char			*nname,
		const char			*filename	/* .pwn/.obj file */
	)
{
	nname = prepNewNodeName<Group>(nname);
	Group *node = new Group(nname, filename);
	return ensureNodeSanity(node);
}

/*---------------------------------------------------------------------------*/

/* Camera */
Camera *SceneGraph::createCamera(
		const char			*nname
	)
{
	return createEmptyNode<Camera>(nname);
}

/*---------------------------------------------------------------------------*/

/* Light */
Light *SceneGraph::createLight(
		const char			*nname
	)
{
	return createEmptyNode<Light>(nname);
}

/*---------------------------------------------------------------------------*/

Geometry *SceneGraph::createGeometry(
		const char		*nname
	)
{
	return createEmptyNode<Geometry>(nname);
}

/*---------------------------------------------------------------------------*/

Geometry *SceneGraph::createGeometry(
		const char		*nname,
		VertexArray		*vertex_array,
		bool			instantiate_resources
	)
{
	nname = prepNewNodeName<Geometry>(nname);
	Geometry *node = new Geometry(nname, vertex_array, instantiate_resources);
	return ensureNodeSanity(node);
}

/*---------------------------------------------------------------------------*/

Geometry *SceneGraph::createGeometry(
		const char		*nname,
		const char		*filename,
		bool			instantiate_resources
	)
{
	nname = prepNewNodeName<Geometry>(nname);
	Geometry *node = new Geometry(nname, filename); // TODO: Listen to instantiate...
	return ensureNodeSanity(node);
}

/*---------------------------------------------------------------------------*/

Node *SceneGraph::getNode(
		const char		*name
	)
{
	return nodeList->getNode(name);
}

/*---------------------------------------------------------------------------*/

u32 SceneGraph::getNodeCount()
{
	return nodeList->getNodeCount();
}

/*---------------------------------------------------------------------------*/

void SceneGraph::getNodes(
		Node			**node_list
	)
{
	nodeList->getNodes(node_list);
}

/*---------------------------------------------------------------------------*/

u32 SceneGraph::getNodeCount(
		NodeType		type
	)
{
	return nodeList->getNodeCount(type);
}

/*---------------------------------------------------------------------------*/

void SceneGraph::getNodes(
		Node			**node_list,
		NodeType		type
	)
{
	nodeList->getNodes(node_list, type);
}

/*---------------------------------------------------------------------------*/

void SceneGraph::getNodes(
		const char		*name,
		u32				max_nodes,
		Node			**node_list,
		u32				*n,
		NodeType		type
	)
{
	nodeList->getNodes(name, max_nodes, node_list, n, type);
}

/*---------------------------------------------------------------------------*/

bool SceneGraph::dereferenceResource(
		Resource		*resource
	)
{

	ResourceType type = resource->getType();

	switch (type) {
		case RESOURCE_VERTEX_ARRAY:
			return vertexList->dereference(resource);
			break;
		case RESOURCE_RENDER_TARGET:
			return renderTargetList->dereference(resource);
			break;
		case RESOURCE_TEXTURE:
			return textureList->dereference(resource);
			break;
		case RESOURCE_CUBEMAP:
			return cubeMapList->dereference(resource);
			break;
		case RESOURCE_SHADER_PROGRAM:
			return shaderProgramList->dereference(resource);
			break;
		default:
			break;
	}


	return false;
}

/*---------------------------------------------------------------------------*/

void SceneGraph::purgeResources(
		u32				max_purge_level
	)
{
	/* These have dependencies to other resources = delete first */
	shaderProgramList->purge(max_purge_level, false);
	renderTargetList->purge(max_purge_level, false);

	/* These do not have any dependencies */
	vertexList->purge(max_purge_level, false);
	textureList->purge(max_purge_level, false);
	cubeMapList->purge(max_purge_level, false);

}

/*---------------------------------------------------------------------------*/

void SceneGraph::forcePurgeResources(
		u32				max_purge_level
	)
{
	/* These have dependencies to other resources = delete first */
	shaderProgramList->purge(max_purge_level, true);
	renderTargetList->purge(max_purge_level, true);
	
	/* These do not have any dependencies */
	vertexList->purge(max_purge_level, true);
	textureList->purge(max_purge_level, true);
	cubeMapList->purge(max_purge_level, true);
	
}

/*---------------------------------------------------------------------------*/

VertexArray *SceneGraph::createVertexArray(
		const char				*nidentifier,

		void					*new_array,
		u32						nstride,
		u32						n,

		PrimitiveLayout_t		layout,

		AttributeUsage_t		usage
	)
{
	nidentifier = prepNewResourceName<VertexArray>(nidentifier);
	VertexArray *a = new VertexArray(nidentifier, new_array, nstride, n, layout, usage);
	return ensureResourceSanity(a);
}

/*---------------------------------------------------------------------------*/

VertexArray *SceneGraph::createVertexArray(
		const char				*nidentifier,

		u32						nstride,

		PrimitiveLayout_t		layout,

		AttributeUsage_t		usage
	)
{
	nidentifier = prepNewResourceName<VertexArray>(nidentifier);
	VertexArray *a = new VertexArray(nidentifier, nstride, layout, usage);
	return ensureResourceSanity(a);
}

/*---------------------------------------------------------------------------*/

VertexArray *SceneGraph::createFullScreenVertexArray(
		const char				*nidentifier
	)
{
	nidentifier = prepNewResourceName<VertexArray>(nidentifier);
	f32 vtx[6] = {-1.0f, -1.0f, 3.0f, -1.0f, -1.0f, 3.0f};
	VertexArray *a = new VertexArray(nidentifier, vtx, 2 * sizeof(f32), 3, TRIANGLES, USAGE_STATIC);
	a->setAttribute("Vertex", 0, 2, ATTRIB_FLOAT32);
	return ensureResourceSanity(a);
}

/*---------------------------------------------------------------------------*/

RenderTarget *SceneGraph::createRenderTarget(
		const char				*nidentifier,

		u32						width,
		u32						height,

		u32						n_target_textures,

		bool					depth_buffer,
		bool					stencil_buffer,
		TextureFilter_t			textureFilter
	)
{
	nidentifier = prepNewResourceName<RenderTarget>(nidentifier);
	RenderTarget *a = new RenderTarget(nidentifier, width, height, n_target_textures, textureFilter, TEXTURE_WRAP_REPEAT, depth_buffer, stencil_buffer);
	return ensureResourceSanity(a);
}

/*---------------------------------------------------------------------------*/

Texture *SceneGraph::createTexture(
		const char				*nidentifier,
		const char				*filename,
		bool					generate_mipmaps,
		TextureFilter_t			filter,
		TextureWrap_t			wrap
	)
{
	nidentifier = prepNewResourceName<Texture>(nidentifier);
	Texture *a = new Texture(nidentifier, filename, generate_mipmaps, filter, wrap);
	if (!(a = ensureResourceSanity(a)))
		a = DefaultObjects::failureTexture;
	return a;
}

/*---------------------------------------------------------------------------*/

Texture *SceneGraph::createTexture(
		const char				*filename
	)
{
	Texture *a = (Texture *) textureList->getResource(filename);
	if (a)
		return (Texture *) a->instantiate();
	filename = prepNewResourceName<Texture>(filename);
	a = new Texture(filename, filename, true, TEXTURE_FILTER_TRILINEAR, TEXTURE_WRAP_REPEAT);
	if (!(a = ensureResourceSanity(a)))
		a = DefaultObjects::failureTexture;
	return a;
}

/*---------------------------------------------------------------------------*/

Texture *SceneGraph::createTexture(
		const char				*nidentifier,
		u8						*pixels,
		TextureFormat_t			nformat,
		u32						nwidth,
		u32						nheight,
		bool					generate_mipmaps,
		TextureFilter_t			filter,
		TextureWrap_t			wrap
	)
{
	nidentifier = prepNewResourceName<Texture>(nidentifier);
	Texture *a = new Texture(nidentifier, pixels, nformat, nwidth, nheight, generate_mipmaps, filter, wrap);
	if (!(a = ensureResourceSanity(a)))
		a = DefaultObjects::failureTexture;
	return a;
}

/*---------------------------------------------------------------------------*/

CubeMap *SceneGraph::createCubeMap(
		const char			*nidentifier,

		const char			*nleft,
		const char			*nright,
		const char			*nup,
		const char			*ndown,
		const char			*nfront,
		const char			*nback
	)
{
	nidentifier = prepNewResourceName<CubeMap>(nidentifier);
	CubeMap *a = new CubeMap(nidentifier, nleft, nright, nup, ndown, nfront, nback);
	if (!(a = ensureResourceSanity(a)))
		a = DefaultObjects::failureCubeMap;
	return a;
}

/*---------------------------------------------------------------------------*/

/*
Resource *SceneGraph::getResource(
		const char			*identifier,
		ResourceType		type
	)
{
	switch (type) {
		case RESOURCE_VERTEX_ARRAY:
			return vertexList->getResource(identifier);
			break;
		case RESOURCE_TEXTURE:
			return textureList->getResource(identifier);
			break;
		case RESOURCE_RENDER_TARGET:
			return renderTargetList->getResource(identifier);
			break;
		case RESOURCE_CUBEMAP:
			return cubeMapList->getResource(identifier);
			break;
		case RESOURCE_SHADER_PROGRAM:
			return shaderProgramList->getResource(identifier);
			break;
		default:
			break;
	}

	return 0;
}
*/

/*---------------------------------------------------------------------------*/

/*
u32 SceneGraph::getResourceCount(
		ResourceType		type
	)
{
	switch (type) {
		case RESOURCE_VERTEX_ARRAY:
			return vertexList->getResourceCount();
			break;
		case RESOURCE_TEXTURE:
			return textureList->getResourceCount();
			break;
		case RESOURCE_RENDER_TARGET:
			return renderTargetList->getResourceCount();
			break;
		case RESOURCE_CUBEMAP:
			return cubeMapList->getResourceCount();
			break;
		case RESOURCE_SHADER_PROGRAM:
			return shaderProgramList->getResourceCount();
			break;
		default:
			break;
	}

	return 0;
}
*/

/*---------------------------------------------------------------------------*/

/*
void SceneGraph::getResourceList(
		Resource			**list,
		ResourceType		type
	)
{
	switch (type) {
		case RESOURCE_VERTEX_ARRAY:
			vertexList->getAllResources(list);
			break;
		case RESOURCE_TEXTURE:
			textureList->getAllResources(list);
			break;
		case RESOURCE_RENDER_TARGET:
			renderTargetList->getAllResources(list);
			break;
		case RESOURCE_CUBEMAP:
			cubeMapList->getAllResources(list);
			break;
		case RESOURCE_SHADER_PROGRAM:
			shaderProgramList->getAllResources(list);
			break;
		default:
			break;
	}
}
*/

/*---------------------------------------------------------------------------*/

ShaderProgram *SceneGraph::createShaderProgram(
		const char		*nidentifier,
		char			*prefix,
		char			*shaderFile0,
		...
	)
{
	nidentifier = prepNewResourceName<ShaderProgram>(nidentifier);
	char **desc = (char **) malloc(sizeof(char *) * 16);
	va_list args;
	va_start(args, shaderFile0);
	i32 i = 0;
	while (shaderFile0) {
		desc[i++] = shaderFile0;
		shaderFile0 = va_arg(args, char *);
	}
	ShaderProgram *a = new ShaderProgram(nidentifier, desc, i, prefix);
	va_end(args);
	free(desc);
	if (!(a = ensureResourceSanity(a)))
		a = DefaultObjects::failureShaderProgram;
	return a;
}

/*---------------------------------------------------------------------------*/

ShaderProgram *SceneGraph::createShaderProgram(
		const char		*nidentifier,
		char			*prefix,
		char			**shaderFiles,
		i32				n
	)
{
	if (n == 0) {
		for (; n < 8; n++) {
			if (shaderFiles[n] == 0)
				break;
		}
	}
	nidentifier = prepNewResourceName<ShaderProgram>(nidentifier);
	ShaderProgram *a = new ShaderProgram(nidentifier, shaderFiles, n, prefix);
	if (!(a = ensureResourceSanity(a)))
		a = DefaultObjects::failureShaderProgram;
	return a;
}

/*---------------------------------------------------------------------------*/

#ifdef DEBUG

void SceneGraph::printDebugInfo()
{
	ensure_heap();

	shaderProgramList->printContents("ShaderProgram");
	vertexList->printContents("VertexArray");
	textureList->printContents("Texture");
	cubeMapList->printContents("CubeMap");

	ensure_heap();
}

#else

void SceneGraph::printDebugInfo()
{
}

#endif /* DEBUG */

/*---------------------------------------------------------------------------*/

void p_scene_tree_write(
		Node	*node,
		u32		level
	)
{
	char buf[1024];
	char *b;
	char *name = node->getName();

	memset(buf, ' ', level << 1);
	b = buf + (level << 1);

	switch (node->getNodeType()) {
		case NODE_CAMERA:
			sprintf(b, "~c7779<Cam> ~r%s", name);
			break;
		case NODE_GROUP:
			sprintf(b, "~c7989<Grp> ~r%s", name);
			break;
		case NODE_LIGHT:
			sprintf(b, "~c9879<Lgt> ~r%s", name);
			break;
		case NODE_WORLD:
			sprintf(b, "~c7979<Wld> ~r%s", name);
			break;
		case NODE_GEOMETRY:
			sprintf(b, "~c7899<Geo> ~r%s", name);
			break;
		default:
			sprintf(b, "~c6669<???> ~r%s", name);
			break;
	}

	Console::log(buf);
}

void p_scene_tree_traverse(
		Node	*curr,
		u32		level
	)
{
	u32 i;
	u32 n = curr->getChildrenCount();

	p_scene_tree_write(curr, level);

	level++;

	for (i = 0; i < n; i++) {
		p_scene_tree_traverse(curr->getChild(i), level);
	}
}

static const char *CMDHELP_scene_tree = {
	"/scene_tree"
};

bool CMD_scene_tree(
		u32		n_args,
		char	(*args)[RC_COMMAND_MAX_ARG_LEN]
	)
{
	u32 n_nodes, i;
	Node **node_list;

	n_nodes = SceneGraph::getNodeCount();

	node_list = (Node **) malloc(sizeof(Node *) * n_nodes);

	SceneGraph::getNodes(node_list);

	for (i = 0; i < n_nodes; i++) {
		Node *node = node_list[i];

		if (!node->getParent()) {
			p_scene_tree_traverse(node, 0);
		}

	}

	free(node_list);

	return true;
}

/*---------------------------------------------------------------------------*/

void *p_get_resource_list(
		ResourceType	type,
		u32				*n
	)
{
	// MTODO
	/*
	Resource **res_list;

	*n = SceneGraph::getResourceCount(type);

	res_list = (Resource **) malloc(sizeof(Resource *) * (*n));

	SceneGraph::getResourceList(res_list, type);

	return res_list;
	*/
	return 0;
}

static const char *CMDHELP_list_textures = {
	"/list_textures"
};

bool CMD_list_textures(
		u32		n_args,
		char	(*args)[RC_COMMAND_MAX_ARG_LEN]
	)
{
	/*
	u32 i, n;
	Texture **tex;

	tex = (Texture **) p_get_resource_list(RESOURCE_TEXTURE, &n);

	Console::log("Using %u textures...", n);
	Console::log(" ");

	for (i = 0; i < n; i++)
		Console::log(tex[i]->getIdentifier());

	free(tex);
	*/
	return true;
}

/*---------------------------------------------------------------------------*/

static const char *CMDHELP_node_info = {
	"/node_info <Node>"
};

bool CMD_node_info(
		u32		n_args,
		char	(*args)[RC_COMMAND_MAX_ARG_LEN]
	)
{
	Node *node;

	if (!Command::checkArgCount(1, n_args, args))
		return false;

	node = SceneGraph::getNode(args[0]);

	if (!node) {
		Console::log("Node not found");
		return false;
	}

	node->logInfo();

	return true;
}

/*---------------------------------------------------------------------------*/

void SceneGraph::registerCommands()
{
	Command::add("scene_tree", CMDHELP_scene_tree, CMD_scene_tree);

	Command::add("list_textures", CMDHELP_list_textures, CMD_list_textures);
	Command::add("node_info", CMDHELP_node_info, CMD_node_info);
}

/*---------------------------------------------------------------------------*/

template <> ResourceList *SceneGraph::getInternalResourceList<VertexArray>()   {return vertexList;}
template <> ResourceList *SceneGraph::getInternalResourceList<RenderTarget>()  {return renderTargetList;}
template <> ResourceList *SceneGraph::getInternalResourceList<Texture>()       {return textureList;}
template <> ResourceList *SceneGraph::getInternalResourceList<CubeMap>()       {return cubeMapList;}
template <> ResourceList *SceneGraph::getInternalResourceList<ShaderProgram>() {return shaderProgramList;}

/*---------------------------------------------------------------------------*/

