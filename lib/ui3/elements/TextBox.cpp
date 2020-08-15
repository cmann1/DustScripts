#include '../../enums/VK.cpp';
#include 'Element.cpp';

class TextBox : Element, IStepHandler
{
	
	protected string _text =
		'This is a really really long line of text!!\n'
		'\n'
		'asdf\n'
		'Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n'
		'Fusce finibus purus in mauris iaculis, in luctus dolor consequat. Fusce finibus purus in mauris iaculis, in luctus dolor consequat.\n'
		'The quick brown fox jumped over the lazy dog.';
	// TODO: Set to false
	protected bool _multi_line = true;
	protected string _font;
	protected uint _size;
	
	protected float _line_height = 5;
	protected float _line_spacing = 6;
	
	protected int _selection_start = 0;
	protected int _selection_end = 64;
	
	/// The actual line height in pixels
	protected float real_line_height;
	protected array<float>@ font_metrics;
	protected float padding;
	protected float text_scale;
	protected int first_char_index;
	protected int last_char_index;
	protected float text_width;
	protected float text_height;
	protected float scroll_max_x;
	protected float scroll_max_y;
	
	protected int _text_length;
	protected int _num_lines;
	protected array<int> line_end_indices;
	
	protected int first_visible_line;
	protected int num_visible_lines;
	protected array<string> visible_lines;
	protected array<float> visible_lines_offset;
	protected array<float> visible_lines_selection;
	
	TextBox(UI@ ui, const string font='', const uint size=0)
	{
		super(ui);
		
		_width  = _set_width  = 140;
		// TODO: Set to 34
		_height = _set_height = 54;
		
		first_char_index = ui.first_char_index;
		last_char_index = ui.last_char_index;
		
		this._font = font;
		this._size = size;
		
		ui.get_font_metrics(_font, _size, @font_metrics, real_line_height);
		
		update();
		step_registered = true;
		ui._step_subscribe(this);
	}
	
	string element_type { get const { return 'TextBox'; } }
	
	// ///////////////////////////////////////////////////////////////////
	// Basic properties
	// ///////////////////////////////////////////////////////////////////
	
	string text
	{
		get const { return _text; }
		set
		{
			if(_text == value)
				return;
			
			_text = value;
			update();
		}
	}
	
	bool multi_line
	{
		get const { return _multi_line; }
		set
		{
			if(_multi_line == value)
				return;
			
			_multi_line = value;
			update();
		}
	}
	 
	string font
	{
		get const { return _font; }
		set
		{
			if(_font == value)
				return;
			
			_font = value;
			ui.get_font_metrics(_font, _size, @font_metrics, real_line_height);
			update();
		}
	}
	 
	uint size
	{
		get const { return _size; }
		set
		{
			if(_size == value)
				return;
			
			_size = value;
			ui.get_font_metrics(_font, _size, @font_metrics, real_line_height);
			update();
		}
	}
	
	float scroll_x
	{
		set override
		{
			value = clamp_scroll(value, scroll_max_x);
			
			if(_scroll_x == value)
				return;
			
			_scroll_x = value;
			validate_layout = true;
		}
	}
	
	float scroll_y
	{
		set override
		{
			value = clamp_scroll(value, scroll_max_y);
			
			if(_scroll_y == value)
				return;
			
			_scroll_y = value;
			validate_layout = true;
		}
	}
	
	int selection_start
	{
		get const { return _selection_start; }
		set
		{
			value = clamp(value, 0, _text_length);
			
			if(_selection_start == value)
				return;
			
			_selection_start = value;
			validate_layout = true;
		}
	}
	
	int selection_end
	{
		get const { return _selection_end; }
		set
		{
			value = clamp(value, 0, _text_length);
			
			if(_selection_end == value)
				return;
			
			_selection_end = value;
			validate_layout = true;
		}
	}
	
	int caret_index
	{
		get const { return _selection_end; }
		set
		{
			value = clamp(value, 0, _text_length);
			
			if(_selection_start == value && _selection_end == value)
				return;
			
			_selection_start = value;
			_selection_end = value;
			validate_layout = true;
		}
	}
	
	void select(int start, int end)
	{
		start = clamp(start, 0, _text_length);
		end = clamp(end, 0, _text_length);
		
		if(_selection_start == start && _selection_end == end)
			return;
		
		_selection_start = start;
		_selection_end = end;
		validate_layout = true;
	}
	
