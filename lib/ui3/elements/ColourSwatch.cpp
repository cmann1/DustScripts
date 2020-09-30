#include 'colour_picker/BasicColourSwatch.cpp';

namespace ColourSwatch { const string TYPE_NAME = 'ColourSwatch'; }

class ColourSwatch : BasicColourSwatch
{
	
	Event change;
	
	protected uint _colour = 0xffaa4444;
	protected bool _show_alpha = true;
	protected bool selected;
	
	protected EventCallback@ on_colour_picker_change_delegate;
	
	ColourSwatch(UI@ ui)
	{
		super(ui);
		
		mouse_enabled = true;
		
		background_colour = _colour;
		border_size = ui.style.border_size;
		@on_colour_picker_change_delegate = EventCallback(on_colour_picker_change);
	}
	
	string element_type { get const override { return ColourSwatch::TYPE_NAME; } }
	
	// ///////////////////////////////////////////////////////////////////
	// Basic properties
	// ///////////////////////////////////////////////////////////////////
	
	/// Sets the current colour
	uint colour
	{
		get const { return _colour; }
		set
		{
			if(_colour == value)
				return;
			
			_colour = value;
			background_colour = _colour;
			
			if(selected)
			{
				_mouse_click(null);
			}
		}
	}
	
	/// Shwo the alpha inputs or not
	bool show_alpha
	{
		get const { return _show_alpha; }
		set
		{
			if(_show_alpha == value)
				return;
			
			_show_alpha = value;
			
			if(selected)
			{
				_mouse_click(null);
			}
		}
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Internal
	// ///////////////////////////////////////////////////////////////////
	
	void _draw(Style@ style, DrawingContext@ ctx) override
	{
		border_colour = style.get_interactive_element_border_colour(hovered, selected, false, disabled);
		border_size = selected ? style.selected_border_size : style.border_size;
		BasicColourSwatch::_draw(style, ctx);
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////////////////////////////
	
	void _mouse_click(EventInfo@ event) override
	{
		selected = true;
		
		ui.show_colour_picker(_colour,
			on_colour_picker_change_delegate, on_colour_picker_change_delegate,
			_show_alpha);
	}
	
	void on_colour_picker_change(EventInfo@ event)
	{
		ColourPicker@ colour_picker = cast<ColourPicker@>(event.target);
		
		_colour = event.type == EventType::CANCEL
			? colour_picker.previous_colour
			: colour_picker.colour;
		background_colour = _colour;
		
		if(event.type != EventType::CHANGE)
		{
			selected = false;
		}
		
		@event.target = this;
		change.dispatch(event);
	}
	
}