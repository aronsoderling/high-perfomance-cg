#include "RenderChimp.h"

#if defined RC_WIN32 && defined RC_SDL

#include <windows.h>
#include <tchar.h>

#include "Command.h"
#include "Platform.h"
#include "EventHandler.h"

#ifdef _DEBUG 
	#define CRTDBG_MAP_ALLOC 
	#include <crtdbg.h> 
#endif

#include "KeyMappingSDL.h"

static DeviceState_t deviceState;


static SDL_Surface *screen;
static SDL_Surface *skin;

static f64 timer_freq;

//void (*init_func)();
//unsigned int (*update_func)();
//void (*destroy_func)();

static u32 displayWidth, displayHeight;

static bool joy_dir_pressed[4] = {false, false, false, false};

static bool alt_pressed = false;
static bool shift_pressed = false;
static bool ctrl_pressed = false;
static bool f4_pressed = false;

static bool alt_changed = false;
static bool shift_changed = false;
static bool ctrl_changed = false;

static vec2f mousePixelPos;
static vec2f mouseDevicePos;

static SDL_Joystick *joypad = 0;
static u32 nJoypadButtons = 0;


bool *Platform::getKeyState()
{
	return deviceState.key;
}

bool *Platform::getMouseButtonState()
{
	return deviceState.button;
}

vec2f Platform::getMousePosition()
{
	return vec2f(deviceState.x, deviceState.y);
}


void *Platform::getWindowHandle()
{
	HWND hWnd = 0;

	SDL_SysWMinfo wmInfo;

		/* Get window context */
	SDL_VERSION(&wmInfo.version);
	SDL_GetWMInfo(&wmInfo);

//	hWnd = (NativeWindowType) wmInfo.window;
	hWnd = wmInfo.window;

	return (void *) hWnd;
}


/* Timer functions */
f32 Platform::getFrameTime()
{
	return (f32) deviceState.time;
}

f32 Platform::getFrameTimeStep()
{
	return (f32) deviceState.timeStep;
}

f32 Platform::getCurrentTime()
{
	/*
	u64 t;
	QueryPerformanceCounter((LARGE_INTEGER *) &t);
	return (f32) (((f64) t) / timer_freq);
	*/
	i32 t;

	t = SDL_GetTicks();

	return ((f32) t) * 0.001f;
}

i32 Platform::run()
{
	while(!RCRun());

	RCEnd();

	return 0;
}

inline void Platform::update_time()
{
	/*
	u64 t;
	QueryPerformanceCounter((LARGE_INTEGER *) &t);
	f32 newTime = (f32) (((f64) t) / timer_freq);
	*/
	f32 f;
	i32 t;

	t = SDL_GetTicks();

	f = ((f32) t) * 0.001f;

	deviceState.timeStep = f - deviceState.time;
	deviceState.time = f;
}

u32 key_pressed = 0;
u32 mouse_pressed = 0;

