/* TODO: Rewrite this utter mess from scratch... */

#include "RenderChimp.h"

#include "Command.h"
#include "Console.h"




static f32 backdrop_quad[2 * 6] = {
	-1.0f, 0.0f,
	1.0f, 0.0f,
	-1.0f, 1.0f,

	1.0f, 0.0f,
	-1.0f, 1.0f,
	1.0f, 1.0f
};



static const unsigned char shiftmap[32] = { ' ',
	'!', '"', '#', '$', '%', '&', '\"', '(',
	')', '*', '+', '<', '_', '>', '?', ')',
	'!', 64, '#', '$', '%', '^', '&', '*',
	'(', ':', ':', '<', '+', '>', '?'};



/*---------------------------------------------------------------------------*/

Geometry			*Console::backdrop;

//char				Console::cmdline[RC_CONSOLE_MAX_LINE_WIDTH];
//u32					Console::cmdline_pos;

char				Console::cmdbuf[RC_CONSOLE_MAX_LINE_WIDTH];


u32					Console::curr_msg;
ConsoleMessage		Console::msg_list[RC_CONSOLE_MAX_MESSAGES];
f32					Console::msg_linger_time;
u32					Console::msg_offset;
i32					Console::n_msgs;

u32					Console::off_console_draw_n_msgs;
u32					Console::on_console_draw_n_msgs;

Text				*Console::text;

bool				Console::show;

ConsoleKey			Console::key[KeyLast];

bool				Console::draw_fps;
f32					Console::fps_buf[RC_FPS_BUF];
u32					Console::curr_fps;


bool				Console::initiated = false;

/*---------------------------------------------------------------------------*/

#define KILL_RATE			50.0f
#define KILL_OFFSET			0.25f

class ConsoleInputHandler : public KeyListener {
	public:
		ConsoleInputHandler() : KeyListener() {
			inputLine[0] = 0;
			for (i32 i = 0; i < KeyLast; i++)
				keyDown[i] = fMax;
			nDeleted = 0;
			cmdstack_ptr = 0;
			cmdstack_next = 0;
			pgup = false;
			pgdn = false;
			for (i32 i = 0; i < RC_CONSOLE_MAX_STACK; i++)
				cmdstack[i][0] = 0;
		}
		~ConsoleInputHandler() {}
	public:
		void consumeKeyEvent(const KeyEvent *e) {
			Key k = e->getKey();
			if (!Console::isShowing() && k != KeyF1)
				return;
			if (e->getAction() == KeyPress && k == KeyF1) {
				Console::toggle();
				return;
			}
			if (e->getAction() == KeyPress) {
				keyDown[k] = e->getTime();
				if (k >= KeySpace && k <= KeyZ) {
					i32 len = strlen(inputLine);
					if (len >= 4000)
						return;

					char c = (char) k;
					if (e->getModifiers() & KeyModShift) {
						if (k >= KeyA && k <= KeyZ) {
							c -= 32;
						} else if (k >= 32 && k < 64) {
							c = shiftmap[k - 32];
						} else if (k >= KeyLeftBracket && k <= KeyRightBracket) {
							c += 32;
						}
					}
					inputLine[len] = c;
					inputLine[len + 1] = 0;
				} else if (k == KeyBackspace) {
					i32 len = strlen(inputLine);
					if (len > 0) {
						inputLine[len - 1] = 0;
						nDeleted = 1;
					} else
						nDeleted = 0;
					keyDown[k] += KILL_OFFSET;
				} else if (k == KeyReturn) {
					if (inputLine[0] == 0)
						return;
					if (inputLine[0] == '/')
						Command::run(inputLine);
					else
						Console::log(inputLine);
					strcpy(cmdstack[cmdstack_next], inputLine);
					cmdstack_next = (cmdstack_next + 1) % RC_CONSOLE_MAX_STACK;
					cmdstack_ptr = cmdstack_next;
					inputLine[0] = 0;
				} else if (k == KeyTab) {
					u32 n_hits = 0;
					i32 len;
					Command_t *c = Command::command_head;
					Command_t *first = 0;
					len = strlen(inputLine) - 1;
					if (len == 0)
						return;
					while (c) {
						if (strncmp(&inputLine[1], c->name, len) == 0) {
							if (n_hits == 0) {
								first = c;
							} else if (n_hits == 1) {
								Console::log("  Multiple matches:");
								Console::log(first->name);
								Console::log(c->name);
							} else {
								Console::log(c->name);
							}

							n_hits++;
						}

						c = c->list_next;
					}

					if (n_hits == 1)
						strcpy(&inputLine[1], first->name);
				} else if (k == KeyUpArrow) {
					cmdstack_ptr = (cmdstack_ptr + RC_CONSOLE_MAX_STACK - 1) % RC_CONSOLE_MAX_STACK;
					if (cmdstack[cmdstack_ptr][0] == 0 || cmdstack_ptr == cmdstack_next) {
						cmdstack_ptr = (cmdstack_ptr + 1) % RC_CONSOLE_MAX_STACK;
						memset(inputLine, 0, sizeof(char) * RC_CONSOLE_MAX_LINE_WIDTH);
						//cmdline_pos = 0;
						cmdstack_ptr = cmdstack_next;
					} else {
						strcpy(inputLine, cmdstack[cmdstack_ptr]);
						//cmdline_pos = strlen(cmdstack[cmdstack_ptr]);
					}
				} else if (k == KeyDownArrow) {
					cmdstack_ptr = cmdstack_next;
					memset(inputLine, 0, sizeof(char) * RC_CONSOLE_MAX_LINE_WIDTH);
					//cmdline_pos = 0;
				} else if (k == KeyPageUp) {
					pgup = true;
				} else if (k == KeyPageDown) {
					pgdn = true;
				}
			} else if (e->getAction() == KeyRelease) {
				if (e->getKey() == KeyBackspace) {
					deleteToTime(e->getTime());
					nDeleted = 0;
				}
				keyDown[k] = fMax;
			}
		}
		void deleteToTime(f32 time) {
			i32 target = 1 + (i32) (KILL_RATE * (time - keyDown[KeyBackspace]));
			if (nDeleted < target) {
				i32 len = strlen(inputLine);
				i32 i = len - (target - nDeleted);
				if (len > 0 && i >= 0) {
					inputLine[i] = 0;
					nDeleted = target;
				}
			}
		}
		void update() {
			if (isKeyPressed(KeyBackspace))
				deleteToTime(Platform::getFrameTime());
		}

