#include "RenderChimp.h"
#include "VertexArray.h"

/*---------------------------------------------------------------------------*/

VertexArray::VertexArray(
		const char				*nidentifier,

		void					*new_array,
		u32						nstride,
		u32						n,

		PrimitiveLayout_t		layout,

		AttributeUsage_t		nusage

	) : Resource(nidentifier, NO_PURGE, RESOURCE_VERTEX_ARRAY), Uniform()
{
	handle = 0;
	n_attribs = 0;
	n_indices = 0;
	stride = nstride;
	varray_bytes = 0;
	iarray_bytes = 0;

	indexArrayTypeSize = 0;

	primitiveLayout = layout;

	usage = nusage;
	n_elements = n;

	holds_index_array = false;
	holds_array = false;

	iarray = 0;
	varray = 0;

	has_bounds = false;

	setArray(new_array, n);
}

/*---------------------------------------------------------------------------*/

VertexArray::VertexArray(
		const char				*nidentifier,
		u32						nstride,
		PrimitiveLayout_t		layout,
		AttributeUsage_t		nusage

	) : Resource(nidentifier, NO_PURGE, RESOURCE_VERTEX_ARRAY), Uniform()
{
	handle = 0;
	n_attribs = 0;
	n_indices = 0;
	stride = nstride;
	varray_bytes = 0;
	iarray_bytes = 0;

	indexArrayTypeSize = 0;

	primitiveLayout = layout;

	usage = nusage;
	n_elements = 0;

	holds_index_array = false;
	holds_array = false;

	has_bounds = false;

	iarray = 0;
	varray = 0;
}

/*---------------------------------------------------------------------------*/

VertexArray::~VertexArray()
{
	u32 i;

	for (i = 0; i < n_attribs; i++)
		free(attrib[i].name);

	Renderer::unloadVertexArray(this);

	if (iarray)
		free(iarray);

	if (varray)
		free(varray);
}

/*---------------------------------------------------------------------------*/

void VertexArray::setIndexArray(
		void					*new_array,
		u32						type_size,
		u32						n
	)
{
//#ifdef RC_IPHONE
	u16 *small_array = 0;
//#endif
	
	if (n == 0 || !new_array || type_size == 0) {
		/* TODO: Remove index array */

		holds_index_array = false;

		return;
	}

	ASSERT(type_size == 1 || type_size == 2 || type_size == 4);

	index_usage = USAGE_STATIC;
	
	/*
	if (type_size == 4 && n < 65535) {
		u32 *ii = (u32 *) new_array;
		u32 i;
		small_array = (u16 *) malloc(sizeof(u16) * n);
		for (i = 0; i < n; i++) {
			small_array[i] = (u16) ii[i];
		}
		
		type_size = 2;
		new_array = small_array;
	}
	*/
	
	if (!holds_index_array) {
		Renderer::loadIndexArray(this, new_array, type_size, n, index_usage);
	} else {
		Renderer::reloadIndexArray(this, new_array, n);
	}

	if (iarray) {
		if (n * type_size > iarray_bytes) {
			iarray = realloc(iarray, n * type_size);
			iarray_bytes = n * type_size;
		}
	} else {
		iarray = malloc(n * type_size);
		iarray_bytes = n * type_size;
	}
	memcpy(iarray, new_array, n * type_size);

	if (small_array)
		free(small_array);
	
	holds_index_array = true;

	n_indices = n;
	indexArrayTypeSize = type_size;
}

/*---------------------------------------------------------------------------*/

void VertexArray::setAttribute(
		const char				*name,
		u32						offset,
		u32						length,
		AttributeType_t			type,
		bool					normalized
	)
{
	i32 i;

	if (length == 0)
		return;

	if (n_attribs >= RC_MAX_ATTRIBS) {
		REPORT_WARNING("Too many attributes added for vertex array resource %s", identifier);
		return;
	}

	if (offset >= stride) {
		REPORT_WARNING("The offset is greater or equal to stride (%s)", identifier);
		return;
	}

	for (i = 0; i < (i32) n_attribs; i++) {
		if (strcmp(name, attrib[i].name) == 0) {
			REPORT_WARNING("Attribute %s already exists", name);
			return;
		}
	}

	i = n_attribs - 1;

	while (i >= 0 && offset < attrib[i].offset) {
		attrib[i + 1].offset = attrib[i].offset;
		attrib[i + 1].length = attrib[i].length;
		attrib[i + 1].name = attrib[i].name;
		attrib[i + 1].type = attrib[i].type;
		attrib[i + 1].normalized = attrib[i].normalized;
		i--;
	}

	i++;

	attrib[i].offset = offset;
	attrib[i].length = length;
	attrib[i].name = duplicateString(name);
	attrib[i].type = type;
	attrib[i].normalized = normalized;

	n_attribs++;
}

