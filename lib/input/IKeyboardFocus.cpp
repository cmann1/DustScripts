#include 'BlurAction.cpp';

interface IKeyboardFocus
{
	
	bool has_focus { get const; }
	
	void on_focus(Keyboard@ keyboard);
	
	void on_blur(Keyboard@ keyboard, const BlurAction type);
	
	void on_key_press(Keyboard@ keyboard, const int key, const bool is_gvb, const string text);
	
	void on_key(Keyboard@ keyboard, const int key, const bool is_gvb, const string text);
	
	void on_key_release(Keyboard@ keyboard, const int key, const bool is_gvb);
	
}
