#include '../Container.cpp';

/// Internal. See LayerSelector instead
class LayerSelectorSet : Container
{
	
	LayerSelector@ layer_selector;
	int num_layers;
	
	array<Checkbox@> checkboxes;
	array<Label@> labels;
	array<int> groups;
	array<bool> visibility;
	
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
	int select_layer_group_modifier;
	bool labels_first;
	float label_spacing;
	float layer_spacing;
	
	bool toggle_all_top;
	
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
		
		checkboxes.resize(num_layers);
		labels.resize(num_layers);
		groups.resize(num_layers);
		visibility.resize(num_layers);
		active_indices.resize(num_layers);
	}
	
	string element_type { get const override { return 'LayerSelectorSet'; } }
	
	void update_multi_select(const bool multi_select, const bool trigger_event=true)
	{
		this.multi_select = multi_select;
		
		if(multi_select)
			return;
		
		// Make sure only one is selected >>
		
		bool found_selected = false;
		int result = 0;
		
		for(int i = active_count - 1; i >= 0; i--)
		{
			Checkbox@ checkbox = checkboxes[active_indices[i]];
			
			if(@checkbox == @all_checkbox)
				continue;
			
			if(checkbox.checked)
			{
				if(found_selected)
				{
					checkbox.initialise_state(false);
					result++;
				}
				else
				{
					found_selected = true;
				}
			}
		}
		
		if(result > 0 && trigger_event)
		{
			ui._dispatch_event(@select_event, select_event_type, layer_selector);
		}
	}
	
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
	
	void update_labels_first(const bool labels_first)
	{
		this.labels_first = labels_first;
		validate_layout = true;
	}
	
	void update_label_spacing(float label_spacing)
	{
		this.label_spacing = label_spacing;
		
		label_spacing = is_nan(label_spacing) ? ui.style.spacing : label_spacing;
		
		active_width = 0;
		
		for(int i = 0; i < num_layers; i++)
		{
			Checkbox@ checkbox = @checkboxes[i];
			Label@ label = @labels[i];
			
			checkbox.fit(0);
			label.fit_to_contents();
			
			if(label.visible)
			{
				active_width = max(active_width, checkbox._width + label._width - label.real_padding_left - label.real_padding_right + label_spacing);
			}
		}
		
		validate_layout = true;
	}
	
	void update_layer_spacing(float layer_spacing)
	{
		this.layer_spacing = layer_spacing;
		
		layer_spacing = is_nan(layer_spacing) ? ui.style.spacing : layer_spacing;
		
		active_height = 0;
		
		for(int i = 0; i < num_layers; i++)
		{
			Checkbox@ checkbox = @checkboxes[i];
			Label@ label = @labels[i];
			
			checkbox.fit(0);
			label.fit_to_contents();
			
			const float layer_height = max(checkbox._set_height + layer_spacing, label._set_height + layer_spacing);
			label.height = layer_height;
			checkbox.height = layer_height;
			
			if(label.visible)
			{
				active_height += max(checkbox._height, label._height);
			}
		}
		
		validate_layout = true;
	}
	
	int initialise_all_states(const bool checked, const bool trigger_event=true)
	{
		return initialise_all_states(checked, -1, trigger_event);
	}
	
	int initialise_all_states(const bool checked, const int group, const bool trigger_event=true)
	{
		return initialise_states(0, num_layers - 1, checked, group, trigger_event);
	}
	
	int initialise_states(int start_layer, int end_layer, const bool checked, const int group=-1, const bool trigger_event=true)
	{
		if(!validate_layer_range(start_layer, end_layer, start_layer, end_layer))
			return 0;
		
		int result = 0;
		
		for(int i = start_layer; i <= end_layer; i++)
		{
			Checkbox@ checkbox = checkboxes[i];
			
			if(!checkbox.visible || @checkbox == @all_checkbox || (group != -1 && groups[i] != group))
				continue;
			
			if(checkbox.checked != checked)
			{
				checkbox.initialise_state(checked);
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
	
	int count_selected(int start_layer, int end_layer)
	{
		if(!validate_layer_range(start_layer, end_layer, start_layer, end_layer))
			return 0;
		
		int result = 0;
		
		for(int i = start_layer; i <= end_layer; i++)
		{
			Checkbox@ checkbox = checkboxes[i];
			
			if(!checkbox.visible || @checkbox == @all_checkbox)
				continue;
			
			if(checkbox.checked)
			{
				result++;
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
		return initialise_all_states(true);
	}
	
	int select_none()
	{
		return initialise_all_states(false);
	}
	
	bool get_layer_colour(const int layer, uint &out colour)
	{
		if(layer < 0 || layer >= num_layers)
			return false;
		
		Label@ label = @labels[layer];
		
		if(@label == null)
			return false;
		
		colour = label.colour;
		return true;
	}
	
	bool set_layer_colour(int layer, const uint colour, int end_layer=-1)
	{
		if(!validate_layer_range(layer, end_layer, layer, end_layer))
			return false;
		
		int result = 0;
		
		for(int i = layer; i <= end_layer; i++)
		{
			Label@ label = @labels[i];
			
			if(@label == null)
				continue;
			
			label.colour = colour;
			result++;
		}
		
		return result > 0;
	}
	
	bool clear_layer_colour(int layer, int end_layer=-1)
	{
		if(!validate_layer_range(layer, end_layer, layer, end_layer))
			return false;
		
		int result = 0;
		
		for(int i = layer; i <= end_layer; i++)
		{
			Label@ label = @labels[i];
			
			if(@label == null)
				continue;
			
			label.has_colour = false;
			result++;
		}
		
		return result > 0;
	}
	
	int update_visibility(int start_layer, int end_layer, const bool visible)
	{
		if(!validate_layer_range(start_layer, end_layer, start_layer, end_layer))
			return 0;
		
		int result = 0;
		
		for(int i = start_layer; i <= end_layer; i++)
		{
			if(visibility[i] != visible)
			{
				visibility[i] = visible;
				result++;
			}
		}
		
		return result;
	}
	
	bool update_toggle_all_visibility(const bool visible)
	{
		return update_visibility(num_layers - 1, num_layers - 1, visible) > 0;
	}
	
	int get_visible_count(int start_layer, int end_layer)
	{
		if(!validate_layer_range(start_layer, end_layer, start_layer, end_layer))
			return 0;
		
		int result = 0;
		
		for(int i = start_layer; i <= end_layer; i++)
		{
			if(visibility[i])
			{
				result++;
			}
		}
		
		return result;
	}
	
	void initialise_layer_values(const int start_layer, const int end_layer, const int group, const bool visible)
	{
		for(int i = start_layer; i <= end_layer; i++)
		{
			visibility[i] = visible;
			groups[i] = group;
		}
	}
	
	void rebuild()
	{
		active_count = 0;
		active_width = 0;
		active_height = 0;
	}
	
	void rebuild_checkboxes(const bool use_default_layer_colours)
	{
		const float label_spacing		= is_nan(this.label_spacing)	? ui.style.spacing : this.label_spacing;
		const float layer_spacing		= is_nan(this.layer_spacing)	? ui.style.spacing : this.layer_spacing;
		
		for(int i = 0; i < num_layers; i++)
		{
			const bool visible = visibility[i];
			
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
					checkbox.fit(0);
					
					label.align_v = GraphicAlign::Middle;
					label.fit_to_contents();
					
					if(use_default_layer_colours)
					{
						const uint default_clr = get_default_layer_colour(i);
						
						if(default_clr != 0)
						{
							label.colour = default_clr;
						}
					}
					
					const float layer_height = max(checkbox._set_height + layer_spacing, label._set_height + layer_spacing);
					label.height = layer_height;
					checkbox.height = layer_height;
					
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
				active_width = max(active_width, checkbox._width + label._width - label.real_padding_left - label.real_padding_right + label_spacing);
				active_height += max(checkbox._height, label._height);
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
		
		int start_index = 0;
		int end_index = active_count;
		
		if(toggle_all_top && @all_checkbox != null && all_checkbox.visible)
		{
			start_index= -1;
			end_index--;
		}
		
		for(int i = start_index; i < end_index; i++)
		{
			const int index = i == -1 ? active_indices[num_layers - 1] : active_indices[i];
			Checkbox@ checkbox = @checkboxes[index];
			Label@ label = @labels[index];
			
			checkbox._y = y;
			label._y = y;
			label._width = active_width - checkbox._width;
			
			if(labels_first)
			{
				label.align_h = GraphicAlign::Right;
				checkbox._x = label._width;
				label._x = 0;
				label.padding_left = 0;
				label.padding_right = label_spacing;
			}
			else
			{
				label.align_h = GraphicAlign::Left;
				checkbox._x = 0;
				label._x = checkbox._width;
				label.padding_left = label_spacing;
				label.padding_right = 0;
			}
			
			y += checkbox._height;
		}
		
		width = active_width;
		height = active_height;
		
		validate_layout = false;
	}
	
	protected void force_single_select(Checkbox@ checkbox)
	{
		for(int i = 0; i < active_count; i++)
		{
			Checkbox@ other_checkbox = checkboxes[active_indices[i]];
			
			if(@other_checkbox == @checkbox)
				continue;
			
			other_checkbox.initialise_state(false);
		}
	}
	
	protected bool validate_layer_range(const int in_layer, const int in_end_layer, int &out layer, int &out end_layer)
	{
		layer = in_layer;
		end_layer = in_end_layer;
		
		if(layer < 0 || layer >= num_layers)
			return false;
		
		if(end_layer == -1)
		{
			end_layer = layer;
		}
		else
		{
			if(end_layer < 0) end_layer = 0;
			if(end_layer >= num_layers) end_layer = num_layers - 1;
			
			if(end_layer < layer)
			{
				const int end_layer_t = end_layer;
				end_layer = layer;
				layer = end_layer_t;
			}
		}
		
		return true;
	}
	
	protected void update_toggle_all_checkbox()
	{
		if(@all_checkbox == null)
			return;
		
		all_checkbox.initialise_state(calculate_all_state());
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
	
	protected uint get_default_layer_colour(const int layer)
	{
		if(layer <= 5)
			return 0xffe7e6a7;
		if(layer <= 11)
			return 0xffbea7e7;
		if(layer <= 17)
			return 0;
		if(layer == 18)
			return 0xff7bc4d9;
		if(layer == 19)
			return 0xff7bd98f;
		if(layer == 21 || layer == 22)
			return 0xff818181;
		
		return 0;
	}
	
	// Events
	
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
		
		// Prevent deselect
		if(!allow_deselect && !checkbox.checked)
		{
			checkbox.initialise_state(true);
			return;
		}
		
		// Single select - deselect all others
		if(!multi_select && checkbox.checked)
		{
			force_single_select(checkbox);
			update_toggle_all_checkbox();
			ui._dispatch_event(@select_event, select_event_type, layer_selector);
		}
		
		if(
			select_layer_group_modifier >= 0 && multi_select && (allow_deselect || checkbox.checked) &&
			ui._has_editor && ui._editor.key_check_gvb(select_layer_group_modifier))
		{
			const int group = groups[checkboxes.findByRef(@checkbox)];
			
			if(initialise_all_states(checkbox.checked, group) == 0)
			{
				update_toggle_all_checkbox();
				ui._dispatch_event(@select_event, select_event_type, layer_selector);
			}
		}
		else
		{
			update_toggle_all_checkbox();
			ui._dispatch_event(@select_event, select_event_type, layer_selector);
		}
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