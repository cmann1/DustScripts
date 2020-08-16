#include 'BlurAction.cpp';

interface IKeyboardFocus
{
	
	void on_focus(Keyboard@ keyboard);
	
	void on_blur(Keyboard@ keyboard, const BlurAction type);
	
	void on_key_press(Keyboard@ keyboard, const int key, const bool is_gvb);
	
	void on_key(Keyboard@ keyboard, const int key, const bool is_gvb);
	
	void on_key_release(Keyboard@ keyboard, const int key, const bool is_gvb);
	
}