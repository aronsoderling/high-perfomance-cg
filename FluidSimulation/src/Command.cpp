#include "RenderChimp.h"

#include "Command.h"

/*---------------------------------------------------------------------------*/

Command_t		*Command::command_hash[RC_COMMAND_HASH_SIZE];
Command_t		*Command::command_head;

u32				Command::n_bindings;
KeyBinding_t	Command::binding[KeyLast];

ConsoleKey		Command::key[KeyLast];


static char *key_name[KeyLast] = {0};

void setup_key_names()
{
	key_name[KeyNone]				= "undefined";
	key_name[KeyBackspace]			= "backspace";
	key_name[KeyTab]				= "tab";
	key_name[KeyReturn]				= "return";
	key_name[KeyEscape]				= "escape";

	key_name[KeySingleQuote]		= "'";
	key_name[KeyComma]				= ",";
	key_name[KeyMinus]				= "-";
	key_name[KeyPeriod]				= ".";
	key_name[KeySlash]				= "/";

	key_name[Key0]					= "0";
	key_name[Key1]					= "1";
	key_name[Key2]					= "2";
	key_name[Key3]					= "3";
	key_name[Key4]					= "4";
	key_name[Key5]					= "5";
	key_name[Key6]					= "6";
	key_name[Key7]					= "7";
	key_name[Key8]					= "8";
	key_name[Key9]					= "9";

	key_name[KeySemiColon]			= ";";
	key_name[KeyEquals]				= "=";
	key_name[KeyLeftBracket]		= "[";
	key_name[KeyBackslash]			= "\\";
	key_name[KeyRightBracket]		= "]";
	key_name[KeyGraveAccent]		= "`";

	key_name[KeyA]					= "a";
	key_name[KeyB]					= "b";
	key_name[KeyC]					= "c";
	key_name[KeyD]					= "d";
	key_name[KeyE]					= "e";
	key_name[KeyF]					= "f";
	key_name[KeyG]					= "g";
	key_name[KeyH]					= "h";
	key_name[KeyI]					= "i";
	key_name[KeyJ]					= "j";
	key_name[KeyK]					= "k";
	key_name[KeyL]					= "l";
	key_name[KeyM]					= "m";
	key_name[KeyN]					= "n";
	key_name[KeyO]					= "o";
	key_name[KeyP]					= "p";
	key_name[KeyQ]					= "q";
	key_name[KeyR]					= "r";
	key_name[KeyS]					= "s";
	key_name[KeyT]					= "t";
	key_name[KeyU]					= "u";
	key_name[KeyV]					= "v";
	key_name[KeyW]					= "w";
	key_name[KeyX]					= "x";
	key_name[KeyY]					= "y";
	key_name[KeyZ]					= "z";

	key_name[KeyShift]				= "shift";
	key_name[KeyCtrl]				= "ctrl";
	key_name[KeyAlt]				= "alt";

	key_name[KeyUpArrow]			= "up";
	key_name[KeyDownArrow]			= "down";
	key_name[KeyLeftArrow]			= "left";
	key_name[KeyRightArrow]			= "right";

	key_name[KeyPageUp]				= "pgup";
	key_name[KeyPageDown]			= "pgdn";

	key_name[KeyF1]					= "f1";
	key_name[KeyF2]					= "f2";
	key_name[KeyF3]					= "f3";
	key_name[KeyF4]					= "f4";
	key_name[KeyF5]					= "f5";
	key_name[KeyF6]					= "f6";
	key_name[KeyF7]					= "f7";
	key_name[KeyF8]					= "f8";
	key_name[KeyF9]					= "f9";
	key_name[KeyF10]				= "f10";
	key_name[KeyF11]				= "f11";
	key_name[KeyF12]				= "f12";
}

/*---------------------------------------------------------------------------*/

static const char *CMDHELP_bind = {
	"/bind <key> <string>"
};

bool CMD_bind(
		u32		n_args,
		char	(*args)[RC_COMMAND_MAX_ARG_LEN]
	)
{
	if (!Command::checkArgCount(2, n_args, args))
		return false;

	return Command::bindKey(args[0], args[1]);
}

