#include '../Mouse.cpp';

class UIMouse : Mouse
{
	
	bool primary_down;
	bool primary_press;
	bool primary_release;
	
	UIMouse(bool hud=true, int layer=19, int player=0)
	{
		super(hud, layer, player);
	}
	
}