#include '../../../../lib/ui3/elements/LayerButton.cpp';
#include '../../../../lib/ui3/elements/RotationWheel.cpp';
#include '../../../../lib/ui3/elements/Select.cpp';
#include '../../../../lib/ui3/elements/Window.cpp';

class EmitterToolWindow
{
	
	private AdvToolScript@ script;
	private EmitterTool@ tool;
	
	private Window@ window;
	private Select@ emitter_id_select;
	private LayerButton@ layer_button;
	private RotationWheel@ rotation_wheel;
	
	private Button@ other_ids_button;
	private Image@ other_ids_img;
	private ListView@ other_ids_list_view;
	private PopupOptions@ other_ids_popup;
	private PopupOptions@ other_ids_tooltip;
	
	IntSetting@ emitter_id;
	IntSetting@ layer;
	IntSetting@ sublayer;
	FloatSetting@ rotation;
	
	private void create_ui()
	{
		UI@ ui = script.ui;
		Style@ style = ui.style;
		
		@window = Window(ui, 'Emitter Properties', false, true);
		window.name = 'EmitterToolWindow';
		window.x = 20;
		window.y = 20;
		window.drag_anywhere = true;
		window.title_bar_height = 32;
		window.title_label.set_padding(style.spacing, 2);
		
		// Emitter Id
		
		@emitter_id_select = Select(ui, '- No emitter selected -');
		emitter_id_select.width = 260;
		emitter_id_select.allow_custom_value = true;
		@emitter_id_select.tooltip = PopupOptions(ui, 'Emitter Id', false, PopupPosition::Right);
		window.add_child(emitter_id_select);
		
		for(uint i = 0; i < Emitters::MainEmitterNames.length(); i++)
		{
			emitter_id_select.add_value(Emitters::MainEmitterNames[i], Emitters::MainEmitterNames[i]);
		}
		
		emitter_id_select.selected_value = Emitters::MainEmitterNames[Emitters::MainEmitterIds.find(emitter_id.value)];
		emitter_id_select.change.on(EventCallback(on_emitter_id_change));
		
		// Other Emitters
		
		@other_ids_button = Button(ui, 'editor', 'emittericon');
		other_ids_button.icon.width  = Settings::IconSize;
		other_ids_button.icon.height = Settings::IconSize;
		other_ids_button.fit_to_contents();
		other_ids_button.x = emitter_id_select.x + emitter_id_select.width - other_ids_button.width;
		other_ids_button.y = emitter_id_select.y + emitter_id_select.height + style.spacing;
		@other_ids_button.tooltip = PopupOptions(ui, 'Other Emitters', false, PopupPosition::Above);
		other_ids_button.mouse_click.on(EventCallback(on_other_ids_button_click));
		window.add_child(other_ids_button);
		
		// Layer
		
		@layer_button = LayerButton(ui);
		layer_button.y = emitter_id_select.y + emitter_id_select.height + style.spacing;
		@layer_button.tooltip = PopupOptions(ui, 'Layer');
		layer_button.layer_select.set_selected_layer(layer.value);
		layer_button.layer_select.set_selected_sub_layer(sublayer.value);
		layer_button.layer_select.layer_select.on(EventCallback(on_layer_change));
		layer_button.layer_select.sub_layer_select.on(EventCallback(on_layer_change));
		window.add_child(layer_button);
		
		// Rotation
		
		@rotation_wheel = RotationWheel(ui);
		rotation_wheel.allow_range = false;
		rotation_wheel.auto_tooltip = true;
		rotation_wheel.tooltip_prefix = 'Rotation: ';
		rotation_wheel.x = layer_button.x + layer_button.width + style.spacing;
		rotation_wheel.y = layer_button.y;
		rotation_wheel.degrees = rotation.value;
		rotation_wheel.change.on(EventCallback(on_rotation_change));
		window.add_child(rotation_wheel);
		
		layer_button.height = rotation_wheel.height;
		
		// Finish
		
		window.fit_to_contents(true);
		script.window_manager.register_element(window);
		ui.add_child(window);
	}
	
