/*
 * KeyDef
 *
 */

#ifndef RC_KEYS_H
#define RC_KEYS_H

typedef enum {
	KeyNone				= 0,
	KeyBackspace		= 8,
	KeyTab				= 9,
	KeyReturn			= 13,
	KeyEscape			= 27,
	KeySpace			= 32,
	
	KeySingleQuote		= 39,
	KeyComma			= 44,
	KeyMinus			= 45,
	KeyPeriod			= 46,
	KeySlash			= 47,

	Key0				= 48,
	Key1				= 49,
	Key2				= 50,
	Key3				= 51,
	Key4				= 52,
	Key5				= 53,
	Key6				= 54,
	Key7				= 55,
	Key8				= 56,
	Key9				= 57,

	KeySemiColon		= 59,
	KeyEquals			= 61,
	KeyLeftBracket		= 91,
	KeyBackslash		= 92,
	KeyRightBracket		= 93,
	KeyGraveAccent		= 96,

	KeyA				= 97,
	KeyB				= 98,
	KeyC				= 99,
	KeyD				= 100,
	KeyE				= 101,
	KeyF				= 102,
	KeyG				= 103,
	KeyH				= 104,
	KeyI				= 105,
	KeyJ				= 106,
	KeyK				= 107,
	KeyL				= 108,
	KeyM				= 109,
	KeyN				= 110,
	KeyO				= 111,
	KeyP				= 112,
	KeyQ				= 113,
	KeyR				= 114,
	KeyS				= 115,
	KeyT				= 116,
	KeyU				= 117,
	KeyV				= 118,
	KeyW				= 119,
	KeyX				= 120,
	KeyY				= 121,
	KeyZ				= 122,
	
	KeyShift			= 128,
	KeyCtrl				= 129,
	KeyAlt				= 130,
	
	KeyUpArrow			= 140,
	KeyDownArrow		= 141,
	KeyLeftArrow		= 142,
	KeyRightArrow		= 143,
	
	KeyPageUp			= 150,
	KeyPageDown			= 151,
	
	KeyF1				= 160,
	KeyF2				= 161,
	KeyF3				= 162,
	KeyF4				= 163,
	KeyF5				= 164,
	KeyF6				= 165,
	KeyF7				= 166,
	KeyF8				= 167,
	KeyF9				= 168,
	KeyF10				= 169,
	KeyF11				= 170,
	KeyF12				= 171,

	KeyLast				= 180
} Key;

#define KeyModNone			0
#define KeyModAlt			(1 << 0)
#define KeyModCtrl			(1 << 1)
#define KeyModShift			(1 << 2)




#endif /* RC_KEYS_H */

