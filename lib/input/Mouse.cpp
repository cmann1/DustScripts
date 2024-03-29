class Mouse
{
	
	bool hud = false;
	int layer = 19;
	int sub_layer = 0;
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
	private input_api@ input;
	private float scale = 1;
	
	private float prev_x;
	private float prev_y;
	
	private bool raw_left_down;
	private bool raw_right_down;
	private bool raw_middle_down;
	private bool prev_left_down;
	private bool prev_right_down;
	private bool prev_middle_down;
	
	private float left_double_click_timer;
	private float right_double_click_timer;
	private float middle_double_click_timer;
	
	Mouse(bool hud=true, int layer=19, int sub_layer=19, int player=0)
	{
		this.hud = hud;
		this.layer = layer;
		this.sub_layer = sub_layer;
		this.player = player;
		
		@g = get_scene();
	}
	
	/// Use the input api for mouse access. Will get a new instance, or
	/// use @input if given
	void use_input(input_api@ input=null)
	{
		@this.input = @input != null ? input : input_api;
	}
	
	/// @param block_mouse Button presses and mouse scroll won't register
	void step(bool block_mouse=false)
	{
		if(hud)
		{
			x = @input != null
				? input.mouse_x_hud(scale_hud)
				: g.mouse_x_hud(player, scale_hud);
			y = @input != null
				? input.mouse_y_hud(scale_hud)
				: g.mouse_y_hud(player, scale_hud);
		}
		else
		{
			if(@input != null)
			{
				input.mouse_world(layer, sub_layer, x, y);
			}
			else
			{
				g.mouse_world(player, layer, sub_layer, x, y);
			}
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
		
		state = @input != null
			? input.mouse_state()
			: g.mouse_state(0);
		scroll = !block_mouse
			? (state & 1 != 0) ? -1 : ((state & 2 != 0) ? 1 : 0)
			: 0;
		
		raw_left_down = (state & 0x4) != 0;
		raw_right_down = (state & 0x8) != 0;
		raw_middle_down = (state & 0x10) != 0;
		
		if(!block_mouse)
		{
			if(@input != null)
			{
				left_press = (state & 0x20) != 0;
				right_press = (state & 0x40) != 0;
				middle_press = (state & 0x80) != 0;
			}
			else
			{
				left_press = raw_left_down && !prev_left_down;
				right_press = raw_right_down && !prev_right_down;
				middle_press = raw_middle_down && !prev_middle_down;
			}
		}
		
		left_down = !block_mouse || left_down || left_press
			? raw_left_down : false;
		right_down = !block_mouse || right_down || right_press
			? raw_right_down : false;
		middle_down = !block_mouse || middle_down || middle_press
			? raw_middle_down : false;
		
		if(@input != null)
		{
			left_release = (state & 0x100) != 0;
			right_release = (state & 0x200) != 0;
			middle_release = (state & 0x400) != 0;
		}
		else
		{
			left_release = !left_down && prev_left_down;
			right_release = !right_down && prev_right_down;
			middle_release = !middle_down && prev_middle_down;
		}
		
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
