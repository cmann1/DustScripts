#include 'Button.cpp';
#include 'Divider.cpp';
#include 'Image.cpp';
#include 'Label.cpp';
#include 'MoveableDialog.cpp';
#include 'shapes/Cross.cpp';
#include '../layouts/flow/FlowLayout.cpp';
#include '../utils/WindowCloseType.cpp';

namespace Window { const string TYPE_NAME = 'Window'; }

class Window : MoveableDialog
{
	
	Event close;
	
	protected bool _show_title_bar = true;
	protected string _title;
	protected Image@ _title_icon;
	protected Label@ _title_label;
	protected float _title_bar_height = -1;
	protected Container@ _title_before;
	protected Container@ _title_after;
	protected FlowLayout@ title_flow_layout;
	protected Divider@ _title_divider;
	protected bool _show_close_button;
	protected bool _close_on_click_outside;
	protected Button@ _close_button;
	protected Container@ _contents;
	protected Divider@ _buttons_divider;
	protected Container@ _buttons_left;
	protected Container@ _buttons_right;
	protected FlowLayout@ _buttons_left_flow_layout;
	protected FlowLayout@ _buttons_right_flow_layout;
	
	protected bool open = false;
	protected bool fading = false;
	protected float fade;
	
	protected bool layout_buttons_left;
	protected bool layout_buttons_right;
	
	protected EventCallback@ ui_mouse_press_delegate;
	
	Window(UI@ ui, const string title, bool show_close_button=true, bool draggable=true)
	{
		super(ui);
		
		this.title = title;
		this.show_close_button = show_close_button;
		this.draggable = draggable;
		
		@_title_divider = Divider(ui, Orientation::Vertical);
		Container::add_child(_title_divider);
		
		@_contents = Container(ui);
		Container::add_child(_contents);
		
		_width = _set_width = 200;
		_height = _set_height = 200;
		
		visible = false;
		alpha = 0;
	}
	
	string element_type { get const override { return Window::TYPE_NAME; } }
	
	void centre()
	{
		const float x = @parent != null ? parent._width  : ui.region_width;
		const float y = @parent != null ? parent._height : ui.region_height;
		
		this.x = (x - _width)  * 0.5;
		this.y = (y - _height) * 0.5;
	}
	
	void show(bool fade=true)
	{
		if(open)
			return;
		
		open = true;
		visible = true;
		
		if(_close_on_click_outside)
		{
			toggle_ui_mouse_press(true);
		}
		
		if(!fade)
		{
			alpha = 1;
			fading = false;
			mouse_enabled = true;
			this.fade = ui.style.tooltip_fade_frames;
		}
		else
		{
			fading = true;
			mouse_enabled = false;
			step_subscribed = ui._step_subscribe(@this, step_subscribed);
		}
	}
	
	void hide(const string hide_type='user', bool fade=true)
	{
		if(!open)
			return;
		
		open = false;
		mouse_enabled = false;
		busy_dragging = false;
		
		if(_close_on_click_outside)
		{
			toggle_ui_mouse_press(false);
		}
		
		if(!fade)
		{
			alpha = 0;
			this.fade = 0;
			visible = false;
			fading = false;
		}
		else
		{
			fading = true;
			step_subscribed = ui._step_subscribe(@this, step_subscribed);
		}
		
		ui._dispatch_event(@close, EventType::CLOSE, this, hide_type);
	}
	
