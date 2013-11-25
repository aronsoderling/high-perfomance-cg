#ifndef RC_CONSOLE_H
#define RC_CONSOLE_H

#define RC_CONSOLE_MAX_LINE_WIDTH				512
#define RC_CONSOLE_MAX_MESSAGES					2048
#define RC_CONSOLE_MAX_STACK					32

#define RC_FPS_BUF								32

typedef struct {
	char		msg[4096];
	union {
		u32		alive;
		f32		timestamp;
	};

} ConsoleMessage;

typedef union {
	f32			time;
	u32			alive;
} ConsoleKey;

static const char default_desc[] = "No command description added";

class Console {

	public:

		static void init();

		static void destroy();

		static void toggle();

		static void log(
				const char	*str,
				...
			);

		static void keyPress();

		static void render();

		static void toggleDrawFPS(
				bool		draw
			);

		static bool drawsFPS() { return draw_fps; }

		static bool isShowing();

	private:

		static void initBackdrop();

	private:

		static Geometry			*backdrop;

		/* Command line */
		//static char				cmdline[RC_CONSOLE_MAX_LINE_WIDTH];
		//static u32				cmdline_pos;

		static char				cmdbuf[RC_CONSOLE_MAX_LINE_WIDTH];


		static u32				curr_msg;
		static ConsoleMessage	msg_list[RC_CONSOLE_MAX_MESSAGES];
		static f32				msg_linger_time;
		static u32				msg_offset;
		static i32				n_msgs;

		static u32				off_console_draw_n_msgs;
		static u32				on_console_draw_n_msgs;

		static bool				show;

		static f32				nextBackspace;

		static ConsoleKey		key[KeyLast];

		/* FPS counter */
		static bool				draw_fps;
		static f32				fps_buf[RC_FPS_BUF];
		static u32				curr_fps;


	public:
		static Text				*text;
		static bool				initiated;

};

#endif /* RC_CONSOLE_H */

