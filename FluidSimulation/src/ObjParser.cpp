/*
 *  This OBJ-parser is based on code created by Petrik Clarberg on 2006-02-14.
 *  Copyright 2006 Lund University. All rights reserved.
 */

#include <vector>
#include <sstream>
#include <fstream>

#include "RenderChimp.h"
#include "SceneParser.h"
#include "Result.h"

#include "ObjParser.h"


using namespace std;


bool OBJParser::parseMaterials(
		const char *mtlfile,
		std::vector<Material> &materials
	)
{
	// Open file
	ifstream is;
	is.open(mtlfile, ios_base::in);	// open in text mode
	if (!is.is_open())
		return false;

	char line[4096], name[256];
	int line_num = 0;

	bool first = true;

	Material m;

	while (is.good()) {
		is.getline(line, 4096);
		string s(line);
		istringstream iss(s);
		line_num++;
		
		string what;
		iss >> what;

		if (what[0] == '#')
			continue;
		if (what=="newmtl") {	// Material
			if (!first) {
				m.name = duplicateString(name);
				materials.push_back(m);
			}
			first = false;
			iss >> name;
			m = Material();
		}
		else if (what == "Ka")
			iss >> m.ambientColor.r >> m.ambientColor.g >> m.ambientColor.b;
		else if (what == "Kd")
			iss >> m.diffuseColor.r >> m.diffuseColor.g >> m.diffuseColor.b;
		else if (what == "Ks")
			iss >> m.specularColor.r >> m.specularColor.g >> m.specularColor.b;
		else if (what == "map_Ka") {
			iss >> what;
			m.ambientTexture = duplicateString(what.c_str());
		} else if (what == "map_Kd") {
			iss >> what;
			m.diffuseTexture = duplicateString(what.c_str());
		} else if (what == "map_Ks") {
			iss >> what;
			m.specularTexture = duplicateString(what.c_str());
		} else if ((what == "map_bump" || what == "bump") && m.bumpTexture == 0) {
			iss >> what;
			m.bumpTexture = duplicateString(what.c_str());
		} else if (what == "Ns")
			iss >> m.shininess;
		else if (what == "sharpness")
			iss >> m.sharpness;
		else if (what == "d")
			iss >> m.transparency;
		else if (what == "r")
			iss >> m.reflectivity;
		else if (what == "Ni")
			iss >> m.refractionIndex;
	}

	// Add final material to list
	if (!first) {
		m.name = duplicateString(name);
		materials.push_back(m);
	}

	return true;
}

OBJParser::OBJParser()
{

}

OBJParser::~OBJParser()
{

}

