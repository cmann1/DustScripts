#include '../MouseButton.cpp';
#include 'EventType.cpp';

class EventInfo
{
	
	string type;
	UIMouse@ mouse;
	Element@ target;
	MouseButton button;
	float x, y;
	
	void reset(string type, MouseButton button, float x, float y)
	{
		this.type = type;
		this.button = button;
		this.x = x;
		this.y = y;
	}
	
	void reset(string type, Element@ target)
	{
		this.type = type;
		this.button = MouseButton::None;
		this.x = 0;
		this.y = 0;
		@this.target = target;
	}
	
}