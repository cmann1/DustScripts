#include '../lib/props.cpp';
#include '../lib/drawing/Sprite.cpp';
#include '../lib/ui/prop-selector/PropSelector.cpp';

/*
 * PropSelector:
 * 	Scroll into view for ScrollView
 * 	Clear button
 */

class script
{
	
	scene@ g;
	UI@ ui = UI();
	PropSelector prop_selector(ui);
	
	[text] uint prop_set;
	[text] uint prop_group;
	[text] uint prop_index;
	[text] uint prop_palette;
	
	[text] bool select_prop;
	[hidden] private bool select_prop_prev;
	
	[hidden] private bool has_sprite;
	[hidden] private string sprite_set;
	[hidden] private string sprite_name;
	
	private bool started = false;
	private Sprite sprite;
	
	script()
	{
		@g = get_scene();
	}
	
	void start()
	{
		prop_selector.hide();
		
		if(has_sprite)
		{
			sprite.set(sprite_set, sprite_name);
		}
	}
	
	void editor_step()
	{
		if(!started)
		{
			start();
			started = true;
		}
		
		ui.step();
		
		if(select_prop != select_prop_prev)
		{
			if(!prop_selector.visible)
			{
				if(has_sprite)
				{
					prop_selector.select_prop(prop_set, prop_group, prop_index, prop_palette);
				}
				else
				{
					prop_selector.select_prop(null);
				}
				
				prop_selector.show();
			}
			else
			{
				prop_selector.hide();
			}
			
			select_prop_prev = select_prop;
		}
		
//		if(prop_selector.visible)
//		{
//			prop_selector.step();
//		}
	}
	
	void editor_draw(float sub_frame)
	{
		if(prop_selector.visible)
		{
			prop_selector.draw();
			
			if(prop_selector.result == Selected)
			{
				prop_set = prop_selector.result_prop.set;
				prop_group = prop_selector.result_prop.group;
				prop_index = prop_selector.result_prop.index;
				prop_palette = prop_selector.result_palette;
				has_sprite = true;
				
				sprite_from_prop(prop_set, prop_group, prop_index, sprite_set, sprite_name);
				sprite.set(sprite_set, sprite_name);
				
				prop_selector.hide();
			}
			else if(prop_selector.result == None)
			{
				prop_set = 0;
				prop_group = 0;
				prop_index = 0;
				prop_palette = 0;
				has_sprite = false;
			}
		}
		
		if(has_sprite)
		{
			const float mouse_x = g.mouse_x_world(0, 19);
			const float mouse_y = g.mouse_y_world(0, 19);
			sprite.draw(22, 24, 0, prop_palette, mouse_x, mouse_y, 0, 1, 1, 0xFFFFFFFF);
		}
	}
	
}