	void show(AdvToolScript@ script, EmitterTool@ tool)
	{
		if(@this.script == null)
		{
			@this.script = script;
			@this.tool = tool;
			
			@emitter_id	= script.get_int(tool, 'emitter_id', EmitterId::DustGround);
			@layer		= script.get_int(tool, 'layer', 19);
			@sublayer	= script.get_int(tool, 'sublayer', 12);
			@rotation	= script.get_float(tool, 'rotation', 0);
			
			create_ui();
		}
		
		window.show();
	}
	
	void hide()
	{
		window.hide();
	}
	
	private void create_other_ids_popup()
	{
		if(@other_ids_list_view != null)
			return;
		
		@other_ids_list_view = ListView(script.ui);
		other_ids_list_view.border_size = 0;
		other_ids_list_view.border_colour = 0;
		other_ids_list_view.background_colour = 0;
		other_ids_list_view.select.on(EventCallback(on_other_ids_select));
		
		for(uint i = 0; i < Emitters::OtherEmitterNames.length(); i++)
		{
			ListViewItem@ item = other_ids_list_view.add_item(Emitters::OtherEmitterNames[i], Emitters::OtherEmitterNames[i]);
		}
		
		@other_ids_popup = PopupOptions(script.ui, other_ids_list_view, true, PopupPosition::Right, PopupTriggerType::Manual, PopupHideType::MouseDownOutside);
		other_ids_popup.wait_for_mouse = true;
		other_ids_popup.allow_target_overlap = false;
		other_ids_popup.spacing = script.ui.style.spacing;
		other_ids_popup.padding = 0;
		other_ids_popup.show.on(EventCallback(on_other_ids_popup_show));
		other_ids_popup.hide.on(EventCallback(on_other_ids_popup_hide));
	}
	
	// //////////////////////////////////////////////////////////
	// Events
	// //////////////////////////////////////////////////////////
	
	void on_emitter_id_change(EventInfo@ event)
	{
		puts('on_emitter_id_change: ' + emitter_id_select.selected_value);
		emitter_id.value = Emitters::MainEmitterIds[emitter_id_select.selected_index];
	}
	
	void on_layer_change(EventInfo@ event)
	{
		puts('on_layer_change: ' + layer_button.layer_select.get_selected_layer() + '.' + layer_button.layer_select.get_selected_sub_layer());
		layer.value = layer_button.layer_select.get_selected_layer();
		sublayer.value = layer_button.layer_select.get_selected_sub_layer();
	}
	
	void on_rotation_change(EventInfo@ event)
	{
		puts('on_rotation_change: ' + rotation_wheel.degrees);
		rotation.value = rotation_wheel.degrees;
	}
	
	void on_other_ids_button_click(EventInfo@ event)
	{
		create_other_ids_popup();
		
		other_ids_button.selected = true;
		other_ids_button.selectable = true;
		other_ids_list_view.fit_to_contents(true);
		
		other_ids_list_view.select_none();
		const int other_id_index = Emitters::OtherEmitterIds.find(emitter_id.value);
		
		if(other_id_index != -1)
		{
			int index;
			ListViewItem@ selected_item = other_ids_list_view.get_item(Emitters::OtherEmitterNames[other_id_index], index);
			
			if(@selected_item != null)
			{
				selected_item.selected = true;
				@other_ids_list_view.content.scroll_into_view = selected_item;
			}
		}
		
		script.ui.show_tooltip(other_ids_popup, other_ids_button);
	}
	
	void on_other_ids_select(EventInfo@ event)
	{
		if(other_ids_list_view.selected_index == -1)
			return;
		
		puts('on_other_ids_select: ' + event.value + ' [' + other_ids_list_view.selected_index + ']');
		
		puts(other_ids_list_view.selected_index);
		script.ui.hide_tooltip(other_ids_popup);
		emitter_id.value = Emitters::OtherEmitterIds[other_ids_list_view.selected_index];
	}
	
	void on_other_ids_popup_show(EventInfo@ event)
	{
		other_ids_button.tooltip.enabled = false;
	}
	
	void on_other_ids_popup_hide(EventInfo@ event)
	{
		other_ids_button.selectable = false;
		other_ids_button.tooltip.enabled = true;
	}
	
}