	public:

		char inputLine[4096];
		f32 keyDown[KeyLast];
		i32 nDeleted;
		char cmdstack[RC_CONSOLE_MAX_STACK][RC_CONSOLE_MAX_LINE_WIDTH]; /* TODO: Fulalloc */
		u32 cmdstack_ptr;
		u32 cmdstack_next;
		bool pgup;
		bool pgdn;
};


static ConsoleInputHandler *inputHandler;


/*---------------------------------------------------------------------------*/

void Console::init()
{
	u32 w, h;
	//u32 i;


	inputHandler = new ConsoleInputHandler();

	msg_offset = 0;

	text = new Text();
	text->setFont("fonts/console.png", false);

	text->lineSpacing = -((i32) text->getGlyphHeight() + 2);

	show = false;

	memset(cmdbuf, 0, sizeof(char) * RC_CONSOLE_MAX_LINE_WIDTH);

	//cmdline_pos = 0;
	//memset(cmdline, 0, sizeof(char) * RC_CONSOLE_MAX_LINE_WIDTH);
	memset(key, 0, sizeof(u32) * KeyLast);

	curr_msg = 0;

	Platform::getDisplaySize(&w, &h);

	msg_linger_time = 10.0f;
	on_console_draw_n_msgs = ((i32) ((f32) h * 0.5f) - (text->getGlyphHeight() + 3) * 2) / -text->getVerticalSpacing();
	off_console_draw_n_msgs = ((i32) ((f32) h * 0.25f) + text->getGlyphHeight() + 3) / -text->getVerticalSpacing();

	//msg_list = new ConsoleMessage[off_console_draw_n_msgs];
	//memset(msg_list, 0, sizeof(ConsoleMessage) * off_console_draw_n_msgs);
	memset(msg_list, 0, sizeof(ConsoleMessage) * RC_CONSOLE_MAX_MESSAGES);
	initBackdrop();


	draw_fps = false;

	initiated = true;
}

/*---------------------------------------------------------------------------*/

void Console::destroy()
{
	u32 i;

	initiated = false;

	for (i = 0; i < RC_CONSOLE_MAX_MESSAGES; i++) {

		//if (msg_list[i].alive && msg_list[i].msg/* && *msg_list[i].msg*/)
		//	free(msg_list[i].msg);

	}

	if (text)
		delete text;

	if (backdrop)
		SceneGraph::deleteNode(backdrop);

}

/*---------------------------------------------------------------------------*/

