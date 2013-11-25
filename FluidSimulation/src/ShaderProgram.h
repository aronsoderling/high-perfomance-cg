
#ifndef RC_SHADERPROGRAM_H
#define RC_SHADERPROGRAM_H

#define RC_MAX_SHADERS				4

/**
 * ShaderProgram.
 */
class ShaderProgram : public Resource, public Uniform {

	friend class SceneGraph;
	friend class Geometry;

	friend class Renderer;
#ifdef RC_OPENGL
	friend class RendererGL;
#endif

/*----------------------------------------------------------[ Functions ]----*/

	protected:
		ShaderProgram(
				const char			*nidentifier,
				char				*filenames[],
				i32					n,
				char				*prefix
			);

		~ShaderProgram();

	public:

		static void registerCommands();

		void setRenderState(RenderState *rs);

		RenderState *getRenderState();

		bool hasUniform(
				const char			*name
			);

		void recompile();


	//protected:

		void *getHandle() const;

	protected:
		void setHandle(
				void				*nhandle
			);

#ifdef DEBUG
	protected:
		void print(
				FILE	*f
			);
#endif /* DEBUG */

/*------------------------------------------------------------[ Members ]----*/

	public:
		RenderState		renderState;

	private:
		i32				n_desc;
		char			**shader_description;
		char			*shader_prefix;

		void			*handle;

};

#endif /* RC_SHADERPROGRAM_H */

