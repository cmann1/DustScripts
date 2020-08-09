#include '../MouseButton.cpp';
#include '../elements/Element.cpp';
#include 'EventType.cpp';
#include 'IGenericEventTarget.cpp';

class EventInfo
{
	
	string type;
	string value;
	UIMouse@ mouse;
	Element@ target;
	IGenericEventTarget@ generic_target;
	MouseButton button;
	float x, y;
	
	void reset(const string type, MouseButton button, const float x, const float y)
	{
		this.type = type;
		this.value = '';
		this.button = button;
		this.x = x;
		this.y = y;
		@this.target = null;
		@this.generic_target = null;
	}
	
	void reset(const string type)
	{
		reset(type, null, null, '');
	}
	
	void reset(const string type, Element@ target, const string value='')
	{
		reset(type, target, null, value);
	}
	
	void reset(const string type, IGenericEventTarget@ generic_target, const string value='')
	{
		reset(type, null, @generic_target, value);
	}
	
	void reset(const string type, Element@ target, IGenericEventTarget@ generic_target, const string value='')
	{
		this.type = type;
		this.value = value;
		@this.target = @target;
		@this.generic_target = @generic_target;
		
		this.button = MouseButton::None;
		this.x = 0;
		this.y = 0;
	}
	
}