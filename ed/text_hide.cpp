#include '../lib/input/Mouse.cpp';
#include '../lib/enums/ColType.cpp';
#include '../lib/enums/GVB.cpp';

/// Right click to hide and left click to show text triggers
class script
{
	
	Mouse mouse(false);
	scene@ g;
	editor_api@ editor;
	
	script()
	{
		@g = get_scene();
		@editor = get_editor_api();
	}
	
	void editor_step()
	{
		mouse.step();
		
		if(!editor.mouse_in_gui() && !editor.key_check_gvb(GVB::Space) && (mouse.left_press || mouse.right_press))
		{
			int count = g.get_entity_collision(mouse.y-15, mouse.y+15, mouse.x-15, mouse.x+15, ColType::Trigger);
			for(int i = 0; i < count; i++)
			{
				entity@ e = g.get_entity_collision_index(i);
				
				if(e.type_name() != 'text_trigger')
					continue;
				
				varstruct@ vars = e.vars();
				vars.get_var('hide').set_bool(!mouse.left_press);
			}
		}
	}
	
}