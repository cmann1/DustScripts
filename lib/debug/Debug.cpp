#include '../std.cpp';
#include '../fonts.cpp';
#include '../Mouse.cpp';
#include '../drawing/canvas.cpp';
#include '../math/math.cpp';
#include 'DebugItemList.cpp';
#include 'DebugTextLineList.cpp';
#include 'DebugTextLine.cpp';
#include 'DebugLine.cpp';
#include 'DebugRect.cpp';
#include 'DebugText.cpp';
#include 'DebugTextState.cpp';

class Debug
{
	
	/*
	 * Text options
	 */
	 
	bool text_display_newset_first = true;
	uint text_bg_colour = 0xaa000000;
	bool text_bg_glass = true;
	uint text_shadow_colour = 0x00000000;
	float text_shadow_ox = 2;
	float text_shadow_oy = 2;
	bool text_outline = false;

	private string _text_font = font::ENVY_BOLD;
	private uint _text_size = 20;
	private int _text_align_x = -1;
	private int _text_align_y = -1;
	private float _text_line_spacing = 8;
	private float _text_scale = 1;
	private float text_lines_padding = 10;
	
	private textfield@ print_text_field;
	private scene@ g;
	private canvas@ text_canvas;
	
	private DebugTextLineList text_lines;
	private float text_lines_x1, text_lines_y1;
	private float text_lines_x2, text_lines_y2;
	private bool recalculate_text_height;
	private float text_lines_scroll;
	private float single_line_height;
	private uint text_layer = 22;
	private uint text_sub_layer = 15;
	
	private DebugItemList items;
	private DebugLinePool line_pool;
	private DebugRectPool rect_pool;
	private DebugTextPool text_pool;
	private DebugTextState@ text_state = DebugTextState();
	
	private Mouse mouse(true);
	
	Debug()
	{
		mouse.scale_hud = false;
		@g = get_scene();
		@text_canvas = create_canvas(true, 22, 15);
		text_canvas.scale_hud(false);
		
		@print_text_field = create_textfield();
		print_text_field.set_font(_text_font, _text_size);
		print_text_field.align_horizontal(_text_align_x);
		print_text_field.align_vertical(_text_align_y);
		
		@text_lines.text_field = print_text_field;
		text_lines.spacing = _text_line_spacing;
		
		print_text_field.text(' ');
		single_line_height = print_text_field.text_height() + _text_line_spacing;
		
		@text_pool.text_state = text_state;
	}
	
	void step()
	{
		mouse.step();
		
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
		
		if(text_lines.count > 0)
		{
			int scroll;
			
			if(mouse.scrolled(scroll) && mouse.x >= text_lines_x1 && mouse.x <= text_lines_x2 && mouse.y >= text_lines_y1 && mouse.y <= text_lines_y2)
			{
				text_lines_scroll -= scroll * single_line_height;
			}
			
			text_lines_scroll = clamp(text_lines_scroll, 0, max(0, text_lines.text_height + text_lines_padding * 2 - (SCREEN_BOTTOM - SCREEN_TOP)));
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
		const float hud_width  = g.hud_screen_width(false);
		const float hud_height = g.hud_screen_height(false);
		const float SCREEN_LEFT   = -hud_width * 0.5;
		const float SCREEN_RIGHT  = -SCREEN_LEFT;
		const float SCREEN_TOP    = -hud_height * 0.5;
		const float SCREEN_BOTTOM = -SCREEN_TOP;
		
		const float text_height = text_lines.text_height;
		const float direction = text_display_newset_first ? -_text_align_y : _text_align_y;
		const float text_x = _text_align_x == -1 ? SCREEN_LEFT + text_lines_padding : (_text_align_x == 1 ? SCREEN_RIGHT  - text_lines_padding : 0);
		float y1, y2;
		
		if(_text_align_y == -1)
		{
			y1 = SCREEN_TOP + text_lines_padding;
			y2 = y1 + text_height;
		}
		else if(_text_align_y == 1)
		{
			y2 = SCREEN_BOTTOM - text_lines_padding;
			y1 = y2 - text_height;
		}
		else // 0
		{
			y1 = -text_height * 0.5;
			y2 =  text_height * 0.5;
		}
		
		if(!text_display_newset_first && _text_align_y == -1 || !text_display_newset_first && _text_align_y == 0)
		{
			if(y2 >= SCREEN_BOTTOM - text_lines_padding)
			{
				y2 = SCREEN_BOTTOM - text_lines_padding;
				y1 = y2 - text_height;
			}
		}
		else if(!text_display_newset_first && _text_align_y == 1 || text_display_newset_first && _text_align_y == 0)
		{
			if(y1 < SCREEN_TOP + text_lines_padding)
			{
				y1 = SCREEN_TOP + text_lines_padding;
				y2 = y1 + text_height;
			}
		}
		
		DebugTextLine@ line = text_lines.last;
		float y = 0;
		uint current_colour = 0;
		float text_width = 0;
		
		text_canvas.layer(text_layer);
		text_canvas.sub_layer(text_sub_layer);
		
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
			
			text_y += text_lines_scroll;
			min_y += text_lines_scroll;
			max_y += text_lines_scroll;
			
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
					print_text_field.colour(current_colour);
				}
				
				print_text_field.text(line.text);
				
				if(text_shadow_colour != 0)
				{
					if(text_outline)
					{
						draw::outlined_text(text_canvas, print_text_field, text_layer, text_sub_layer, text_x, text_y, text_lines.scale, text_lines.scale, 0, text_shadow_colour, text_shadow_ox);
					}
					else
					{
						draw::shadowed_text(text_canvas, print_text_field, text_layer, text_sub_layer, text_x, text_y, text_lines.scale, text_lines.scale, 0, text_shadow_colour, text_shadow_ox, text_shadow_oy);
					}
				}
				else
				{
					text_canvas.draw_text(print_text_field, text_x, text_y, text_lines.scale, text_lines.scale, 0);
				}
				
				const float width = print_text_field.text_width() * text_lines.scale;
				
				if(width > text_width)
				{
					text_width = width;
				}
			}
			
