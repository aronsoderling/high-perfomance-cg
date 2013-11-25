
#ifndef RC_RENDERER_GL_H
#define RC_RENDERER_GL_H

static void setCubeTex(
		u8					*pixels,
		u32					w,
		u32					h,
		u32					bpp,
		u32					side
	);

static GLuint loadShader(
		const char			*parent,
		char				*prefix,
		char				*code,
		ShaderType_t		type
	);

static void unloadShader(
		void				*handle
	);

static void clearAttributes(
		ShaderProgram		*shader_program,
		VertexArray			*vertex_array
	);

		
static void setRenderState(
		RenderState			*render_state
	);
			
#endif /* RC_RENDERER_GL_H */



#if 0

#ifndef RC_RENDERER_GL_H
#define RC_RENDERER_GL_H

/*===========================================================================*/


/*---------------------------------------------------------------------------*/

class RendererGL : public Renderer {

	public:

		RendererGL();

		~RendererGL();

		void init();

	public:

/*----------------------------------------------------------[ Rendering ]----*/

		void enqueue(
				Geometry			*mesh
			);

		void renderQueued();

		void render(
				Geometry			*mesh
			);

		void swapBuffers();

/*------------------------------------------------------------[ Shading ]----*/

		void setShaderProgram(
				ShaderProgram		*shader_program
			);

		bool loadShaderProgram(
				ShaderProgram		*shader_program,
				char				*desc,
				char				*prefix
			);

		void unloadShaderProgram(
				ShaderProgram		*shader_program
			);

		/* Shader attrib. */
		void setAttributes(
				VertexArray			*vertex_array
			);

		void clearAttributes(
				VertexArray			*vertex_array
			);


		/* Shader uniforms */
		void setUniform(
				Uniform				*uniform
			);

		bool getShaderHasUniform(
				ShaderProgram		*shader_program,
				const char			*name
			);

//		u32 getShaderSuffix(
//				char				***suffix,
//				ShaderType_t		**type,
//				bool				**required
//			);

/*-----------------------------------------------------------[ Textures ]----*/

		bool loadRenderTarget(
				RenderTarget		*target,
				u32					n_textures,
				bool				include_depth_buffer,
				bool				include_stencil_buffer
			);

		void unloadRenderTarget(
				RenderTarget		*target
			);

		bool shareDepthBuffer(
				RenderTarget		*dst,
				RenderTarget		*src
			);

		void loadTexture(
				Texture				*texture,
				u8					*pixels,
				u32					width,
				u32					height,
				TextureFormat_t		format,
				bool				generate_mipmaps,
				TextureFilter_t		filter,
				TextureWrap_t		wrap
			);

		void unloadTexture(
				Texture				*texture
			);

		void loadCubeMap(
				CubeMap				*cubemap,
				u8					**pixels,
				u32					width,
				u32					height,
				u32					bpp
			);

		void unloadCubeMap(
				CubeMap				*cubemap
			);

		void loadVertexArray(
				VertexArray			*vertex_array,
				void				*new_array,
				u32					n,
				AttributeUsage_t	usage
			);

		void unloadVertexArray(
				VertexArray			*vertex_array
			);

		void reloadVertexArray(
				VertexArray			*vertex_array,
				void				*new_array,
				u32					n
			);

		void loadIndexArray(
				VertexArray			*vertex_array,
				void				*new_array,
				u32					n,
				AttributeUsage_t	usage
			);

		void unloadIndexArray(
				VertexArray			*vertex_array
			);

		void reloadIndexArray(
				VertexArray			*vertex_array,
				void				*new_array,
				u32					n
			);

/*-------------------------------------------------------[ Render state ]----*/

		void setRenderState(
				RenderState			*render_state
			);

		void setViewport(
				i32					x,
				i32					y,
				u32					width,
				u32					height
			);

		void getViewportDimensions(
				u32					*width,
				u32					*height
			);

		void setRenderTarget(
				RenderTarget		*target
			);

		void setClearColor(
				vec4f				color
			);

		void setClearDepth(
				f32					depth
			);

		void setClearStencil(
				i32					stencil
			);

		void clearColor();

		void clearDepth();

		void clearStencil();

		void flush();

		void finish();

/*-----------------------------------------------------------[ Matrices ]----*/

		void setMatrices(
				Camera				*camera
			);

/*---------------------------------------------------------------------------*/

		void endFrame();

		void produceScreenshot(
				char				*filename
			);

		void ensure();

/*---------------------------------------------------------------------------*/

	private:


	private:


};

#endif /* RC_RENDERER_GL_H */


#endif

