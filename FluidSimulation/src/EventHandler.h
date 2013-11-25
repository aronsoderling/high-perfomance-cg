
#ifndef RC_EVENT_HANDLER_H
#define RC_EVENT_HANDLER_H

#include "Keys.h"

#define MAX_BUFFERED_EVENTS				1024
#define MAX_LISTENERS					512

class EventListener;

/*----------------------------------------------------------------------------*/

typedef enum {
	EventTypeKey = 0,
	EventTypeMouse,
	EventTypeJoypad,
	EventTypeLast
} EventType;

class Event {
protected:
    Event();
    virtual ~Event();
public:
    f32 getTime() const;
    void setTime(f32 t);
    void setTimeNow();
    virtual EventType getType() const = 0;
protected:
    f32 timeStamp;
};


typedef enum {
	MouseMove = 0,
	MousePress,
	MouseRelease,
	MouseScrollUp,
	MouseScrollDown,
	MouseActionLast
} MouseAction;

typedef enum {
	MouseButtonNone = 0,
	MouseButtonLeft,
	MouseButtonMiddle,
	MouseButtonRight,
	MouseButtonLast
} MouseButton;

class MouseEvent : public Event {
public:
    MouseEvent() : Event() {}
    MouseEvent(MouseAction a, MouseButton b, vec2f dpos, vec2f ppos);
    ~MouseEvent() {}
public:
    MouseAction getAction() const;
    MouseButton getActionButton() const;
    vec2f getPixelPosition() const;
    vec2f getDevicePosition() const;
    EventType getType() const {return EventTypeMouse;}
private:
    MouseAction		action;
    MouseButton		button;
    vec2f			devicePosition;
    vec2f			pixelPosition;
};

typedef enum {
	KeyPress = 0,
	KeyRelease,
	KeyActionLast
} KeyAction;

class KeyEvent : public Event {
public:
    KeyEvent() : Event() {}
    KeyEvent(KeyAction ka, Key k, u8 m);
    ~KeyEvent() {}
public:
    KeyAction getAction() const;
    Key getKey() const;
    u8 getModifiers() const;
    EventType getType() const {return EventTypeKey;}
private:
    KeyAction		action;
    Key				key;
    u8				modifiers;
};

typedef enum {
	JoypadPress = 0,
	JoypadRelease,
	JoypadActionLast
} JoypadAction;

class JoypadEvent : public Event {
public:
    JoypadEvent() : Event() {}
    JoypadEvent(JoypadAction ja, u32 device, u32 button);
    ~JoypadEvent() {}
public:
    JoypadAction getAction() const;
    u32 getDevice() const;
    u32 getButton() const;
    EventType getType() const {return EventTypeJoypad;}
    EventType getListerType() const {return EventTypeJoypad;}
private:
    JoypadAction	action;
    u32				device;
    u32				button;
};

/*----------------------------------------------------------------------------*/

typedef enum {
	ListenToKeys = 0,
	ListenToMouse,
	ListenToJoypad,
	ListenToAll,
	ListenToLast
} ListenerType;

class EventHandler {
public:
    EventHandler();
    ~EventHandler();
    void registerListener(EventListener *l);
    bool unregisterListener(EventListener *l);
    void resetListeners();
    void resetEvents();
    void advance();
    
    bool postMouseEvent(MouseEvent me);
    bool postKeyEvent(KeyEvent ke);
    bool postJoypadEvent(JoypadEvent je);
    
private:
    void dispatchEvent(const Event *e, ListenerType type) const;
    
private:
    u32					nListeners;
    //EventListener		*listener[MAX_LISTENERS];
    EventListener		*firstListener;
    
    u32					nMouseEvents;
    MouseEvent			mouseEvent[MAX_BUFFERED_EVENTS];
    
    u32					nKeyEvents;
    KeyEvent			keyEvent[MAX_BUFFERED_EVENTS];
    
    u32					nJoypadEvents;
    JoypadEvent			joypadEvent[MAX_BUFFERED_EVENTS];
    
    u32					nEvents;
    Event				*eventQueue[MAX_BUFFERED_EVENTS];
};

/*----------------------------------------------------------------------------*/



#define EVENT_HANDLE_INSTANTLY		fMax
#define EVENT_DEFAULT_PRIORITY		1.0f
#define EVENT_NOT_INTERESTED		0.0f

/* Return priority - higher value = higher priority
 * If EVENT_NOT_INTERESTED is returned, the listener isn't considered
 * If EVENT_HANDLE_INSTANTLY is returned, the highest priority search is
 * terminated, and handleEvent on the listener is called instantly.
 */

class EventListener {
	friend class EventHandler;
	public:
		EventListener();
		EventListener(ListenerType ntype);
		virtual ~EventListener();
	protected:
		ListenerType type;
		EventHandler *handler;
		EventListener *next;
};

/*----------------------------------------------------------------------------*/

class MouseListener : public EventListener
{
	public:
		MouseListener();
		~MouseListener();
	public:
		virtual void consumeMouseEvent(const MouseEvent *e) {}
		bool isMousePressed(MouseButton button);
};

class KeyListener : public EventListener
{
	public:
		KeyListener();
		~KeyListener();
	public:
		virtual void consumeKeyEvent(const KeyEvent *e) {}
		bool isKeyPressed(Key key);
};

class JoypadListener : public EventListener
{
	public:
		JoypadListener();
		~JoypadListener();
	public:
		virtual void consumeJoypadEvent(const JoypadEvent *e) {}
		bool isJoypadPressed(u32 button);
};


/*----------------------------------------------------------------------------*/

void EventHandlerRegisterListener(EventListener *l);
bool EventHandlerUnregisterListener(EventListener *l);
void EventHandlerAdvance();
bool EventHandlerPostMouseEvent(MouseEvent me);
bool EventHandlerPostKeyEvent(KeyEvent ke);
bool EventHandlerPostJoypadEvent(JoypadEvent ke);

/*----------------------------------------------------------------------------*/

#endif /* RC_EVENT_HANDLER_H */