			y += line.height + _text_line_spacing;
			
			@line = line.prev;
		}
		
		/*
		 * Draw background
		 */
		
		//{
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
			
			x1 -= text_lines_padding;
			x2 += text_lines_padding;
			y1 -= text_lines_padding;
			y2 += text_lines_padding;
			
			text_lines_x1 = x1;
			text_lines_y1 = y1;
			text_lines_x2 = x2;
			text_lines_y2 = y2;
			
			if(text_bg_colour != 0 && text_width > 0 && (y2 - y1 > 0))
			{
				text_canvas.layer(text_layer);
				text_canvas.sub_layer(text_sub_layer - 1);
				
				text_canvas.draw_rectangle(
					x1, y1, x2, y2,
					0, text_bg_colour);
					
				if(text_bg_glass)
				{
					text_canvas.draw_glass(x1, y1, x2, y2, 0, 0x00ffffff);
				}
			}
		//}
	}
	
	/*
	 * Debug methods
	 */
	 
	void print(string text, uint colour, int key, int frames = 120)
	{
		print(text, colour, key + '', frames);
	}
	 
	void print(string text, int key)
	{
		print(text, 0xffffffff, key + '');
	}
	 
	void print(string text, string key)
	{
		print(text, 0xffffffff, key);
	}
	 
	void print(string text, uint colour=0xffffffff, string key = '', int frames = 120)
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
	
	DebugLine@ line(uint layer, uint sub_layer, float x1, float y1, float x2, float y2, float thickness=2, uint colour=0xFFFFFFFF, bool world=true, int frames=1)
	{
		DebugLine@ line = line_pool.get();
		line.set(layer, sub_layer, x1, y1, x2, y2, thickness, colour, world, frames);
		items.insert(line);
		return line;
	}
	
	DebugRect@ rect(uint layer, uint sub_layer, float x1, float y1, float x2, float y2, float rotation=0, float thickness=-1, uint colour=0xFFFFFFFF, bool world=true, int frames=1)
	{
		DebugRect@ rect = rect_pool.get();
		rect.set(layer, sub_layer, x1, y1, x2, y2, rotation, thickness, colour, world, frames);
		items.insert(rect);
		return rect;
	}
	
	DebugText@ text(uint layer, uint sub_layer,
		string text, float x, float y, float scale=1, float rotation=0,
		string font_name=font::PROXIMANOVA_REG, uint size=36, int align_x=-1, int align_y=1,
		uint colour=0xFFFFFFFF, uint shadow_colour=0xaa000000, float width=2, bool outline=false,
		bool world=true, int frames=1)
	{
		DebugText@ text_item = text_pool.get();
		text_item.set(layer, sub_layer,
			text, x, y, scale, rotation,
			font_name, size, align_x, align_y,
			colour, shadow_colour, width, outline,
			world, frames);
		items.insert(text_item);
		return text_item;
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
		set { print_text_field.set_font(_text_font = value, _text_size); recalculate_text_height = true; }
	}
	 
	uint text_size
	{
		get const { return _text_size; }
		set { print_text_field.set_font(_text_font, _text_size = value); recalculate_text_height = true; }
	}
	
	int text_align_x
	{
		get const { return _text_align_x; }
		set { print_text_field.align_horizontal(_text_align_x = value); }
	}
	
	int text_align_y
	{
		get const { return _text_align_y; }
		set { print_text_field.align_vertical(_text_align_y = value); }
	}
	
	float text_line_spacing
	{
		get const { return _text_line_spacing; }
		set { _text_line_spacing = text_lines.spacing = value; recalculate_text_height = true; }
	}
	
	float text_scale
	{
		get const { return text_lines.scale; }
		set { text_lines.scale = value; recalculate_text_height = true; }
	}
	
	void set_align(int text_align_x, int text_align_y)
	{
		this.text_align_x = text_align_x;
		this.text_align_y = text_align_y;
	}
	
}