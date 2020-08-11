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
		editor.key_clear_gvb(GVB::Space);
	}
	
}