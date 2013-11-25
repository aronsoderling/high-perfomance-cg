#include "RenderChimp.h"
#include "Resource.h"
#include "ResourceList.h"
#include "Command.h"
#include "ShaderProgram.h"

#define SHADER_PROGRAM_ERROR_BUFFER_LENGTH			16384

/*---------------------------------------------------------------------------*/

ShaderProgram::ShaderProgram(
		const char			*nidentifier,
		char				*filenames[],
		i32					n,
		char				*prefix

	) : Resource(nidentifier, NO_PURGE, RESOURCE_SHADER_PROGRAM), Uniform()
{
	handle = 0;

	if (prefix && *prefix)
		shader_prefix = duplicateString(prefix);
	else
		shader_prefix = 0;

	n_desc = n;
	shader_description = (char **) malloc(sizeof(char *) * n);
	for (i32 i = 0; i < n; i++)
		shader_description[i] = duplicateString(filenames[i]);

	if (!Renderer::loadShaderProgram(this, shader_description, shader_prefix, n)) {
		Renderer::unloadShaderProgram(this);
		static char *defaultShader[] = {"Default.vs", "Default.fs"};
		if (!Renderer::loadShaderProgram(this, defaultShader, 0, 2))
			broken = true;
	}
}

/*---------------------------------------------------------------------------*/

ShaderProgram::~ShaderProgram()
{
	Renderer::unloadShaderProgram(this);

	if (shader_prefix)
		free(shader_prefix);

	if (shader_description) {
		for (i32 i = 0; i < n_desc; i++)
			free(shader_description[i]);
		free(shader_description);
	}

}

/*---------------------------------------------------------------------------*/

void ShaderProgram::recompile()
{
	Renderer::unloadShaderProgram(this);

	if (!Renderer::loadShaderProgram(this, shader_description, shader_prefix, n_desc)) {
		REPORT("Reverting to default shader program. Failed to compile \"%s\" for \"%s\"", shader_description, identifier);
		static char *defaultShader[] = {"Default.vs", "Default.fs"};
		Renderer::loadShaderProgram(this, defaultShader, 0, 2);
	} /*else
		REPORT("Shader program \"%s\" successfully recompiled for \"%s\"", shader_description, identifier);
	*/
}

/*---------------------------------------------------------------------------*/

void *ShaderProgram::getHandle() const
{
	return handle;
}

/*---------------------------------------------------------------------------*/

void ShaderProgram::setHandle(
		void				*nhandle
	)
{
	handle = nhandle;
}

/*---------------------------------------------------------------------------*/

void ShaderProgram::setRenderState(RenderState *rs)
{
	renderState = *rs;
}

/*---------------------------------------------------------------------------*/

RenderState *ShaderProgram::getRenderState()
{
	return &renderState;
}

/*---------------------------------------------------------------------------*/

bool ShaderProgram::hasUniform(
		const char			*name
	)
{
	return Renderer::getShaderHasUniform(this, name);
}

/*---------------------------------------------------------------------------*/

#ifdef DEBUG

void ShaderProgram::print(
		FILE		*f
	)
{
//	char *c = shader[0]->getIdentityStringCopy();
//	char *d = shader[1]->getIdentityStringCopy();

#ifdef RC_LOG_TO_FILE
	#define prnt0(a)		fprintf(f, a)
	#define prnt1(a, b)		fprintf(f, a, b)
#else /* RC_LOG_TO_FILE */
	#define prnt0(a)		printf(a)
	#define prnt1(a, b)		printf(a, b)
#endif /* RC_LOG_TO_FILE */

//	prnt1("    [ V.SHADER ][ %s ]\n", c);
//	prnt1("    [ F.SHADER ][ %s ]\n", d);
	//prnt1("    [ GL ID    ][ %u ]\n", id);

//	free(c);
//	free(d);
}

#endif /* DEBUG */

/*---------------------------------------------------------------------------*/

static const char *CMDHELP_shader_recompile = {
	"/shader_recompile"
};

bool CMD_shader_recompile(
		u32		n_args,
		char	(*args)[RC_COMMAND_MAX_ARG_LEN]
	)
{
	std::vector<Resource *> shader_list;
	u32 n = SceneGraph::getResourceCount<ShaderProgram>();
	SceneGraph::getResourceList<ShaderProgram>(shader_list);
	for (u32 i = 0; i < n; i++)
		((ShaderProgram *) shader_list[i])->recompile();
	REPORT("Done recompiling shaders");
	return true;
}

/*---------------------------------------------------------------------------*/

void ShaderProgram::registerCommands()
{
	Command::add("shader_recompile", CMDHELP_shader_recompile, CMD_shader_recompile);
}

/*---------------------------------------------------------------------------*/
