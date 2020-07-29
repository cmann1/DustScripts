#include '../UI.cpp';
#include '../Style.cpp';
#include '../TextAlign.cpp';
#include '../events/Event.cpp';
#include '../utils/ButtonGroup.cpp';
#include 'SingleContainer.cpp';

class Button : SingleContainer
{
	
	bool selectable;
	bool _selected;
	
	Event select;
	
	protected ButtonGroup@ _group;
	
	Button(UI@ ui, Element@ content)
	{
		super(ui, content, 'btn');
		
		init();
	}
	
	Button(UI@ ui, const string text)
	{
		Label@ label = ui._label_pool.get(
			ui, text,
			TextAlign::Left, TextAlign::Top,
			ui.style.default_text_scale, ui.style.text_clr,
			ui.style.default_font, ui.style.default_text_size);
		
		super(ui, label, 'btn');
		
		init();
	}
	
	Button(UI@ ui, const string sprite_text, const string sprite_name, const float width=-1, const float height=-1, const float offset_x=-0.5, const float offset_y=-0.5)
	{
		Image@ image = Image(ui, sprite_text, sprite_name, width, height, offset_x, offset_y);
		
		super(ui, image, 'btn');
		
		init();
	}
	
	protected void init()
	{
		children_mouse_enabled = false;
		_set_width  = _width  = 40;
		_set_height = _height = 40;
	}
	
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
		style.draw_interactive_element(this, hovered, selectable && selected, disabled);
	}
	
	void _mouse_click() override
	{
		if(selectable)
		{
			selected = !_selected;
		}
	}
	
	protected float border_size { get const override { return ui.style.border_size; } }
	
}