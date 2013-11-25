#include "RenderChimp.h"
#include "DefaultObjects.h"
#include "Command.h"


/*---------------------------------------------------------------------------*/

template<class T>
T *SceneGraph::getResource(
		const char		*identifier
	)
{
	ResourceList *list = getInternalResourceList<T>();
	return (T *) list->getResource(identifier);
}

/*---------------------------------------------------------------------------*/

template<class T>
u32 SceneGraph::getResourceCount()
{
	ResourceList *list = getInternalResourceList<T>();
	return list->getResourceCount();
}

/*---------------------------------------------------------------------------*/

template<class T>
void SceneGraph::getResourceList(
		std::vector<Resource *> &list
	)
{
	ResourceList *rlist = getInternalResourceList<T>();
	rlist->getAllResources(list);
}

/*---------------------------------------------------------------------------*/

template<class T>
const char *SceneGraph::prepNewNodeName(const char *nname)
{
	if (!nname || !*(nname))
		nname = Node::getNewNodeName(nameBuf, nodeGetTypeOf<T>());
	return nname;
}

/*---------------------------------------------------------------------------*/

template<class T>
T *SceneGraph::ensureNodeSanity(T *node)
{
	if (node->broken) {
		deleteNode(node);
		return 0;
	}
	nodeList->add(node);
	return node;
}

/*---------------------------------------------------------------------------*/

template<class T>
const char *SceneGraph::prepNewResourceName(const char *nidentifier)
{
	Resource *a;
	if (!nidentifier || !(*nidentifier))
		nidentifier = getNewResourceName<T>(nameBuf, false);
		//nidentifier = getNewResourceName(buf, Resource::getResourceTypeOf<T>(), false);
	ResourceList *rlist = SceneGraph::getInternalResourceList<T>();
	a = rlist->getResource(nidentifier);
	if (a) {
		REPORT_WARNING("A resource with the specified identifier \"%s\" already exists", nidentifier);
		nidentifier = getNewResourceName<T>(nameBuf, true);
	}
	return nidentifier;
}


/*---------------------------------------------------------------------------*/

static char *ResourceName[] = {
	"VertexArray",
	"Texture",
	"RenderTarget",
	"Cubemap",
	"ShaderProgram",
	""
};

/*---------------------------------------------------------------------------*/

template<class T>
char *SceneGraph::getNewResourceName(
		char			*buf,
		bool			conflicted
	)
{
	u32 idx = getResourceCount<T>();
	do {
		sprintf(buf, "%s%s%.2d", ResourceName[(i32) Resource::getResourceTypeOf<T>()], conflicted ? "Conflicted" : "", idx);
	} while (getResource<T>(buf));
	return buf;
}


/*---------------------------------------------------------------------------*/

template<class T>
T *SceneGraph::ensureResourceSanity(T *resource)
{
	if (resource->broken) {
		REPORT_WARNING("Resource \"%s\" broken. Returning NULL pointer", resource->getIdentifier());
		delete resource;
		resource = 0;
	} else {
		ResourceList *rlist = getInternalResourceList<T>();
		rlist->add(resource);
	}
	return resource;
}

/*---------------------------------------------------------------------------*/

template<class T>
T *SceneGraph::createEmptyNode(const char *nname)
{
	nname = prepNewNodeName<T>(nname);
	T *node = new T(nname);
	return ensureNodeSanity(node);
}

/*---------------------------------------------------------------------------*/