/*---------------------------------------------------------------------------*/

void VertexArray::setArray(
		void					*new_array,
		u32						n				/* Number of structs */
	)
{
	if (n == 0)
		REPORT_WARNING("Number of structs set to 0");

	if (holds_array) {
		Renderer::reloadVertexArray(this, new_array, n);
	} else {
		Renderer::loadVertexArray(this, new_array, stride, n, usage);
		holds_array = true;
	}

	has_bounds = false;

	if (varray) {
		if (n * stride > varray_bytes) {
			varray = realloc(varray, n * stride);
			varray_bytes = n * stride;
		}
	} else {
		varray = malloc(n * stride);
		varray_bytes = n * stride;
	}
	memcpy(varray, new_array, n * stride);

	n_elements = n;
}

/*---------------------------------------------------------------------------*/

u32 VertexArray::getLength()
{
	return n_elements;
}

/*---------------------------------------------------------------------------*/

u32 VertexArray::getStride()
{
	return stride;
}

/*---------------------------------------------------------------------------*/

PrimitiveLayout_t VertexArray::getPrimitiveLayout()
{
	return primitiveLayout;
}

/*---------------------------------------------------------------------------*/

//void VertexArray::generateNormals()
//{
#if 0
	u32 i, offset;
	u32 old_stride = stride;
	f32 *new_array;
	f32 *old_iter, *new_iter;
	u16 *index_array;

	/* TODO: Fix u16 */

	if (indexArray) {
		REPORT_WARNING("Can't generate normals without index array");
		/* TODO: But it should be able to... */
		return;
	}

	for (i = 0; i < n_attribs; i++) {

		if (strcmp(attrib[i].name, "Vertex") == 0)
			break;

	}

	if (i == n_attribs) {
		REPORT_WARNING("No vertex positions found");
		return;
	}

	offset = attrib[i].offset;

	for (i = 0; i < n_attribs; i++) {
		if (strcmp(attrib[i].name, "Normal") == 0) {
			REPORT_WARNING("Normal attribute already found");
			return;
		}
	}

	stride += 3 * 4;

	setAttribute("Normal", stride - 3 * 4, 3, ATTRIB_FLOAT32, false);

	new_array = (f32 *) malloc(sizeof(f32) * n_elements * stride);

	old_iter = vertexArray;
	new_iter = new_array;

	for (i = 0; i < n_elements; i++) {
		memcpy(new_iter, old_iter, old_stride * sizeof(f32));
		new_iter += old_stride;

		new_iter[0] = 0.0f;
		new_iter[1] = 0.0f;
		new_iter[2] = 0.0f;

		new_iter += 3;

		old_iter += old_stride;
	}

	index_array = (u16 *) indexArray;

	for (i = 0; i < n_indices; i += 3) {
		f32 *p0 = &vertexArray[index_array[i  ] * old_stride + offset];
		f32 *p1 = &vertexArray[index_array[i+1] * old_stride + offset];
		f32 *p2 = &vertexArray[index_array[i+2] * old_stride + offset];

		f32 *t0 = &new_array[index_array[i  ] * stride + old_stride];
		f32 *t1 = &new_array[index_array[i+1] * stride + old_stride];
		f32 *t2 = &new_array[index_array[i+2] * stride + old_stride];

		vec3f v0 = vec3f(p0[0], p0[1], p0[2]);
		vec3f v1 = vec3f(p1[0], p1[1], p1[2]);
		vec3f v2 = vec3f(p2[0], p2[1], p2[2]);

		vec3f e0 = v2 - v1;
		vec3f e1 = v1 - v0;

		vec3f normal = e0 % e1;

		f32 area = 0.5f * normal.length();

		normal.normalize();

		normal *= area;

		t0[0] += normal.x;
		t0[1] += normal.y;
		t0[2] += normal.z;

		t1[0] += normal.x;
		t1[1] += normal.y;
		t1[2] += normal.z;

		t2[0] += normal.x;
		t2[1] += normal.y;
		t2[2] += normal.z;
	}

	new_iter = &new_array[old_stride];

	/* Normalize our new normals */
	for (i = 0; i < n_elements; i++) {
		f32 *p = new_iter;

		f32 len = p[0] * p[0] + p[1] * p[1] + p[2] * p[2];

		if (len != 0.0f) {
			f32 invlen = 1.0f / sqrt(len);

			p[0] *= invlen;
			p[1] *= invlen;
			p[2] *= invlen;
		} else { /* Just in case... */
			p[0] = 1.0f;
			p[1] = 0.0f;
			p[2] = 0.0f;
		}

		new_iter += stride;
	}

	free(vertexArray);
	vertexArray = new_array;