	string selection
	{
		get const
		{
			return _text.substr(_selection_start, _selection_end - _selection_start);
		}
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Internal
	// ///////////////////////////////////////////////////////////////////
	
	bool ui_step() override
	{
//		step_registered = false;
		
		if(_debug_drag)
		{
			if(ui.mouse.primary_down)
			{
				scroll_x = _scroll_x + ui.mouse.delta_x;
				scroll_y = _scroll_y + ui.mouse.delta_y;
				step_registered = true;
			}
			else
			{
				_debug_drag = false;
			}
		}
		
		if(ui.mouse.middle_press)
		{
			puts('"' + selection + '"');
		}
		
		return step_registered;
	}
	
	void _do_layout(LayoutContext@ ctx) override
	{
		padding = ui.style.spacing;
		text_scale = ui.style.default_text_scale;
		
		visible_lines.resize(0);
		visible_lines_offset.resize(0);
		visible_lines_selection.resize(0);
		num_visible_lines = 0;
		
		float y = 0;
		int line_start_index = 0;
		const float available_width  = _width  - padding * 2;
		const float available_height = _height - padding * 2;
		
		const float scroll_x = this._scroll_x;
		const float scroll_y = this._scroll_y;
		const float text_scale = this.text_scale;
		const float line_height = real_line_height * text_scale;
		
		float line_width;
		
		////////////////////////////////////////////////////////////
		// Step 1. Calculate the total text width and height
		
		text_width = 0;
		text_height = 0;
		
		for(int i = 0; i < _num_lines; i++)
		{
			const int line_end_index = line_end_indices[i];
			
			string line_text = _text.substr(line_start_index, line_end_index - line_start_index);
			const int line_length = int(line_text.length());
			
			line_width = 0;
			
			for(int j = 0; j < line_length; j++)
			{
				const int chr = int(line_text[j]);
				
				line_width += chr <= last_char_index && chr >= first_char_index
					? font_metrics[chr - first_char_index] * text_scale
					: 0;
			}
			
			if(line_width > text_width)
			{
				text_width = line_width;
			}
			
			if(i > 0)
			{
				text_height += _line_spacing;
			}
			
			text_height += line_height;
			line_start_index = line_end_index + 1;
		}
		
		scroll_max_x = max(0, text_width  - available_width);
		scroll_max_y = max(0, text_height - available_height);
		
		_scroll_x = clamp_scroll(_scroll_x, scroll_max_x);
		_scroll_y = clamp_scroll(_scroll_y, scroll_max_y);
		
		////////////////////////////////////////////////////////////
		// Step 1. Calculate the visible lines, and the start end
		//         visible character per line
		
		first_visible_line = ceil_int(-_scroll_y / (line_height + _line_spacing));
		y = _scroll_y + first_visible_line * (line_height + _line_spacing);
		line_start_index = _num_lines > 0 && first_visible_line > 0 ? line_end_indices[first_visible_line - 1] + 1 : 0;
		
		array<float>@ font_metrics = @this.font_metrics;
		const int first_char_index = this.first_char_index;
		const int last_char_index = this.last_char_index;
		
		for(int i = first_visible_line; i < _num_lines; i++)
		{
			if(y + line_height > available_height)
				break;
			
			const int current_line_start_index = line_start_index;
			const int line_end_index = line_end_indices[i];
			
			string line_text = _text.substr(line_start_index, line_end_index - line_start_index + 1);
			const int line_length = int(line_text.length());
			
			// Move to the next line - these values aren't used below so they can be set here
			y += line_height + _line_spacing;
			line_start_index = line_end_index + 1;
			
			if(line_length == 0)
			{
				visible_lines.insertLast('');
				visible_lines_offset.insertLast(0);
				num_visible_lines++;
				
				if(
					scroll_x >= 0 &&
					current_line_start_index >= _selection_start && current_line_start_index < _selection_end ||
					current_line_start_index >= _selection_end   && current_line_start_index < _selection_start
				)
				{
					visible_lines_selection.insertLast(0);
					visible_lines_selection.insertLast(0);
				}
				else
				{
					visible_lines_selection.insertLast(-1);
					visible_lines_selection.insertLast(-1);
				}
				
				continue;
			}
			
			int start_index = 0;
			int end_index = 0;
			float line_offset = 0;
			
			float x = scroll_x;
			line_width = 0;
			
			int chr_index = current_line_start_index;
			float line_selection_start = NAN;
			float line_selection_end   = NAN;
			
			for(int j = 0; j < line_length; j++)
			{
				const int chr = int(line_text[j]);
				const float chr_width = chr <= last_char_index && chr >= first_char_index
					? font_metrics[chr - first_char_index] * text_scale
					: 0;
				
				line_width += chr_width;
				
				if(
					chr_index >= _selection_start && chr_index < _selection_end ||
					chr_index >= _selection_end   && chr_index < _selection_start
				)
				{
					if(is_nan(line_selection_start))
						line_selection_start = x + line_width - chr_width;
					
					line_selection_end = is_nan(line_selection_end)
						? min(line_selection_end,   x + line_width)
						: x + line_width;
					
					// Add some extra to display a selected newline
					if(chr_index == current_line_start_index + line_length - 1 && (chr_index + 1 <= _selection_start || chr_index + 1 <= _selection_end))
					{
						line_selection_end += 6;
					}
				}
				
				if(x + line_width - EPSILON > available_width)
					break;
				
				end_index = j;
				
				if(x + line_width - chr_width < 0)
				{
					line_offset = line_width;
					start_index = j + 1;
					end_index = start_index;
				}
				
				chr_index++;
			}
			
			if(
				!is_nan(line_selection_start) && !is_nan(line_selection_end) &&
				line_selection_end > 0
			)
			{
				visible_lines_selection.insertLast(clamp(line_selection_start, 0, available_width) - scroll_x);
				visible_lines_selection.insertLast(clamp(line_selection_end, 0, available_width) - scroll_x);
			}
			else
			{
				visible_lines_selection.insertLast(-1);
				visible_lines_selection.insertLast(-1);
			}
			
			if(end_index - start_index == line_length)
			{
				visible_lines.insertLast(line_text);
			}
			else
			{
				visible_lines.insertLast(line_text.substr(start_index, end_index - start_index + 1));
			}
			
			visible_lines_offset.insertLast(line_offset);
			num_visible_lines++;
		}
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		const uint border_clr = style.get_interactive_element_border_colour(hovered, false, false, disabled);
		
		const uint bg_clr = style.get_interactive_element_background_colour(false, false, false, disabled, true);
		const float inset = border_clr != 0 ? max(0, style.border_size) : 0;
		
		// Fill
		style.draw_rectangle(
			x1 + inset, y1 + inset,
			x2 - inset, y2 - inset,
			0, bg_clr);
		
		// Border
		if(border_clr != 0 && style.border_size > 0)
		{
			style.outline(
				x1, y1,
				x2, y2,
				style.border_size, border_clr);
		}
		
		float text_scale = this.text_scale;
		const float line_height = real_line_height * text_scale;
		float x = x1 + padding + _scroll_x;
		float y = y1 + padding + _scroll_y + first_visible_line * (line_height + _line_spacing);
		int selection_index = 0;
		const bool draw_selection = _selection_start != _selection_end;
		
		if(!multi_line)
		{
			y = y1 + (_height - line_height) * 0.5;
		}
		
		canvas@ c;
		textfield@ text_field = style._initialise_text_field(
			@c,
			style.text_clr,
			text_scale, text_scale,
			text_scale, text_scale, 0,
			TextAlign::Left, TextAlign::Top,
			_font, _size);
		const float dx = style.text_offset_x * text_scale;
		const float dy = style.text_offset_y * text_scale;
		
		for(int i = 0; i < num_visible_lines; i++)
		{
			const string text = visible_lines[i];
			
			if(draw_selection)
			{
				const float selection_start = visible_lines_selection[selection_index++];
				float selection_end   = visible_lines_selection[selection_index++];
				
				if(selection_start >= 0)
				{
					style.draw_rectangle(
						x + selection_start,
						y - 1,
						x + selection_end,
						y + line_height + 3,
						0, style.secondary_bg_clr);
				}
			}
			
			if(text != '')
			{
				// TODO: Draw in chunks of 64 characters since that seems to be the limit for that textfield can draw
				const int chunk_size = 64;
				float chunk_x = x + visible_lines_offset[i];
				const int line_length = int(text.length());
				int characters_drawn = 0;
				const bool draw_in_chunks = line_length > chunk_size;
				
				while(characters_drawn < line_length)
				{
					text_field.text(draw_in_chunks
						? text.substr(characters_drawn, chunk_size)
						: text);
					c.draw_text(text_field, chunk_x, y, text_scale, text_scale, 0);
					
					if(draw_in_chunks)
					{
						chunk_x += text_field.text_width() * text_scale;
						characters_drawn += chunk_size;
					}
					else
					{
						break;
					}
				}
			}
			
			y += line_height + _line_spacing;
		}
	}
	
	protected void update()
	{
		int line_start_index = 0;
		_num_lines = 0;
		line_end_indices.resize(0);
		
		_text_length = int(_text.length());
		
		// TODO: Remove line breaks, or just ignore other lines, when multi_line is false
		
		while(true)
		{
			_num_lines++;
			const int line_end_index = _text.findFirst('\n', line_start_index);
			
			if(line_end_index == -1)
			{
				line_end_indices.insertLast(int(_text.length()));
				break;
			}
			
			line_end_indices.insertLast(line_end_index);
			line_start_index = line_end_index + 1;
		}
		
		validate_layout = true;
	}
	
	protected float clamp_scroll(const float scroll, const float max)
	{
		if(scroll < -max)
			return -max;
		
		if(scroll > 0)
			return 0;
		
		return scroll;
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////////////////////////////
	
	// TODO: REMOVE
	bool _debug_drag;
	bool step_registered;
	
	void _mouse_press(const MouseButton button)
	{
		if(button == ui.primary_button)
		{
			_debug_drag = true;
			step_registered = ui._step_subscribe(this, step_registered);
		}
	}
	
	void _mouse_release(const MouseButton button)
	{
		if(button == ui.primary_button)
		{
			_debug_drag = false;
		}
	}
	
	void _mouse_scroll(const int scroll_dir)
	{
		scroll_x -= scroll_dir;// * 10;
	}
	
}