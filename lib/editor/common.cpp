#include '../enums/GVB.cpp';

namespace editor_api
{
	
	void block_all_mouse(editor_api@ editor)
	{
		editor.key_clear_gvb(GVB::LeftClick);
		editor.key_clear_gvb(GVB::RightClick);
		editor.key_clear_gvb(GVB::MiddleClick);
		editor.key_clear_gvb(GVB::WheelDown);
		editor.key_clear_gvb(GVB::WheelUp);
	}
	
	/// Returns true if the given global virtual button is down and then clears it
	bool consume_gvb(editor_api@ editor, int gvb)
	{
		if(editor.key_check_gvb(gvb))
		{
			editor.key_clear_gvb(gvb);
			return true;
		}
		
		return false;
	}
	
	/// Returns true if the given global virtual button is pressed and then clears it
	bool consume_gvb_press(editor_api@ editor, int gvb)
	{
		if(editor.key_check_pressed_gvb(gvb))
		{
			editor.key_clear_gvb(gvb);
			return true;
		}
		
		return false;
	}
	
}