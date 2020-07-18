#include '../std.cpp';
#include '../drawing/common.cpp';
#include 'DebugTextLine.cpp';

class Debug
{
	
	/*
	 * Text options
	 */
	 
	int  text_max_lines = 100;
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
	
	private DebugTextLine@ first_line = null;
	private DebugTextLine@ last_line = null;
	private int num_lines;
	private int num_lines_id;
	private float text_height;
	private bool recalculate_text_height;
	
	private dictionary text_line_ids;
	private uint debug_text_line_pool_size =  0;
	private array<DebugTextLine@> debug_text_line_pool(text_max_lines);
	
	Debug()
	{
		@g = get_scene();
		@text_field = create_textfield();
		text_field.set_font(_text_font, _text_size);
		text_field.align_horizontal(_text_align_x);
		text_field.align_vertical(_text_align_y);
	}
	
	void step()
	{
		DebugTextLine@ line = first_line;
		
		while(@line != null)
		{
			DebugTextLine@ next = line.next;
			
			if(line.frames == 0)
			{
				remove_line(line);
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
			DebugTextLine@ line = first_line;
			text_height = 0;
			
			while(@line != null)
			{
				text_field.text(line.text);
				line.height = text_field.text_height();
				text_height += line.height * _text_scale;
				
				@line = line.next;
			}
			
			text_height += _text_line_spacing * (num_lines - 1);
			recalculate_text_height = false;
		}
		
		draw_text();
	}
	
	void draw_text()
	{
		const float padding = 10;
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
		
		DebugTextLine@ line = last_line;
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
	
	private void insert_line(DebugTextLine@ line, bool update_id_dict = true)
	{
//		puts('inserting: ' + line.text);
		
		if(@last_line == null)
		{
			@first_line = @last_line = line;
			
			if(line.key != '')
				num_lines_id = 1;
			
			num_lines = 1;
//			puts('  first');
		}
		else
		{
//			puts('  old last: ' + last_line.text);
//			puts('  new last: ' + line.text);
			@last_line.next = line;
			@line.prev = last_line;
			@last_line = line;
			
			num_lines++;
			
			if(line.key != '')
				num_lines_id++;
				
			if(num_lines - num_lines_id > text_max_lines)
			{
				DebugTextLine@ first_transient_line = first_line;
				
				while(@first_transient_line != null)
				{
					if(first_transient_line.key == '')
						break;
					
					@first_transient_line = first_transient_line.next;
				}
				
				if(@first_transient_line != null)
				{
//					puts('  removing first: ' + first_transient_line.text);
					remove_line(first_transient_line);
				}
			}
		}
		
		if(update_id_dict && line.key != '')
		{
			@text_line_ids[line.key] = @line;
		}
		
		text_field.text(line.text);
		line.height = text_field.text_height();
		text_height += line.height * _text_scale;
		
		if(num_lines > 1)
		{
			text_height += _text_line_spacing;
		}
		
//		debug_lines();
	}
	
	private void remove_line(DebugTextLine@ line, bool release = true)
	{
//		puts('removing: ' + line.text);
		
		if(@line.prev != null)
		{
//			puts('  remove from prev: ' + line.prev.text);
			@line.prev.next = line.next;
		}
		else
		{
//			puts('  new first: ' + (@line.next != null ? line.next.text : 'null'));
			@first_line = line.next;
		}
		
		if(@line.next != null)
		{
//			puts('  remove from next: ' + line.next.text);
			@line.next.prev = line.prev;
		}
		else
		{
//			puts('  new last: ' + (@line.prev != null ? line.prev.text : 'null'));
			@last_line = line.prev;
		}
		
		@line.prev = null;
		@line.next = null;
		
		if(release)
		{
			if(line.key != '')
			{
				text_line_ids.delete(line.key);
			}
			
			if(debug_text_line_pool_size == debug_text_line_pool.size())
			{
				debug_text_line_pool.insertLast(@line);
				debug_text_line_pool_size++;
			}
			else
			{
				@debug_text_line_pool[debug_text_line_pool_size++] = line;
			}
			
//			puts('  pool size: ' + debug_text_line_pool_size);
		}
		
		text_height -= line.height * _text_scale;
		
		if(num_lines > 1)
		{
			text_height -= _text_line_spacing;
		}
		num_lines--;
		
		if(line.key != '')
			num_lines_id--;
		
//		debug_lines();
	}
	
	private void debug_lines()
	{
		DebugTextLine@ line = first_line;
		
		int a = 0;
		
		while(@line != null)
		{
			if(++a > num_lines)
			{
				puts('Possible error in lines linked list');
				break;
			}
			
			string str = '         '+a+' - ';
			
			if(@line.prev != null)
				str += '(prev:' + line.prev.text + ') ';
			
			if(line.key != '')
				str += '[' + line.key + '] ';
			
			str += line.text;
			
			if(@line.next != null)
				str += ' (next:' + line.next.text + ')';
			
			puts(str);
			
			@line = line.next;
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
		
		if(key != '' && text_line_ids.exists(key))
		{
			@line = cast<DebugTextLine@>(@text_line_ids[key]);
			line.text = text;
			line.colour = colour;
			line.frames = frames;
			
			remove_line(line, false);
			insert_line(line, false);
		}
		else
		{
			if(debug_text_line_pool_size > 0)
			{
				@line = debug_text_line_pool[--debug_text_line_pool_size];
				@line.next = @line.prev = null;
				line.text = text;
				line.colour = colour;
				line.key = key;
				line.frames = frames;
			}
			else
			{
				@line = DebugTextLine(text, colour, key, frames);
			}
			
			insert_line(line);
		}
	}
	
	/*
	 * Text option methods
	 */
	 
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
		set { _text_line_spacing = value; recalculate_text_height = true; }
	}
	
	float text_scale
	{
		get const { return _text_scale; }
		set { _text_scale = value; recalculate_text_height = true; }
	}
	
	void set_align(int text_align_x, int text_align_y)
	{
		this.text_align_x = text_align_x;
		this.text_align_y = text_align_y;
	}
	
}