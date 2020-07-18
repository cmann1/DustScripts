#include '../fonts.cpp';
#include '../drawing/common.cpp';
#include 'DebugItem.cpp';
#include 'DebugTextState.cpp';

class DebugText : DebugItem
{
	
	string text;
	float x;
	float y;
	float rotation;
	float scale;
	string font_name;
	uint size;
	int align_x;
	int align_y;
	uint colour;
	uint shadow_colour;
	float width;
	bool outline;
	
	DebugTextState@ state;
	
	void set(
		uint layer, uint sub_layer,
		string text, float x, float y, float scale, float rotation,
		string font_name, uint size, int align_x, int align_y,
		uint colour, uint shadow_colour, float width, bool outline,
		bool world, int frames)
	{
		DebugItem::set(layer, sub_layer, frames, world);
		
		this.text = text;
		this.x = x;
		this.y = y;
		this.scale = scale;
		this.rotation = rotation;
		this.font_name = font_name;
		this.size = size;
		this.align_x = align_x;
		this.align_y = align_y;
		this.colour = colour;
		this.shadow_colour = shadow_colour;
		this.width = width;
		this.outline = outline;
	}
	
	void draw(scene@ g)
	{
		state.set(font_name, size, align_x, align_y, colour);
		state.text_field.text(text);
		
		if(shadow_colour != 0)
		{
			if(outline)
			{
				if(world)
					outlined_text_world(state.text_field, layer, sub_layer, x, y, scale, scale, rotation, shadow_colour, width);
				else
					outlined_text_hud(state.text_field, layer, sub_layer, x, y, scale, scale, rotation, shadow_colour, width);
			}
			else
			{
				if(world)
					shadowed_text_world(state.text_field, layer, sub_layer, x, y, scale, scale, rotation, shadow_colour, width, width);
				else
					shadowed_text_hud(state.text_field, layer, sub_layer, x, y, scale, scale, rotation, shadow_colour, width, width);
			}
		}
		else
		{
			if(world)
				state.text_field.draw_world(layer, sub_layer, x, y, scale, scale, rotation);
			else
				state.text_field.draw_hud(layer, sub_layer, x, y, scale, scale, rotation);
		}
		
		if(shadow_colour != 0)
		if(world)
			state.text_field.draw_world(layer, sub_layer, x, y, scale, scale, rotation);
		else
			state.text_field.draw_hud(layer, sub_layer, x, y, scale, scale, rotation);
	}
	
}

class DebugTextPool
{
	
	private uint pool_index = 0;
	private uint pool_size = 8;
	private array<DebugText@> pool(pool_size);
	
	DebugTextState@ text_state;
	
	DebugText@ get()
	{
		if(pool_index > 0)
			return pool[--pool_index];
		
		DebugText@ text = DebugText();
		@text.state = text_state;
		return text;
	}
	
	void release(DebugText@ obj)
	{
		if(pool_index == pool_size)
		{
			pool_size += 8;
			pool.resize(pool_size);
		}
		
		@pool[pool_index++] = obj;
	}
	
}