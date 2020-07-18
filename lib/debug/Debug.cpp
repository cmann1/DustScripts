#include '../std.cpp';
#include '../drawing/common.cpp';
#include 'DebugItemList.cpp';
#include 'DebugTextLineList.cpp';
#include 'DebugTextLine.cpp';
#include 'DebugLine.cpp';

class Debug
{
	
	/*
	 * Text options
	 */
	 
	bool text_display_newset_first = true;
	uint text_bg_colour = 0x55000000;
	bool text_bg_glass = false;
	uint text_shadow_colour = 0x00000000;
	float text_shadow_ox = 2;
	float text_shadow_oy = 2;
	bool text_outline = false;

	private string _text_font = 'ProximaNovaReg';
	private uint _text_size = 36;
	private int _text_align_x = -1;
	private int _text_align_y = -1;
	private float _text_line_spacing = 8;
	private float _text_scale = 1;
	
	private textfield@ text_field;
	private scene@ g;
	
	private DebugTextLineList text_lines;
	private float text_height;
	private bool recalculate_text_height;
	
	private DebugItemList items;
	private DebugLinePool line_pool;
	
	Debug()
	{
		@g = get_scene();
		@text_field = create_textfield();
		text_field.set_font(_text_font, _text_size);
		text_field.align_horizontal(_text_align_x);
		text_field.align_vertical(_text_align_y);
		
		@text_lines.text_field = text_field;
		text_lines.spacing = _text_line_spacing;
		text_lines.scale = _text_scale;
	}
	
	void step()
	{
		/*
		 * Items
		 */
		
		DebugItem@ item = items.first;
		
		while(@item != null)
		{
			DebugItem@ next = item.next;
			
			if(item.frames == 0)
			{
				items.remove(item);
			}
			else
			{
				item.frames--;
			}
			
			@item = next;
		}
		
		/*
		 * Text
		 */
		
		DebugTextLine@ line = text_lines.first;
		
		while(@line != null)
		{
			DebugTextLine@ next = line.next;
			
			if(line.frames == 0)
			{
				text_lines.remove(line);
			}
			else
			{
				line.frames--;
			}
			
			@line = next;
		}
	}
	
	void draw(float sub_frame)
	{
		if(recalculate_text_height)
		{
			text_lines.calculate_height();
			recalculate_text_height = false;
		}
		
		draw_items();
		draw_text();
	}
	
	void draw_items()
	{
		DebugItem@ item = items.first;
		
		while(@item != null)
		{
			item.draw(g);
			@item = item.next;
		}
	}
	
