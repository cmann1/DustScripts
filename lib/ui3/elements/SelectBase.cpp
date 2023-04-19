#include '../events/Event.cpp';
#include 'LockedContainer.cpp';

abstract class SelectBase : LockedContainer
{
	
	Event change;
	
	protected Image@ _icon;
	protected Label@ _label;
	
	protected array<string> _values;
	protected array<string> _text;
	protected int _num_values;
	protected array<string> icons;
	protected array<float> icon_sizes;
	protected int _selected_index = -1;
	protected bool custom_value_selected;
	protected string custom_value;
	
	protected string _placeholder_text;
	protected string _placeholder_icon_set;
	protected string _placeholder_icon_name;
	protected float _placeholder_icon_width;
	protected float _placeholder_icon_height;
	protected float _placeholder_icon_offset_x;
	protected float _placeholder_icon_offset_y;
	protected int _num_icons;
	protected bool _show_icons;
	protected bool _allow_custom_value;
	protected bool _allow_reselect;
	
	SelectBase(UI@ ui, const string placeholder_text='',
		const string placeholder_icon_set='', const string placeholder_icon_name='',
		const float placeholder_icon_width=-1, const float placeholder_icon_height=-1,
		const float placeholder_icon_offset_x=0, const float placeholder_icon_offset_y=0)
	{
		super(ui);
		
		@_label = Label(ui, placeholder_text);
		_label.padding = NAN;
		_label.align_h = GraphicAlign::Left;
		_label.align_v = GraphicAlign::Middle;
		_label.sizing = ImageSize::ConstrainInside;
		_label.fit_to_contents();
		Container::add_child(_label);
		
		children_mouse_enabled = false;
		_set_width  = _width  = 120;
		_set_height = _height = 34;
		
		_show_icons = placeholder_icon_set != '' || placeholder_icon_name != '';
		set_placeholder_text(placeholder_text);
		set_placeholder_icon(placeholder_icon_set, placeholder_icon_name, placeholder_icon_width, placeholder_icon_height, placeholder_icon_offset_x, placeholder_icon_offset_y);
	}
	
	void set_placeholder_text(const string text='')
	{
		if(_placeholder_text == text)
			return;
		
		_placeholder_text = text;
		
		if(_selected_index == -1)
		{
			_label.text = text;
		}
	}
	
	void set_placeholder_icon(const string set='', const string name='', const float width=-1, const float height=-1, const float offset_x=0, const float offset_y=0)
	{
		if(_placeholder_icon_set == set && _placeholder_icon_name == name)
			return;
		
		_placeholder_icon_set = set;
		_placeholder_icon_name = name;
		_placeholder_icon_width = width;
		_placeholder_icon_height = height;
		_placeholder_icon_offset_x = offset_x;
		_placeholder_icon_offset_y = offset_y;
		
		if(_selected_index == -1)
		{
			update_icon();
		}
	}
	
	bool show_icons
	{
		get const { return _show_icons; }
		set
		{
			if(_show_icons == value)
				return;
			
			_show_icons = value;
			
			if(_show_icons)
			{
				icons.resize(_num_values * 2);
				icon_sizes.resize(_num_values * 4);
				_num_icons = _num_values;
			}
			
			update_icon();
		}
	}
	
	/// Normally if a value that does not exist is selected the placeholder text will be displayed, selected_index returns -1,
	/// and selected_value returns an empty string.
	/// If this is set to true, any value can be set. select_index will still return -1, but selected_value will return the custom value instead.
	bool allow_custom_value
	{
		get const { return _allow_custom_value; }
		set
		{
			if(_allow_custom_value == value)
				return;
			
			_allow_custom_value = value;
			
			if(!value)
			{
				update_custom_value('', false);
			}
		}
	}
	
	/// If true will dispatch the change event, even if the same item is selected.
	bool allow_reselect
	{
		get const { return _allow_reselect; }
		set
		{
			if(_allow_reselect == value)
				return;
			
			_allow_reselect = value;
		}
	}
	
	int num_values { get const { return _num_values; } }
	
