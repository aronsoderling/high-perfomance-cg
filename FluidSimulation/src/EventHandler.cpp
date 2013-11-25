
#include "RenderChimp.h"

#include "EventHandler.h"
#include "Platform.h"

/*----------------------------------------------------------------------------*/

static EventHandler eventHandler;

/*----------------------------------------------------------------------------*/

Event::Event()
{
	setTimeNow();
}

Event::~Event()
{

}

f32 Event::getTime() const
{
	return timeStamp;
}

void Event::setTime(f32 t)
{
	timeStamp = t;
}

void Event::setTimeNow()
{
	timeStamp = Platform::getCurrentTime();
}

EventListener::EventListener(ListenerType ntype) : type(ntype)
{
	type = ListenToAll;
	EventHandlerRegisterListener(this);
}

EventListener::EventListener()
{
	type = ListenToAll;
	EventHandlerRegisterListener(this);
}

EventListener::~EventListener()
{

}

/*----------------------------------------------------------------------------*/

MouseEvent::MouseEvent(MouseAction a, MouseButton b, vec2f dpos, vec2f ppos)
{
	action = a;
	button = b;
	pixelPosition = ppos;
	devicePosition = dpos;
}

MouseAction MouseEvent::getAction() const
{
	return action;
}

MouseButton MouseEvent::getActionButton() const
{
	return button;
}

vec2f MouseEvent::getPixelPosition() const
{
	return pixelPosition;
}

vec2f MouseEvent::getDevicePosition() const
{
	return devicePosition;
}

MouseListener::MouseListener() : EventListener()
{
	type = ListenToMouse;
}

MouseListener::~MouseListener()
{

}

bool MouseListener::isMousePressed(MouseButton button)
{
	bool *buttons = Platform::getMouseButtonState();
	return buttons[(i32) button];
}

/*----------------------------------------------------------------------------*/

KeyEvent::KeyEvent(KeyAction ka, Key k, u8 m)
{
	action = ka;
	key = k;
	modifiers = m;
}

KeyAction KeyEvent::getAction() const
{
	return action;
}

Key KeyEvent::getKey() const
{
	return key;
}

u8 KeyEvent::getModifiers() const
{
	return modifiers;
}

KeyListener::KeyListener() : EventListener()
{
	type = ListenToKeys;
}

KeyListener::~KeyListener()
{

}

bool KeyListener::isKeyPressed(Key key)
{
	bool *keys = Platform::getKeyState();
	return keys[(i32) key];
}

/*----------------------------------------------------------------------------*/

JoypadEvent::JoypadEvent(JoypadAction ja, u32 device_, u32 button_)
{
	action = ja;
	device = device_;
	button = button_;
}

JoypadAction JoypadEvent::getAction() const
{
	return action;
}

u32 JoypadEvent::getDevice() const
{
	return device;
}

u32 JoypadEvent::getButton() const
{
	return button;
}

JoypadListener::JoypadListener() : EventListener()
{
	type = ListenToJoypad;
}

JoypadListener::~JoypadListener()
{

}

bool JoypadListener::isJoypadPressed(u32 button)
{
	REPORT_ONCE("To be implemented...");
	return false;
}

/*----------------------------------------------------------------------------*/

void EventHandlerRegisterListener(EventListener *l)
{
	eventHandler.registerListener(l);
}

bool EventHandlerUnregisterListener(EventListener *l)
{
	return eventHandler.unregisterListener(l);
}

void EventHandlerAdvance()
{
	eventHandler.advance();
}

bool EventHandlerPostMouseEvent(MouseEvent me)
{
	return eventHandler.postMouseEvent(me);
}

bool EventHandlerPostKeyEvent(KeyEvent ke)
{
	return eventHandler.postKeyEvent(ke);
}

bool EventHandlerPostJoypadEvent(JoypadEvent je)
{
	return eventHandler.postJoypadEvent(je);
}

/*----------------------------------------------------------------------------*/

EventHandler::EventHandler()
{
	resetListeners();
	resetEvents();
}

EventHandler::~EventHandler()
{

}

void EventHandler::registerListener(EventListener *l)
{
	if (nListeners >= MAX_LISTENERS) {
		REPORT_ONCE("Too many listeners (max is %d). Ignoring all coming listeners.", MAX_LISTENERS);
		return;
	}
	l->next = firstListener;
	firstListener = l;
	nListeners++;
}

bool EventHandler::unregisterListener(EventListener *l)
{
	EventListener *p = 0;
	EventListener *e = firstListener;
	while (e) {
		if (e == l) {
			if (p)
				p->next = e->next;
			else
				firstListener = e->next;
			return true;
		}
		p = e;
		e = e->next;
	}
	return false;
}

void EventHandler::resetListeners()
{
	nListeners = 0;
}

void EventHandler::resetEvents()
{
	nMouseEvents = 0;
	nKeyEvents = 0;
	nJoypadEvents = 0;
	nEvents = 0;
}

void EventHandler::advance()
{
	for (u32 i = 0; i < nEvents; i++) {
		EventType etype = eventQueue[i]->getType();
		ListenerType ltype = (ListenerType) etype;
		dispatchEvent(eventQueue[i], ltype);
	}
	resetEvents();
}

bool EventHandler::postMouseEvent(MouseEvent me)
{
	if (nEvents >= MAX_BUFFERED_EVENTS)
		return false;
	mouseEvent[nMouseEvents] = me;
	eventQueue[nEvents++] = &mouseEvent[nMouseEvents++];
	return true;
}

bool EventHandler::postKeyEvent(KeyEvent ke)
{
	if (nEvents >= MAX_BUFFERED_EVENTS)
		return false;
	keyEvent[nKeyEvents] = ke;
	eventQueue[nEvents++] = &keyEvent[nKeyEvents++];
	return true;
}

bool EventHandler::postJoypadEvent(JoypadEvent je)
{
	if (nEvents >= MAX_BUFFERED_EVENTS)
		return false;
	joypadEvent[nJoypadEvents] = je;
	eventQueue[nEvents++] = &joypadEvent[nJoypadEvents++];
	return true;
}

void EventHandler::dispatchEvent(const Event *me, ListenerType type) const
{
	EventListener *e = firstListener;
	while (e) {
		switch (type) {
			case EventTypeKey:		((KeyListener *) e)->consumeKeyEvent((KeyEvent *)me); break;
			case EventTypeMouse:	((MouseListener *) e)->consumeMouseEvent((MouseEvent *)me); break;
			case EventTypeJoypad:	((JoypadListener *) e)->consumeJoypadEvent((JoypadEvent *)me); break;
			default: break;
		}
		e = e->next;
	}
}

/*----------------------------------------------------------------------------*/

