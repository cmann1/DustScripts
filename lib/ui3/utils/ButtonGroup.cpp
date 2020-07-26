#include '../elements/Button.cpp';
#include '../events/GenericEvent.cpp';

class ButtonGroup : IGenericEventTarget
{
	
	string _name = '';
	bool allow_deselect;
	
	GenericEvent select;
	
	private UI@ ui;
	private array<Button@> buttons;
	private bool busy_deselecting;
	private Button@ selected_button;
	
	ButtonGroup(UI@ ui, bool allow_deselect=true)
	{
		@this.ui = ui;
		this.allow_deselect = allow_deselect;
	}
	
	ButtonGroup(UI@ ui, const string name, bool allow_deselect=true)
	{
		@this.ui = ui;
		this.name = name;
		this.allow_deselect = allow_deselect;
	}
	
	string name
	{
		get const { return _name; }
		set { _name = value; }
	}
	
	void add(Button@ button)
	{
		if(@button.group == @this || buttons.size() > 0 && @buttons[buttons.size() - 1] == @button)
			return;
		
		if(@button.group != null)
		{
			button.group.remove(button);
		}
		
		buttons.insertLast(button);
		@button.group = this;
		button.selectable = true;
		
		if(button.selected)
		{
			_set_selected_button(button);
		}
	}
	
	void remove(Button@ button)
	{
		if(@button.group != @this)
			return;
		
		int index = buttons.findByRef(button);
		
		if(index == -1)
			return;
		
		buttons.removeAt(index);
		@button.group = null;
		
		if(@button == @selected_button)
		{
			_set_selected_button(null);
		}
	}
	
	bool _try_select(Button@ button, bool selected)
	{
		if(busy_deselecting)
			return true;
		
		if(!allow_deselect && @button == @selected_button)
			return false;
		
		if(selected && @button != @selected_button && @selected_button != null)
		{
			busy_deselecting = true;
			selected_button.selected = false;
			busy_deselecting = false;
		}
		
		return true;
	}
	
	void _change_selection(Button@ button, bool selected)
	{
		if(@button == @selected_button)
		{
			if(!selected)
			{
				@selected_button = null;
			}
		}
		else if(selected)
		{
			@selected_button = button;
		}
		
		ui._generic_event_info.reset(EventType::SELECT, @this, @selected_button);
		select.dispatch(ui._generic_event_info);
	}
	
	private void _set_selected_button(Button@ button)
	{
		if(@button == @selected_button)
			return;
		
		if(@selected_button != null)
		{
			busy_deselecting = true;
			selected_button.selected = false;
			busy_deselecting = false;
		}
		
		@selected_button = @button;
		ui._generic_event_info.reset(EventType::SELECT, @this, @selected_button);
		select.dispatch(ui._generic_event_info);
	}
	
}