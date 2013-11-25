/*
 *
 *
 */

#ifndef RC_UNIFORM_H
#define RC_UNIFORM_H

		/* Types */

#define UNIFORM_FLAG_CLASS_MASK			0xFF

#define UNIFORM_FLAG_SCALAR				(1 << 0)

#define UNIFORM_FLAG_VECTOR2			(1 << 1)
#define UNIFORM_FLAG_VECTOR3			(1 << 2)
#define UNIFORM_FLAG_VECTOR4			(1 << 3)

#define UNIFORM_FLAG_MATRIX2			(1 << 4)
#define UNIFORM_FLAG_MATRIX3			(1 << 5)
#define UNIFORM_FLAG_MATRIX4			(1 << 6)

#define UNIFORM_FLAG_INT				(1 << 16)
#define UNIFORM_FLAG_FLOAT				(1 << 17)

#define UNIFORM_FLAG_TEXTURE			(1 << 18)
#define UNIFORM_FLAG_CUBEMAP			(1 << 19)

#define UNIFORM_FLAG_STRING				(1 << 24)
#define UNIFORM_FLAG_ARBITRARY			(1 << 25)

#define UNIFORM_FLAG_VECTOR				(UNIFORM_FLAG_VECTOR2 | UNIFORM_FLAG_VECTOR3 | UNIFORM_FLAG_VECTOR4)
#define UNIFORM_FLAG_MATRIX				(UNIFORM_FLAG_MATRIX2 | UNIFORM_FLAG_MATRIX3 | UNIFORM_FLAG_MATRIX4)

typedef struct {
	char			*name;
	u32				flags;
	u16				n;
	u16				size;
	void			*data;
} UniformEntry;

class Uniform {
	public:
		Uniform();
		virtual ~Uniform();

	public:
		template<class T> void setValue(const char *name, T value);
		template<class T> void setValueArray(const char *name, T *list, u32 n_values);
		template<class T> void setValueArray(const char *name, std::vector<T> &list);

		template<class T> T getValue(const char *name);
		template<class T> T getValueAt(const char *name, u32 i);

		void setTexture(const char *name, Texture *tex);
		void setCubeMap(const char *name, CubeMap *cubemap);

		Texture *getTexture(const char *name);
		CubeMap *getCubeMap(const char *name);

		void setString(const char *name, const char *string);
		void setData(const char *name, void *data, i32 size);
		
		const char *getString(const char *name);
		void *getData(const char *name);

		void remove(const char *name);

		UniformEntry *getEntries() const;

		u32 getEntryCount() const;

		void copyDataTo(
				Uniform				*uniform
			);

		void logData();

	private:
		void removeAt(u32 i);

		UniformEntry *prepEntry(const char *name, u32 size, u32 flags, u32 n);
		i32 findEntry(const char *name);

		

	private:
		std::vector<UniformEntry> *entries;

};

template<class T> i32 uniformMapFlag();
template<> i32 uniformMapFlag<i32>();
template<> i32 uniformMapFlag<f32>();
template<> i32 uniformMapFlag<vec2i>();
template<> i32 uniformMapFlag<vec2f>();
template<> i32 uniformMapFlag<vec3i>();
template<> i32 uniformMapFlag<vec3f>();
template<> i32 uniformMapFlag<vec4i>();
template<> i32 uniformMapFlag<vec4f>();
template<> i32 uniformMapFlag<mat2f>();
template<> i32 uniformMapFlag<mat3f>();
template<> i32 uniformMapFlag<mat4f>();
#include "Uniform.inl"

#endif /* RC_UNIFORM_H */


