#include 'ListView.cpp';
#include 'Image.cpp';
#include 'Label.cpp';
#include '../TextAlign.cpp';

class ListViewItem : Container
{
	
	string value;
	
	ListView@ _list_view;
	
	protected bool _selected;
	protected Image@ _icon;
	protected Label@ _label;
	protected bool _has_custom_content;
	
	string element_type { get const override { return 'ListViewItem'; } }
	
	ListViewItem(UI@ ui, const string value)
	{
		super(@ui);
		init(value);
	}
	
	ListViewItem(UI@ ui, const string value, Element@ content)
	{
		super(@ui);
		
		set(@content);
		init(value);
	}
	
	ListViewItem(UI@ ui, const string value, const string text, const TextAlign text_align_h=TextAlign::Left)
	{
		super(@ui);
		
		set(text, text_align_h);
		init(value);
	}
	
	ListViewItem(UI@ ui, const string value, const string sprite_set, const string sprite_name, const float width=-1, const float height=-1, const float offset_x=0, const float offset_y=0)
	{
		super(@ui);
		
		set(sprite_set, sprite_name, width, height, offset_x, offset_y);
		init(value);
	}
	
	ListViewItem(UI@ ui, const string value,
		const string sprite_set, const string sprite_name,
		const string text,
		const float width=-1, const float height=-1, const float offset_x=0, const float offset_y=0,
		const TextAlign text_align_h=TextAlign::Left)
	{
		super(@ui);
		
		set(sprite_set, sprite_name, text, width, height, offset_x, offset_y, text_align_h);
		init(value);
	}
	
	private void init(const string value)
	{
		this.value = value;
		children_mouse_enabled = false;
	}
	
	private void calculate_size()
	{
		if(_has_custom_content)
		{
			fit_to_contents();
		}
		else if(@_icon != null || @_label != null)
		{
			_width = 0;
			
			if(@_icon != null)
				_width += _icon._set_width;
			if(@_label != null)
				_width += _label._set_width;
		}
		else
		{
			_width = _set_width = ui.style.default_list_view_item_width;
		}
		
		_set_width = _width;
		_height = _set_height = ui.style.default_list_view_item_height;
	}
	
	void set(Element@ content)
	{
		if(!_has_custom_content)
		{
			if(@_icon != null)
			{
				Container::remove_child(_icon);
			}
			
			if(@_label != null)
			{
				Container::remove_child(_label);
			}
		}
		
		clear_custom_content();
		
		if(@content != null)
		{
			Container::add_child(content);
			_has_custom_content = true;
			fit_to_contents();
		}
		else
		{
			_height = _set_height = ui.style.default_list_view_item_height;
		}
	}
	
	void set(const string text, const TextAlign text_align_h=TextAlign::Left)
	{
		clear_custom_content();
		
		if(@_label == null)
		{
			@_label = Label(ui, text);
			_label.padding = NAN;
			_label.align_h = GraphicAlign::Left;
			_label.align_v = GraphicAlign::Middle;
			_label.sizing = ImageSize::ConstrainInside;
			Container::add_child(_label);
		}
		
		_label.text_align_h = text_align_h;
		_label.text = text;
		_label.fit_to_contents();
		
		calculate_size();
	}
	
	void set(const string sprite_set, const string sprite_name, const float width=-1, const float height=-1, const float offset_x=0, const float offset_y=0)
	{
		clear_custom_content();
		
		if(@_icon == null)
		{
			@_icon = Image(ui, sprite_set, sprite_name, width, height, offset_x, offset_y);
			_icon.width  = ui.style.default_list_view_item_height;
			_icon.height = ui.style.default_list_view_item_height;
			Container::add_child(_icon, 0);
		}
		else
		{
			_icon.set_sprite(sprite_set, sprite_name, width, height, offset_x, offset_y);
		}
		
		calculate_size();
	}
	
	void set(
		const string sprite_set, const string sprite_name,
		const string text,
		const float width=-1, const float height=-1, const float offset_x=0, const float offset_y=0,
		const TextAlign text_align_h=TextAlign::Left)
	{
		clear_custom_content();
		
		set(text, text_align_h);
		set(sprite_set, sprite_name, width, height, offset_x, offset_y);
	}
	
	Image@ icon { get { return _icon; } }
	
	Label@ label { get { return _label; } }
	
	bool has_custom_content { get const { return _has_custom_content; } }
	
	bool selected
	{
		get const { return _selected; }
		set
		{
			if(_selected == value)
				return;
			
			if(@_list_view != null)
			{
				if(value)
					_list_view.select_item(this);
				else
					_list_view.deselect_item(this);
			}
			
			_selected = value;
		}
	}
	
	bool add_child(Element@ child, int index=-1) override
	{
		clear_icon_and_label();
		return Container::add_child(child);
	}
	
	bool remove_child(Element@ child) override
	{
		clear_icon_and_label();
		return Container::remove_child(child);
	}
	
	void _queue_children_for_layout(ElementStack@ stack) override
	{
		if(_has_custom_content)
		{
			Container::_queue_children_for_layout(@stack);
		}
		else
		{
			if(@_label != null)
			{
				stack.push(_label);
			}
			
			if(@_icon != null)
			{
				stack.push(_icon);
			}
		}
	}
	
	void _do_layout(LayoutContext@ ctx) override
	{
		if(_has_custom_content)
		{
			Container::_do_layout(@ctx);
		}
		else
		{
			if(@_icon != null)
			{
				_icon._x = 0;
				_icon._y = 0;
				_icon._width = min(_height, _width);
				_icon._height = _height;
				_icon.validate_layout =true;
			}
			
			if(@_label != null)
			{
				
				_label._x = @_icon != null ? _icon._width : 0;
				_label._y = 0;
				_label._width  = _width - _label._x;
				_label._height = _height;
				_label.validate_layout =true;
				
				_label.visible = (_label._width - _label.real_padding_left - _label.real_padding_right) > 0;
			}
		}
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		const bool pressed = @_list_view == null || !_list_view.drag_select ? this.pressed : false;
		
		if(hovered || _selected || pressed)
		{
			style.draw_interactive_element(
				x1, y1, x2, y2,
				hovered, _selected, pressed, disabled,
				true, true, true);
		}
	}
	
	//
	
	protected void clear_custom_content()
	{
		if(!_has_custom_content)
			return;
		
		clear();
		_has_custom_content = false;
	}
	
	protected void clear_icon_and_label()
	{
		if(_has_custom_content)
			return;
		
		if(@_label != null)
		{
			Container::remove_child(_label);
			@_label = null;
		}
		
		if(@_icon != null)
		{
			Container::remove_child(_icon);
			@_icon = null;
		}
		
		_has_custom_content = true;
	}
	
	// Events
	
	void _mouse_click() override
	{
		if(@_list_view == null || !_list_view.drag_select)
		{
			selected = !_selected;
		}
	}
	
}