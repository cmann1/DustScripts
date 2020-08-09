#include '../enums/GlobalVirtualButton.cpp';

namespace editor_api
{
	
	void block_all_mouse(editor_api@ editor)
	{
		editor.key_clear_gvb(GlobalVirtualButton::LeftClick);
		editor.key_clear_gvb(GlobalVirtualButton::RightClick);
		editor.key_clear_gvb(GlobalVirtualButton::MiddleClick);
		editor.key_clear_gvb(GlobalVirtualButton::WheelDown);
		editor.key_clear_gvb(GlobalVirtualButton::WheelUp);
		editor.key_clear_gvb(GlobalVirtualButton::Space);
	}
	
}