#include '../Container.cpp';

/// Internal. See LayerSelector instead
class LayerSelectorSet : Container
{
	
	LayerSelector@ layer_selector;
	int num_layers;
	
	array<Checkbox@> checkboxes;
	array<Label@> labels;
	
	array<int> active_indices;
	int active_count;
	float active_width;
	float active_height;
	
	bool multi_select;
	bool allow_deselect;
	bool drag_select;
	string font;
	uint font_size;
	bool toggle_on_press;
	float label_spacing;
	
	bool validate_layout = true;
	EventCallback@ layer_select_delegate;
	
	protected Event@ select_event;
	protected string select_event_type;
	
	protected Checkbox@ all_checkbox;
	
	protected bool busy_drag_select;
	protected bool drag_select_value;
	
	LayerSelectorSet(UI@ ui, LayerSelector@ layer_selector,
		const int num_layers,
		Event@ select_event, const string select_event_type)
	{
		super(ui);
		
		@this.layer_selector = layer_selector;
		this.num_layers = num_layers;
		@this.select_event = @select_event;
		this.select_event_type = select_event_type;
		
		@layer_select_delegate = EventCallback(on_layer_select);
	}
	
	string element_type { get const override { return 'LayerSelectorSet'; } }
	
	void update_toggle_on_press(const bool toggle_on_press)
	{
		this.toggle_on_press = toggle_on_press;
		
		for(int i = 0; i < num_layers; i++)
		{
			Checkbox@ checkbox = @checkboxes[i];
			
			if(@checkbox != null)
			{
				checkbox.toggle_on_press = toggle_on_press;
			}
		}
	}
	
	void set_font(const string font, const uint size)
	{
		for(int i = 0; i < num_layers; i++)
		{
			Label@ label = @labels[i];
			
			if(@label != null)
			{
				label.set_font(font, size);
			}
		}
	}
	
	void update_label_spacing(const float label_spacing)
	{
		this.label_spacing = label_spacing;
		
		active_width = 0;
		active_height = 0;
		
		for(int i = 0; i < num_layers; i++)
		{
			Label@ label = @labels[i];
			
			if(@label == null)
				return;
			
			label.padding_left = label_spacing;
			label.fit_to_contents();
			
			if(label.visible)
			{
				active_width = max(active_width, label._x + label._set_width);
				active_height += label._height;
			}
		}
		
		validate_layout = true;
	}
	
	int initialise_all_states(const CheckboxState state, const bool trigger_event=true)
	{
		int result = 0;
		
		for(int i = 0; i < active_count; i++)
		{
			Checkbox@ checkbox = checkboxes[active_indices[i]];
			
			if(@checkbox == @all_checkbox)
				continue;
			
			if(checkbox.state != state)
			{
				checkbox.initialise_state(state);
				result++;
			}
		}
		
		if(result > 0)
		{
			update_toggle_all_checkbox();
			
			if(trigger_event)
			{
				ui._dispatch_event(@select_event, select_event_type, layer_selector);
			}
		}
		
		return result;
	}
	
	CheckboxState calculate_all_state()
	{
		CheckboxState state = CheckboxState::Off;
		
		for(int i = 0; i < active_count; i++)
		{
			Checkbox@ checkbox = @checkboxes[active_indices[i]];
			
			if(i == 0)
			{
				state = checkbox.state;
			}
			else if(@checkbox != @all_checkbox && state != CheckboxState::Indeterminate)
			{
				if(checkbox.state == CheckboxState::Off)
				{
					if(state == CheckboxState::On)
					{
						state = CheckboxState::Indeterminate;
					}
				}
				else if(state == CheckboxState::Off)
				{
					state = CheckboxState::Indeterminate;
				}
			}
		}
		
		return state;
	}
	
	int select_all()
	{
		return initialise_all_states(CheckboxState::On);
	}
	
	int select_none()
	{
		return initialise_all_states(CheckboxState::Off);
	}
	
	void rebuild()
	{
		if(checkboxes.length() != uint(num_layers))
		{
			checkboxes.resize(num_layers);
			labels.resize(num_layers);
			active_indices.resize(num_layers);
		}
		
		active_count = 0;
		active_width = 0;
		active_height = 0;
	}
	