	int selected_index
	{
		get const { return _selected_index; }
		set
		{
			if(value > _num_values - 1)
				value = _num_values - 1;
			
			if(_selected_index == value && !(value == -1 && custom_value_selected))
			{
				if(_allow_reselect)
				{
					ui._dispatch_event(@change, EventType::CHANGE, this);
				}
				return;
			}
			
			_selected_index = value;
			custom_value = '';
			custom_value_selected = false;
			
			update_label();
			update_icon();
			ui._dispatch_event(@change, EventType::CHANGE, this);
			validate_layout = true;
		}
	}
	
	string selected_value
	{
		get const
		{
			if(allow_custom_value && custom_value_selected && _selected_index == -1)
			{
				return custom_value;
			}
			
			return _selected_index == -1 ? '' : _values[_selected_index];
		}
		set
		{
			const int index = _values.find(value);
			
			if(_allow_custom_value && index == -1)
			{
				update_custom_value(value);
				_selected_index = -1;
				update_label();
				update_icon();
				ui._dispatch_event(@change, EventType::CHANGE, this);
				validate_layout = true;
			}
			else
			{
				selected_index = index;
			}
		}
	}
	
	string get_value(const int index)
	{
		if(index < 0 || index >= _num_values)
			return '';
		
		return _values[index];
	}
	
	void set_value(const int index, const string value)
	{
		if(index < 0 || index >= _num_values)
			return;
		
		_values[index] = value;
		
		if(index == _selected_index)
		{
			ui._dispatch_event(@change, EventType::CHANGE, this);
			validate_layout = true;
		}
	}
	
	string get_text(const int index)
	{
		if(index < 0 || index >= _num_values)
			return '';
		
		return _text[index];
	}
	
	void set_text(const int index, const string text)
	{
		if(index < 0 || index >= _num_values)
			return;
		
		_text[index] = text;
		
		if(index == selected_index)
		{
			update_label();
		}
	}
	
	bool get_icon(int index, string &out set, string &out name, float &out width, float &out height, float &out offset_x, float &out offset_y)
	{
		if(index < 0 || index >= _num_icons)
			return false;
		
		index *= 2;
		set  = icons[index];
		name = icons[index + 1];
		index *= 2;
		width    = icon_sizes[index];
		height   = icon_sizes[index + 1];
		offset_x = icon_sizes[index + 2];
		offset_y = icon_sizes[index + 3];
		
		return true;
	}
	
	bool set_icon(int index, const string set, const string name, const float width, const float height, const float offset_x=0, const float offset_y=0)
	{
		if(index < 0 || index >= _num_icons)
			return false;
		
		show_icons = true;
		
		index *= 2;
		icons[index]     = set;
		icons[index + 1] = name;
		index *= 2;
		icon_sizes[index]		= width;
		icon_sizes[index + 1]	= height;
		icon_sizes[index + 2]	= offset_x;
		icon_sizes[index + 3]	= offset_y;
		
		return true;
	}
	
	void add_value(const string value, const int index=-1)
	{
		add_value(value, value, index);
	}
	
	void add_value(const string value, const string text, int index=-1)
	{
		if(index < 0 || index >= _num_values)
		{
			_values.insertLast(value);
			_text.insertLast(text);
		}
		else
		{
			_values.insertAt(index, value);
			_text.insertAt(index, text);
		}
		
		if(_show_icons)
		{
			_num_icons++;
			icons.resize(_num_icons * 2);
			icon_sizes.resize(_num_icons * 4);
		}
		
		_num_values++;
	}
	
	void add_value(const string value, const string text,
		const string icon_set, const string icon_name,
		const float icon_width=-1, const float icon_height=-1,
		const float icon_offset_x=0, const float icon_offset_y=0,
		int index=-1)
	{
		show_icons = true;
		
		add_value(value, text, index);
		
		index = (_num_values - 1) * 2;
		icons[index] = icon_set;
		icons[index + 1] = icon_name;
		index *= 2;
		icon_sizes[index] = icon_width;
		icon_sizes[index + 1] = icon_height;
		icon_sizes[index + 2] = icon_offset_x;
		icon_sizes[index + 3] = icon_offset_y;
		
		_num_icons++;
	}
	
