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
	private Button@ _selected_button;
	
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
			_set__selected_button(button);
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
		
		if(@button == @_selected_button)
		{
			_set__selected_button(null);
		}
	}
	
	bool _try_select(Button@ button, bool selected)
	{
		if(busy_deselecting)
			return true;
		
		if(!allow_deselect && @button == @_selected_button)
			return false;
		
		if(selected && @button != @_selected_button && @_selected_button != null)
		{
			busy_deselecting = true;
			_selected_button.selected = false;
			busy_deselecting = false;
		}
		
		return true;
	}
	
	void _change_selection(Button@ button, bool selected)
	{
		if(@button == @_selected_button)
		{
			if(!selected)
			{
				@_selected_button = null;
			}
		}
		else if(selected)
		{
			@_selected_button = button;
		}
		
		ui._generic_event_info.reset(EventType::SELECT, @this, @_selected_button);
		select.dispatch(ui._generic_event_info);
	}
	
	Button@ selected_button { get { return _selected_button; } }
	
	private void _set__selected_button(Button@ button)
	{
		if(@button == @_selected_button)
			return;
		
		if(@_selected_button != null)
		{
			busy_deselecting = true;
			_selected_button.selected = false;
			busy_deselecting = false;
		}
		
		@_selected_button = @button;
		ui._generic_event_info.reset(EventType::SELECT, @this, @_selected_button);
		select.dispatch(ui._generic_event_info);
	}
	
}