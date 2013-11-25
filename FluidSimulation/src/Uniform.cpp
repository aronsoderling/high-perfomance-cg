#include "RenderChimp.h"
#include "Uniform.h"



/*---------------------------------------------------------------------------*/

Uniform::Uniform()
{
	entries = new std::vector<UniformEntry>();
	entries->reserve(10);
}

/*---------------------------------------------------------------------------*/

Uniform::~Uniform()
{
	u32 n;
	n = entries->size();
	for (u32 i = 0; i < n; i++)
		removeAt(0);
	ASSERT(entries->size() == 0);
	delete entries;
}

/*---------------------------------------------------------------------------*/

UniformEntry *Uniform::prepEntry(
		const char			*name,
		u32					size,
		u32					flags,
		u32					n
	)
{
	u32 ne = entries->size();
	for (u32 i = 0; i < ne; i++) {
		UniformEntry *ue = &entries->at(i);
		if (strcmp(ue->name, name) == 0) {
			if (ue->flags != flags || ue->n != n) {
				removeAt(i);
				i--;
				continue;
			} else
				return ue;
		}
	}

	UniformEntry ue;
	ue.name = duplicateString(name);
	ue.data = (void *) malloc(size * n);

	entries->push_back(ue);
	return &entries->back();
}

/*---------------------------------------------------------------------------*/

i32 Uniform::findEntry(
		const char			*name
	)
{
	u32 n = entries->size();
	for (u32 i = 0; i < n; i++) {
		UniformEntry *ue = &entries->at(i);
		if (strcmp(ue->name, name) == 0)
			return (i32) i;
	}

	return -1;
}

/*---------------------------------------------------------------------------*/

void Uniform::setTexture(
		const char			*name,
		Texture				*texture
	)
{
	UniformEntry *entry;
	u32 flags;
	flags = UNIFORM_FLAG_TEXTURE;
	entry = prepEntry(name, 4, flags, 1);
	size_t *ptr = (size_t *) texture->getHandle();
	memcpy(entry->data, &ptr, sizeof(size_t));
	entry->n = 1;
	entry->flags = flags;
	entry->size = sizeof(size_t);
}

/*---------------------------------------------------------------------------*/

void Uniform::setCubeMap(
		const char			*name,
		CubeMap				*cubemap
	)
{
	UniformEntry *entry;
	u32 flags;
	flags = UNIFORM_FLAG_CUBEMAP;
	entry = prepEntry(name, 4, flags, 1);
	size_t *ptr = (size_t *) cubemap->getHandle();
	memcpy(entry->data, &ptr, sizeof(size_t));
	entry->n = 1;
	entry->flags = flags;
	entry->size = sizeof(size_t);
}

/*---------------------------------------------------------------------------*/

void Uniform::setString(const char *name, const char *string)
{
	UniformEntry *entry;
	u32 flags;
	flags = UNIFORM_FLAG_STRING;
	i32 len = strlen(string);
	entry = prepEntry(name, 1, flags, len + 1);
	memcpy(entry->data, string, len + 1);
	entry->n = (i16) (len + 1);
	entry->flags = flags;
	entry->size = 1;
}

/*---------------------------------------------------------------------------*/

void Uniform::setData(const char *name, void *data, i32 size)
{
	UniformEntry *entry;
	u32 flags;
	flags = UNIFORM_FLAG_STRING;
	entry = prepEntry(name, 1, flags, size);
	memcpy(entry->data, data, size);
	entry->n = (i16) size;
	entry->flags = flags;
	entry->size = 1;
}

/*---------------------------------------------------------------------------*/

void Uniform::removeAt(
		u32					i
	)
{
	UniformEntry *ue = &entries->at(i);
	free(ue->data);
	free(ue->name);
	entries->erase(entries->begin() + i);
}

/*---------------------------------------------------------------------------*/

void Uniform::remove(
		const char		*name
	)
{
	u32 n = entries->size();
	for (u32 i = 0; i < n; i++) {
		UniformEntry *ue = &entries->at(i);
		if (strcmp(name, ue->name) == 0) {
			removeAt(i);
			return;
		}
	}
}

/*---------------------------------------------------------------------------*/

UniformEntry *Uniform::getEntries() const
{
	if (entries->empty())
		return 0;
	return &entries->at(0);
}

/*---------------------------------------------------------------------------*/

u32 Uniform::getEntryCount() const
{
	return entries->size();
}

/*---------------------------------------------------------------------------*/

void Uniform::copyDataTo(
		Uniform				*uniform
	)
{
	u32 i;
	u32 n = entries->size();

	for (i = 0; i < n; i++) {
		UniformEntry *u = &entries->at(i);
		UniformEntry *un = uniform->prepEntry(u->name, u->size, u->flags, u->n);
		memcpy(un->data, u->data, u->size * u->n);
		un->n = u->n;
		un->flags = u->flags;
		un->size = u->size;
	}
}

/*---------------------------------------------------------------------------*/

