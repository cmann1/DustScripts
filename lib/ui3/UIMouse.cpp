#include '../Mouse.cpp';

class UIMouse : Mouse
{
	
	bool primary_down;
	bool primary_press;
	bool primary_release;
	bool primary_double_click;
	bool secondary_down;
	bool secondary_press;
	bool secondary_release;
	bool secondary_double_click;
	
	UIMouse(bool hud=true, int layer=19, int player=0)
	{
		super(hud, layer, player);
	}
	
}