void Console::initBackdrop()
{
	RenderState *state;
	ShaderProgram *sp;

	VertexArray *va = SceneGraph::createVertexArray("ConsoleVA", backdrop_quad, 2 * sizeof(f32), 6, TRIANGLES, USAGE_STATIC);
	va->setAttribute("Vertex", 0, 2, ATTRIB_FLOAT32, false);

	backdrop = SceneGraph::createGeometry("ConsoleBackdropGEO", va, false);

	sp = SceneGraph::createShaderProgram("ConsoleBackdropSP", 0, "Console.vs", "Console.fs", 0);
	state = sp->getRenderState();

	state->enableBlend(SB_SRC_ALPHA, DB_ONE_MINUS_SRC_ALPHA);
	state->disableScissor();
	state->disableCulling();
	state->disableStencil();
	state->disableDepthTest();
	state->enableDepthWrite();

	backdrop->setShaderProgram(sp, false);
}

/*---------------------------------------------------------------------------*/

void Console::toggle()
{
	show = !show;
}

/*---------------------------------------------------------------------------*/

void Console::log(
		const char		*str,
		...
	)
{
	u32 w, h;
	char *cstr, *pstr;
//	char *msg_buf[32];
	u32 len, i;

	va_list args;
	va_start(args, str);

	vsnprintf(cmdbuf, RC_CONSOLE_MAX_LINE_WIDTH - 1, str, args);

	va_end(args);

	cmdbuf[RC_CONSOLE_MAX_LINE_WIDTH - 1] = 0;

	len = strlen(cmdbuf);

	if (len >= (RC_CONSOLE_MAX_LINE_WIDTH - 1)) {
		strcpy(&cmdbuf[RC_CONSOLE_MAX_LINE_WIDTH - 4], "...");
	}

#ifdef RC_LOG_FRENZY
	printf("Console: %s\n", cmdbuf);
#endif

	cstr = cmdbuf;
	pstr = cmdbuf;

	Platform::getDisplaySize(&w, &h);

	text->maxWidth = w - 20;

	for (i = 0; i < 256; i++) {
		u32 inc;

		if (!cstr || !(*cstr))
			break;

		cstr = text->nextLineSplit(cstr);

		if (!cstr) {

			strncpy(msg_list[curr_msg].msg, pstr, 4094);
			msg_list[curr_msg].msg[4095] = 0;
			msg_list[curr_msg].timestamp = Platform::getFrameTime();

			inc = (curr_msg + 1) % RC_CONSOLE_MAX_MESSAGES;

			if (msg_offset == curr_msg)
				msg_offset = inc;

			curr_msg = inc;

			break;

		} else {

			len = cstr - pstr;

			if (len == 0)
				break;

			//msg_list[curr_msg].msg = duplicateStringN(pstr, len);
			strncpy(msg_list[curr_msg].msg, pstr, len);
			msg_list[curr_msg].msg[len] = 0;
			msg_list[curr_msg].timestamp = Platform::getFrameTime();

			inc = (curr_msg + 1) % RC_CONSOLE_MAX_MESSAGES;

			if (msg_offset == curr_msg)
				msg_offset = inc;

			curr_msg = inc;
		}

		pstr = cstr;
	}

}

/*---------------------------------------------------------------------------*/

