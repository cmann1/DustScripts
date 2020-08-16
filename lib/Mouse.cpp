#include 'std.cpp';
#include 'enums/GVB.cpp';

class Mouse
{
	
	bool hud = false;
	int layer = 19;
	int player = 0;
	bool scale_hud = false;
	int double_click_period = 30;
	
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
	
	bool left_double_click;
	bool right_double_click;
	bool middle_double_click;
	
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
	
	private float left_double_click_timer;
	private float right_double_click_timer;
	private float middle_double_click_timer;
	
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
	
	/// @param block_mouse Button presses and mouse scroll won't register
	void step(bool block_mouse=false)
	{
		if(hud)
		{
			x = g.mouse_x_hud(player, scale_hud);
			y = g.mouse_y_hud(player, scale_hud);
		}
		else
		{
			x = g.mouse_x_world(player, layer);
			y = g.mouse_y_world(player, layer);
		}
		
		delta_x = x - prev_x;
		delta_y = y - prev_y;
		moved = delta_x != 0 || delta_y != 0;
		
		if(moved)
		{
			left_double_click_timer = double_click_period + 1;
			right_double_click_timer = double_click_period + 1;
			middle_double_click_timer = double_click_period + 1;
		}
		
		state = g.mouse_state(0);
		scroll = (state & 1 != 0) ? -1 : ((state & 2 != 0) ? 1 : 0);
		
		left_down = (state & 4) != 0;
		right_down = (state & 8) != 0;
		middle_down = (state & 16) != 0;
		
		if(block_mouse)
		{
			if(left_down && !prev_left_down)
				left_down = false;
			if(right_down && !prev_right_down)
				right_down = false;
			if(middle_down && !prev_middle_down)
				middle_down = false;
		}
		
		left_press = left_down && !prev_left_down;
		right_press = right_down && !prev_right_down;
		middle_press = middle_down && !prev_middle_down;
		
		left_release = !left_down && prev_left_down;
		right_release = !right_down && prev_right_down;
		middle_release = !middle_down && prev_middle_down;
		
		left_double_click = left_press && left_double_click_timer <= double_click_period;
		right_double_click = right_press && right_double_click_timer <= double_click_period;
		middle_double_click = middle_press && middle_double_click_timer <= double_click_period;
		
		if(left_press)
			left_double_click_timer = 0;
		else if(left_double_click_timer <= double_click_period)
			left_double_click_timer++;
		
		if(right_press)
			right_double_click_timer = 0;
		else if(right_double_click_timer <= double_click_period)
			right_double_click_timer++;
		
		if(middle_press)
			middle_double_click_timer = 0;
		else if(middle_double_click_timer <= double_click_period)
			middle_double_click_timer++;
		
		prev_left_down = left_down;
		prev_right_down = right_down;
		prev_middle_down = middle_down;
		
		prev_x = x;
		prev_y = y;
	}
	
	bool scrolled(int &out dir)
	{
		 dir = scroll;
		 return dir != 0;
	}
	
}