/*---------------------------------------------------------------------------*/

static const char *CMDHELP_list_commands = {
	"/list_commands"
};

bool CMD_list_commands(
		u32		n_args,
		char	(*args)[RC_COMMAND_MAX_ARG_LEN]
	)
{
	Command_t *cmd = Command::command_head;
	
	Console::log("  List of commands:");

	while (cmd) {
		Console::log(cmd->name);

		cmd = cmd->list_next;
	}

	return true;
}

/*---------------------------------------------------------------------------*/

static const char *CMDHELP_screenshot = {
	"/screenshot <filename>"
};

bool CMD_screenshot(
		u32		n_args,
		char	(*args)[RC_COMMAND_MAX_ARG_LEN]
	)
{
	if (!Command::checkArgCount(1, n_args, args))
		return false;

	Renderer::produceScreenshot(args[0]);

	return true;
}

/*---------------------------------------------------------------------------*/

static const char *CMDHELP_help = {
	"/help"
};

bool CMD_help(
		u32		n_args,
		char	(*args)[RC_COMMAND_MAX_ARG_LEN]
	)
{
	if (n_args > 0) {
		Command_t *cmd = Command::command_head;

		while (cmd) {
			if (strcmp(cmd->name, args[0]) == 0) {
				Console::log("  Help for \"%s\":", cmd->name);
				Console::log(cmd->desc);

				return true;
			}

			cmd = cmd->list_next;
		}

		REPORT_WARNING("Command not found");

	} else {
		Console::log("Type \"/help <Command>\" for more info on a particular command");
		Console::log("Type \"/list_commands\" to list all available commands");
		Console::log("Use F1 to toggle console");
	}

	return true;
}

/*---------------------------------------------------------------------------*/

static const char *CMDHELP_config = {
	"/config <filename>"
};

bool CMD_config(
		u32		n_args,
		char	(*args)[RC_COMMAND_MAX_ARG_LEN]
	)
{
	char buf[RC_CONSOLE_MAX_LINE_WIDTH];
	FILE *f;

	if (!Command::checkArgCount(1, n_args, args))
		return false;

	Console::log("Loading config file \"%s\"", args[0]);

	f = fopen(args[0], "r");

	if (!f) {
		REPORT_WARNING("Couldn't read config file");
		return false;
	}

	while (fgets(buf, RC_CONSOLE_MAX_LINE_WIDTH, f)) {

		if (strlen(buf) > 0)
			Command::run(buf);

	}


	fclose(f);

	return true;
}

/*---------------------------------------------------------------------------*/

static const char *CMDHELP_draw_fps = {
	"/draw_fps <0/1>"
};

bool CMD_draw_fps(
		u32		n_args,
		char	(*args)[RC_COMMAND_MAX_ARG_LEN]
	)
{
	if (n_args == 0) {
		Console::toggleDrawFPS(!Console::drawsFPS());
	} else {
		Console::toggleDrawFPS(!(args[0][0] == '0' && args[0][1] == 0));
	}

	return true;
}

/*---------------------------------------------------------------------------*/

class CommandInputHandler : public KeyListener {
	public:
		CommandInputHandler() : KeyListener() {
		}
		~CommandInputHandler() {}
	public:
		void consumeKeyEvent(const KeyEvent *e) {
			if (e->getAction() != KeyPress)
				return;
			if (Console::isShowing())
				return;
			Key key = e->getKey();
			for (u32 i = 0; i < Command::n_bindings; i++) {
				Key bk = Command::binding[i].key;
				if (bk == key) {
					Command::run(Command::binding[i].line);
					return;
				}
			}
		}

};

