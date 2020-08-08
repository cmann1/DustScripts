#include 'Button.cpp';
#include 'Image.cpp';
#include 'Label.cpp';
#include 'MoveableDialog.cpp';
#include 'shapes/Cross.cpp';
#include '../layouts/flow/FlowLayout.cpp';

class Window : MoveableDialog
{
	
	protected string _title;
	protected Image@ _title_icon;
	protected Label@ _title_label;
	protected Container@ _title_before;
	protected Container@ _title_after;
	protected FlowLayout@ title_flow_layout;
	protected Divider@ _title_divider;
	protected bool _show_close_button;
	protected Button@ _close_button;
	protected Container@ _contents;
	protected Divider@ _buttons_divider;
	protected Container@ _buttons_left;
	protected Container@ _buttons_right;
	protected FlowLayout@ buttons_flow_layout;
	
	Window(UI@ ui, const string title, bool show_close_button=true, bool draggable=true)
	{
		super(ui);
		
		this.title = title;
		this.show_close_button = show_close_button;
		this.draggable = draggable;
		
		@_title_divider = Divider(ui, Orientation::Horizontal);
		Container::add_child(_title_divider);
		
		@_contents = Container(ui);
		Container::add_child(_title_divider);
		
		_width = _set_width = 200;
		_height = _set_height = 200;
	}
	
	string element_type { get const override { return 'Window'; } }
	
	bool get_icon(string &out set, string &out name, float &out width, float &out height, float &out offset_x, float &out offset_y)
	{
		if(@_title_icon == null)
			return false;
		
		set  = _title_icon._sprite_set;
		name = _title_icon._sprite_name;
		width    = _title_icon.graphic_width;
		height   = _title_icon.graphic_height;
		offset_x = _title_icon.graphic_offset_x;
		offset_y = _title_icon.graphic_offset_y;
		
		return true;
	}
	
	void set_icon( const string set, const string name, const float width, const float height, const float offset_x=0, const float offset_y=0)
	{
		if(@_title_icon == null)
		{
			@_title_icon = Image(ui, set, name, width, height, offset_x, offset_y);
			_title_icon.mouse_enabled = false;
			Container::add_child(_title_icon, 0);
		}
		
		_title_icon.set_sprite(set, name, width, height, offset_x, offset_y);
	}
	
	void remove_icon()
	{
		if(@_title_icon == null)
			return;
		
		_title_icon.visible = false;
	}
	
	string title
	{
		get const { return _title; }
		set
		{
			if(_title == value)
				return;
			
			_title = value;
			
			if(_title != '')
			{
				if(@_title_label == null)
				{
					@_title_label = Label(ui, '');
					_title_label.mouse_enabled = false;
					_title_label.align_v = GraphicAlign::Middle;
					Container::add_child(_title_label, 1);
				}
				
				_title_label.text = _title;
				_title_label.visible = true;
			}
			else if(@_title_label != null)
			{
				_title_label.text = '';
				_title_label.visible = false;
			}
		}
	}
	
	bool show_close_button
	{
		get const { return _show_close_button; }
		set
		{
			if(_show_close_button == value)
				return;
			
			_show_close_button = value;
			
			if(_show_close_button)
			{
				if(@_close_button == null)
				{
					Cross@ cross = Cross(ui);
					cross.use_highlight_colour = true;
					cross.use_parent_hover = true;
					@_close_button = Button(ui, cross);
					_close_button.draw_border = DrawOption::Never;
					_close_button.draw_background = DrawOption::Never;
					_close_button.mouse_click.on(EventCallback(on_close_button_click));
					Container::add_child(_close_button, 0);
				}
				
				_close_button.visible = true;
			}
			else if(@_close_button != null)
			{
				_close_button.visible = false;
			}
		}
	}
	
	bool add_child(Element@ child, int index=-1) override
	{
		// TODO: Delegate to contents
		return false;
	}
	
	bool remove_child(Element@ child) override
	{
		// TODO: Delegate to contents
		return false;
	}
	
	void set_child_index(Element@ child, int index) override
	{
		// TODO: Delegate to contents
	}
	
	void move_to_front(Element@ child) override
	{
		// TODO: Delegate to contents
	}
	
	void move_to_back(Element@ child) override
	{
		// TODO: Delegate to contents
	}
	
	void move_up(Element@ child) override
	{
		// TODO: Delegate to contents
	}
	
	void move_down(Element@ child) override
	{
		// TODO: Delegate to contents
	}
	
	void fit_to_contents(const bool fit_min=false) override
	{
		// TODO:
	}
	
	void _do_layout(LayoutContext@ ctx) override
	{
		const float spacing = ui.style.spacing;
		const float title_width = _width - spacing * 2;
		const float title_height = ui.style.titlebar_height;
		const float title_item_height = title_height - spacing * 2;
		const bool has_icon = @_title_icon != null && _title_icon.visible;
		const bool has_label = @_title_label != null && _title_label.visible;
		const bool has_close_button = @_close_button != null && _close_button.visible;
		const bool has_title_before = @_title_before != null && _title_before.visible;
		const bool has_title_after = @_title_after != null && _title_after.visible;
		
		if(has_icon)
		{
			_title_icon._x = spacing;
			_title_icon._y = spacing;
			_title_icon._width = _title_icon._height = title_item_height;
		}
		
		if(has_title_before)
		{
			_title_before._x = has_icon ? (_title_icon._x + _title_icon._width + spacing) : spacing;
			_title_before._y = spacing;
			_title_before.height = title_item_height;
		}
		
		if(has_label)
		{
			_title_label._x = spacing;
			_title_label._y = spacing;
			
			if(has_icon)
				_title_label._x += _title_icon._x + _title_icon._width;
			if(has_title_before)
				_title_label._x += _title_before._x + _title_before._width;
			
			_title_label._height = title_height - spacing * 2;
			_title_label._width = _width - spacing - _title_label._x;
			
			if(has_close_button)
				_title_label._width -= _close_button._width + spacing;
			if(has_title_after)
				_title_label._width -= _title_after._width + spacing;
		}
		
		if(has_title_after)
		{
			_title_after._x = _width - _title_after._width - spacing;
			_title_after._y = spacing;
			_title_after.height = title_item_height;
			
			if(has_close_button)
				_title_after._x -= _close_button._width + spacing;
		}
		
		if(has_close_button)
		{
			_close_button._x = _width - title_item_height - spacing;
			_close_button._y = spacing;
			_close_button._width = title_item_height;
			_close_button._height = title_item_height;
		}
		
		_title_divider._x = spacing;
		_title_divider._y = title_height;
		_title_divider._width = title_width;
		
		do_drag(ctx);
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		style.draw_dialog_element(this);
	}
	
	protected bool is_mouse_over_draggable_region() override
	{
		return hovered && ui.mouse.y <= y1 + _title_divider._y && @ui.mouse_over_element == @this;
	}
	
	// Events
	
	private void on_close_button_click(EventInfo@ event)
	{
		puts('CLOSING');
	}
	
}