void Console::keyPress()
{
#if 0
	bool *key_list;
	u32 i;
	f32 time;

	key_list = Platform::getKeyState();

	time = Platform::getFrameTime();

	/* Update command line */
	if (cmdline_pos > 0) {
		i = RC_KEY_RETURN;
		if (key_list[i]) {
			if (!key[i].alive) {
				cmdline[cmdline_pos] = 0;
				Command::run(cmdline);

				strcpy(cmdstack[cmdstack_next], cmdline);

				cmdstack_next = (cmdstack_next + 1) % RC_CONSOLE_MAX_STACK;
				cmdstack_ptr = cmdstack_next;

				memset(cmdline, 0, sizeof(char) * RC_CONSOLE_MAX_LINE_WIDTH);
				cmdline_pos = 0;

				key[i].time = time;
			}
		} else {
			key[i].alive = 0;
		}
	}

	if (key_list[RC_KEY_UP_ARROW]) {
		if (!key[RC_KEY_UP_ARROW].alive || time > key[RC_KEY_UP_ARROW].time + 0.3f) {

			cmdstack_ptr = (cmdstack_ptr + RC_CONSOLE_MAX_STACK - 1) % RC_CONSOLE_MAX_STACK;
			if (cmdstack[cmdstack_ptr][0] == 0 || cmdstack_ptr == cmdstack_next) {
				cmdstack_ptr = (cmdstack_ptr + 1) % RC_CONSOLE_MAX_STACK;
				memset(cmdline, 0, sizeof(char) * RC_CONSOLE_MAX_LINE_WIDTH);
				cmdline_pos = 0;
				cmdstack_ptr = cmdstack_next;
			} else {
				strcpy(cmdline, cmdstack[cmdstack_ptr]);
				cmdline_pos = strlen(cmdstack[cmdstack_ptr]);
			}
			key[RC_KEY_UP_ARROW].time = time;
		}
	} else {
		key[RC_KEY_UP_ARROW].alive = 0;
	}

	if (key_list[RC_KEY_DOWN_ARROW]) {
		if (!key[RC_KEY_DOWN_ARROW].alive || time > key[RC_KEY_DOWN_ARROW].time + 0.3f) {
			cmdstack_ptr = cmdstack_next;

			memset(cmdline, 0, sizeof(char) * RC_CONSOLE_MAX_LINE_WIDTH);
			cmdline_pos = 0;
		}
		key[RC_KEY_DOWN_ARROW].time = time;
	} else {
		key[RC_KEY_DOWN_ARROW].alive = 0;
	}


	if (key_list[RC_KEY_PAGE_UP]) {
		if (!key[RC_KEY_PAGE_UP].alive || time > key[RC_KEY_PAGE_UP].time + 0.02f) {
			u32 n = (msg_offset + RC_CONSOLE_MAX_MESSAGES - 1) % RC_CONSOLE_MAX_MESSAGES;
			u32 k = (msg_offset + RC_CONSOLE_MAX_MESSAGES - 2) % RC_CONSOLE_MAX_MESSAGES;

			if (msg_list[k].alive && k != curr_msg) {
				msg_offset = n;
			}
			key[RC_KEY_PAGE_UP].time = time;
		}
	} else {
		key[RC_KEY_PAGE_UP].alive = 0;
	}

	if (key_list[RC_KEY_PAGE_DOWN]) {
		if (!key[RC_KEY_PAGE_DOWN].alive || time > key[RC_KEY_PAGE_DOWN].time + 0.02f) {
			u32 n = (msg_offset + 1) % RC_CONSOLE_MAX_MESSAGES;

			if (msg_list[msg_offset].alive && msg_offset != curr_msg) {
				msg_offset = n;
			}
			key[RC_KEY_PAGE_DOWN].time = time;
		}
	} else {
		key[RC_KEY_PAGE_DOWN].alive = 0;
	}


	if (cmdline_pos < (RC_CONSOLE_MAX_LINE_WIDTH - 3)) {
		for (i = 32; i <= RC_KEY_Z; i++) {
			if (key_list[i]) {
				if (!key[i].alive) {
					u32 k = i;

					if (key_list[RC_KEY_SHIFT]) {
						if (k >= RC_KEY_A && k <= RC_KEY_Z) {
							k -= 32;
						} else if (k >= 32 && k < 64) {
							k = shiftmap[k - 32];
						} else if (k >= RC_KEY_LEFT_BRACKET && k <= RC_KEY_RIGHT_BRACKET) {
							k += 32;
						}
					}


					cmdstack_ptr = cmdstack_next;

					key[i].time = time;
					cmdline[cmdline_pos] = (char) k;
					cmdline_pos++;
				}
			} else {
				key[i].alive = 0;
			}
		}
	}

	if (cmdline_pos > 0) {
		i = RC_KEY_BACKSPACE;
		if (key_list[i]) {

			if (!key[i].alive || time > key[i].time + nextBackspace) {
				key[i].time = time;
				cmdline[cmdline_pos] = 0;
				cmdline_pos--;
				cmdline[cmdline_pos] = 0;

				nextBackspace -= 0.02f;
				if (nextBackspace < 0.02f)
					nextBackspace = 0.02f;

			}
		} else {
			key[i].alive = 0;
			nextBackspace = 0.2f;
		}
	}

	{
		i32 blink = (i32) (time * 3.0f);

		if (blink & 1) {
			cmdline[cmdline_pos] = '_';
			cmdline[cmdline_pos + 1] = 0;
		} else {
			cmdline[cmdline_pos] = 0;
		}
	}
#endif
}


/*---------------------------------------------------------------------------*/