static CommandInputHandler *inputHandler;

		
void Command::init()
{
	setup_key_names();

	n_bindings = 0;

	inputHandler = new CommandInputHandler();

	command_head = 0;
	memset(command_hash, 0, sizeof(Command_t *) * RC_COMMAND_HASH_SIZE);

#if 0
	if (!console)
		return;

		/* Console */
	Console::add("bind", CMDHELP_bind, CMD_bind);

		/* Informative */
	Console::add("draw_fps", CMDHELP_draw_fps, CMD_draw_fps); /* 1 / 0 */
	Console::add("mem_info", CMDHELP_mem_info, CMD_mem_info);
	Console::add("mem_log", CMDHELP_mem_log, CMD_mem_log);
	//Console::add("list", CMD_mem_info, CMD_list);

		/* Scene graph */
	Console::add("scene_tree", CMDHELP_scene_tree, CMD_scene_tree);
	Console::add("load_scene", CMDHELP_load_scene, CMD_load_scene); /* group name, file name, purge level */

		/* Nodes */
	/* Transformable */
	Console::add("set_translate", CMDHELP_set_translate, CMD_set_translate);
	Console::add("translate", CMDHELP_translate, CMD_translate);
	Console::add("set_rotate", CMDHELP_set_rotate, CMD_set_rotate);
	Console::add("rotate", CMDHELP_rotate, CMD_rotate);
	Console::add("set_scale", CMDHELP_set_scale, CMD_set_scale);
	Console::add("scale", CMDHELP_scale, CMD_scale);

		/* Resources */
	/* Shader program */
	Console::add("shader_recompile", CMDHELP_shader_recompile, CMD_shader_recompile);

#endif

	Command::add("bind", CMDHELP_bind, CMD_bind);
	Command::add("list_commands", CMDHELP_list_commands, CMD_list_commands);
	Command::add("help", CMDHELP_help, CMD_help);
	Command::add("config", CMDHELP_config, CMD_config);
	Command::add("screenshot", CMDHELP_screenshot, CMD_screenshot);
	Command::add("draw_fps", CMDHELP_draw_fps, CMD_draw_fps);

	/* System */
	MemoryRegisterCommands();

	/* Nodes */
	SceneGraph::registerCommands();
	Camera::registerCommands();
	Light::registerCommands();
	Transformable::registerCommands();

	/* Resources */
	ShaderProgram::registerCommands();

}

/*---------------------------------------------------------------------------*/

void Command::destroy()
{
	Command_t *e, *c;
	u32 i;

	for (i = 0; i < RC_COMMAND_HASH_SIZE; i++) {

		e = command_hash[i];

		while (e) {
			c = e;

			e = e->hash_next;

			free(c->name);

			if (c->desc)
				free(c->desc);

			delete c;
		}

	}

	for (i = 0; i < n_bindings; i++) {
		free(binding[i].line);
	}

}

/*---------------------------------------------------------------------------*/

void Command::add(
		const char	*command_name,
		const char	*desc,
		bool		(* func_ptr)(u32, char (*)[RC_COMMAND_MAX_ARG_LEN])
	)
{
	Command_t *e, *p, *n;
	u32 h;

	h = calculateStringHash(command_name) % RC_COMMAND_HASH_SIZE;

	e = command_hash[h];

	while (e) {

		if (strcmp(e->name, command_name) == 0) {
			remove(command_name);
			break;
		}

		e = (Command_t *) e->hash_next;
	}

	n = new Command_t;

	n->name = duplicateString(command_name);
	n->desc = duplicateString(desc);
	n->funcptr = func_ptr;
	n->hash_next = command_hash[h];
	n->list_next = 0;
	command_hash[h] = n;

	e = command_head;
	p = 0;

	while (e) {

		if (strcmp(e->name, command_name) > 0) {
			n->list_next = e;

			if (p) {
				p->list_next = n;
			} else {
				command_head = n;
			}

			return;
		}

		p = e;
		e = (Command_t *) e->list_next;
	}

	if (p) {
		p->list_next = n;
	} else {
		command_head = n;
	}

}

/*---------------------------------------------------------------------------*/