bool OBJParser::parse(
		Node				*target,
		const char			*filename
	)
{
	vPos.clear();
	vNorm.clear();
	vUv.clear();
	faces.clear();
	faceOffset.clear();
	polyOffset.clear();
	materials.clear();
	vhash.clear();

	faceOffset.push_back(0);
	polyOffset.push_back(0);

	// Open file
	ifstream is;
	is.open(filename, ios_base::in);	// open in text mode
	if (!is.is_open()) {
		REPORT_WARNING("Couldn't open obj file %s", filename);
		return false;
	}
	
	// Reserve size in arrays to hold the loaded data.
	// Note the std::vector arrays grows in multiples of the initial capacity,
	// hence it is important to set these to a reasonable number.
	vPos.reserve(50000);
	vNorm.reserve(50000);
	vUv.reserve(50000);
	faces.reserve(50000);
	attribs.reserve(50000);
	materials.reserve(10);
	
	char line[4096];
	i32 line_num = 0;
	f32 a, b, c;
	bool has_normals=true, has_uv=true;

	bool mtlFound = false;

	bool newGroup = false;


	//----------- reading starts here -------------
	
	// All vertex positions, normals, texture coordinates,
	// and faces are read and appended to the respective arrays.
	// Polygons with n>3 sides are triangulated on-the-fly.

	i32 nFaces = 0;
	i32 nPolys = 0;

	bool isGroup = target->getNodeType() == NODE_GROUP;
	bool dirty = false;

	while (is.good()) {
		is.getline(line, 4096);
		string s(line);
		istringstream iss(s);
		line_num++;
		
		string what;
		iss >> what;
		
		if (what=="v") {				// Vertex
			iss >> a >> b >> c;
			vPos.push_back(vec3f(a,b,c));
		} else if (what=="vn") {		// Normal
			iss >> a >> b >> c;
			vNorm.push_back(vec3f(a,b,c));
		} else if (what=="vt") {		// Texture coordinate
			iss >> a >> b;
			vUv.push_back(vec2f(a,b));
		} else if (what=="f") {		// Face
			IVertex vtx[3];
			int n = 0, k = 0;
			nPolys++;
			
			while (iss.good()) {
				// Read indices
				int pidx = 0, nidx = 0, tidx = 0;
				iss >> pidx;
				if (pidx==0) break;
				if (iss.peek()=='/') {
					char ch = 0;
					iss >> ch;
					if (iss.peek()=='/')
					{
						// format: vertex//normal
						iss >> ch >> nidx;
						has_uv = false;
					}
					else
					{
						iss >> tidx;
						if (iss.peek()=='/')
						{
							// format: vertex/texture/normal
							iss >> ch >> nidx;
						}
						else
						{
							// format: vertex/texture
							has_normals = false;
						}
					}
				}
				
				// Setup vertex (the OBJ indices starts at 1, hence we subtract)
				vtx[k].pos = pidx - 1;
				vtx[k].norm = nidx - 1;
				vtx[k].uv = tidx - 1;
				
				if (k<2)		// first two just read and pass on
				{
					k++;
				}
				else
				{
					// one complete triangle read
					n++;

					ITriangle tri;
					tri.vtx[0] = vtx[0];
					tri.vtx[1] = vtx[1];
					tri.vtx[2] = vtx[2];

					// add triangle to list of triangles
					faces.push_back(tri);
					
					nFaces++;
					dirty = true;
					
					// move last index to middle position to prepare for next set of indices (if any)
					vtx[1] = vtx[2];
				}
			}

			// if a face with less than 3 valid set of indices is found, we cast an exception
			if (n==0) {
				REPORT_WARNING("Error reading obj file on line %d", line_num);
				return false;
			}
		}
		else if (what == "mtllib") { // Parse material file
			iss >> what;
			char buf[FILENAME_MAX];
			buf[0] = 0;
			const char *f;
			if ((f = strrchr(filename, '\\')) || (f = strrchr(filename, '/'))) {
				strncpy(buf, filename, f - filename + 1);
				buf[f - filename + 1] = 0;
				strcat(buf, what.c_str());
				f = buf;
			} else
				f = what.c_str();
			
			mtlFound = parseMaterials(f, materials);
		}
		if (what=="g" || what == "usemtl") { // Group
			if ((i32) faces.size() != faceOffset.back()) {
				faceOffset.push_back(faces.size());
				polyOffset.push_back(nPolys);
			}
		}
	}

	if ((i32) faces.size() != faceOffset.back()) {
		faceOffset.push_back(faces.size());
		polyOffset.push_back(nPolys);
	}

	is.close();		// close input file

	i32 ntris = faces.size();
	i32 nverts = vPos.size();

	if (!has_normals) {
		for (i32 i=0; i<ntris; i++) {
			ITriangle& t = faces[i];
			vec3f e1 = vPos[t.vtx[1].pos] - vPos[t.vtx[0].pos];
			vec3f e2 = vPos[t.vtx[2].pos] - vPos[t.vtx[0].pos];
			vec3f normal = e1.cross(e2);
			f32 len = normal.length();
			f32 area = len * 0.5f;
			normal /= len;
			vec3f wn = area * normal;
			for(int j=0; j<3; j++)
			{
				t.vtx[j].norm = t.vtx[j].pos;
				vNorm[t.vtx[j].norm] += wn;
			}
		}
		for(int i=0; i<nverts; i++)
			vNorm[i].normalize();
	}

	vBinorm.clear();
	vTangent.clear();
	vBinorm.reserve(vNorm.size());
	vTangent.reserve(vNorm.size());

	// Generate binormal and tangent
	for(u32 i=0; i<vNorm.size(); i++) {
		vBinorm.push_back(vec3f(0.0f, 0.0f, 0.0f));
		vTangent.push_back(vec3f(0.0f, 0.0f, 0.0f));
	}
	for (i32 i=0; i<ntris; i++) {
		ITriangle& t = faces[i];

		vec2f t1 = vUv[t.vtx[1].uv] - vUv[t.vtx[0].uv];
		vec2f t2 = vUv[t.vtx[2].uv] - vUv[t.vtx[0].uv];
		vec3f e1 = vPos[t.vtx[1].pos] - vPos[t.vtx[0].pos];
		vec3f e2 = vPos[t.vtx[2].pos] - vPos[t.vtx[0].pos];

		vec3f normal = e1.cross(e2);
		f32 len = normal.length();
		f32 area = len * 0.5f;

		vec3f tangent, binormal;

		f32 c = 1.0f / (t1.u * t2.v - t1.v * t2.u);
		tangent.x = (t2.v * e1.x - t1.v * e2.x) * c;
		tangent.y = (t2.v * e1.y - t1.v * e2.y) * c;
		tangent.z = (t2.v * e1.z - t1.v * e2.z) * c;
		binormal.x = (t2.u * e1.x - t1.u * e2.x) * c;
		binormal.y = (t2.u * e1.y - t1.u * e2.y) * c;
		binormal.z = (t2.u * e1.z - t1.u * e2.z) * c;

		tangent.normalize();
		binormal.normalize();

		vec3f wbn = area * binormal;
		vec3f wtn = area * tangent;

		for(int j=0; j<3; j++) {
			i32 ni = t.vtx[j].norm;
			//vBinorm[ni] += normal;
			//vTangent[ni] += normal;
			vBinorm[ni] += wbn;
			vTangent[ni] += wtn;
		}
	}
	for(u32 i=0; i<vNorm.size(); i++) {
		vBinorm[i].normalize();
		vTangent[i].normalize();
		//vBinorm[i] = vec3f(1.0f, 0.0f, 0.0f);
		//vTangent[i] = vec3f(1.0f, 0.0f, 0.0f);
	}

	if (!has_uv) {
		vUv.resize(3);
		vUv[0] = vec2f(0,0);
		vUv[1] = vec2f(1,0);
		vUv[2] = vec2f(0,1);
		for(int i=0; i<ntris; i++)
			for(int j=0; j<3; j++)
				faces[i].vtx[j].uv = j;
	}


	is.open(filename, ios_base::in);	// open in text mode	

	Material *mtl = 0;
	//bool first = true;
	char groupName[256];
	char mtlName[256];
	mtlName[0] = 0;
	groupName[0] = 0;

	i32 ngeo = 0;

	newGroup = false;

	i32 nf = 0;

	while (is.good()) {
		is.getline(line, 4096);
		string s(line);
		istringstream iss(s);
		line_num++;
		
		string what;
		iss >> what;

		if (isGroup && what=="f") {		// Face
			dirty = true;
			if (polyOffset[ngeo] == nf) {
				char buf[512];
				sprintf(buf, "%s_%s", groupName, mtlName);
				Geometry *geo = createGeometry(filename, buf, mtl, faceOffset[ngeo], faceOffset[ngeo + 1]);
				ngeo++;
				if (geo) {
					target->attachChild(geo);
					dirty = false;
				}
			}
			nf++;
		} else if (what == "usemtl") { // Set material
			if (mtlFound) {
				iss >> what;
				std::vector<Material>::iterator itr;
				for (itr = materials.begin(); itr != materials.end(); ++itr) {
					if (strcmp((*itr).name, what.c_str()) == 0) {
						mtl = &(*itr);
						break;
					}
				}
				if (itr == materials.end())
					mtl = &materials.front();
			}
			strcpy(mtlName, what.c_str());
			what = "usemtl";
		} else if (what=="g") { // Group
			char buf[256];
			iss >> buf;
			if (buf[0])
				strcpy(groupName, buf);
#if 0
			if (isGroup) {
				if (!first) {
				}
			}
			first = false;
			newGroup = true;
#endif
		}
	}


	if (dirty && (u32) ngeo < (faceOffset.size() - 1)) {
		char buf[512];
		sprintf(buf, "%s_%s", groupName, mtlName);		
		Geometry *geo = createGeometry(filename, buf, mtl, faceOffset[ngeo], faceOffset[ngeo + 1]);
		if (geo) {
			if (isGroup)
				target->attachChild(geo);
			else {
				((Geometry *) target)->setVertexArray((VertexArray *) geo->getVertexArray()->instantiate());
				SceneGraph::deleteNode(geo);
			}
		}
	}

	// ----------- reading done --------------
	
	is.close();		// close input file

	// debug
	if (nFaces==0) {
		REPORT_WARNING("Invalid mesh");
		return false;
	}

	printf("Faces: %d\n", nFaces);

	for (u32 i = 0; i < materials.size(); i++)
		materials[i].clear();

	materials.clear();
	vPos.clear();
	vNorm.clear();
	vUv.clear();
	vhash.clear();
	
	// All done!
	return true;
}

