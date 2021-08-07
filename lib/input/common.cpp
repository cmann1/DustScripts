#include '../enums/GVB.cpp';

namespace input_api
{
	
	void block_all_mouse(input_api@ input)
	{
		input.key_clear_gvb(GVB::LeftClick);
		input.key_clear_gvb(GVB::RightClick);
		input.key_clear_gvb(GVB::MiddleClick);
		input.key_clear_gvb(GVB::WheelDown);
		input.key_clear_gvb(GVB::WheelUp);
	}
	
	/// Returns true if the given global virtual button is down and then clears it
	bool consume_gvb(input_api@ input, int gvb)
	{
		if(input.key_check_gvb(gvb))
		{
			input.key_clear_gvb(gvb);
			return true;
		}
		
		return false;
	}
	
	/// Returns true if the given global virtual button is pressed and then clears it
	bool consume_gvb_press(input_api@ input, int gvb)
	{
		if(input.key_check_pressed_gvb(gvb))
		{
			input.key_clear_gvb(gvb);
			return true;
		}
		
		return false;
	}
	
}