void Command::remove(
		const char	*command_name
	)
{
	Command_t *e, *p;
	u32 h = calculateStringHash(command_name) % RC_COMMAND_HASH_SIZE;

	e = command_hash[h];
	p = 0;

	while (e) {

		if (strcmp(e->name, command_name) == 0) {

			if (p) {
				p->hash_next = e->hash_next;
			} else {
				command_hash[h] = e->hash_next;
			}

			break;
		}

		p = e;
		e = e->hash_next;
	}

	e = command_head;
	p = 0;

	while (e) {

		if (strcmp(e->name, command_name) == 0) {

			if (p) {
				p->list_next = e->list_next;
			} else {
				command_head = e->list_next;
			}

			free(e->name);
			delete e;

			return;
		}

		p = e;
		e = e->list_next;
	}

}

/*---------------------------------------------------------------------------*/

void Command::run(
		char		*str
	)
{
	char *p;
	char *orig = str;

//	static const char delim[] = " \t";
	u32 i, k, n_args;
//	char buf[RC_CONSOLE_MAX_LINE_WIDTH];
	char cmd[RC_COMMAND_MAX_LEN];
	char argbuf[RC_COMMAND_MAX_ARGS][RC_COMMAND_MAX_ARG_LEN]; /* Command + RC_CONSOLE_MAX_ARGS args */

	bool success = false;
	bool in_quotes = false;

	Command_t *e;
	u32 h, len;

	while (*str && (*str == ' ' || *str == '\t'))
		str++;

	if (*str == '/') {
		str++;
	} else {
		Console::log(orig);
		return;
	}

	p = str;

	len = strlen(str);

	if (len == 0) {

		REPORT_WARNING("No command given");
		return;
	}

	Console::log(orig);

	k = 0;

	while (*p) {
		if (*p == ' ' || *p == '\t') {
			break;
		}

		p++;
		k++;
	}

	//cmd = new char[k + 1];
	strncpy(cmd, str, k);
	cmd[k] = 0;

	/* Search for command */
	h = calculateStringHash(cmd) % RC_COMMAND_HASH_SIZE;

	e = command_hash[h];

	while (e) {
		if (strcmp(e->name, cmd) == 0) {
			break;
		}

		e = e->hash_next;
	}

	if (!e) {
		REPORT_WARNING("Command %s not found", cmd);
		return;
	}

	/* Parse arguments */
	str += k;

	i = 0;

	while (*str && *str != '\n' && *str != '\r' && i < RC_COMMAND_MAX_ARGS) {
		k = 0;
		in_quotes = false;

		while (*str && (*str == ' ' || *str == '\t'))
			str++;

		if (*str == '\"') {
			in_quotes = true;
			str++;
		}

		p = str;

		while (*p) {
			if ((*p == ' ' || *p == '\t') && !in_quotes) {
				break;
			} else if (*p == '\"' && in_quotes) {
				break;
			} else if (*p == 0) {
				break;
			}

			p++;
			k++;
		}

		//argbuf[i] = new char[k + 1];
		strncpy(argbuf[i], str, k);
		argbuf[i][k] = 0;

		if (in_quotes)
			k++;

		str += k;
		i++;
	}

	n_args = i;

	success	= e->funcptr(n_args, argbuf);

	if (!success) {
		REPORT_WARNING("Command %s failed", cmd);
		Console::log("Usage: %s", e->desc);
	}

	//delete [] cmd;

	//for (i = 0; i < n_args; i++)
	//	delete [] argbuf[i];

}

/*---------------------------------------------------------------------------*/

void Command::consumeKeys()
{
#if 0
	bool *key_list;
	u32 i;

	key_list = Platform::getKeyState();

	if (key_list[KeyF1]) {
		if (!key[KeyF1].alive) {
			
			Console::toggle();
			key[KeyF1].time = Platform::getFrameTime();
			
		}
	} else {
		key[KeyF1].alive = 0;
	}

	if (Console::isShowing()) {
		Console::keyPress();
	} else {

		for (i = 0; i < n_bindings; i++) {
			u32 k = binding[i].key;

			if (key_list[k]) {
				if (!key[k].alive) {
					
					Command::run(binding[i].line);
					key[k].time = Platform::getFrameTime();
					
				}
			} else {
				key[k].alive = 0;
			}
		}

	}
#endif

}

/*---------------------------------------------------------------------------*/