Geometry *OBJParser::createGeometry(const char *fn, char *name, Material *m, i32 from, i32 to)
{
	attribs.clear();
	indices.clear();
	vhash.clear();

	i32 nAttribs = 0;

	if (to - from <= 0)
		return 0;
	for (i32 i = from; i < to; i++) {
		for (i32 k = 0; k < 3; k++) {
			auto it = vhash.find(faces[i].vtx[k]);
            //hash_map<IVertex, i32, IVertexHash>::iterator it = vhash.find(faces[i].vtx[k]);
			i32 idx;
			if (it != vhash.end()) {
                idx = it->second;
			} else {
				ITriangle face = faces[i];
				IVertex vtx = face.vtx[k];
				// The index combination was nowhere to be found...
				vhash[vtx] = nAttribs;
				idx = nAttribs;
				vec3f pos = vPos[vtx.pos];
				vec3f norm = vNorm[vtx.norm];
				vec3f binorm = vBinorm[vtx.norm];
				vec3f tangent = vTangent[vtx.norm];
				vec2f uv = vUv[vtx.uv];
				attribs.push_back(Vertex(pos, norm, binorm, tangent, uv));
				nAttribs++;
            }
			indices.push_back(idx);
		}
	}

	char buf[256];
	sprintf(buf, "%s_%s_VertexArray", fn, name);
	VertexArray *va = SceneGraph::createVertexArray(buf, (void *) &attribs[0], sizeof(Vertex), attribs.size(), TRIANGLES, USAGE_STATIC);
	va->setIndexArray((void *) &indices[0], sizeof(i32), indices.size());

	va->setAttribute("Vertex", 0, 3, ATTRIB_FLOAT32, false);
	va->setAttribute("Normal", sizeof(vec3f), 3, ATTRIB_FLOAT32, false);
	va->setAttribute("Binormal", sizeof(vec3f) * 2, 3, ATTRIB_FLOAT32, false);
	va->setAttribute("Tangent", sizeof(vec3f) * 3, 3, ATTRIB_FLOAT32, false);
	va->setAttribute("Texcoord", sizeof(vec3f) * 4, 2, ATTRIB_FLOAT32, false);

	sprintf(buf, "%s_%s_Geometry", fn, name);
	Geometry *geo = SceneGraph::createGeometry(buf, va, true);

	if (m) {
		geo->setString("MaterialName", m->name);
		geo->setValue("AmbientColor", m->ambientColor);
		geo->setValue("DiffuseColor", m->diffuseColor);
		geo->setValue("SpecularColor", m->specularColor);
		geo->setValue("Shininess", m->shininess);
		geo->setValue("Sharpness", m->sharpness);
		geo->setValue("Transparency", m->transparency);
		geo->setValue("Reflectivity", m->reflectivity);
		geo->setValue("RefractionIndex", m->refractionIndex);
		if (m->ambientTexture) {
			Texture *t = (Texture *) SceneGraph::getResource<Texture>(m->ambientTexture);
			if (!t) t = SceneGraph::createTexture(m->ambientTexture, m->ambientTexture, true, TEXTURE_FILTER_TRILINEAR, TEXTURE_WRAP_REPEAT);
			else t = (Texture *) t->instantiate();
			geo->setTexture("AmbientTexture", t);
			geo->setValue("HasAmbientTexture", 1);
		} else
			geo->setValue("HasAmbientTexture", 0);
		if (m->diffuseTexture) {
			Texture *t = (Texture *) SceneGraph::getResource<Texture>(m->diffuseTexture);
			if (!t)	t = SceneGraph::createTexture(m->diffuseTexture, m->diffuseTexture, true, TEXTURE_FILTER_TRILINEAR, TEXTURE_WRAP_REPEAT);
			else t = (Texture *) t->instantiate();
			geo->setTexture("DiffuseTexture", t);
			geo->setValue("HasDiffuseTexture", 1);
		} else
			geo->setValue("HasDiffuseTexture", 0);
		if (m->specularTexture) {
			Texture *t = (Texture *) SceneGraph::getResource<Texture>(m->specularTexture);
			if (!t)	t = SceneGraph::createTexture(m->specularTexture, m->specularTexture, true, TEXTURE_FILTER_TRILINEAR, TEXTURE_WRAP_REPEAT);
			else t = (Texture *) t->instantiate();
			geo->setTexture("SpecularTexture", t);
			geo->setValue("HasSpecularTexture", 1);
		} else
			geo->setValue("HasSpecularTexture", 0);
		if (m->bumpTexture) {
			Texture *t = (Texture *) SceneGraph::getResource<Texture>(m->bumpTexture);
			if (!t)	t = SceneGraph::createTexture(m->bumpTexture, m->bumpTexture, true, TEXTURE_FILTER_TRILINEAR, TEXTURE_WRAP_REPEAT);
			else t = (Texture *) t->instantiate();
			geo->setTexture("NormalTexture", t);
			geo->setValue("HasNormalTexture", 1);
		} else
			geo->setValue("HasNormalTexture", 0);
	} else
		geo->setString("MaterialName", "None");

	vPos.clear();
	vNorm.clear();
	vBinorm.clear();
	vTangent.clear();
	vUv.clear();
	vhash.clear();
	faces.clear();
	attribs.clear();
	indices.clear();

	return geo;
}

