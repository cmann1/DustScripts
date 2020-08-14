#include '../UI.cpp';
#include '../Style.cpp';
#include '../TextAlign.cpp';
#include '../events/Event.cpp';
#include '../utils/ButtonGroup.cpp';
#include '../utils/DrawOption.cpp';
#include '../utils/GraphicAlign.cpp';
#include 'SingleContainer.cpp';

class Button : SingleContainer
{
	
	bool selectable;
	bool _selected;
	
	DrawOption draw_background = DrawOption::Always;
	DrawOption draw_border = DrawOption::Always;
	
	Event select;
	
	protected ButtonGroup@ _group;
	
	Button(UI@ ui, Element@ content)
	{
		super(ui, content);
		
		init();
	}
	
	Button(UI@ ui, const string text, const TextAlign text_align_h=TextAlign::Left)
	{
		Label@ label = ui._label_pool.get(
			ui, text, true,
			text_align_h, GraphicAlign::Centre, GraphicAlign::Middle,
			ui.style.default_text_scale, ui.style.text_clr,
			ui.style.default_font, ui.style.default_text_size);
		
		super(ui, label);
		
		init();
	}
	
	Button(UI@ ui, const string sprite_set, const string sprite_name, const float width=-1, const float height=-1, const float offset_x=0, const float offset_y=0)
	{
		Image@ image = Image(ui, sprite_set, sprite_name, width, height, offset_x, offset_y);
		
		super(ui, image);
		
		init();
	}
	
	protected void init()
	{
		children_mouse_enabled = false;
		_set_width  = _width  = 50;
		_set_height = _height = 30;
	}
	
	string element_type { get const override { return 'Button'; } }
	
	ButtonGroup@ group
	{
		get { return _group; }
		set
		{
			if(@_group == @value)
				return;
			
			value.add(this);
			@_group = value;
		}
	}
	
	bool selected
	{
		get const { return _selected; }
		set
		{
			if(_selected == value)
				return;
			
			if(@_group != null && !_group._try_select(this, value))
				return;
			
			_selected = value;
			ui._event_info.reset(EventType::SELECT, this);
			select.dispatch(ui._event_info);
			
			if(@_group != null)
			{
				_group._change_selection(this, _selected);
			}
		}
	}
	
	void _do_layout(LayoutContext@ ctx) override
	{
		if(@_content != null)
		{
			_content._x = (_width  - _content._width)  * 0.5;
			_content._y = (_height - _content._height) * 0.5;
			
			if(pressed)
			{
				_content._x += ui.style.button_pressed_icon_offset;
				_content._y += ui.style.button_pressed_icon_offset;
			}
		}
	}
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		style.draw_interactive_element(
			x1, y1, x2, y2,
			hovered || pressed,
			selectable && selected,
			pressed,
			disabled,
			draw_background == DrawOption::Always || draw_background == DrawOption::Hover && (hovered || pressed),
			draw_border == DrawOption::Always || draw_border == DrawOption::Hover && hovered);
	}
	
	protected float layout_padding_left		{ get const override { return ui.style.spacing; } }
	
	protected float layout_padding_right	{ get const override { return ui.style.spacing; } }
	
	protected float layout_padding_top		{ get const override { return ui.style.spacing; } }
	
	protected float layout_padding_bottom	{ get const override { return ui.style.spacing; } }
	
	protected float layout_border_size		{ get const override { return ui.style.border_size; } }
	
	// ///////////////////////////////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////////////////////////////
	
	void _mouse_press(const MouseButton button) override
	{
		if(button != ui.primary_button)
			return;
		
		validate_layout = true;
		@ui._active_mouse_element = @this;
	}
	
	void _mouse_release(const MouseButton button) override
	{
		validate_layout = true;
		@ui._active_mouse_element = null;
	}
	
	void _mouse_click() override
	{
		if(selectable)
		{
			selected = !_selected;
		}
	}
	
}