int Platform::update_input()
{
	SDL_Event event;
	i32 more;

	more = SDL_PollEvent(&event);

	while (more) {
		SDLKey key = event.key.keysym.sym;

		if (event.key.type == SDL_QUIT)
			return 1;
		else if (event.key.type == SDL_KEYDOWN || event.key.type == SDL_KEYUP) {
			bool down = event.key.type == SDL_KEYDOWN;
			
			switch (key) {
				case SDLK_ESCAPE:
					return 1;
				case SDLK_RALT:
				case SDLK_LALT:
					alt_pressed = down;
					break;
				case SDLK_RSHIFT:
				case SDLK_LSHIFT:
					shift_pressed = down;
					break;
				case SDLK_RCTRL:
				case SDLK_LCTRL:
					ctrl_pressed = down;
					break;
			}

			Key mkey = keyMap[key];

			if (mkey != KeyNone) {
				deviceState.key[mkey] = down;
				u8 modifiers = alt_pressed ? KeyModAlt : 0;
				modifiers |= ctrl_pressed ? KeyModCtrl : 0;
				modifiers |= shift_pressed ? KeyModShift : 0;
				KeyAction a = down ? KeyPress : KeyRelease;
				KeyEvent ke = KeyEvent(a, mkey, modifiers);
				EventHandlerPostKeyEvent(ke);
			}

		} else if (event.key.type == SDL_MOUSEBUTTONDOWN || event.key.type == SDL_MOUSEBUTTONUP) {
			MouseAction action = event.key.type == SDL_MOUSEBUTTONDOWN ? MousePress : MouseRelease;
			MouseButton button = MouseButtonNone;

			switch (event.button.button) {
				case SDL_BUTTON_LEFT:		button = MouseButtonLeft; break;
				case SDL_BUTTON_RIGHT:		button = MouseButtonRight; break;
				case SDL_BUTTON_MIDDLE:		button = MouseButtonMiddle; break;
				case SDL_BUTTON_WHEELUP:	action = MouseScrollUp; break;
				case SDL_BUTTON_WHEELDOWN:	action = MouseScrollDown; break;
				default: break;
			}
			MouseEvent me = MouseEvent(action, button, mouseDevicePos, mousePixelPos);
			deviceState.button[button] = event.key.type == SDL_MOUSEBUTTONDOWN;
			EventHandlerPostMouseEvent(me);

		} else if (event.key.type == SDL_MOUSEMOTION) {
			mousePixelPos.x = (f32) event.motion.x;
			mousePixelPos.y = (f32) event.motion.y;
			mouseDevicePos.x = 2.0f * ((f32) event.motion.x) / ((f32) displayWidth) - 1.0f;
			mouseDevicePos.y = 1.0f - 2.0f * ((f32) event.motion.y) / ((f32) displayHeight);
			MouseEvent me = MouseEvent(MouseMove, MouseButtonNone, mouseDevicePos, mousePixelPos);
			EventHandlerPostMouseEvent(me);
			deviceState.x = event.motion.x;
			deviceState.y = event.motion.y;
		} else if (event.jbutton.type == SDL_JOYAXISMOTION) {
			// Dirs: 0 = left, 1 = right, 2 = down, 3 = up
			u32 device = event.jaxis.which;
			if (event.jaxis.which == 0) {
				if (sabs<i32>(event.jaxis.value) < 5000) {
					i32 d0 = event.jaxis.axis * 2 + 0;
					if (joy_dir_pressed[d0])
						EventHandlerPostJoypadEvent(JoypadEvent(JoypadRelease, device, d0));
					if (joy_dir_pressed[d0 + 1])
						EventHandlerPostJoypadEvent(JoypadEvent(JoypadRelease, device, d0 + 1));
					joy_dir_pressed[d0] = false;
					joy_dir_pressed[d0 + 1] = false;
				} else {
					i32 dir = (event.jaxis.value < 0 ? 0 : 1) + (event.jaxis.axis * 2);
					if (!joy_dir_pressed[dir]) {
					//if ((sabs<i32>(event.jaxis.value) < 5000 && joy_dir_pressed[dir]) || (sabs<i32>(event.jaxis.value) >= 5000 && !joy_dir_pressed[dir])) {
						EventHandlerPostJoypadEvent(JoypadEvent(JoypadPress, device, dir));
						joy_dir_pressed[dir] = true;
					}
				}
			}
		} else if (event.jbutton.type == SDL_JOYBUTTONDOWN || event.jbutton.type == SDL_JOYBUTTONUP) {
			u32 device = event.jaxis.which;
			JoypadAction a = event.key.type == SDL_JOYBUTTONDOWN ? JoypadPress : JoypadRelease;
			u32 button = event.jbutton.button + 8; // Make room for cross buttons // TODO: Unhack
			EventHandlerPostJoypadEvent(JoypadEvent(a, device, button));
		}

		more = SDL_PollEvent(&event);
	}

	return 0;
}

