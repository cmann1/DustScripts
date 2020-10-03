class PropToolToolbar
{
	
	private AdvToolScript@ script;
	private PropTool@ tool;
	
	private Toolbar@ toolbar;
	private PopupOptions@ info_popup;
	private Label@ info_label;
	
	private Button@ custom_grid_button;
	private PopupOptions@ custom_grid_tooltip;
	private PopupOptions@ custom_grid_popup;
	private NumberSlider@ custom_grid_slider;
	
	private Button@ origin_button;
	private Image@ origin_img;
	private ListView@ origin_list_view;
	private PopupOptions@ origin_popup;
	private PopupOptions@ origin_tooltip;
	
	private Button@ align_button;
	private PopupOptions@ align_tooltip;
	private PopupOptions@ align_popup;
	
	// Settings
	
	private BoolSetting@ pick_through_tiles;
	private FloatSetting@ custom_grid;
	private StringSetting@ default_origin;
	private BoolSetting@ custom_anchor_lock;
	private BoolSetting@ show_selection;
	private BoolSetting@ show_info;
	
	void build_sprites(message@ msg)
	{
		build_sprite(msg, 'origin_centre');
		build_sprite(msg, 'origin_top');
		build_sprite(msg, 'origin_top_left');
		build_sprite(msg, 'prop_tool_align_centre');
		build_sprite(msg, 'prop_tool_align_left');
		build_sprite(msg, 'prop_tool_custom_anchor_lock');
		build_sprite(msg, 'prop_tool_custom_grid');
		build_sprite(msg, 'prop_tool_dist_centre');
		build_sprite(msg, 'prop_tool_dist_left');
		build_sprite(msg, 'prop_tool_show_info');
		build_sprite(msg, 'prop_tool_show_selection');
		build_sprite(msg, 'prop_tool_tiles_blocking');
	}
	
	void show(AdvToolScript@ script, PropTool@ tool)
	{
		if(@this.script == null)
		{
			@this.script = script;
			@this.tool = tool;
			
			init_settings();
			create_ui();
		}
		
		script.ui.add_child(toolbar);
	}
	
	void hide()
	{
		script.ui.remove_child(toolbar);
	}
	
	private void init_settings()
	{
		@pick_through_tiles	= @tool.pick_through_tiles;
		@custom_grid		= @tool.custom_grid;
		@default_origin		= @tool.default_origin;
		@custom_anchor_lock	= @tool.custom_anchor_lock;
		@show_selection		= @tool.show_selection;
		@show_info			= @tool.show_info;
	}
	
	private void create_ui()
	{
		UI@ ui = script.ui;
		Style@ style = ui.style;
		
		@toolbar = Toolbar(ui, true, true);
		toolbar.name = 'PropToolToolbar';
		toolbar.x = 20;
		toolbar.y = 20;
		
		Button@ button;
		EventCallback@ button_click = EventCallback(on_toolbar_button_click);
		
		// Pick through tiles button
		
		@button = toolbar.add_button('script', 'prop_tool_tiles_blocking', Settings::IconSize, Settings::IconSize);
		button.name = 'pick_through_tiles';
		button.selectable = true;
		button.selected = pick_through_tiles.value;
		@button.tooltip = PopupOptions(ui, 'Ignore Tiles');
		button.mouse_click.on(button_click);
		
		// Custom grid button
		
		@button = @custom_grid_button = toolbar.add_button('script', 'prop_tool_custom_grid', Settings::IconSize, Settings::IconSize);
		button.name = 'custom_grid';
		@button.tooltip = @custom_grid_tooltip = PopupOptions(ui, '');
		button.mouse_click.on(button_click);
		update_custom_grid_tooltip();
		
		@custom_grid_slider = NumberSlider(ui);
		custom_grid_slider.value = custom_grid.value;
		custom_grid_slider.change.on(EventCallback(on_custom_grid_slider_change));
		
		@custom_grid_popup = PopupOptions(ui, custom_grid_slider, true, PopupPosition::Below, PopupTriggerType::Manual, PopupHideType::MouseDownOutside, false);
		custom_grid_popup.wait_for_mouse = true;
		custom_grid_popup.spacing = style.spacing;
		custom_grid_popup.padding = 0;
		custom_grid_popup.background_colour = 0;
		custom_grid_popup.blur_inset = 0;
		custom_grid_popup.show.on(EventCallback(on_custom_grid_slider_popup_show));
		custom_grid_popup.hide.on(EventCallback(on_custom_grid_slider_popup_hide));
		
		// Anchor button
		
		float icon_rotation;
		@origin_img = Image(ui, '', '', Settings::IconSize, Settings::IconSize);
		update_origin_icon();
		
		@origin_button = toolbar.add_button(origin_img);
		origin_button.name = 'default_origin';
		@origin_button.tooltip = PopupOptions(ui, '');
		origin_button.mouse_click.on(button_click);
		update_origin_tooltip();
		
		@origin_list_view = ListView(ui);
		origin_list_view.border_size = 0;
		origin_list_view.border_colour = 0;
		origin_list_view.background_colour = 0;
		origin_list_view.select.on(EventCallback(on_origin_select));
		
		for(int i = 0, count = int(PropToolSettings::Origins.length()); i < count; i++)
		{
			float rotation;
			ListViewItem@ item = origin_list_view.add_item(PropToolSettings::Origins[i],
				'script', get_origin_icon(PropToolSettings::Origins[i], rotation),
				string::nice(PropToolSettings::Origins[i]), Settings::IconSize, Settings::IconSize);
			item.icon.rotation = rotation;
		}
		
		@origin_popup = PopupOptions(ui, origin_list_view, true, PopupPosition::Below, PopupTriggerType::Manual, PopupHideType::MouseDownOutside);
		origin_popup.wait_for_mouse = true;
		origin_popup.allow_target_overlap = false;
		origin_popup.spacing = style.spacing;
		origin_popup.padding = 0;
		origin_popup.show.on(EventCallback(on_origin_slider_popup_show));
		origin_popup.hide.on(EventCallback(on_origin_slider_popup_hide));
		
		// Custom anchor lock button
		
		@button = toolbar.add_button('script', 'prop_tool_custom_anchor_lock', Settings::IconSize, Settings::IconSize);
		button.name = 'custom_anchor_lock';
		button.selectable = true;
		button.selected = custom_anchor_lock.value;
		@button.tooltip = PopupOptions(ui, 'Lock custom anchor');
		button.mouse_click.on(button_click);
		
		// Align button
		
		@align_button = toolbar.add_button('script', 'prop_tool_align_left', Settings::IconSize, Settings::IconSize);
		align_button.name = 'align';
		@align_button.tooltip = PopupOptions(ui, 'Align');
		align_button.mouse_click.on(button_click);
		
		@align_popup = PopupOptions(ui, 'ALIGN ME BABY!', true, PopupPosition::Below, PopupTriggerType::Manual, PopupHideType::MouseDownOutside, false);
		align_popup.wait_for_mouse = true;
		align_popup.spacing = style.spacing;
		align_popup.show.on(EventCallback(on_align_popup_show));
		align_popup.hide.on(EventCallback(on_align_popup_hide));
		
		// Selection button
		
		toolbar.add_divider();
		
		@button = toolbar.add_button('script', 'prop_tool_show_selection', Settings::IconSize, Settings::IconSize);
		button.name = 'show_selection';
		button.selectable = true;
		button.selected = show_selection.value;
		@button.tooltip = PopupOptions(ui, 'Always show selection');
		button.mouse_click.on(button_click);
		
		// Info button
		
		@button = toolbar.add_button('script', 'prop_tool_show_info', Settings::IconSize, Settings::IconSize);
		button.name = 'show_info';
		button.selectable = true;
		button.selected = show_info.value;
		@button.tooltip = PopupOptions(ui, 'Show prop info');
		button.mouse_click.on(button_click);
		
		// Info popup
		
		@info_label = Label(ui, '', true, font::SANS_BOLD, 20);
		info_label.scale_x = 0.75;
		info_label.scale_y = 0.75;
		
		@info_popup = PopupOptions(ui, info_label, false, PopupPosition::BelowLeft, PopupTriggerType::Manual, PopupHideType::Manual);
		info_popup.spacing = 0;
		info_popup.background_colour = multiply_alpha(style.normal_bg_clr, 0.5);
		
		ui.add_child(toolbar);
		script.window_manager.register_element(toolbar);
	}
	
	private void update_custom_grid_tooltip()
	{
		custom_grid_tooltip.content_string = 'Custom grid: ' + int(custom_grid.value) + 'px';
	}
	
	private void update_origin_tooltip()
	{
		origin_button.tooltip.content_string = 'Origin: ' + string::nice(default_origin.value);
	}
	
	private string get_origin_icon(const string origin, float &out rotation)
	{
		const int index = PropToolSettings::Origins.find(origin);
		
		if(index <= 0)
		{
			rotation = 0;
			return 'origin_centre';
		}
		
		rotation = (index - 1) / 2 * 90;
		return ((index - 1) % 2) == 0 ? 'origin_top_left' : 'origin_top';
	}
	
	private void update_origin_icon()
	{
		float rotation;
		origin_img.set_sprite('script', get_origin_icon(default_origin.value, rotation), Settings::IconSize, Settings::IconSize);
		origin_img.rotation = rotation;
	}
	
	// //////////////////////////////////////////////////////////
	// Methods
	// //////////////////////////////////////////////////////////
	
	void show_prop_info(PropData@ prop_data)
	{
		if(!show_info.value)
			return;
		
		prop@ p = prop_data.prop;
		int index = prop_index_to_array_index(p.prop_set(), p.prop_group(), p.prop_index());
		const PropIndex@ prop_index = @PROP_INDICES[p.prop_group()][index];
		
		info_label.text =
			// Group and name
			string::nice(PROP_GROUP_NAMES[p.prop_group()]) + '::' + prop_index.name + '\n' +
			// Layer
			p.layer() + '.' + p.sub_layer();
		
		script.ui.show_tooltip(info_popup, toolbar);
	}
	
	void hide_info_popup()
	{
		script.ui.hide_tooltip(info_popup);
	}
	
	// //////////////////////////////////////////////////////////
	// Events
	// //////////////////////////////////////////////////////////
	
	private void on_toolbar_button_click(EventInfo@ event)
	{
		Button@ button = cast<Button@>(event.target);
		const string name = button.name;
		
		if(name == 'pick_through_tiles')
		{
			pick_through_tiles.value = button.selected;
		}
		else if(name == 'custom_grid')
		{
			custom_grid_button.selected = true;
			custom_grid_button.selectable = true;
			script.ui.show_tooltip(custom_grid_popup, custom_grid_button);
		}
		else if(name == 'default_origin')
		{
			int index;
			origin_button.selected = true;
			origin_button.selectable = true;
			origin_list_view.fit_to_contents(true);
			
			ListViewItem@ selected_item = origin_list_view.get_item(default_origin.value, index);
			
			if(@selected_item != null)
			{
				selected_item.selected = true;
				@origin_list_view.content.scroll_into_view = selected_item;
			}
			
			script.ui.show_tooltip(origin_popup, origin_button);
		}
		else if(name == 'custom_anchor_lock')
		{
			custom_anchor_lock.value = button.selected;
		}
		else if(name == 'show_selection')
		{
			show_selection.value = button.selected;
		}
		else if(name == 'show_info')
		{
			show_info.value = button.selected;
		}
		else if(name == 'align')
		{
			align_button.selected = true;
			align_button.selectable = true;
			script.ui.show_tooltip(align_popup, align_button);
		}
	}
	
	// Grid
	
	private void on_custom_grid_slider_change(EventInfo@ event)
	{
		custom_grid.value = round(custom_grid_slider.value);
		update_custom_grid_tooltip();
	}
	
	private void on_custom_grid_slider_popup_show(EventInfo@ event)
	{
		custom_grid_tooltip.enabled = false;
	}
	
	private void on_custom_grid_slider_popup_hide(EventInfo@ event)
	{
		custom_grid_button.selectable = false;
		custom_grid_tooltip.enabled = true;
	}
	
	// Origin
	
	private void on_origin_select(EventInfo@ event)
	{
		if(default_origin.value == event.value)
			return;
		
		script.ui.hide_tooltip(origin_popup);
		default_origin.value = event.value;
		tool.update_alignments_from_origin();
		
		update_origin_icon();
		update_origin_tooltip();
	}
	
	private void on_origin_slider_popup_show(EventInfo@ event)
	{
		origin_button.tooltip.enabled = false;
	}
	
	private void on_origin_slider_popup_hide(EventInfo@ event)
	{
		origin_button.selectable = false;
		origin_button.tooltip.enabled = true;
	}
	
	// Align
	
	private void on_align_popup_show(EventInfo@ event)
	{
		align_button.tooltip.enabled = false;
	}
	
	private void on_align_popup_hide(EventInfo@ event)
	{
		align_button.selectable = false;
		align_button.tooltip.enabled = true;
	}
}