	Layout@ layout
	{
		get override { return @_contents.layout; }
		set override
		{
			@_contents.layout = value;
		}
	}
	
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
		_title_icon.visible = _show_title_bar;
		validate_layout = true;
	}
	
	void remove_icon()
	{
		if(@_title_icon == null)
			return;
		
		_title_icon.visible = false;
		validate_layout = true;
	}
	
	bool show_title_bar
	{
		get const { return _show_title_bar; }
		set
		{
			if(_show_title_bar == value)
				return;
			
			_show_title_bar = value;
			
			if(@_title_icon != null)
				_title_icon.visible = _show_title_bar;
			if(@_title_label != null)
				_title_label.visible = _show_title_bar;
			if(@_close_button != null)
				_close_button.visible = _show_title_bar;
			if(@_title_before != null)
				_title_before.visible = _show_title_bar;
			if(@_title_after != null)
				_title_after.visible = _show_title_bar;
			
			_title_divider.visible = _show_title_bar;
			
			validate_layout = true;
		}
	}
	
	float title_bar_height
	{
		get const { return _title_bar_height; }
		set
		{
			if(_title_bar_height == value)
				return;
			
			_title_bar_height = value;
			validate_layout = true;
		}
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
					_title_label.sizing = ImageSize::ConstrainInside;
					Container::add_child(_title_label, 1);
				}
				
				_title_label.text = _title;
				_title_label.visible = _show_title_bar;
			}
			else if(@_title_label != null)
			{
				_title_label.text = '';
				_title_label.visible = false;
			}
			
			validate_layout = true;
		}
	}
	
	Label@ title_label
	{
		get const { return _title_label; }
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
				
				_close_button.visible = _show_title_bar;
			}
			else if(@_close_button != null)
			{
				_close_button.visible = false;
			}
			
			validate_layout = true;
		}
	}
	
	bool close_on_click_outside
	{
		get const { return _close_on_click_outside; }
		set
		{
			if(_close_on_click_outside == value)
				return;
			
			_close_on_click_outside = value;
			
			if(open)
			{
				toggle_ui_mouse_press(_close_on_click_outside);
			}
		}
	}
	
	// Title before
	
	bool add_title_before(Element@ child, int index=-1)
	{
		if(@_title_before == null)
		{
			create_title_layout();
			
			@_title_before = Container(ui);
			@_title_before.layout = @title_flow_layout;
			_title_before.name = 'title_before';
			Container::add_child(_title_before);
			validate_layout = true;
		}
		
		if(_title_before.add_child(child, index))
		{
			_title_before.fit_to_contents(true);
			validate_layout = true;
			return true;
		}
		
		return false;
	}
	
	bool remove_title_before(Element@ child)
	{
		if(_title_before.remove_child(child))
		{
			_title_before.fit_to_contents(true);
			validate_layout = true;
			return true;
		}
		
		return false;
	}
	
	void set_title_before_index(Element@ child, int index)
	{
		_title_before.set_child_index(child, index);
		validate_layout = true;
	}
	
	void move_title_before_to_front(Element@ child)
	{
		_title_before.move_to_front(child);
		validate_layout = true;
	}
	
	void move_title_before_to_back(Element@ child)
	{
		_title_before.move_to_back(child);
		validate_layout = true;
	}
	
	void move_title_before_up(Element@ child)
	{
		_title_before.move_up(child);
		validate_layout = true;
	}
	
	void move_title_before_down(Element@ child)
	{
		_title_before.move_down(child);
		validate_layout = true;
	}
	
	// Title after
	
	bool add_title_after(Element@ child, int index=-1)
	{
		if(@_title_after == null)
		{
			create_title_layout();
			
			@_title_after = Container(ui);
			@_title_after.layout = @title_flow_layout;
			_title_after.name = 'title_after';
			Container::add_child(_title_before);
			validate_layout = true;
		}
		
		if(_title_after.add_child(child, index))
		{
			_title_after.fit_to_contents(true);
			validate_layout = true;
			return true;
		}
		
		return false;
	}
	
	bool remove_title_after(Element@ child)
	{
		if(_title_after.remove_child(child))
		{
			_title_after.fit_to_contents(true);
			validate_layout = true;
			return true;
		}
		
		return false;
	}
	
	void set_title_after_index(Element@ child, int index)
	{
		_title_after.set_child_index(child, index);
		validate_layout = true;
	}
	
	void move_title_after_to_front(Element@ child)
	{
		_title_before.move_to_front(child);
		validate_layout = true;
	}
	
	void move_title_after_to_back(Element@ child)
	{
		_title_after.move_to_back(child);
		validate_layout = true;
	}
	
	void move_title_after_up(Element@ child)
	{
		_title_after.move_up(child);
		validate_layout = true;
	}
	
	void move_title_after_down(Element@ child)
	{
		_title_after.move_down(child);
		validate_layout = true;
	}
	
	// Content
	
	bool add_child(Element@ child, int index=-1) override
	{
		validate_layout = true;
		return _contents.add_child(child, index);
	}
	
	bool remove_child(Element@ child) override
	{
		validate_layout = true;
		return _contents.remove_child(child);
	}
	
	void set_child_index(Element@ child, int index) override
	{
		_contents.set_child_index(child, index);
	}
	
	void move_to_front(Element@ child) override
	{
		_contents.move_to_front(child);
	}
	
	void move_to_back(Element@ child) override
	{
		_contents.move_to_back(child);
	}
	
	void move_up(Element@ child) override
	{
		_contents.move_up(child);
	}
	
	void move_down(Element@ child) override
	{
		_contents.move_down(child);
	}
	
	void fit_to_contents(const bool fit_min=false) override
	{
		_contents.fit_to_contents(fit_min);
		
		const float spacing = ui.style.spacing;
		const float title_height = _title_bar_height <= 0 ? ui.style.titlebar_height : _title_bar_height;
		const bool has_buttons_left = @_buttons_left != null && _buttons_left.visible;
		const bool has_buttons_right = @_buttons_right != null && _buttons_right.visible;
		
		_width = _set_width = _contents._width + spacing * 2;
		_height = _contents._height + spacing * 2;
		
		if(_show_title_bar)
		{
			_height += title_height + _title_divider._height;
		}
		
		if(has_buttons_left || has_buttons_right)
		{
			if(@_buttons_left != null)
			{
				_buttons_left.fit_to_contents(fit_min);
			}
			
			if(@_buttons_right != null)
			{
				_buttons_right.fit_to_contents(fit_min);
			}
			
			_height += _buttons_divider._height + spacing * 2;
			_height += max(@_buttons_left != null ? _buttons_left._height : 0, @_buttons_right != null ? _buttons_right._height : 0);
		}
		
		_set_height = _height;
		validate_layout = true;
	}
	
	// Buttons left
	
	bool add_button_left(Element@ child, int index=-1)
	{
		if(@_buttons_left == null)
		{
			@_buttons_left = Container(ui);
			@_buttons_left_flow_layout = FlowLayout(ui, FlowDirection::Row, FlowAlign::Start, FlowAlign::Centre, FlowWrap::Wrap);
			_buttons_left_flow_layout.set_padding(0);
			@_buttons_left.layout = @_buttons_left_flow_layout;
			_buttons_left.width = _width - ui.style.spacing * 2;
			Container::add_child(_buttons_left);
			
			create_buttons_divider();
			validate_layout = true;
		}
		
		_buttons_divider.visible = true;
		
		if(_buttons_left.add_child(child, index))
		{
			layout_buttons_left = true;
			validate_layout = true;
			return true;
		}
		
		return false;
	}
	
	bool remove_button_left(Element@ child)
	{
		if(_buttons_left.remove_child(child))
		{
			if(_buttons_left.child_count == 0 && (@_buttons_right == null || _buttons_right.child_count == 0))
				_buttons_divider.visible = false;
			
			layout_buttons_left = true;
			validate_layout = true;
			return true;
		}
		
		validate_layout = true;
		return false;
	}
	
	void set_button_left_index(Element@ child, int index)
	{
		_buttons_left.set_child_index(child, index);
	}
	
	void move_button_left_to_front(Element@ child)
	{
		_buttons_left.move_to_front(child);
	}
	
	void move_button_left_to_back(Element@ child)
	{
		_buttons_left.move_to_back(child);
	}
	
	void move_button_left_up(Element@ child)
	{
		_buttons_left.move_up(child);
	}
	
	void move_button_left_down(Element@ child)
	{
		_buttons_left.move_down(child);
	}
	
	// Buttons right
	
	bool add_button_right(Element@ child, int index=-1)
	{
		if(@_buttons_right == null)
		{
			@_buttons_right = Container(ui);
			@_buttons_right_flow_layout = FlowLayout(ui, FlowDirection::RowReverse, FlowAlign::Start, FlowAlign::Centre, FlowWrap::Wrap);
			_buttons_right_flow_layout.set_padding(0);
			@_buttons_right.layout = @_buttons_right_flow_layout;
			_buttons_right.width = _width - ui.style.spacing * 2;
			Container::add_child(_buttons_right);
			
			create_buttons_divider();
			validate_layout = true;
		}
		
		_buttons_divider.visible = true;
		
		if(_buttons_right.add_child(child, index))
		{
			layout_buttons_right = true;
			validate_layout = true;
			return true;
		}
		
		return false;
	}
	
	bool remove_button_right(Element@ child)
	{
		if(_buttons_right.remove_child(child))
		{
			if(_buttons_right.child_count == 0 && (@_buttons_left == null || _buttons_left.child_count == 0))
				_buttons_divider.visible = false;
			
			layout_buttons_right = true;
			validate_layout = true;
			return true;
		}
		
		return false;
	}
	
	void set_button_right_index(Element@ child, int index)
	{
		_buttons_right.set_child_index(child, index);
		validate_layout = true;
	}
	
	void move_button_right_to_front(Element@ child)
	{
		_buttons_right.move_to_front(child);
		validate_layout = true;
	}
	
	void move_button_right_to_back(Element@ child)
	{
		_buttons_right.move_to_back(child);
		validate_layout = true;
	}
	
	void move_button_right_up(Element@ child)
	{
		_buttons_right.move_up(child);
		validate_layout = true;
	}
	
	void move_button_right_down(Element@ child)
	{
		_buttons_right.move_down(child);
		validate_layout = true;
	}
	
	// Internal
	
	bool ui_step() override
	{
		MoveableDialog::ui_step();
		
		if(!fading && !busy_dragging)
		{
			step_subscribed = false;
			return step_subscribed;
		}
		
		if(fading)
		{
			if(open)
			{
				if(++fade == ui.style.tooltip_fade_frames)
				{
					mouse_enabled = true;
					fading = false;
				}
				else
				{
					step_subscribed = true;
				}
				
				alpha = (fade / ui.style.tooltip_fade_frames);
			}
			else if(fade > 0)
			{
				if(--fade == 0)
				{
					visible = false;
					fading = false;
				}
				else
				{
					step_subscribed = true;
				}
				
				alpha = (fade / ui.style.tooltip_fade_frames);
			}
		}
		
		return step_subscribed;
	}
	
	void _do_layout(LayoutContext@ ctx) override
	{
		const float spacing = ui.style.spacing;
		const float title_width = _width - spacing * 2;
		const float title_height = _title_bar_height <= 0 ? ui.style.titlebar_height : _title_bar_height;
		const float title_item_height = title_height - spacing * 2;
		const bool has_icon = _show_title_bar && @_title_icon != null && _title_icon.visible;
		const bool has_label = _show_title_bar && @_title_label != null && _title_label.visible;
		const bool has_close_button = _show_title_bar && @_close_button != null && _close_button.visible;
		const bool has_title_before = _show_title_bar && @_title_before != null && _title_before.visible;
		const bool has_title_after = _show_title_bar && @_title_after != null && _title_after.visible;
		const bool has_buttons_left = @_buttons_left != null && _buttons_left.visible;
		const bool has_buttons_right = @_buttons_right != null && _buttons_right.visible;
		
		if(has_icon)
		{
			_title_icon.x = spacing;
			_title_icon.y = spacing;
			_title_icon.width = _title_icon._height = title_item_height;
		}
		
		if(has_title_before)
		{
			_title_before.x = has_icon ? (_title_icon._x + _title_icon._width + spacing) : spacing;
			_title_before.y = spacing;
			_title_before.height = title_item_height;
		}
		
		if(has_label)
		{
			_title_label.x = spacing;
			_title_label.y = spacing;
			
			if(has_icon)
				_title_label.x += _title_icon._width + spacing;
			if(has_title_before)
				_title_label.x += _title_before._width + spacing;
			
			_title_label.height = title_height - spacing * 2;
			_title_label.width = _width - spacing - _title_label._x;
			
			if(has_close_button)
				_title_label.width -= _close_button._width + spacing;
			if(has_title_after)
				_title_label.width -= _title_after._width + spacing;
		}
		
		if(has_title_after)
		{
			_title_after.x = _width - _title_after._width - spacing;
			_title_after.y = spacing;
			_title_after.height = title_item_height;
			
			if(has_close_button)
				_title_after.x -= _close_button._width + spacing;
		}
		
		if(has_close_button)
		{
			_close_button.x = _width - title_item_height - spacing;
			_close_button.y = spacing;
			_close_button.width = title_item_height;
			_close_button.height = title_item_height;
		}
		
		if(_show_title_bar)
		{
			_title_divider.x = spacing;
			_title_divider.y = title_height;
			_title_divider.width = title_width;
		}
		
		float content_height = _height - spacing * 2;
		
		if(_show_title_bar)
		{
			content_height -= _title_divider._y + _title_divider._height;
		}
		
		if(has_buttons_left || has_buttons_right)
		{
			if(layout_buttons_left)
			{
				_buttons_left.fit_to_contents();
				layout_buttons_left = false;
			}
			
			if(layout_buttons_right)
			{
				_buttons_right.fit_to_contents();
				layout_buttons_right = false;
			}
			
			float buttons_height = max(has_buttons_left ? _buttons_left._height : 0, has_buttons_right ? _buttons_right._height : 0);
			
			content_height -= buttons_height;
			content_height -= _buttons_divider._height + spacing * 2;
			
			if(has_buttons_left)
			{
				_buttons_left.y = _height - spacing - buttons_height;
				_buttons_left.x = spacing;
				_buttons_left.width = _buttons_left.scroll_max_x - _buttons_left.scroll_min_x;
				_buttons_left.height = buttons_height;
			}
			
			if(has_buttons_right)
			{
				_buttons_right.y = _height - spacing - buttons_height;
				_buttons_right.width = _buttons_right.scroll_max_x - _buttons_right.scroll_min_x;
				_buttons_right.height = buttons_height;
				_buttons_right.x = title_width - _buttons_right._width + spacing;
			}
		}
		
		_contents.x = spacing;
		_contents.y = _show_title_bar ? _title_divider._y + _title_divider._height + spacing : spacing;
		_contents.width = title_width;
		_contents.height = content_height;
		
		if(@_buttons_divider != null && _buttons_divider.visible)
		{
			_buttons_divider.x = spacing;
			_buttons_divider.y = _contents._y + _contents._height + spacing;
			_buttons_divider.width = title_width;
		}
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		style.draw_dialog_element(this);
	}
	
	private void create_buttons_divider()
	{
		if(@_buttons_divider != null)
			return;
		
		@_buttons_divider = Divider(ui, Orientation::Vertical);
		Container::add_child(_buttons_divider);
	}
	
	private void create_title_layout()
	{
		if(@title_flow_layout != null)
			return;
		
		@title_flow_layout = FlowLayout(ui, FlowDirection::Row, FlowAlign::Start, FlowAlign::Centre, FlowWrap::NoWrap);
		title_flow_layout.set_padding(0);
	}
	
	protected bool is_mouse_over_draggable_region() override
	{
		return hovered && ui.mouse.y <= y1 + _title_divider._y && @ui.mouse_over_element == @this;
	}
	
	protected bool is_drag_anywhere() override
	{
		return @ui.mouse_over_element == @_contents || MoveableDialog::is_drag_anywhere();
	}
	
	protected void toggle_ui_mouse_press(const bool on)
	{
		if(@ui_mouse_press_delegate == null)
		{
			@ui_mouse_press_delegate = EventCallback(on_ui_mouse_press);
		}
		
		if(on)
		{
			ui.mouse_press.on(ui_mouse_press_delegate);
		}
		else
		{
			ui.mouse_press.off(ui_mouse_press_delegate);
		}
	}
	
	// Events
	
	protected void on_close_button_click(EventInfo@ event)
	{
		hide(WindowCloseType::Button);
	}
	
	protected void on_ui_mouse_press(EventInfo@ event)
	{
		if(!hovered)
		{
			hide(WindowCloseType::Modal);
		}
	}
	
}