i32 Platform::initFrame()
{
	update_time();
	u32 a = update_input();
	if (a != 0)
		return a;
	EventHandlerAdvance();
	return 0;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
//int RCMain_()
{
	return Platform::init();
}

i32 Platform::init()
{

	i32 result;
	u64 tf;

	setupKeyMapping();

		/* Set up timer */
	HANDLE process_handle = GetCurrentProcess();
	SetProcessAffinityMask(process_handle, 1);

	QueryPerformanceFrequency((LARGE_INTEGER *) &tf);
	timer_freq = (f64) tf;


	displayWidth = RC_DEFAULT_DISPLAY_WIDTH;
	displayHeight = RC_DEFAULT_DISPLAY_HEIGHT;

		/* Set up display */
	result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
	if (result < 0)
		return 1;

	SDL_WM_SetCaption("RenderChimp (SDL)", 0);

	memset(&deviceState, 0, sizeof(DeviceState_t));

	Platform::update_time();

	RCMain();

	#ifdef _DEBUG
		_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF);
	#endif

	return 0;

}





/* Texture functions */

void Platform::getTexture(
		const char			*filename,
		bool				flip_y,
		u8					**pixels,
		u32					*width,
		u32					*height,
		u32					*bytes_per_pixel
	)
{
	u32 s;
	SDL_Surface *img = IMG_Load(filename);

	if (!img) {
		REPORT_WARNING("Texture image \"%s\" not found", filename);
		*pixels = 0;
		return;
	}

	*width = img->w;
	*height = img->h;
	*bytes_per_pixel = img->format->BytesPerPixel;

	s = (*bytes_per_pixel) * (*width) * (*height);

	*pixels = (u8 *) malloc(s);
	memcpy(*pixels, img->pixels, s);

	/* LOWPRIO: Better implementation... when I'm feeling less lazy */
	if (flip_y) {
		u32 x, y, i;

		for (y = 0; y < (*height) >> 1; y++) {
			for (x = 0; x < *width; x++) {

				for (i = 0; i < *bytes_per_pixel; i++) {
					u8 *p0, *p1;
					u8 t;

					p0 = &(*pixels)[(y * (*width) + x) * (*bytes_per_pixel) + i];
					p1 = &(*pixels)[((*height - y - 1) * (*width) + x) * (*bytes_per_pixel) + i];

					t = *p0;
					*p0 = *p1;
					*p1 = t;
				}

			}
		}
		
	}


	if (img->format->Rmask != 255 && *bytes_per_pixel != 1) {
		u32 x, y;

		for (y = 0; y < *height; y++) {
			for (x = 0; x < *width; x++) {

				u8 *p0, *p1;
				u8 t;

				p0 = &(*pixels)[(y * (*width) + x) * (*bytes_per_pixel) + 0];
				p1 = &(*pixels)[(y * (*width) + x) * (*bytes_per_pixel) + 2];

				t = *p0;
				*p0 = *p1;
				*p1 = t;

			}
		}
	}
	
	SDL_FreeSurface((SDL_Surface *) img);
}

void Platform::destroyTexture(
		unsigned char		*pixels
	)
{

	if (pixels)
		free(pixels);

}


void Platform::getDisplaySize(
		u32					*width,
		u32					*height
	)
{
	*width = displayWidth;
	*height = displayHeight;
}

void Platform::setDisplay(
		bool				fullscreen,
		u32					width,
		u32					height
	)
{
	u32 full = fullscreen ? SDL_FULLSCREEN : 0;

#if defined RC_OPENGL
	screen = SDL_SetVideoMode(width, height, 0, SDL_OPENGL | full);
#elif defined RC_OPENGL_ES_20
	screen = SDL_SetVideoMode(width, height, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | full);
#endif

	if (!screen) {

		REPORT_WARNING("Couldn't initiate screen resolution (%d, %d) with fullscreen %s", width, height, fullscreen ? "on" : "off");

		/* Could not initialize the specified resolution. Revert to old settings */
#if defined RC_OPENGL
		screen = SDL_SetVideoMode(displayWidth, displayHeight, 0, SDL_OPENGL);
#elif defined RC_OPENGL_ES_20
		screen = SDL_SetVideoMode(displayWidth, displayHeight, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
#endif
	
		if (!screen) {
			REPORT_ERROR("Couldn't revert to old screen resolution (%d, %d)", displayWidth, displayHeight);
		}

	} else {

		displayWidth = width;
		displayHeight = height;

	}

}

#endif /* RC_WIN32 */


