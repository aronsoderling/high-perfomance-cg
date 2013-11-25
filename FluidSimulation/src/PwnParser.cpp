
/* LOWPRIO: Sorry! I'm out of order */


#include "RenderChimp.h"


float float_r(FILE *f)
{
	char buf[4];
	buf[0] = (char) fgetc(f);
	buf[1] = (char) fgetc(f);
	buf[2] = (char) fgetc(f);
	buf[3] = (char) fgetc(f);
	return *((float *) buf);
}
u32 uint_r(FILE *f)
{
	char buf[4];
	buf[0] = (char) fgetc(f);
	buf[1] = (char) fgetc(f);
	buf[2] = (char) fgetc(f);
	buf[3] = (char) fgetc(f);
	return  *((u32 *) buf);
}
int int_r(FILE *f)
{
	char buf[4];
	buf[0] = (char) fgetc(f);
	buf[1] = (char) fgetc(f);
	buf[2] = (char) fgetc(f);
	buf[3] = (char) fgetc(f);
	return *((int *) buf);
}
u8 uchar_r(FILE *f)
{
	char buf[1];
	buf[0] = (char) fgetc(f);
	
	return *((u8 *) buf);
}

void PwnParse(
		Node				*target,
		const char			*filename
	     )
{
	FILE* f;

	if (target->getNodeType() == NODE_GROUP) {

		f = fopen(filename, "rb");


		if (!f) {
			//REPORT_FILE(WRN_OPEN);
			printf("Error: could not open file\n");	
			return;
		}

		char str[1024];
		for (int i=0;i<8;++i){
			fscanf(f,"%s\n",str);
			//printf("%s\n",str);
		}
		unsigned int version = int_r(f);
		//printf("Version: %u\n",version);
		if (version != 12567){
			//REPORT(WRN_OPEN);
			return;
		}
		u32 numMeshes = uint_r(f);
		printf("%d meshes in pwn-file\n",numMeshes);
		for (u32 m = 0; m < numMeshes; ++m){
			char mat[1024] = "";
			char g[1024] = "";
			char map_Ka[1024] = "";
			char map_Kd[1024] = "";
			char map_Ks[1024] = "";
			char map_Ns[1024] = "";
			char map_d[1024] = "";
			char map_bump[1024] = "";
			
			fscanf(f,"g %s g",g);
			fscanf(f,"usemtl %s usemtl",mat);
			
			// Textures
			u8 mask = uchar_r(f);
			if (mask & 1)
				fscanf(f,"t %s t",map_Ka);
			if (mask & 2)
				fscanf(f,"t %s t",map_Kd);
			if (mask & 4)
				fscanf(f,"t %s t",map_Ks);
			if (mask & 8)
				fscanf(f,"t %s t",map_Ns);
			if (mask & 16)
				fscanf(f,"t %s t",map_d);
			if (mask & 32)
				fscanf(f,"t %s t",map_bump);
			
			
			
			
			
			//printf("g %s and usemtl %s\n",g,mat);
			u32 numV = int_r(f);
			u32 numI = int_r(f);
			u32 sPos = int_r(f);
			u32 sUV = int_r(f);
			u32 sNorm = int_r(f);
			u32 sTan = int_r(f);
			u32 sBitan = int_r(f);
			//printf("%u vertices, %u indices, vert: %u,%u,%u,%u,%u\n",numV,numI,sPos,sUV,sNorm,sTan,sBitan);
			u32 stride = 14;
			f32* sv_array = (f32*)(malloc(sizeof(f32)* numV*stride));
			u32* si_array = (u32*)(malloc(sizeof(u32)* numI));
			for (unsigned int i=0;i<numV*stride;++i)
				sv_array[i] = float_r(f);
			for (unsigned int i=0;i<numI;++i)
				si_array[i] = uint_r(f);


			VertexArray* va = SceneGraph::createVertexArray(0, sv_array, stride*sizeof(f32), numV, TRIANGLES, USAGE_STATIC);
			va->setIndexArray(si_array, sizeof(u32), numI);

			u32 offset = 0;
			if (sPos){
				va->setAttribute("Vertex", offset, sPos, ATTRIB_FLOAT32, false);
				offset += sPos * sizeof(f32);
			}
			if (sUV){
				va->setAttribute("Texcoord", offset, sUV, ATTRIB_FLOAT32, false);
				offset += sUV * sizeof(f32);
			}
			if (sNorm){
				va->setAttribute("Normal", offset, sNorm, ATTRIB_FLOAT32, false);
				offset += sNorm * sizeof(f32);
			}
			if (sTan){
				va->setAttribute("Tangent", offset, sTan, ATTRIB_FLOAT32, false);
				offset += sTan * sizeof(f32);
			}
			if (sBitan){
				va->setAttribute("Binormal", offset, sBitan, ATTRIB_FLOAT32, false);
				offset += sBitan * sizeof(f32);
			}



			Geometry* mesh = SceneGraph::createGeometry(g, va, false);
			target->attachChild(mesh);

            mesh->setValue("HasAmbientTexture", 0);
            mesh->setValue("HasDiffuseTexture", 0);
            mesh->setValue("HasSpecularTexture", 0);
            mesh->setValue("HasNormalTexture", 0);

            mesh->setValue("AmbientColor", vec3f(0.2f));
            mesh->setValue("DiffuseColor", vec3f(0.5f));
            mesh->setValue("SpecularColor", vec3f(0.5f));
            mesh->setValue("Shininess", 8.f);
          
            


			mesh->setString("Material", mat);
			if (map_Ka[0]){
                mesh->setTexture("AmbientTexture", SceneGraph::createTexture(map_Ka));
                mesh->setValue("HasAmbientTexture", 1);
            }
			if (map_Kd[0]){
                mesh->setTexture("DiffuseTexture", SceneGraph::createTexture(map_Kd));
                mesh->setValue("HasDiffuseTexture", 1);
                }
			if (map_Ks[0]){
				mesh->setString("map_Ks", map_Ks);
            }
			if (map_Ns[0]){
                mesh->setTexture("SpecularTexture", SceneGraph::createTexture(map_Ns));
                mesh->setValue("HasSpecularTexture", 1);
            }
			if (map_d[0]){
				mesh->setString("map_d", map_d);
            }
			if (map_bump[0]){
                mesh->setTexture("NormalTexture", SceneGraph::createTexture(map_bump));
                mesh->setValue("HasNormalTexture", 1);
            }

	
			free(sv_array);
			free(si_array);

		}

		



		fclose(f);
	}
}




