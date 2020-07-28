#include 'std.cpp';

class Mouse
{
	
	bool hud = false;
	int layer = 19;
	int player = 0;
	
	bool moved;
	
	bool left_down;
	bool right_down;
	bool middle_down;
	
	bool left_press;
	bool right_press;
	bool middle_press;
	
	bool left_release;
	bool right_release;
	bool middle_release;
	
	float x;
	float y;
	float delta_x;
	float delta_y;
	int scroll;
	int state;
	
	private scene@ g;
	private float scale = 1;
	
	private float prev_x;
	private float prev_y;
	
	private bool prev_left_down;
	private bool prev_right_down;
	private bool prev_middle_down;
	
	Mouse(bool hud=true, int layer=19, int player=0)
	{
		this.hud = hud;
		this.layer = layer;
		this.player = player;
		initialise();
	}
	
	private void initialise()
	{
		@g = get_scene();
	}
	
	void step()
	{
		if(hud)
		{
			scale = calibrated_mouse_hud(g, x, y, scale, player);
		}
		else
		{
			x = g.mouse_x_world(player, layer);
			y = g.mouse_y_world(player, layer);
		}
		
		state = g.mouse_state(0);
		scroll = (state & 1 != 0) ? -1 : ((state & 2 != 0) ? 1 : 0);
		
		left_down = (state & 4) != 0;
		right_down = (state & 8) != 0;
		middle_down = (state & 16) != 0;
		
		left_press = left_down && !prev_left_down;
		right_press = right_down && !prev_right_down;
		middle_press = middle_down && !prev_middle_down;
		
		left_release = !left_down && prev_left_down;
		right_release = !right_down && prev_right_down;
		middle_release = !middle_down && prev_middle_down;
		
		prev_left_down = left_down;
		prev_right_down = right_down;
		prev_middle_down = middle_down;
		
		delta_x = x - prev_x;
		delta_y = y - prev_y;
		moved = delta_x != 0 || delta_y != 0;
		
		prev_x = x;
		prev_y = y;
	}
	
	bool scrolled(int &out dir)
	{
		 dir = scroll;
		 return dir != 0;
	}
	
}