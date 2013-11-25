#ifndef FL_KEY_MAPPING_H
#define FL_KEY_MAPPING_H

#include "SDL.h"
#include "VectorMath.h"
#include "Keys.h"

Key keyMap[SDLK_LAST];

void setupKeyMapping() {

	for (i32 i = 0; i < SDLK_LAST; i++)
		keyMap[i] = KeyNone;

	keyMap[SDLK_UNKNOWN]		=	KeyNone;
	keyMap[SDLK_BACKSPACE]		=	KeyBackspace;
	keyMap[SDLK_TAB]			=	KeyTab;
	keyMap[SDLK_RETURN]			=	KeyReturn;
	keyMap[SDLK_ESCAPE]			=	KeyEscape;
	keyMap[SDLK_SPACE]			=	KeySpace;
	
	keyMap[SDLK_QUOTE]			=	KeySingleQuote;
	keyMap[SDLK_COMMA]			=	KeyComma;
	keyMap[SDLK_MINUS]			=	KeyMinus;
	keyMap[SDLK_PERIOD]			=	KeyPeriod;
	keyMap[SDLK_SLASH]			=	KeySlash;

	keyMap[SDLK_0]				=	Key0;
	keyMap[SDLK_1]				=	Key1;
	keyMap[SDLK_2]				=	Key2;
	keyMap[SDLK_3]				=	Key3;
	keyMap[SDLK_4]				=	Key4;
	keyMap[SDLK_5]				=	Key5;
	keyMap[SDLK_6]				=	Key6;
	keyMap[SDLK_7]				=	Key7;
	keyMap[SDLK_8]				=	Key8;
	keyMap[SDLK_9]				=	Key9;

	keyMap[SDLK_SEMICOLON]		=	KeySemiColon;
	keyMap[SDLK_EQUALS]			=	KeyEquals;
	keyMap[SDLK_LEFTBRACKET]	=	KeyLeftBracket; 
	keyMap[SDLK_BACKSLASH]		=	KeyBackslash; 
	keyMap[SDLK_RIGHTBRACKET]	=	KeyRightBracket; 
	keyMap[SDLK_BACKQUOTE]		=	KeyGraveAccent; 

	keyMap[SDLK_a]				=	KeyA; 
	keyMap[SDLK_b]				=	KeyB; 
	keyMap[SDLK_c]				=	KeyC; 
	keyMap[SDLK_d]				=	KeyD; 
	keyMap[SDLK_e]				=	KeyE; 
	keyMap[SDLK_f]				=	KeyF; 
	keyMap[SDLK_g]				=	KeyG; 
	keyMap[SDLK_h]				=	KeyH; 
	keyMap[SDLK_i]				=	KeyI; 
	keyMap[SDLK_j]				=	KeyJ; 
	keyMap[SDLK_k]				=	KeyK; 
	keyMap[SDLK_l]				=	KeyL; 
	keyMap[SDLK_m]				=	KeyM; 
	keyMap[SDLK_n]				=	KeyN; 
	keyMap[SDLK_o]				=	KeyO; 
	keyMap[SDLK_p]				=	KeyP; 
	keyMap[SDLK_q]				=	KeyQ; 
	keyMap[SDLK_r]				=	KeyR; 
	keyMap[SDLK_s]				=	KeyS; 
	keyMap[SDLK_t]				=	KeyT; 
	keyMap[SDLK_u]				=	KeyU; 
	keyMap[SDLK_v]				=	KeyV; 
	keyMap[SDLK_w]				=	KeyW; 
	keyMap[SDLK_x]				=	KeyX; 
	keyMap[SDLK_y]				=	KeyY; 
	keyMap[SDLK_z]				=	KeyZ;
	
	keyMap[SDLK_RSHIFT]			=	KeyShift; 
	keyMap[SDLK_LSHIFT]			=	KeyShift; 
	keyMap[SDLK_RCTRL]			=	KeyCtrl; 
	keyMap[SDLK_LCTRL]			=	KeyCtrl; 
	keyMap[SDLK_RALT]			=	KeyAlt; 
	keyMap[SDLK_LALT]			=	KeyAlt; 
	
	keyMap[SDLK_UP]				=	KeyUpArrow; 
	keyMap[SDLK_DOWN]			=	KeyDownArrow; 
	keyMap[SDLK_LEFT]			=	KeyLeftArrow; 
	keyMap[SDLK_RIGHT]			=	KeyRightArrow;
	
	keyMap[SDLK_PAGEUP]			=	KeyPageUp; 
	keyMap[SDLK_PAGEDOWN]		=	KeyPageDown; 
	
	keyMap[SDLK_F1]				=	KeyF1; 
	keyMap[SDLK_F2]				=	KeyF2; 
	keyMap[SDLK_F3]				=	KeyF3; 
	keyMap[SDLK_F4]				=	KeyF4; 
	keyMap[SDLK_F5]				=	KeyF5; 
	keyMap[SDLK_F6]				=	KeyF6; 
	keyMap[SDLK_F7]				=	KeyF7; 
	keyMap[SDLK_F8]				=	KeyF8; 
	keyMap[SDLK_F9]				=	KeyF9; 
	keyMap[SDLK_F10]			=	KeyF10; 
	keyMap[SDLK_F11]			=	KeyF11; 
	keyMap[SDLK_F12]			=	KeyF12; 
};

#endif /* FL_KEY_MAPPING_H */

