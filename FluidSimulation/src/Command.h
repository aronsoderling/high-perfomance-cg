
#ifndef RC_COMMAND_H
#define RC_COMMAND_H

#define RC_COMMAND_MAX_ARGS						32
#define RC_COMMAND_MAX_ARG_LEN					64
#define RC_COMMAND_MAX_LEN						512
#define RC_COMMAND_HASH_SIZE					1001

/*---------------------------------------------------------------------------*/

typedef struct {

	Key			key;
	char		*line;

} KeyBinding_t;

typedef struct Command_t Command_t;

struct Command_t {

	char		*name;
	char		*desc;
	bool		(* funcptr)(u32, char (*)[RC_COMMAND_MAX_ARG_LEN]);

	Command_t	*hash_next;
	Command_t	*list_next;

};

class Command {

	public:

		static void init();

		static void destroy();

		static void add(
				const char	*command_name,
				const char	*desc,
				bool		(* func_ptr)(u32, char (*)[RC_COMMAND_MAX_ARG_LEN])
			);

		static void remove(
				const char	*command_name
			);

		static void run(
				char		*str
			);

		static bool bindKey(
				char		*keyn,
				char		*line
			);

		static bool checkArgCount(
				u32		expected,
				u32		got,
				char	(*args)[RC_COMMAND_MAX_ARG_LEN]
			);

		static void consumeKeys();


	public:

		static Command_t	*command_hash[RC_COMMAND_HASH_SIZE];
		static Command_t	*command_head;

		static u32			n_bindings;
		static KeyBinding_t	binding[KeyLast];

		static ConsoleKey	key[KeyLast];

};

/*========================================================[ Informative ]====*/

#if 0

static const char *CMDHELP_draw_fps = {
	"/draw_fps [bool]"
};

bool CMD_draw_fps(
		u32		n_args,
		char	(*args)[RC_COMMAND_MAX_ARG_LEN]
	);

/*---------------------------------------------------------------------------*/

static const char *CMDHELP_list = {
	"/list"
};

bool CMD_list(
		u32		n_args,
		char	(*args)[RC_COMMAND_MAX_ARG_LEN]
	);

/*========================================================[ Scene graph ]====*/

static const char *CMDHELP_scene_tree = {
	"/scene_tree"
};

bool CMD_scene_tree(
		u32		n_args,
		char	(*args)[RC_COMMAND_MAX_ARG_LEN]
	);

/*---------------------------------------------------------------------------*/

static const char *CMDHELP_load_scene = {
	"/load_scene <Group> <Filename>"
};

bool CMD_load_scene(
		u32		n_args,
		char	(*args)[RC_COMMAND_MAX_ARG_LEN]
	);

/*---------------------------------------------------------------------------*/

#endif

#endif /* RC_COMMAND_H */

