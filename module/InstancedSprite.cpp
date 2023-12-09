#include '../lib/std.cpp';
#include '../lib/input/GVB.cpp';
#include '../lib/input/Mouse.cpp';
#include '../lib/math/math.cpp';

class InstancedSprite : trigger_base
{
	
	[persist] string set = 'script';
	[persist] string sprite = '';
	[persist] array<InstancedSpriteEntry> instances;
	
	scene@ g;
	scripttrigger@ self;
	
	private Mouse mouse(false);
	private editor_api@ editor;
	private InstancedSpriteEntry@ active = null;
	private bool dragging;
	private bool rotating;
	private float rotate_start;
	private float drag_x, drag_y;
	
	sprites@ spr;
	
	InstancedSprite()
	{
		@g = get_scene();
		@spr = create_sprites();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
		spr.add_sprite_set(set);
		self.editor_handle_size(6);
	}
	
	void draw(float sub_frame)
	{
		for(uint i = 0; i < instances.length; i++)
		{
			InstancedSpriteEntry@ instance = @instances[i];
			spr.draw_world(instance.layer, instance.sub_layer, sprite, 0, 0, instance.x, instance.y, instance.rotation, instance.scale_x, instance.scale_y, 0xFFFFFFFF);
		}
	}
	
	void editor_step()
	{
		if(!self.editor_selected())
			return;
		
		if(@editor == null)
		{
			@editor = get_editor_api();
		}
		
		mouse.step();
		
		if(mouse.left_press && editor.key_check_gvb(GVB::Shift))
		{
			instances.resize(instances.length + 1);
			@active = @instances[instances.length - 1];
			dragging = true;
		}
		
		if(dragging)
		{
			active.x = mouse.x + drag_x;
			active.y = mouse.y + drag_y;
			
			if(!mouse.left_down)
			{
				@active = null;
				dragging = false;
			}
		}
		
		for(int i = int(instances.length) - 1; i >= 0; i--)
		{
			InstancedSpriteEntry@ instance = @instances[i];
			const float x = instance.x;
			const float y = instance.y;
			
			if(abs(x - mouse.x) > 4 || abs(y - mouse.y) > 4)
				continue;
			if(@active != null)
				continue;
			
			if(mouse.left_press)
			{
				@active = instance;
				dragging = true;
				drag_x = x - mouse.x;
				drag_y = y - mouse.y;
			}
			else if(mouse.middle_press)
			{
				if(editor.key_check_gvb(GVB::Alt))
				{
					if(editor.key_check_gvb(GVB::Shift))
					{
						instance.scale_y = -instance.scale_y;
					}
					else
					{
						instance.scale_x = -instance.scale_x;
					}
				}
				else
				{
					rotate_start = atan2(y - mouse.y, x - mouse.x) * RAD2DEG - instance.rotation;
					@active = instance;
					rotating = true;
				}
			}
			else if(mouse.right_press)
			{
				instances.removeAt(i);
			}
			else if(mouse.scroll != 0)
			{
				if(!editor.key_check_gvb(GVB::Alt))
				{
					instance.layer = clamp(instance.layer + mouse.scroll, 0, 20);
				}
				else
				{
					instance.sub_layer = clamp(instance.sub_layer + mouse.scroll, 0, 24);
				}
			}
		}
		
		if(rotating)
		{
			active.rotation = atan2(active.y - mouse.y, active.x - mouse.x) * RAD2DEG - rotate_start;
			
			if(!mouse.middle_down)
			{
				@active = null;
				rotating = false;
			}
		}
	}
	
	void editor_draw(float sub_frame)
	{
		draw(sub_frame);
		
		const float sx = self.x();
		const float sy = self.y();
		
		const bool selected = self.editor_selected();
		
		for(int i = int(instances.length()) - 1; i >= 0; i--)
		{
			InstancedSpriteEntry@ instance = @instances[i];
			const float x = instance.x;
			const float y = instance.y;
			
			if(selected)
			{
				g.draw_line_world(21, 19, x, y, sx, sy, 2, 0x77FFFFFF);
			}
			
			g.draw_rectangle_world(21, 19, x - 4, y - 4, x + 4, y + 4, 0,
				selected && abs(x - mouse.x) <= 4 && abs(y - mouse.y) <= 4
					? 0xffdddd33 : 0xffdd3333);
		}
		
		g.draw_rectangle_world(21, 19, sx + 10, sy - 20, sx + 40, sy - 16, 0, 0xFF0000FF);
		g.draw_rectangle_world(21, 19, sx + 10, sy - 2,  sx + 40, sy + 2, 0, 0xFF0000FF);
		g.draw_rectangle_world(21, 19, sx + 10, sy + 16, sx + 40, sy + 20, 0, 0xFF0000FF);
		
		g.draw_rectangle_world(21, 19, sx + 10, sy - 16, sx + 14, sy - 2, 0, 0xFF0000FF);
		g.draw_rectangle_world(21, 19, sx + 36, sy + 2,  sx + 40, sy + 16, 0, 0xFF0000FF);
	}
	
}

class InstancedSpriteEntry
{
	
	[position,layer:19,y:y] float x = 0;
	[hidden] float y = 0;
	[persist] int layer = 19;
	[persist] int sub_layer = 19;
	[angle] float rotation = 0;
	[persist] float scale_x = 1;
	[persist] float scale_y = 1;
	
}
