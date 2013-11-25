/*
 * .obj Parser
 *
 * Parser for the mesh format .obj
 *
 */

#ifndef RC_OBJPARSER_H
#define RC_OBJPARSER_H

class OBJParser {

	private:
		struct IVertex {
			i32 pos;
			i32 norm;
			//i32 binorm;
			//i32 tangent;
			i32 uv;
			bool operator<(const IVertex &other) const {
				if (pos < other.pos) return true;
				if (norm < other.norm) return true;
				//if (binorm < other.binorm) return true;
				//if (tangent < other.tangent) return true;
				return uv < other.uv;
			}
			/*
			bool operator()(const IVertex &a, const IVertex &b) const {
				return a.pos == b.pos && a.norm == b.norm && a.binorm == b.binorm && a.tangent == b.tangent && a.uv == b.uv;
			}
			bool operator()(const IVertex *a, const IVertex *b) const {
				return a->pos == b->pos && a->norm == b->norm && a->binorm == b->binorm && a->tangent == b->tangent && a->uv == b->uv;
			}
			*/
			bool operator()(const IVertex &a, const IVertex &b) const {
				return a.pos == b.pos && a.norm == b.norm && a.uv == b.uv;
			}
			bool operator()(const IVertex *a, const IVertex *b) const {
				return a->pos == b->pos && a->norm == b->norm && a->uv == b->uv;
			}
			//operator size_t() (const IVertex &v) {
			operator size_t() const {
				return pos ^
					((norm << 17) | (norm >> 15)) ^
					((uv << 11)  | (uv << 21));
			}
		};
		struct IVertexHash {
			size_t operator() (const IVertex &a) const {
				return a.pos ^
				((a.norm << 17) | (a.norm >> 15)) ^
				((a.uv << 11)  | (a.uv << 21));
			}
		};

		struct Vertex {
			Vertex(vec3f p, vec3f n, vec2f t) : pos(p), norm(n), uv(t) {}
			Vertex(vec3f p, vec3f n, vec3f nb, vec3f nt, vec2f t) : pos(p), norm(n), binorm(nb), tangent(nt), uv(t) {}
			vec3f pos;
			vec3f norm;
			vec3f binorm;
			vec3f tangent;
			vec2f uv;
		};

		struct ITriangle {
			IVertex vtx[3];
		};

		struct Material {
			Material() {
				name = 0;
				ambientColor = vec3f(0.0f, 0.0f, 0.0f);
				diffuseColor = vec3f(1.0f, 1.0f, 1.0f);
				specularColor = 0.2f * vec3f(1.0f, 1.0f, 1.0f);
				ambientTexture = 0;
				diffuseTexture = 0;
				specularTexture = 0;
				bumpTexture = 0;
				shininess = 10.0f;
				sharpness = 1.0f;
				transparency = 0.0f;
				reflectivity = 0.0f;
				refractionIndex = 1.0f;
			}

			void clear() {
				if (name)
					free(name);
				if (ambientTexture)
					free(ambientTexture);
				if (diffuseTexture)
					free(diffuseTexture);
				if (specularTexture)
					free(specularTexture);
				if (bumpTexture)
					free(bumpTexture);
			}


			char *name;
			vec3f ambientColor;
			vec3f diffuseColor;
			vec3f specularColor;
			char *ambientTexture;
			char *diffuseTexture;
			char *specularTexture;
			char *bumpTexture;
			f32 shininess;
			f32 sharpness;
			f32 transparency;
			f32 reflectivity;
			f32 refractionIndex;
		};

	public:
		OBJParser();
		~OBJParser();

		/** Parse an .obj file and place it under the "target" node */
		bool parse(
			Node				*target,
			const char			*filename
		);

	private:
		Geometry *createGeometry(const char *fn, char *name, Material *m, i32 from, i32 to);
		bool parseMaterials(const char *mtlfile, std::vector<Material> &materials);

	private:
		std::vector<vec3f> vPos;				///< Array of vertex positions.
		std::vector<vec3f> vNorm;				///< Array of vertex normals.
		std::vector<vec3f> vBinorm;				
		std::vector<vec3f> vTangent;			
		std::vector<vec2f> vUv;					///< Array of vertex UV coordinates.
		std::vector<ITriangle> faces;			///< Array of triangles.
		std::vector<i32> faceOffset;
		std::vector<i32> polyOffset;
		std::vector<Material> materials;		///< Array of materials.
		std::vector<Vertex> attribs;
#ifdef RC_WIN32
		hash_map<IVertex, i32> vhash;
#else
		hash_map<IVertex, i32, IVertexHash> vhash;
#endif
		std::vector<u32> indices;

};

#endif /* RC_OBJPARSER_H */

