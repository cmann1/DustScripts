#include '../popups/PopupOptions.cpp';
#include '../events/Event.cpp';
#include 'LockedContainer.cpp';
#include 'ListView.cpp';
#include 'shapes/Arrow.cpp';

class Select : LockedContainer
{
	
	float button_size = 16;
	
	Event change;
	
	protected Image@ _icon;
	protected Label@ _label;
	protected Arrow@ _arrow;
	protected ListView@ list_view;
	protected PopupOptions@ popup;
	
	protected bool open;
	
	protected array<string> _values;
	protected array<string> _text;
	protected int _num_values;
	protected array<string> icons;
	protected array<float> icon_sizes;
	protected int _selected_index = -1;
	
	protected string _placeholder_text;
	protected string _placeholder_icon_set;
	protected string _placeholder_icon_name;
	protected float _placeholder_icon_width;
	protected float _placeholder_icon_height;
	protected float _placeholder_icon_offset_x;
	protected float _placeholder_icon_offset_y;
	protected int _num_icons;
	protected bool _show_icons;
	
	Select(UI@ ui, const string placeholder_text='',
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
		
		@_arrow = Arrow(ui);
		_arrow.rotation = 90;
		Container::add_child(_arrow);
		
		children_mouse_enabled = false;
		_set_width  = _width  = 120;
		_set_height = _height = 30;
		
		@list_view = ListView(ui);
		list_view.border_size = 0;
		list_view.border_colour = 0;
		list_view.background_colour = 0;
		list_view.select.on(EventCallback(on_list_view_select));
		
		@popup = PopupOptions(ui, list_view, true, PopupPosition::Below, PopupTriggerType::Manual, PopupHideType::MouseDownOutside, false);
		popup.allow_target_overlap = false;
		popup.spacing = 2;
		popup.padding = 0;
		popup.hide.on(EventCallback(on_popup_hide));
		
		_show_icons = placeholder_icon_set != '' || placeholder_icon_name != '';
		set_placeholder_text(placeholder_text);
		set_placeholder_icon(placeholder_icon_set, placeholder_icon_name, placeholder_icon_width, placeholder_icon_height, placeholder_icon_offset_x, placeholder_icon_offset_y);
	}
	
	string element_type { get const override { return 'Select'; } }
	
