#include '../../enums/VK.cpp';
#include '../../enums/GVB.cpp';
#include '../../editor/common.cpp';
#include '../../input/Keyboard.cpp';
#include '../../input/navigation/INavigable.cpp';
#include '../../string.cpp';
#include 'Element.cpp';

class TextBox : Element, IStepHandler, IKeyboardFocus, INavigable
{
	
	// TODO: Set to empty
	protected string _text =
		'1\n'
		'This is a     really\n'
		'\n'
		'          asdf\n'
		'Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n'
		'Fusce finibus purus in mauris iaculis, in luctus dolor consequat. Fusce finibus purus in mauris iaculis, in luctus dolor consequat.\n'
		'This is a     really ,,|;,really./\\[] long line of text!!\n'
		'The quick brown fox jumped over the lazy dog.';
	// TODO: Set to false
	protected bool _multi_line = true;
	protected bool _smart_home = true;
	protected bool _accept_on_blur = true;
	protected bool _drag_scroll = true;
	protected string _font;
	protected uint _size;
	
	protected float _line_spacing = 6;
	
	protected int _selection_start = 64;
	protected int _selection_end = 0;
	/// The stored relative index of the caret within the selected line.
	/// When navigating up or down a line, the caret will try to match this position.
	/// Only updated when the position within the line is explicitly changed, e.g. moving left/right, or selecting with the mouse.
	protected int _line_relative_caret_index = -1;
	protected int _selection_start_line_index;
	protected int _selection_end_line_index;
	
	/// The actual line height in pixels
	protected float real_line_height;
	protected array<float>@ font_metrics;
	protected float padding_left;
	protected float padding_right;
	protected float padding_top;
	protected float padding_bottom;
	protected float text_scale;
	protected int first_char_index;
	protected int last_char_index;
	protected float _text_width;
	protected float _text_height;
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
	
	protected int caret_line_index;
	protected float caret_line_x;
	protected int persist_caret_time;
	
	protected bool focused;
	protected NavigationGroup@ _navigation_parent;
	protected NavigateOn _navigate_on = NavigateOn(Inherit | Tab | (_multi_line ? CtrlReturn : Return) | Escape);
	
	protected bool drag_selection;
	protected int double_click_start_index = -1;
	protected int double_click_end_index = -1;
	protected bool scrolled;
	
	protected bool drag_scroll;
	protected float drag_scroll_start_x;
	protected float drag_scroll_start_y;
	protected float drag_mouse_x_start;
	protected float drag_mouse_y_start;
	
	protected bool step_registered;
	