void Uniform::logData()
{
	u32 i, j;
	u32 n = entries->size();

	for (i = 0; i < n; i++) {
		UniformEntry *u = &entries->at(i);

		if (u->flags & UNIFORM_FLAG_TEXTURE)
			Console::log("Texture %s = %x", u->name, *((u32 *) u->data));
		else if (u->flags & UNIFORM_FLAG_CUBEMAP)
			Console::log("CubeMap %s = %x", u->name, *((u32 *) u->data));
		else if (u->flags & UNIFORM_FLAG_SCALAR) {
			UniformType_t t = (u->flags & UNIFORM_FLAG_INT) ? UNIFORM_INT32 : UNIFORM_FLOAT32;
			if (t == UNIFORM_INT32) {
				i32 p = *((i32 *) u->data);
				Console::log("int[%d] %s = ", u->n, u->name);
				for (j = 0; j < u->n; j++) {
					Console::log("  %d", p);
					p++;
				}
			} else {
				f32 p = *((f32 *) u->data);
				Console::log("float[%d] %s = ", u->n, u->name);
				for (j = 0; j < u->n; j++) {
					Console::log("  %.8f", p);
					p++;
				}
			}
		} else if (u->flags & UNIFORM_FLAG_VECTOR) {
			u32 dim = u->flags & UNIFORM_FLAG_VECTOR2 ? 2 : (u->flags & UNIFORM_FLAG_VECTOR3 ? 3 : 4);
			UniformType_t t = (u->flags & UNIFORM_FLAG_INT) ? UNIFORM_INT32 : UNIFORM_FLOAT32;

			if (u->n > 1) {
				if (t == UNIFORM_INT32) {
					i32 *p = (i32 *) u->data;
					Console::log("vec%i[%d] %s = ", dim, u->n, u->name);
					for (j = 0; j < u->n; j++) {
						switch (dim) {
							default:
							case 2: Console::log("  [%d, %d]", p[0], p[1]);
								p += 2;
							break;
							case 3: Console::log("  [%d, %d, %d]", p[0], p[1], p[2]);
								p += 3;
							break;
							case 4: Console::log("  [%d, %d, %d, %d]", p[0], p[1], p[2], p[3]);
								p += 4;
							break;
						}
					}
				} else {
					f32 *p = (f32 *) u->data;
					Console::log("vec%df[%d] %s = ", dim, u->n, u->name);
					for (j = 0; j < u->n; j++) {
						switch (dim) {
							default:
							case 2: Console::log("  [%.8f, %.8f]", p[0], p[1]);
								p += 2;
							break;
							case 3: Console::log("  [%.8f, %.8f, %.8f]", p[0], p[1], p[2]);
								p += 3;
							break;
							case 4: Console::log("  [%.8f, %.8f, %.8f, %.8f]", p[0], p[1], p[2], p[3]);
								p += 4;
							break;
						}
					}
				}
			}

		} else if (u->flags & UNIFORM_FLAG_MATRIX) {
			u32 dim = u->flags & UNIFORM_FLAG_MATRIX2 ? 2 : (u->flags & UNIFORM_FLAG_MATRIX3 ? 3 : 4);
			
			if (u->n > 1) {
				f32 *p = (f32 *) u->data;
				Console::log("mat%d[%d] %s = ", dim, u->n, u->name);
				for (j = 0; j < u->n; j++) {
					switch (dim) {
						default:
						case 2:
							Console::log("  [%.8f %.8f", p[0], p[1]);
							Console::log("   %.8f %.8f]", p[2], p[3]);
							p += 4;
						break;
						case 3:
							Console::log("  [%.8f %.8f %.8f", p[0], p[1], p[2]);
							Console::log("   %.8f %.8f %.8f", p[3], p[4], p[5]);
							Console::log("   %.8f %.8f %.8f]", p[6], p[7], p[8]);
							p += 9;
						break;
						case 4:
							Console::log("  [%.8f %.8f %.8f %.8f", p[0], p[1], p[2], p[3]);
							Console::log("   %.8f %.8f %.8f %.8f", p[4], p[5], p[6], p[7]);
							Console::log("   %.8f %.8f %.8f %.8f", p[8], p[9], p[10], p[11]);
							Console::log("   %.8f %.8f %.8f %.8f]", p[12], p[13], p[14], p[15]);
							p += 16;
						break;
					}
				}
			}

		}
	}
}

template<class T> i32 uniformMapFlag() {REPORT_ERROR("Data type is not supported"); return 0;}
template<> i32 uniformMapFlag<i32>()   {return UNIFORM_FLAG_INT   | UNIFORM_FLAG_SCALAR ;}
template<> i32 uniformMapFlag<f32>()   {return UNIFORM_FLAG_FLOAT | UNIFORM_FLAG_SCALAR ;}
template<> i32 uniformMapFlag<vec2i>() {return UNIFORM_FLAG_INT   | UNIFORM_FLAG_VECTOR2;}
template<> i32 uniformMapFlag<vec2f>() {return UNIFORM_FLAG_FLOAT | UNIFORM_FLAG_VECTOR2;}
template<> i32 uniformMapFlag<vec3i>() {return UNIFORM_FLAG_INT   | UNIFORM_FLAG_VECTOR3;}
template<> i32 uniformMapFlag<vec3f>() {return UNIFORM_FLAG_FLOAT | UNIFORM_FLAG_VECTOR3;}
template<> i32 uniformMapFlag<vec4i>() {return UNIFORM_FLAG_INT   | UNIFORM_FLAG_VECTOR4;}
template<> i32 uniformMapFlag<vec4f>() {return UNIFORM_FLAG_FLOAT | UNIFORM_FLAG_VECTOR4;}
template<> i32 uniformMapFlag<mat2f>() {return UNIFORM_FLAG_FLOAT | UNIFORM_FLAG_MATRIX2;}
template<> i32 uniformMapFlag<mat3f>() {return UNIFORM_FLAG_FLOAT | UNIFORM_FLAG_MATRIX3;}
template<> i32 uniformMapFlag<mat4f>() {return UNIFORM_FLAG_FLOAT | UNIFORM_FLAG_MATRIX4;}

/*---------------------------------------------------------------------------*/