	void set_placeholder_text(const string text='')
	{
		if(_placeholder_text == text)
			return;
		
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
	
	int num_values { get const { return _num_values; } }
	
	int selected_index
	{
		get const { return _selected_index; }
		set
		{
			if(value < 0)
				value = 0;
			else if(value > _num_values - 1)
				value = _num_values - 1;
			
			if(_selected_index == value)
				return;
			
			_selected_index = value;
			
			update_label();
			update_icon();
			dispatch_change_event();
		}
	}
	
	string selected_value
	{
		get const { return _selected_index == -1 ? '' : _values[_selected_index] ; }
		set
		{
			selected_index = _values.find(value);
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
			dispatch_change_event();
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
	
	void get_icon(int index, string &out set, string &out name, float &out width, float &out height, float &out offset_x, float &out offset_y)
	{
		if(index < 0 || index >= _num_icons)
			return;
		
		index *= 2;
		set  = icons[index];
		name = icons[index + 1];
		index *= 2;
		width    = icon_sizes[index];
		height   = icon_sizes[index + 1];
		offset_x = icon_sizes[index + 2];
		offset_y = icon_sizes[index + 3];
	}
	
	void set_icon(int index, const string set, const string name, const float width, const float height, const float offset_x=0, const float offset_y=0)
	{
		if(index < 0 || index >= _num_icons)
			return;
		
		show_icons = true;
		
		index *= 2;
		icons[index]     = set;
		icons[index + 1] = name;
		index *= 2;
		icon_sizes[index]		= width;
		icon_sizes[index + 1]	= height;
		icon_sizes[index + 2]	= offset_x;
		icon_sizes[index + 3]	= offset_y;
		
		ListViewItem@ item = list_view.get_item(index < 0 || index >= _num_values ? _num_values - 1 : index);
		
		if(@item != null)
		{
			item.set(set, name, width, width, offset_x, offset_y);
		}
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
		
		list_view.add_item(value, text);
		
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
		
		ListViewItem@ item = list_view.get_item(index < 0 || index >= _num_values ? _num_values - 1 : index);
		
		if(@item != null)
		{
			item.set(icon_set, icon_name, text, icon_width, icon_width, icon_offset_x, icon_offset_y);
		}
		
		_num_icons++;
	}
	
	void remove_value(int index)
	{
		if(index < 0 || index >= _num_values)
			return;
		
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
		
		list_view.remove_item(index);
		
		_num_values--;
	}
	
	void remove_value(const string value)
	{
		remove_value(_values.find(value));
	}
	
	void set_value_index(int index, int new_index)
	{
		if(index < 0 || index >= _num_values)
			return;
		
		if(new_index < 0)
			new_index = 0;
		else if(new_index > _num_values)
			new_index = _num_values;
		
		if(index == new_index || index == _num_values - 1 && new_index >= _num_values - 1)
			return;
		
		list_view.set_item_index(index, new_index);
		
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
		
		list_view.clear();
	}
	
	//
	
	void _do_layout(LayoutContext@ ctx) override
	{
		const float border_size = max(0, ui.style.border_size);
		
		_label._x = border_size;
		_label._y = border_size;
		_label._width = _width - button_size - border_size * 2;
		_label._height = _height - border_size * 2;
		
		if(_show_icons && _icon.visible)
		{
			_icon._x = border_size;
			_icon._y = border_size;
			_icon._width = min(_height, _width) - border_size * 2;
			_icon._height = _icon._width;
			
			_label._x += _icon._width;
			_label._width -= _icon._width;
		}
		
		_arrow._x = _width - border_size - button_size;
		_arrow._y = border_size;
		_arrow._width  = button_size;
		_arrow._height = _label._height;
	}
	
	void _draw(Style@ style, DrawingContext@ ctx)
	{
		style.draw_interactive_element(this,
			hovered || pressed,
			open,
			pressed, disabled);
	}
	
	protected void update_label()
	{
		_label.text = _selected_index == -1 ? _placeholder_text : _text[_selected_index];
	}
	
	protected void update_icon()
	{
		if(!_show_icons)
		{
			if(@_icon != null)
			{
				_icon.visible = false;
			}
			
			return;
		}
		
		const bool no_icon = _selected_index == -1 || !_show_icons;
		
		const string set  = no_icon ? _placeholder_icon_set  : icons[_selected_index * 2];
		const string name = no_icon ? _placeholder_icon_name : icons[_selected_index * 2 + 1];
		
		if(set == '' || name == '')
		{
			if(@_icon != null)
			{
				_icon.visible = false;
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
	}
	
	protected void dispatch_change_event()
	{
		ui._event_info.reset(EventType::CHANGE, this);
		change.dispatch(ui._event_info);
	}
	
	// Events
	
	void _mouse_press(const MouseButton button)
	{
		if(button != ui.primary_button)
			return;
		
		list_view.fit_to_contents(true);
		
		if(selected_index == -1)
		{
			list_view.select_none();
			list_view.scroll_y = 0;
		}
		else
		{
			ListViewItem@ selected_item = list_view.get_item(selected_index);
			
			if(@selected_item != null)
			{
				selected_item.selected = true;
				@list_view.content.scroll_into_view = selected_item;
			}
		}
		
		if(@tooltip != null)
		{
			tooltip.enabled = false;
		}
		
		open = true;
		ui.show_tooltip(popup, this, true);
	}
	
	protected void on_list_view_select(EventInfo@ event)
	{
		if(!open)
			return;
		
		if(list_view.num_selected_items > 0)
		{
			selected_index = list_view.selected_index;
		}
		
		if(@tooltip != null)
		{
			tooltip.enabled = true;
		}
		
		ui.hide_tooltip(popup);
		open = false;
	}
	
	protected void on_popup_hide(EventInfo@ event)
	{
		on_list_view_select(event);
	}
	
}