	TextBox(UI@ ui, const string font='', const uint size=0)
	{
		super(ui);
		
		// TODO: Set to 140
		_width  = _set_width  = 200;
		// TODO: Set to 34
		_height = _set_height = 84;
		
		first_char_index = ui.first_char_index;
		last_char_index = ui.last_char_index;
		
		this._font = font;
		this._size = size;
		
		ui.get_font_metrics(_font, _size, @font_metrics, real_line_height);
		
		update_line_endings();
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
			update_line_endings();
		}
	}
	
	/// Can this TextBox contain more than one line?
	bool multi_line
	{
		get const { return _multi_line; }
		set
		{
			if(_multi_line == value)
				return;
			
			_multi_line = value;
			update_line_endings();
			
			if(_multi_line)
			{
				if((_navigate_on | NavigateOn::Return) != 0)
				{
					_navigate_on = NavigateOn((_navigate_on & ~NavigateOn::Return) | NavigateOn::CtrlReturn);
				}
			}
			else if((_navigate_on | NavigateOn::CtrlReturn) != 0)
			{
				_navigate_on = NavigateOn((_navigate_on & ~NavigateOn::CtrlReturn) | NavigateOn::Return);
			}
		}
	}
	
	/// If true pressing the home key will move the caret to the first non whitespace character in the line
	bool smart_home
	{
		get const { return _smart_home; }
		set { _smart_home = value; }
	}
	
	/// When this TextBox loses focus, accept the changes if true, otherwise cancel and revert.
	bool accept_on_blur
	{
		get const { return _accept_on_blur; }
		set { _accept_on_blur = value; }
	}
	
	/// If true, dragging with right mouse will scroll the TextBox
	bool drag_scroll
	{
		get const { return _drag_scroll; }
		set { _drag_scroll = value; }
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
			validate_layout = true;
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
			validate_layout = true;
		}
	}
	
	float line_spacing
	{
		get const { return _line_spacing; }
		set
		{
			if(_line_spacing == value)
				return;
			
			_line_spacing = value;
			validate_layout = true;
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
	
	// ///////////////////////////////////////////////////////////////////
	// Selection
	// ///////////////////////////////////////////////////////////////////
	
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
	
	string selection
	{
		get const
		{
			return _selection_start < _selection_end
				? _text.substr(_selection_start, _selection_end   - _selection_start)
				: _text.substr(_selection_end,   _selection_start - _selection_end);
		}
	}
	
	/// Sets the selection from start to end. end can be smaller than start.
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
	
	/// Selects all the text.
	void select_all()
	{
		select(0, _text_length);
	}
	
	/// Sets the selection to the selection end.
	void select_none()
	{
		caret_index = _selection_end;
	}
	
	/// If word is true moves to the next word boundary. extend controls wether to move the caret, or extend the selection
	void move_caret_left(const bool word, const bool extend, const bool scroll_to_caret=false)
	{
		const int index = word
			? find_word_boundary(_selection_end, -1)
			: _selection_end - 1;
		
		if(extend)
		{
			selection_end = index;
		}
		else
		{
			caret_index = index;
		}
		
		if(validate_layout)
		{
			update_selection_line_indices();
			_line_relative_caret_index = -1;
			
			if(scroll_to_caret)
			{
				this.scroll_to_caret(8);
			}
		}
		
		persist_caret();
	}
	
	/// If word is true moves to the next word boundary. extend controls wether to move the caret, or extend the selection
	void move_caret_right(const bool word, const bool extend, const bool scroll_to_caret=false)
	{
		const int index = word
			? find_word_boundary(_selection_end, 1)
			: _selection_end + 1;
		
		if(extend)
		{
			selection_end = index;
		}
		else
		{
			caret_index = index;
		}
		
		if(validate_layout)
		{
			update_selection_line_indices();
			_line_relative_caret_index = -1;
			
			if(scroll_to_caret)
			{
				this.scroll_to_caret(8);
			}
		}
		
		persist_caret();
	}
	
	/// Moves the caret up a line. extend controls wether to move the caret, or extend the selection
	void move_caret_up(const bool extend, const bool scroll_to_caret=false)
	{
		if(_selection_end_line_index == 0)
			return;
		
		update_relative_line_index();
		
		const int line_start = get_line_start(_selection_end_line_index - 1);
		const int line_end   = get_line_end  (_selection_end_line_index - 1);
		const int index = clamp(line_start + _line_relative_caret_index, line_start, line_end);
		
		if(extend)
		{
			selection_end = index;
		}
		else
		{
			caret_index = index;
		}
		
		if(validate_layout)
		{
			update_selection_line_indices();
			
			if(scroll_to_caret)
			{
				this.scroll_to_caret();
			}
		}
		
		persist_caret();
	}
	
	/// Moves the caret down a line. extend controls wether to move the caret, or extend the selection
	void move_caret_down(const bool extend, const bool scroll_to_caret=false)
	{
		if(_selection_end_line_index == _num_lines - 1)
			return;
		
		update_relative_line_index();
		
		const int line_start = get_line_start(_selection_end_line_index + 1);
		const int line_end   = get_line_end  (_selection_end_line_index + 1);
		const int index = clamp(line_start + _line_relative_caret_index, line_start, line_end);
		
		if(extend)
		{
			selection_end = index;
		}
		else
		{
			caret_index = index;
		}
		
		if(validate_layout)
		{
			update_selection_line_indices();
			
			if(scroll_to_caret)
			{
				this.scroll_to_caret();
			}
		}
		
		persist_caret();
	}
	
	/// Moves the caret to the start of the line, or the beginning of the text if start is true.
	/// extend controls wether to move the caret, or extend the selection
	void move_caret_home(const bool start, const bool extend, const bool scroll_to_caret=false)
	{
		const int line_start = start
			? 0
			: get_line_start(_selection_end_line_index);
		
		int home_index = line_start;
		
		if(_smart_home && !start)
		{
			const int line_end = get_line_end(_selection_end_line_index);
			
			for(int i = line_start; i < line_end; i++)
			{
				const int chr = _text[i];
				
				if(!string::is_whitespace(chr))
				{
					home_index = i;
					break;
				}
			}
			
			if(_selection_end == home_index)
			{
				home_index = line_start;
			}
		}
		
		if(extend)
		{
			selection_end = home_index;
		}
		else
		{
			caret_index = home_index;
		}
		
		if(validate_layout)
		{
			update_selection_line_indices();
			_line_relative_caret_index = -1;
			
			if(scroll_to_caret)
			{
				this.scroll_to_caret();
			}
		}
		
		persist_caret();
	}
	
	/// Moves the caret to the end of the line, or the end of the text if end is true.
	/// extend controls wether to move the caret, or extend the selection
	void move_caret_end(const bool end, const bool extend, const bool scroll_to_caret=false)
	{
		const int line_end = end
			? _text_length
			: get_line_end(_selection_end_line_index);
		
		if(extend)
		{
			selection_end = line_end;
		}
		else
		{
			caret_index = line_end;
		}
		
		if(validate_layout)
		{
			update_selection_line_indices();
			_line_relative_caret_index = -1;
			
			if(scroll_to_caret)
			{
				this.scroll_to_caret();
			}
		}
		
		persist_caret();
	}
	
	/// Scrolls the caret into view. padding_x controls approximately how many extra characters
	/// will be scrolled when the caret is not in view horizontally
	void scroll_to_caret(const int padding_x=0)
	{
		float x1, y1;
		get_index_xy(_selection_end, x1 ,y1);
		x1 -= ui.style.caret_width * 0.5;
		y1 -= ui.style.selection_padding_top;
		const float y2 = y1 + real_line_height * text_scale + ui.style.selection_padding_top + ui.style.selection_padding_bottom;
		const float x2 = x1 + ui.style.caret_width;
		
		scroll_into_view(x1, y1, x2, y2, padding_x);
	}
	
	/// Scrolls the given rectangle into view. padding_x controls approximately how many extra characters
	/// will be scrolled when the caret is not in view horizontally
	void scroll_into_view(const float x1, const float y1, const float x2, const float y2, const int padding_x=0)
	{
		const float view_width  = _width  - padding_left - padding_right;
		const float view_height = _height - padding_top - padding_bottom;
		
		if(_scroll_x + x1 < 0)
		{
			scroll_x = -x1 + max(0, padding_x) * font_metrics[0] * text_scale;
		}
		else if(_scroll_x + x2 > view_width)
		{
			scroll_x = view_width - x2 - max(0, padding_x) * font_metrics[0] * text_scale;
		}
		
		if(_scroll_y + y1 < 0)
		{
			scroll_y = -y1;
		}
		else if(_scroll_y + y2 > view_height)
		{
			scroll_y = view_height - y2;
		}
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Helpers and various properties
	// ///////////////////////////////////////////////////////////////////
	
	int text_length
	{
		get const { return _text_length; }
	}
	
	int num_lines
	{
		get const { return _num_lines; }
	}
	
	/// The total width of the text
	float text_width
	{
		get const { return _text_width; }
	}
	
	/// The total height of the text
	float text_height
	{
		get const { return _text_height; }
	}
	
	/// The height of a line of text.
	float line_height
	{
		get const { return real_line_height * text_scale; }
	}
	
	/// Finds the index of the nearest word boundary from start_index in direction dir.
	int find_word_boundary(const int start_index, int dir)
	{
		dir = dir >= 0 ? 1 : -1;
		const int end = dir == 1 ? _text_length : -1;
		int chr_index = dir == 1 ? start_index : max_int(0, start_index - 1);
		// 0 = whitespace
		// 1 = punctuation
		// 2 = alphanumeric
		// 3 = newline
		int chr_type = -1;
		bool found_whitespace = false;
		
		while(chr_index != end)
		{
			const int chr = _text[chr_index];
			const int new_chr_type = (chr == 10 || chr == 13) ? 3 : string::is_whitespace(chr) ? 0 : string::is_punctuation(chr) ? 1 : string::is_alphanumeric(chr) ? 2 : -1;
			
			if(chr_type == -1)
			{
				chr_type = new_chr_type;
				chr_index += dir;
				continue;
			}
			
			if(new_chr_type == 3)
				break;
			
			if(new_chr_type == 0)
			{
				found_whitespace = true;
			}
			
			if(new_chr_type != chr_type)
			{
				if(chr_type != 0 && new_chr_type != 0 || new_chr_type != 0 && found_whitespace)
				{
					break;
				}
				
				chr_type = new_chr_type;
			}
			
			chr_index += dir;
		}
		
		if(dir == -1)
		{
			chr_index++;
		}
		
		return chr_index;
	}
	
	/// Finds the next boundary starting from index in direction
	int expand_to_boundary(const int start_index, int dir)
	{
		if(start_index <= 0)
			return 0;
		if(start_index >= _text_length)
			return _text_length;
		
		dir = dir >= 0 ? 1 : -1;
		const int end = dir == 1 ? _text_length : -1;
		int chr_index = start_index;
		int chr = _text[chr_index];
		// 0 = whitespace
		// 1 = punctuation
		// 2 = alphanumeric
		// 3 = newline
		int chr_type = (chr == 10 || chr == 13) ? 3 : string::is_whitespace(chr) ? 0 : string::is_punctuation(chr) ? 1 : string::is_alphanumeric(chr) ? 2 : -1;
		
		if(chr_type == 3)
		{
			const int line_index = get_line_at_index(start_index);
			const int line_start_index = get_line_start(line_index);
			const int line_end_index = get_line_end(line_index);
			
			// if we're at teh end of the line, instead expand based on the last non newline character on this line
			if(line_start_index != line_end_index)
			{
				chr_index = max_int(chr_index - 1, 0);
				chr = _text[chr_index];
				chr_type = (chr == 10 || chr == 13) ? 3 : string::is_whitespace(chr) ? 0 : string::is_punctuation(chr) ? 1 : string::is_alphanumeric(chr) ? 2 : -1;
			}
			else
			{
				return start_index;
			}
		}
		
		while(chr_index != end)
		{
			chr = _text[chr_index];
			const int new_chr_type = (chr == 10 || chr == 13) ? 3 : string::is_whitespace(chr) ? 0 : string::is_punctuation(chr) ? 1 : string::is_alphanumeric(chr) ? 2 : -1;
			
			if(new_chr_type != chr_type || new_chr_type == 3)
				break;
			
			chr_index += dir;
		}
		
		if(dir == -1)
		{
			chr_index++;
		}
		
		return chr_index;
	}
	
	/// Returns the line at the specified character index. Will return -1 if line_index is not valid.
	int get_line_at_index(int index)
	{
		index = clamp(index, 0, _text_length);
		int line_start_index = 0;
		int line_index = -1;
		
		for(int i = 0; i < _num_lines; i++)
		{
			const int line_end_index = line_end_indices[i];
			
			if(index >= line_start_index && index <= line_end_index)
			{
				line_index = i;
				break;
			}
			
			line_start_index = line_end_index + 1;
		}
		
		return line_index;
	}
	
	/// Returns the relative position within this TextBox from the given global position,
	/// taking the current scroll position into account.
	void get_relative_xy(const float global_x, const float global_y, float &out x, float &out y)
	{
		x = global_x - x1 - padding_left - _scroll_x;
		y = global_y - y1 - padding_top - _scroll_y;
	}
	
	/// Returns the closest line index at the given y value.
	/// If closest_boundary is false the closest index of the chracter at x,y is returned,
	/// otherwise the index/boundary boundary between two characters is returned.
	/// If relative is false, x and y are considered global coordinates.
	int get_index_at(float x, float y, const bool closest_boundary=false, const bool relative=true)
	{
		if(!relative)
		{
			get_relative_xy(x, y, x, y);
		}
		
		const int line_index = get_line_at_y(y);
		const int line_start = get_line_start(line_index);
		const int line_end   = get_line_end(line_index);
		
		if(x <= 0)
			return line_start;
		
		int index = line_start;
		float width = 0;
		
		while(index < line_end)
		{
			const int chr = int(_text[index]);
			const float chr_width = chr <= last_char_index && chr >= first_char_index
				? font_metrics[chr - first_char_index] * text_scale
				: 0;
			
			if(x >= width && x <= width + chr_width)
			{
				if(closest_boundary && chr_width > 0 && x >= width + chr_width * 0.5)
				{
					index++;
				}
				
				break;
			}
			
			width += chr_width;
			index++;
		}
		
		return clamp(index, 0, _text_length);
	}
	
	/// Returns the closest line index at the given y value.
	/// If relative is false, y is considered a global coordinate.
	int get_line_at_y(float y, const bool relative=true)
	{
		if(!relative)
		{
			y = y - y1 - padding_top - _scroll_y;
		}
		
		return clamp(floor_int((y + _line_spacing * 0.25) / (real_line_height * text_scale + _line_spacing)), 0, _num_lines - 1);
	}
	
	/// Returns the starting character index of the given line.
	/// Will return -1 if line_index is not valid.
	int get_line_start(const int line_index)
	{
		if(line_index < 0 || line_index >= _num_lines)
			return -1;
		
		if(line_index == 0)
			return 0;
		
		return line_end_indices[line_index - 1] + 1;
	}
	
	/// Returns the ending character index of the given line.
	/// Will return -1 if line_index is not valid.
	int get_line_end(const int line_index)
	{
		if(line_index < 0 || line_index >= _num_lines)
			return -1;
		
		return line_end_indices[line_index];
	}
	
	/// Returns the character index relative to the line containing it.
	/// Returns -1 if index is not valid
	int get_index_in_line(const int index)
	{
		const int line_index = get_line_at_index(index);
		
		if(line_index == -1)
			return -1;
		
		return index - get_line_start(line_index);
	}
	
	/// Calculates the x and y position at the given index
	void get_index_xy(const int index, float &out x, float &out y)
	{
		if(_num_lines == 0 || index < 0)
			return;
		
		const int line_index = index >= _text_length
			? _num_lines - 1 : get_line_at_index(index);
		
		const int line_start = get_line_start(line_index);
		const int line_end   = min_int(index, get_line_end(line_index));
		
		float width = 0;
		
		for(int chr_index = line_start; chr_index < line_end; chr_index++)
		{
			const int chr = int(_text[chr_index]);
			
			width += chr <= last_char_index && chr >= first_char_index
				? font_metrics[chr - first_char_index] * text_scale
				: 0;
		}
		
		x = width;
		y = (real_line_height * text_scale + _line_spacing) * line_index;
	}
	
	/// Returns the x position of the given index.
	float get_index_x(const int index)
	{
		if(_num_lines == 0 || index < 0)
			return 0;
		
		const int line_index = index >= _text_length
			? _num_lines - 1 : get_line_at_index(index);
		
		const int line_start = get_line_start(line_index);
		const int line_end   = min_int(index, get_line_end(line_index));
		
		float width = 0;
		
		for(int chr_index = line_start; chr_index < line_end; chr_index++)
		{
			const int chr = int(_text[chr_index]);
			
			width += chr <= last_char_index && chr >= first_char_index
				? font_metrics[chr - first_char_index] * text_scale
				: 0;
		}
		
		return width;
	}
	
	/// Returns the relative y position of the given line index.
	float get_line_y(const int line_index)
	{
		if(line_index < 0)
			return 0;
		
		if(line_index >= _num_lines)
			return _text_height;
		
		return (real_line_height * text_scale + _line_spacing) * line_index;
	}
	
	// ///////////////////////////////////////////////////////////////////
	// INavigable
	// ///////////////////////////////////////////////////////////////////
	
	/// Internal - don't set explicitly.
	NavigationGroup@ navigation_parent
	{
		get { return @_navigation_parent; }
		set { @_navigation_parent = @value; }
	}
	
	NavigateOn navigate_on
	{
		get const { return navigation::get(_navigate_on, _navigation_parent); }
		set { _navigate_on = value; }
	}
	
	INavigable@ previous_navigable(INavigable@ from)
	{
		return @_navigation_parent != null ? _navigation_parent.previous_navigable(@this) : null;
	}
	
	INavigable@ next_navigable(INavigable@ from)
	{
		return @_navigation_parent != null ? _navigation_parent.next_navigable(@this) : null;
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Internal
	// ///////////////////////////////////////////////////////////////////
	
	bool ui_step() override
	{
		step_registered = false;
		
		if(drag_scroll)
		{
			if(ui.mouse.secondary_down)
			{
				scroll_x = drag_scroll_start_x + ui.mouse.x - drag_mouse_x_start;
				scroll_y = drag_scroll_start_y + ui.mouse.y - drag_mouse_y_start;
				step_registered = true;
			}
			else
			{
				drag_scroll = false;
			}
		}
		
		if(drag_selection)
		{
			if(ui.mouse.moved || scrolled)
			{
				if(double_click_start_index == -1)
				{
					do_drag_selection();
				}
				else
				{
					do_boundary_drag_selection();
				}
				
				scrolled = false;
			}
			
			step_registered = true;
		}
		
		return step_registered;
	}
	
	void _do_layout(LayoutContext@ ctx) override
	{
		padding_left	= ui.style.spacing;
		padding_right	= ui.style.spacing + 1;
		padding_top		= ui.style.spacing + ui.style.selection_padding_top;
		padding_bottom	= ui.style.spacing + ui.style.selection_padding_bottom + 1;
		text_scale = ui.style.default_text_scale;
		
		visible_lines.resize(0);
		visible_lines_offset.resize(0);
		visible_lines_selection.resize(0);
		num_visible_lines = 0;
		caret_line_index = -1;
		scrolled = false;
		
		float y = 0;
		int line_start_index = 0;
		const float available_width  = _width  - padding_left - padding_right;
		const float available_height = _height - padding_top - padding_bottom;
		
		const float scroll_x = this._scroll_x;
		const float scroll_y = this._scroll_y;
		const float text_scale = this.text_scale;
		const float line_height = real_line_height * text_scale;
		
		float line_width;
		
		////////////////////////////////////////////////////////////
		// Step 1. Calculate the total text width and height
		
		_text_width = 0;
		_text_height = 0;
		
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
			
			if(line_width > _text_width)
			{
				_text_width = line_width;
			}
			
			if(i > 0)
			{
				_text_height += _line_spacing;
			}
			
			_text_height += line_height;
			line_start_index = line_end_index + 1;
		}
		
		scroll_max_x = max(0, _text_width  - available_width);
		scroll_max_y = max(0, _text_height - available_height);
		
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
		
		for(int line_index = first_visible_line; line_index < _num_lines; line_index++)
		{
			if(y + line_height > available_height)
				break;
			
			const int current_line_start_index = line_start_index;
			const int line_end_index = line_end_indices[line_index];
			
			// This will include the newline, so for all lines execpt the last, this will always be >= 1
			string line_text = _text.substr(line_start_index, line_end_index - line_start_index + 1);
			const int line_length = int(line_text.length());
			
			// Move to the next line - these values aren't used below so they can be set here
			y += line_height + _line_spacing;
			line_start_index = line_end_index + 1;
			
			/////////////////////////////////////////////////////////////
			// The last line is empty 
			if(line_length == 0)
			{
				visible_lines.insertLast('');
				visible_lines_offset.insertLast(0);
				num_visible_lines++;
				
				if(
					scroll_x >= 0 &&
					current_line_start_index >= _selection_start && current_line_start_index <= _selection_end ||
					current_line_start_index >= _selection_end   && current_line_start_index <= _selection_start
				)
				{
					visible_lines_selection.insertLast(0);
					visible_lines_selection.insertLast(0);
					
					if(current_line_start_index == _selection_end)
					{
						caret_line_index = line_index;
						caret_line_x = scroll_x;
					}
				}
				else
				{
					visible_lines_selection.insertLast(-1);
					visible_lines_selection.insertLast(-1);
				}
				
				continue;
			}
			/////////////////////////////////////////////////////////////
			
			int start_index = 0;
			int end_index = 0;
			float line_offset = 0;
			
			float x = scroll_x;
			line_width = 0;
			
			int chr_index = current_line_start_index;
			float line_selection_start = NAN;
			float line_selection_end   = NAN;
			
			/////////////////////////////////////////////////////////////
			// Loop through each character in this line to find the first and last visible character.
			// Also find the visible selection bounds for drawing
			for(int j = 0; j < line_length; j++)
			{
				const int chr = int(line_text[j]);
				const float chr_width = chr <= last_char_index && chr >= first_char_index
					? font_metrics[chr - first_char_index] * text_scale
					: 0;
				
				/////////////////////////////////////////
				// Check the caret position
				if(focused)
				{
					if(chr_index == _selection_end)
					{
						caret_line_index = line_index;
						caret_line_x = x + line_width;
					}
					// Special case for the when the caret is at the very end of the text
					else if(_selection_end == _text_length && line_index == _num_lines - 1 && chr_index == _text_length - 1)
					{
						caret_line_index = line_index;
						caret_line_x = x + line_width + chr_width;
					}
				}
				
				line_width += chr_width;
				
				/////////////////////////////////////////////////////
				// Check the selection bounds for this line
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
					if(
						chr_index == current_line_start_index + line_length - 1 &&
						(chr_index + 1 <= _selection_start || chr_index + 1 <= _selection_end) &&
						line_index < _num_lines - 1
					)
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
			// End for - character loop
			////////////////////////////////////////////
			
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
		const uint border_clr = style.get_interactive_element_border_colour(hovered, focused, focused, disabled);
		
		const uint bg_clr = style.get_interactive_element_background_colour(false, false, false, disabled, true);
		const float inset = border_clr != 0 ? max(0, style.border_size) : 0;
		
		// Fill
		style.draw_rectangle(
			x1 + inset, y1 + inset,
			x2 - inset, y2 - inset,
			0, bg_clr);
		
		// Border
		const float border_size = focused ? style.selected_border_size : style.border_size;
		
		if(border_clr != 0 && border_size > 0)
		{
			style.outline(
				x1, y1,
				x2, y2,
				border_size, border_clr);
		}
		
		float text_scale = this.text_scale;
		const float line_height = real_line_height * text_scale;
		float x = x1 + padding_left + _scroll_x;
		float y = y1 + padding_top + _scroll_y + first_visible_line * (line_height + _line_spacing);
		int selection_index = 0;
		const bool draw_selection = _selection_start != _selection_end;
		
		// TODO: Take this into account when in all util methods that conver between position <--> line/index
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
				float selection_end         = visible_lines_selection[selection_index++];
				
				if(selection_start >= 0)
				{
					style.draw_rectangle(
						x + selection_start,
						y - style.selection_padding_top,
						x + selection_end,
						y + line_height + style.selection_padding_bottom,
						0, focused ? style.secondary_bg_clr : multiply_alpha(style.secondary_bg_clr, 0.5));
				}
			}
			
			if(text != '')
			{
				// Draw in chunks of 64 characters since that seems to be the limit for that textfield can draw
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
		
		if(persist_caret_time > 0)
		{
			persist_caret_time--;
		}
		
		if(
			focused &&
			caret_line_index != -1 && caret_line_x >= 0 &&
			((ui._frame % ui.style.caret_blink_rate) > ui.style.caret_blink_rate / 2 || persist_caret_time > 0)
		)
		{
			y  = y1 + padding_top + _scroll_y + caret_line_index * (line_height + _line_spacing);
			x += caret_line_x - _scroll_x;
			
			style.draw_rectangle(
				x - style.caret_width * 0.5,
				y - style.selection_padding_top,
				x + style.caret_width * 0.5,
				y + line_height + style.selection_padding_bottom,
				0, style.selected_highlight_border_clr);
		}
	}
	
	protected void update_line_endings()
	{
		int line_start_index = 0;
		_num_lines = 0;
		line_end_indices.resize(0);
		
		_text_length = int(_text.length());
		
		// TODO: Remove line breaks, or just ignore other lines, when multi_line is false
		
		_selection_start = min_int(_selection_start, _text_length);
		_selection_end   = min_int(_selection_end,   _text_length);
		_selection_start_line_index = 0;
		_selection_end_line_index   = 0;
		
		bool reached_end = false;
		
		while(!reached_end)
		{
			_num_lines++;
			int line_end_index = _text.findFirstOf('\n\r', line_start_index);
			
			if(line_end_index == -1)
			{
				line_end_index = int(_text.length());
				reached_end = true;
			}
			
			if(_selection_start >= line_start_index && _selection_start <= line_end_index)
			{
				_selection_start_line_index = _num_lines - 1;
			}
			
			if(_selection_end >= line_start_index && _selection_end <= line_end_index)
			{
				_selection_end_line_index = _num_lines - 1;
			}
			
			line_end_indices.insertLast(line_end_index);
			line_start_index = line_end_index + 1;
		}
		
		validate_layout = true;
	}
	
	protected void update_selection_line_indices()
	{
		int line_start_index = 0;
		_selection_start_line_index = 0;
		_selection_end_line_index = 0;
		
		for(int i = 0; i < _num_lines; i++)
		{
			const int line_end_index = line_end_indices[i];
			
			if(_selection_start >= line_start_index && _selection_start <= line_end_index)
			{
				_selection_start_line_index = i;
			}
			
			if(_selection_end >= line_start_index && _selection_end <= line_end_index)
			{
				_selection_end_line_index = i;
			}
			
			line_start_index = line_end_index + 1;
		}
	}
	
	protected void update_relative_line_index()
	{
		if(_line_relative_caret_index == -1)
		{
			_line_relative_caret_index = _selection_end - get_line_start(_selection_end_line_index);
		}
	}
	
	protected float clamp_scroll(const float scroll, const float max)
	{
		if(scroll < -max)
			return -max;
		
		if(scroll > 0)
			return 0;
		
		return scroll;
	}
	
	protected void do_drag_selection(const bool extend_selection=true)
	{
		const int index = get_index_at(ui.mouse.x, ui.mouse.y, true, false);
		
		if(extend_selection)
		{
			selection_end = index;
		}
		else
		{
			caret_index = index;
		}
		
		if(validate_layout)
		{
			update_selection_line_indices();
			_line_relative_caret_index = -1;
			this.scroll_to_caret(0);
		}
		
		persist_caret();
	}
	
	protected void start_boundary_drag_selection()
	{
		const int caret_index = get_index_at(ui.mouse.x, ui.mouse.y, false, false);
		double_click_start_index = expand_to_boundary(caret_index, -1);
		double_click_end_index   = expand_to_boundary(caret_index,  1);
		selection_start = double_click_start_index;
		selection_end = double_click_end_index;
		
		if(validate_layout)
		{
			update_selection_line_indices();
			_line_relative_caret_index = -1;
			this.scroll_to_caret(0);
		}
		
		drag_selection = true;
		persist_caret();
	}
	
	protected void do_boundary_drag_selection()
	{
		const int caret_index = get_index_at(ui.mouse.x, ui.mouse.y, false, false);
		
		if(caret_index > double_click_start_index)
		{
			selection_start = double_click_start_index;
			selection_end = expand_to_boundary(caret_index,  1);
		}
		else
		{
			selection_start = double_click_end_index;
			selection_end = expand_to_boundary(caret_index,  -1);
		}
		
		if(validate_layout)
		{
			update_selection_line_indices();
			_line_relative_caret_index = -1;
			this.scroll_to_caret(0);
		}
		
		persist_caret();
	}
	
	protected void persist_caret()
	{
		persist_caret_time = ui.style.caret_blink_rate;
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////////////////////////////
	
	void _mouse_press(const MouseButton button)
	{
		@ui.focus = @this;
		
		if(button == ui.primary_button)
		{
			if(ui.mouse.primary_double_click)
			{
				start_boundary_drag_selection();
			}
			else
			{
				drag_selection = true;
				do_drag_selection(ui._has_editor && ui._editor.key_check_gvb(GVB::Shift));
			}
			
			step_registered = ui._step_subscribe(this, step_registered);
		}
		else if(_drag_scroll && button == ui.secondary_button)
		{
			drag_mouse_x_start = ui.mouse.x;
			drag_mouse_y_start = ui.mouse.y;
			drag_scroll_start_x = _scroll_x;
			drag_scroll_start_y = _scroll_y;
			drag_scroll = true;
			
			step_registered = ui._step_subscribe(this, step_registered);
		}
	}
	
	void _mouse_release(const MouseButton button)
	{
		if(button == ui.primary_button)
		{
			drag_scroll = false;
		}
		
		if(button != ui.primary_button)
			return;
		
		drag_selection = false;
		double_click_start_index = -1;
		double_click_end_index = -1;
	}
	
	void _mouse_scroll(const int scroll_dir)
	{
		scroll_y -= scroll_dir * (real_line_height * text_scale + _line_spacing);
		scrolled = true;
	}
	
	void on_focus(Keyboard@ keyboard) override
	{
		focused = true;
		keyboard.update_modifiers = true;
		keyboard.register_arrows_gvb();
		keyboard.register_vk(VK::End, VK::Home);
		keyboard.register_vk(VK::Digit0, VK::Z);
		keyboard.register_vk(VK::Oem1, VK::Oem7);
		keyboard.register_vk(VK::Numpad0, VK::Divide);
		keyboard.register_vk(VK::Back);
		keyboard.register_vk(VK::Delete);
	}
	
	void on_blur(Keyboard@ keyboard, const BlurAction type) override
	{
		focused = false;
		// TODO: accept on unknown, or cancel if _accept_on_blur is false
		// TODO: accept on accept
		// TODO: reset text on cancel
	}
	
	void on_key_press(Keyboard@ keyboard, const int key, const bool is_gvb)
	{
		if(!is_gvb && key == VK::A && keyboard.ctrl)
		{
			select_all();
			return;
		}
		
		on_key(keyboard, key, is_gvb);
	}
	
	void on_key(Keyboard@ keyboard, const int key, const bool is_gvb)
	{
		if(is_gvb)
		{
			switch(key)
			{
				case GVB::LeftArrow:
					move_caret_left(ui.keyboard.ctrl, ui.keyboard.shift, true);
					break;
				case GVB::RightArrow:
					move_caret_right(ui.keyboard.ctrl, ui.keyboard.shift, true);
					break;
				case GVB::UpArrow:
					move_caret_up(ui.keyboard.shift, true);
					break;
				case GVB::DownArrow:
					move_caret_down(ui.keyboard.shift, true);
					break;
			}
		}
		else
		{
			switch(key)
			{
				case VK::Home:
					move_caret_home(ui.keyboard.ctrl, ui.keyboard.shift, true);
					break;
				case VK::End:
					move_caret_end(ui.keyboard.ctrl, ui.keyboard.shift, true);
					break;
			}
		}
	}
	
	void on_key_release(Keyboard@ keyboard, const int key, const bool is_gvb) { }
	
}