bool Command::bindKey(
		char		*keyn,
		char		*line
	)
{
	u32 i, j;

	if (!line || *line == 0) {
		REPORT_WARNING("No string given");
		return false;
	}

	for (i = 0; i < KeyLast; i++) {
		if (key_name[i] && strcmp(keyn, key_name[i]) == 0) {
			break;
		}
	}

	if (i == KeyLast) {
		REPORT_WARNING("Couldn't bind key - key name \"%s\"not found", keyn);
		return false;
	}

	for (j = 0; j < n_bindings; j++) {
		if (binding[j].key == (Key) i) {
			free(binding[j].line);

			binding[j].line = duplicateString(line);
			return true;
		}
	}

	binding[n_bindings].line = duplicateString(line);
	binding[n_bindings].key = (Key) i;
	n_bindings++;
	return true;
}

/*---------------------------------------------------------------------------*/

bool Command::checkArgCount(
		u32		expected,
		u32		got,
		char	(*args)[RC_COMMAND_MAX_ARG_LEN]
	)
{
	u32 i;

	if (expected != got) {
		REPORT_WARNING("Wrong number of arguments given - got %d but expected %d.", got, expected);

		for (i = 0; i < got; i++)
			Console::log("Arg %d: \"%s\"", i, args[i]);

		return false;
	}

	return true;
}

#if 0

/*========================================================[ Informative ]====*/

bool CMD_draw_fps(
		u32		n_args,
		char	**args
	)
{
	if (n_args == 0 || strcmp(args[0], "0") != 0) {
		Console::log("  Draw FPS enabled");
		Console::toggleDrawFPS(true);
	} else {
		Console::log("  Draw FPS disabled");
		Console::toggleDrawFPS(false);
	}

	return true;
}


/*========================================================[ Scene graph ]====*/

void p_scene_tree_write(
		Node	*node,
		u32		level
	)
{
	char buf[1024];
	char *b;
	char *name = node->getName();

	memset(buf, ' ', level << 1);
	b = buf + (level << 1);

	switch (node->getNodeType()) {
		case NODE_CAMERA:
			sprintf(b, "~c7779<Cam> ~r%s", name);
			break;
		case NODE_GROUP:
			sprintf(b, "~c7989<Grp> ~r%s", name);
			break;
		case NODE_LIGHT:
			sprintf(b, "~c9879<Lgt> ~r%s", name);
			break;
		case NODE_WORLD:
			sprintf(b, "~c7979<Wld> ~r%s", name);
			break;
		case NODE_GEOMETRY:
			sprintf(b, "~c7899<Geo> ~r%s", name);
			break;
		default:
			sprintf(b, "~c6669<???> ~r%s", name);
			break;
	}

	Console::log(buf);
}

void p_scene_tree_traverse(
		Node	*curr,
		u32		level
	)
{
	u32 i;
	u32 n = curr->getChildrenCount();

	p_scene_tree_write(curr, level);

	level++;

	for (i = 0; i < n; i++) {
		p_scene_tree_traverse(curr->getChild(i), level);
	}
}

bool CMD_scene_tree(
		u32		n_args,
		char	**args
	)
{
	u32 n_nodes, i;
	Node **node_list;

	n_nodes = SceneGraph::getNodeCount();

	node_list = (Node **) malloc(sizeof(Node *) * n_nodes);

	SceneGraph::getNodes(node_list);

	for (i = 0; i < n_nodes; i++) {
		Node *node = node_list[i];

		if (!node->getParent()) {
			p_scene_tree_traverse(node, 0);
		}

	}

	free(node_list);

	return true;
}

/*---------------------------------------------------------------------------*/

bool CMD_load_scene(
		u32		n_args,
		char	**args
	)
{
	bool parse_success;
	i32 arg2;

	if (n_args != 2) {
		REPORT_WARNING(RC_WRN_CMD_ARG, 2);
		return false;
	}

	parse_success = parseInt(&arg2, args[2]);

	if (!parse_success)
		return false;

	SceneGraph::createGroup(args[0], args[1], (u32) arg2);

	return true;
}

/*---------------------------------------------------------------------------*/

#endif