void Console::render()
{
	bool *key_list;
	u32 n, i;
	u32 w, h;
	i32 y;
	f32 time;

	if (inputHandler->pgup) {
		for (i32 i = 0; i < 10; i++) {
			u32 n = (msg_offset + RC_CONSOLE_MAX_MESSAGES - 1) % RC_CONSOLE_MAX_MESSAGES;
			u32 k = (msg_offset + RC_CONSOLE_MAX_MESSAGES - 2) % RC_CONSOLE_MAX_MESSAGES;
			if (msg_list[k].alive && k != curr_msg)
				msg_offset = n;
		}
		inputHandler->pgup = false;
	} else if (inputHandler->pgdn) {
		for (i32 i = 0; i < 10; i++) {
			u32 n = (msg_offset + 1) % RC_CONSOLE_MAX_MESSAGES;
			if (msg_list[msg_offset].alive && msg_offset != curr_msg)
				msg_offset = n;
		}
		inputHandler->pgdn = false;
	}

	inputHandler->update();

	Platform::getDisplaySize(&w, &h);

	text->maxWidth = w - 20;
	text->maxLines = 1;

	key_list = Platform::getKeyState();

	time = Platform::getFrameTime();

	if (show) {

		backdrop->render();

		/* Render log */
		/* APANS */
		n = msg_offset;//(msg_offset + 1) % RC_CONSOLE_MAX_MESSAGES;
		y = (i32) ((f32) h * 0.5f) + text->getGlyphHeight() + 3;

		text->resetEffects();
		text->setXY(20, y);
		//text->printLine(cmdline, 0);
		text->printLine(inputHandler->inputLine, 0);

		y += text->getGlyphHeight() + 3;

		if (msg_offset != curr_msg) {
			i = 1;

			text->setXY(10, y);
			text->print("...");
			y -= text->getVerticalSpacing();

			n = (n + RC_CONSOLE_MAX_MESSAGES - 1) % RC_CONSOLE_MAX_MESSAGES;
		} else {
			i = 0;
		}

		for (; i < on_console_draw_n_msgs; i++) {
			n = (n + RC_CONSOLE_MAX_MESSAGES - 1) % RC_CONSOLE_MAX_MESSAGES;

			if (!msg_list[n].alive)
				break;

			text->setXY(10, y);
			text->print(msg_list[n].msg);
			y -= text->getVerticalSpacing();
		}

	} else {

		u32 i, n;
		n = curr_msg;
		y = h + text->getVerticalSpacing() * 2;
		n = (n + RC_CONSOLE_MAX_MESSAGES - off_console_draw_n_msgs) % RC_CONSOLE_MAX_MESSAGES;

		i32 fadeSpacing = 0;

		for (i = 0; i < off_console_draw_n_msgs; i++) {
			n = (n + 1) % RC_CONSOLE_MAX_MESSAGES;
			if (msg_list[n].alive && time <= msg_list[n].timestamp + msg_linger_time) {
				text->setXY(10, y);
				f32 fade = smin2<f32>((msg_list[n].timestamp + msg_linger_time - time) * 2.5f, 1.0f);
				text->setAlphaMultiplier(fade);
				text->print(msg_list[n].msg);
				fadeSpacing = (i32) ((1.0f - fade) * ((f32) text->getVerticalSpacing()));
				y += text->getVerticalSpacing() - fadeSpacing;
			}
		}


	}
	text->setAlphaMultiplier(1.0f);

	if (draw_fps) {
		u32 i;
		char buf[128];

		f32 total = 0.00001f;

		fps_buf[curr_fps] = 1.0f / (Platform::getFrameTimeStep() + 0.00001f);
		curr_fps = (curr_fps + 1) % RC_FPS_BUF;

		for (i = 0; i < RC_FPS_BUF; i++)
			total += fps_buf[i];

		total /= ((f32) RC_FPS_BUF);

		sprintf(buf, "%.2f FPS", total);
		text->setXY(w - 100, 200);
		text->print(buf);
	}
}

/*---------------------------------------------------------------------------*/

void Console::toggleDrawFPS(
		bool		draw
	)
{
	u32 i;

	draw_fps = draw;

	if (draw_fps) {
		for (i = 0; i < RC_FPS_BUF; i++)
			fps_buf[i] = 0.0f;

		curr_fps = 0;
	}
}

/*---------------------------------------------------------------------------*/

bool Console::isShowing()
{
	return show;
}

/*---------------------------------------------------------------------------*/

/* Singleton console object */
//Console *console = 0;