	bool remove_value(int index)
	{
		if(index < 0 || index >= _num_values)
			return false;
		
		_values.removeAt(index);
		_text.removeAt(index);
		
		if(_show_icons)
		{
			index *= 2;
			icons.removeAt(index);
			icons.removeAt(index);
			index *= 2;
			icon_sizes.removeAt(index);
			icon_sizes.removeAt(index);
			icon_sizes.removeAt(index);
			icon_sizes.removeAt(index);
			_num_icons--;
		}
		
		_num_values--;
		return true;
	}
	
	bool remove_value(const string value)
	{
		return remove_value(_values.find(value));
	}
	
	bool set_value_index(int index, int new_index)
	{
		if(index < 0 || index >= _num_values)
			return false;
		
		if(new_index < 0)
			new_index = 0;
		else if(new_index > _num_values)
			new_index = _num_values;
		
		if(index == new_index || index == _num_values - 1 && new_index >= _num_values - 1)
			return false;
		
		if(new_index > index)
			new_index--;
		
		const string value = _values[index];
		const string text = _text[index];
		_values.removeAt(index);
		_text.removeAt(index);
		_values.insertAt(new_index, value);
		_text.insertAt(new_index, text);
		
		if(_show_icons)
		{
			index *= 2;
			new_index *= 2;
			const string set = icons[index];
			const string name = icons[index + 1];
			icons.removeAt(index);
			icons.removeAt(index);
			icons.insertAt(new_index, name);
			icons.insertAt(new_index, name);
			
			index *= 2;
			new_index *= 2;
			const float width = icon_sizes[index];
			const float height = icon_sizes[index + 1];
			const float offset_x = icon_sizes[index + 2];
			const float offset_y = icon_sizes[index + 3];
			icon_sizes.removeAt(index);
			icon_sizes.removeAt(index);
			icon_sizes.removeAt(index);
			icon_sizes.removeAt(index);
			icon_sizes.insertAt(new_index, offset_y);
			icon_sizes.insertAt(new_index, offset_x);
			icon_sizes.insertAt(new_index, height);
			icon_sizes.insertAt(new_index, width);
		}
		
		return true;
	}
	
	void clear()
	{
		_values.resize(0);
		_text.resize(0);
		icons.resize(0);
		icon_sizes.resize(0);
		
		_num_values = 0;
		_num_icons = 0;
		
		selected_index = -1;
	}
	
	protected void update_label()
	{
		if(_allow_custom_value && custom_value_selected)
		{
			_label.text = custom_value;
		}
		else
		{
			_label.text = _selected_index == -1 ? _placeholder_text : _text[_selected_index];
		}
		
		validate_layout = true;
	}
	
	protected void update_icon()
	{
		if(!_show_icons)
		{
			if(@_icon != null)
			{
				_icon.visible = false;
				validate_layout = true;
			}
			
			return;
		}
		
		const bool no_icon = _selected_index == -1 || !_show_icons || _allow_custom_value && custom_value_selected;
		
		const string set  = no_icon ? _placeholder_icon_set  : icons[_selected_index * 2];
		const string name = no_icon ? _placeholder_icon_name : icons[_selected_index * 2 + 1];
		
		if(set == '' || name == '')
		{
			if(@_icon != null)
			{
				_icon.visible = false;
				validate_layout = true;
			}
			
			return;
		}
		
		const float width  = no_icon ? _placeholder_icon_width    : icon_sizes[_selected_index * 4];
		const float height = no_icon ? _placeholder_icon_height   : icon_sizes[_selected_index * 4 + 1];
		const float x      = no_icon ? _placeholder_icon_offset_x : icon_sizes[_selected_index * 4 + 2];
		const float y      = no_icon ? _placeholder_icon_offset_y : icon_sizes[_selected_index * 4 + 3];
		
		if(@_icon == null)
		{
			@_icon = Image(ui, '', '', width, height, x, y);
			_icon.padding = NAN;
			Container::add_child(_icon, 0);
		}
		
		_icon.visible = true;
		_icon.set_sprite(set, name, width, height, x, y);
		
		validate_layout = true;
	}
	
	protected void update_custom_value(const string &in value, const bool has_value=true)
	{
		custom_value = value;
		custom_value_selected = has_value;
	}
	
}
