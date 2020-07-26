#include '../UI.cpp';
#include '../Style.cpp';
#include '../TextAlign.cpp';
#include '../events/Event.cpp';
#include '../utils/ButtonGroup.cpp';
#include 'Element.cpp';
#include 'SingleContainer.cpp';

class Button : SingleContainer
{
	
	bool selectable;
	bool _selected;
	
	Event select;
	
	protected bool pressed;
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
		width  = 40;
		height = 40;
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
	
	void do_layout(const float parent_x, const float parent_y) override
	{
		Element::do_layout(parent_x, parent_y);
		
		if(hovered && ui.mouse.primary_press)
		{
			pressed = true;
		}
		else if(pressed)
		{
			if(selectable && hovered && ui.mouse.primary_release)
			{
				selected = !_selected;
			}
			
			if(!ui.mouse.primary_down)
			{
				pressed = false;
			}
		}
		
		if(@_content != null)
		{
			_content.x = (width  - _content.width)  * 0.5;
			_content.y = (height - _content.height) * 0.5;
			
			if(pressed)
			{
				_content.x += ui.style.button_pressed_icon_offset;
				_content.y += ui.style.button_pressed_icon_offset;
			}
		}
	}
	
	void draw(Style@ style, const float sub_frame) override
	{
		if(alpha != 1)
			style.multiply_alpha(alpha);
		
		style.draw_interactive_element(this, hovered, selectable && selected, disabled);
		
		if(@_content != null)
		{
			if(disabled)
				style.disable_alpha();
			
			_content.draw(style, sub_frame);
			
			if(disabled)
				style.restore_alpha();
		}
		
		if(alpha != 1)
			style.restore_alpha();
	}
	
	protected float border_size { get const override { return ui.style.border_size; } }
	
}