	void draw_text()
	{
		const float padding = 10;
		const float text_height = text_lines.text_height;
		const float direction = text_display_newset_first ? -_text_align_y : _text_align_y;
		const float text_x = _text_align_x == -1 ? SCREEN_LEFT + padding : (_text_align_x == 1 ? SCREEN_RIGHT  - padding : 0);
		float y1, y2;
		
		if(_text_align_y == -1)
		{
			y1 = SCREEN_TOP + padding;
			y2 = y1 + text_height;
		}
		else if(_text_align_y == 1)
		{
			y2 = SCREEN_BOTTOM - padding;
			y1 = y2 - text_height;
		}
		else // 0
		{
			y1 = -text_height * 0.5;
			y2 =  text_height * 0.5;
		}
		
		if(!text_display_newset_first && _text_align_y == -1 || !text_display_newset_first && _text_align_y == 0)
		{
			if(y2 >= SCREEN_BOTTOM - padding)
			{
				y2 = SCREEN_BOTTOM - padding;
				y1 = y2 - text_height;
			}
		}
		else if(!text_display_newset_first && _text_align_y == 1 || text_display_newset_first && _text_align_y == 0)
		{
			if(y1 < SCREEN_TOP + padding)
			{
				y1 = SCREEN_TOP + padding;
				y2 = y1 + text_height;
			}
		}
		
		DebugTextLine@ line = text_lines.last;
		float y = 0;
		uint current_colour = 0;
		float text_width = 0;
		
		const float layer = 22;
		const float sub_layer = 15;
		
		while(@line != null)
		{
			float text_y;
			float min_y, max_y;
			
			if(_text_align_y == -1)
			{
				text_y = text_display_newset_first
					? y1 + y
					: y2 - y - line.height;
				
				min_y = text_y;
				max_y = text_y + line.height;
			}
			else if(_text_align_y == 1)
			{
				text_y = text_display_newset_first
					? y2 - y
					: y1 + y + line.height;
				
				min_y = text_y - line.height;
				max_y = text_y;
			}
			else
			{
				text_y = text_display_newset_first
					? y1 + y + line.height * 0.5
					: y2 - y - line.height * 0.5;
				
				min_y = text_y - line.height * 0.5;
				max_y = text_y + line.height * 0.5;
			}
			
			bool draw = true;
			
			if(min_y > SCREEN_BOTTOM)
			{
				if(text_display_newset_first && _text_align_y <= 0 || !text_display_newset_first && _text_align_y == 1)
					break;
				
				draw = false;
			}
			else if(max_y < SCREEN_TOP)
			{
				if(!text_display_newset_first && _text_align_y <= 0 || _text_align_y == 1 && text_display_newset_first)
					break;
				
				draw = false;
			}
			
			if(draw)
			{
				if(line.colour != current_colour)
				{
					current_colour = line.colour;
					text_field.colour(current_colour);
				}
				
				text_field.text(line.text);
				
				if(text_shadow_colour != 0)
				{
					if(text_outline)
					{
						outlined_text_hud(text_field, layer, sub_layer, text_x, text_y, _text_scale, _text_scale, 0, text_shadow_colour, text_shadow_ox);
					}
					else
					{
						shadowed_text_hud(text_field, layer, sub_layer, text_x, text_y, _text_scale, _text_scale, 0, text_shadow_colour, text_shadow_ox, text_shadow_oy);
					}
				}
				else
				{
					text_field.draw_hud(layer, sub_layer, text_x, text_y, _text_scale, _text_scale, 0);
				}
				
				const float width = text_field.text_width();
				
				if(width > text_width)
				{
					text_width = width;
				}
			}
			
			y += line.height + _text_line_spacing;
			
			@line = line.prev;
		}
		
		if(text_bg_colour != 0 && text_width > 0 && (y2 - y1 > 0))
		{
			float x1, x2;
			
			if(_text_align_x == -1)
			{
				x1 = text_x;
				x2 = x1 + text_width;
			}
			else if(_text_align_x == 1)
			{
				x2 = text_x;
				x1 = x2 - text_width;
			}
			else // 0
			{
				x1 = text_x - text_width * 0.5;
				x2 = text_x + text_width * 0.5;
			}
			
			x1 -= padding;
			x2 += padding;
			y1 -= padding;
			y2 += padding;
			
			g.draw_rectangle_hud(layer, sub_layer - 1,
				x1, y1, x2, y2,
				0, text_bg_colour);
				
			if(text_bg_glass)
			{
				g.draw_glass_hud(layer, sub_layer - 1, x1, y1, x2, y2, 0, 0x00ffffff);
			}
		}
	}
	
	/*
	 * Debug methods
	 */
	 
	void print(string text, uint colour, int key = -1, int frames = 120)
	{
		print(text, colour, key + '', frames);
	}
	 
	void print(string text, uint colour, string key = '', int frames = 120)
	{
		DebugTextLine@ line = null;
		bool is_new;
		
		if(key != '' && text_lines.ids.exists(key))
		{
			@line = cast<DebugTextLine@>(@text_lines.ids[key]);
			line.text = text;
			line.colour = colour;
			line.frames = frames;
			
			text_lines.remove(line, false);
			text_lines.insert(line, false);
		}
		else
		{
			text_lines.insert(text_lines.get(text, colour, key, frames));
		}
	}
	
	DebugLine@ line(float x1, float y1, float x2, float y2, uint layer, uint sub_layer, float thickness=2, uint colour=0xFFFFFFFF, int frames=1, bool world=true)
	{
		DebugLine@ line = line_pool.get();
		line.set(x1, y1, x2, y2, layer, sub_layer, thickness, colour, frames, world);
		items.insert(line);
		return line;
	}
	
	// TODO: Add more drawing types
	
	/*
	 * Text option methods
	 */
	
	uint text_max_lines
	{
		get const { return text_lines.max_lines; }
		set { text_lines.max_lines = value; }
	}
	
	string text_font
	{
		get const { return _text_font; }
		set { text_field.set_font(_text_font = value, _text_size); recalculate_text_height = true; }
	}
	 
	uint text_size
	{
		get const { return _text_size; }
		set { text_field.set_font(_text_font, _text_size = value); recalculate_text_height = true; }
	}
	
	int text_align_x
	{
		get const { return _text_align_x; }
		set { text_field.align_horizontal(_text_align_x = value); }
	}
	
	int text_align_y
	{
		get const { return _text_align_y; }
		set { text_field.align_vertical(_text_align_y = value); }
	}
	
	float text_line_spacing
	{
		get const { return _text_line_spacing; }
		set { _text_line_spacing = text_lines.spacing = value; recalculate_text_height = true; }
	}
	
	float text_scale
	{
		get const { return _text_scale; }
		set { _text_scale = text_lines.scale = value; recalculate_text_height = true; }
	}
	
	void set_align(int text_align_x, int text_align_y)
	{
		this.text_align_x = text_align_x;
		this.text_align_y = text_align_y;
	}
	
}