	void rebuild_checkboxes(const int start_layer, const int end_layer, const bool visible)
	{
		for(int i = start_layer; i <= end_layer; i++)
		{
			Checkbox@ checkbox = @checkboxes[i];
			Label@ label = @labels[i];
			
			if(visible)
			{
				if(@checkbox == null)
				{
					@checkbox = Checkbox(ui);
					@label = Label(ui,
						i == num_layers - 1 ? '-' : i + '',
						false, font, font_size);
					
					@checkbox.label = label;
					checkbox.toggle_on_press = toggle_on_press;
					checkbox.change.on(@layer_select_delegate);
					
					label.align_v = GraphicAlign::Middle;
					
					label._x = checkbox._set_width;
					label.padding_left = label_spacing;
					label.fit_to_contents();
					label.height = checkbox._set_height;
					
					@checkboxes[i] = @checkbox;
					@labels[i] = @label;
					add_child(checkbox);
					add_child(label);
				}
				else if(!checkbox.visible)
				{
					checkbox.initialise_state(false);
					checkbox.visible = true;
					label.visible = true;
				}
				
				active_indices[active_count++] = i;
				active_width = max(active_width, label._x + label._width);
				active_height += checkbox._height;
			}
			else if(@checkbox != null)
			{
				checkbox.visible = false;
				label.visible = false;
			}
		}
	}
	
	void rebuild_hide_other(const int start_layer, const int end_layer)
	{
		int start_index = 0;
		int end_index = start_layer;
		
		for(int j = 0; j <= 1; j++)
		{
			for(int i = start_index; i < end_index; i++)
			{
				if(@checkboxes[i] == null)
					continue;
				
				checkboxes[i].visible = false;
				labels[i].visible = false;
			}
			
			start_index = end_layer + 1;
			end_index = num_layers;
		}
	}
	
	void rebuild_complete()
	{
		if(@all_checkbox == null && @checkboxes[num_layers - 1] != null)
		{
			@all_checkbox = @checkboxes[num_layers - 1];
			all_checkbox.name = 'toggle_all';
			update_toggle_all_checkbox();
		}
		
		width = active_width;
		height = active_height;
		validate_layout = true;
	}
	
	void do_layout()
	{
		float y = 0;
		
		for(int i = 0; i < active_count; i++)
		{
			const int index = active_indices[i];
			Checkbox@ checkbox = @checkboxes[index];
			Label@ label = @labels[index];
			
			checkbox._y = y;
			label._y = y;
			label._width = active_width - label._x;
			
			y += checkbox._height;
		}
		
		width = active_width;
		height = active_height;
		
		validate_layout = false;
	}
	
	protected void update_toggle_all_checkbox()
	{
		if(@all_checkbox == null)
			return;
		
		all_checkbox.initialise_state(calculate_all_state());
	}
	
	protected void on_layer_select(EventInfo@ event)
	{
		Checkbox@ checkbox = cast<Checkbox@>(event.target);
		
		if(checkbox.name == 'toggle_all')
		{
			if(checkbox.checked)
			{
				select_all();
			}
			else
			{
				select_none();
			}
			
			return;
		}
		
		if(!allow_deselect && !multi_select && !checkbox.checked)
		{
			checkbox.initialise_state(true);
		}
		else
		{
			update_toggle_all_checkbox();
			ui._dispatch_event(@select_event, select_event_type, layer_selector);
		}
	}
	
	protected Checkbox@ get_checkbox(Element@ element)
	{
		if(@element.parent != @this)
			return null;
		
		Checkbox@ checkbox = cast<Checkbox@>(@element);
		
		if(@checkbox != null)
			return @checkbox;
		
		Label@ label = cast<Label@>(@element);
		
		int index = labels.findByRef(@label);
		
		return index != -1
			? @checkboxes[index]
			: null;
	}
	
	void _mouse_press(const MouseButton button) override
	{
		if(button != ui.primary_button)
			return;
		
		if(drag_select && toggle_on_press)
		{
			Checkbox@ checkbox = get_checkbox(ui.mouse_over_element);
			
			if(@checkbox != null && checkbox.name != 'toggle_all')
			{
				busy_drag_select = true;
				drag_select_value = checkbox.checked;
			}
		}
	}
	
	void _mouse_move() override
	{
		if(busy_drag_select)
		{
			if(!ui.mouse.primary_down)
			{
				busy_drag_select = false;
			}
			else
			{
				Checkbox@ checkbox = get_checkbox(ui.mouse_over_element);
				
				if(@checkbox != null && checkbox.name != 'toggle_all')
				{
					checkbox.checked = drag_select_value;
				}
			}
		}
	}
	
}