#endif
//}

/*---------------------------------------------------------------------------*/

void VertexArray::setVertexArray(
		VertexArray				**target_vertex_array,
		VertexArray				*new_vertex_array,

		bool					instantiate_resource
	)
{
	(*target_vertex_array)->has_bounds = false;

	if (*target_vertex_array)
		SceneGraph::dereferenceResource(*target_vertex_array);

	if (instantiate_resource) {
		*target_vertex_array = (VertexArray *) new_vertex_array->instantiate();
	} else {
		*target_vertex_array = new_vertex_array;
	}
}

/*---------------------------------------------------------------------------*/

#ifdef DEBUG

void VertexArray::print(
		FILE		*f
	)
{

#ifdef RC_LOG_TO_FILE
	#define prnt0(a)		fprintf(f, a)
	#define prnt1(a, b)		fprintf(f, a, b)
#else /* RC_LOG_TO_FILE */
	#define prnt0(a)		printf(a)
	#define prnt1(a, b)		printf(a, b)
#endif /* RC_LOG_TO_FILE */

	prnt1("    [ N ELEMS  ][ %u ]\n", n_elements);
	prnt1("    [ STRIDE   ][ %u ]\n", stride);
	prnt1("    [ N ATTR.  ][ %u ]\n", n_attribs);

}

#endif /* DEBUG */

/*---------------------------------------------------------------------------*/

u32 VertexArray::getAttributeCount()
{
	return n_attribs;
}

/*---------------------------------------------------------------------------*/

Attribute_t *VertexArray::getAttribute(
		u32						i
	)
{
	return &attrib[i];
}

/*---------------------------------------------------------------------------*/

Attribute_t *VertexArray::getAttribute(
		char					*name
	)
{
	u32 i;

	for (i = 0; i < n_attribs; i++) {

		if (strcmp(attrib[i].name, name) == 0)
			return &attrib[i];

	}

	return 0;
}

/*---------------------------------------------------------------------------*/

u32 VertexArray::getIndexTypeSize()
{
	return indexArrayTypeSize;
}

/*---------------------------------------------------------------------------*/

u32 VertexArray::getIndexLength()
{
	return n_indices;
}

/*---------------------------------------------------------------------------*/

AttributeUsage_t VertexArray::getUsage()
{
	return usage;
}

/*---------------------------------------------------------------------------*/

AttributeUsage_t VertexArray::getIndexUsage()
{
	return index_usage;
}

/*---------------------------------------------------------------------------*/

void *VertexArray::getHandle()
{
	return handle;
}

/*---------------------------------------------------------------------------*/

void VertexArray::setHandle(
		void				*nhandle
	)
{
	handle = nhandle;
}

/*---------------------------------------------------------------------------*/

void VertexArray::getBoundingBox(
		AABox				*target
	)
{
	u8 *va;
	u32 i;
	Attribute_t *a;
	AABox box;

	if (has_bounds) {
		*target = bounds;
		return;
	}

	box.minCorner = vec3f(0.0f, 0.0f, 0.0f);
	box.maxCorner = vec3f(0.0f, 0.0f, 0.0f);

	a = getAttribute("Vertex");

	if (!a) {
		REPORT_WARNING("No \"Vertex\" attribute to extract bounding box from");
		*target = box;
		return;
	}

	if (a->type != ATTRIB_FLOAT32) {
		REPORT_WARNING("The \"Vertex\" attribute must be a 32 bit float");
		*target = box;
		return;
	}

	if (!varray) {
		REPORT_WARNING("No vertex array attached");
		*target = box;
		return;
	}

	if (n_elements == 0) {
		REPORT_WARNING("Vertex array contains no elements");
		*target = box;
		return;
	}

	bounds.minCorner = fMax;
	bounds.maxCorner = -fMax;

	va = (u8 *) varray;
	va += a->offset;

	for (i = 0; i < n_elements; i++) {
		f32 *vtx;
		vec3f pt;

		vtx = (f32 *) va;
		pt = vec3f(vtx[0], vtx[1], vtx[2]);
		bounds.include(pt);

		va += stride;
	}

	has_bounds = true;

	*target = bounds;
}

/*---------------------------------------------------------------------------*/

#if 0
vec3f VertexArray::getPosition(
		u32					i
	)
{
	
}
#endif

/*---------------------------------------------------------------------------*/
