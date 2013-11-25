
#ifndef RC_GEOMETRY_H
#define RC_GEOMETRY_H

/**
 * Defines a Geometry node.
 */
class Geometry : public Transformable, public Uniform {
	friend class SceneGraph;

	friend class Renderer;
#ifdef RC_OPENGL
	friend class RendererGL;
#endif

/*----------------------------------------------------------[ Functions ]----*/

	protected:
		Geometry(
				const char		*nname
			);

		Geometry(
				const char		*nname,

				VertexArray		*vertex_array,

				bool			instantiate_resources = true
			);

		Geometry(
				const char		*nname,

				const char		*filename
			);

		~Geometry();

	public:

		static void registerCommands();
		static NodeType nodeType() {return NODE_GEOMETRY;}

		/**
		 * Render this node only 
		 *
		 */
		void render();

		void setVertexArray(
				VertexArray		*vertex_array,

				bool			instantiate_resource = true
			);

		VertexArray *getVertexArray() const;


		void setShaderProgram(
				ShaderProgram	*shader_program,

				bool			instantiate_resource = true
			);

		ShaderProgram *getShaderProgram() const;


		void getWorldBoundingBox(
				AABox			*target
			);

		void getLocalBoundingBox(
				AABox			*target
			);


		void logInfo();

		static void init();

	protected:

		void drawSelf();

		Node *duplicateSelf();

/*------------------------------------------------------------[ Members ]----*/

	protected:
		VertexArray				*vertexArray;

		ShaderProgram			*shaderProgram;

};


#endif /* RC_GEOMETRY_H */

