/*
 * Default objects
 */

#ifndef RC_DEFAULT_OBJECTS_H
#define RC_DEFAULT_OBJECTS_H

#include "ResourceViewer.h"

/*---------------------------------------------------------------------------*/

class DefaultObjects {

	public:

		static void init();

		static void destroy();

		static void drawHelpers();

	public:

		static ShaderProgram	*failureShaderProgram;
		static VertexArray		*failureVertexArray;
		static Texture			*failureTexture;
		static CubeMap			*failureCubeMap;

		static Geometry			*bulbMesh;
		static ShaderProgram	*bulbShader;

		static Geometry			*camMesh;
		static ShaderProgram	*camShader;

		static Geometry			*arrowMesh;
		static ShaderProgram	*arrowShader;
		static Group			*axisGroup;

		static Geometry			*gridMesh;
		static ShaderProgram	*gridShader;

		static Group			*helperAxis;
		static Geometry			*helperGrid;

		static ResourceViewer	*rview;
};

#endif /* RC_DEFAULT_OBJECTS_H */

