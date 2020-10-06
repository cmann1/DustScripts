#include '../../../../lib/ui3/elements/extra/SelectButton.cpp';
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
	
	private SelectButton@ other_ids_button;
	private ListView@ other_ids_list_view;
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
		
		string emitter_name;
		get_emitter_name(emitter_id.value, emitter_name);
		emitter_id_select.selected_value = emitter_name;
		emitter_id_select.change.on(EventCallback(on_emitter_id_change));
		
		// Other Emitters
		
		@other_ids_button = SelectButton(ui, 'editor', 'emittericon');
		other_ids_button.icon.width  = Settings::IconSize;
		other_ids_button.icon.height = Settings::IconSize;
		other_ids_button.fit_to_contents();
		other_ids_button.x = emitter_id_select.x + emitter_id_select.width - other_ids_button.width;
		other_ids_button.y = emitter_id_select.y + emitter_id_select.height + style.spacing;
		@other_ids_button.tooltip = PopupOptions(ui, 'Other Emitters', false, PopupPosition::Right);
		other_ids_button.open.on(EventCallback(on_other_ids_open));
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
			
			@emitter_id	= tool.emitter_id;
			@layer		= tool.layer;
			@sublayer	= tool.sublayer;
			@rotation	= tool.rotation;
			
			create_ui();
		}
		
		window.show();
	}
	
	void hide()
	{
		window.hide();
	}
	
	private void populate_other_ids()
	{
		if(@other_ids_list_view != null)
			return;
		
		other_ids_button.popup.position = PopupPosition::Right;
		
		@other_ids_list_view = other_ids_button.list_view;
		other_ids_list_view.scroll_amount = 75;
		other_ids_list_view.select.on(EventCallback(on_other_ids_select));
		
		for(uint i = 0; i < Emitters::OtherEmitterNames.length(); i++)
		{
			ListViewItem@ item = other_ids_list_view.add_item(Emitters::OtherEmitterNames[i], Emitters::OtherEmitterNames[i]);
		}
	}
	
	private bool get_emitter_name(const int id, string &out name)
	{
		int index = Emitters::MainEmitterIds.find(id);
		
		if(index == -1)
		{
			index = Emitters::OtherEmitterIds.find(id);
			
			if(index == -1)
			{
				name = Emitters::MainEmitterNames[Emitters::MainEmitterIds[EmitterId::DustGround]];
				return false;
			}
			
			name = Emitters::OtherEmitterNames[index];
			return true;
		}
		else
		{
			name = Emitters::MainEmitterNames[index];
			return true;
		}
	}
	
	private void update_emitter_id(const int id)
	{
		puts('update_emitter_id: ' + id);
		emitter_id.value = id;
	}
	
	// //////////////////////////////////////////////////////////
	// Events
	// //////////////////////////////////////////////////////////
	
	private void on_emitter_id_change(EventInfo@ event)
	{
		if(emitter_id_select.selected_index == -1)
			return;
		
		update_emitter_id(Emitters::MainEmitterIds[emitter_id_select.selected_index]);
	}
	
	private void on_layer_change(EventInfo@ event)
	{
		puts('on_layer_change: ' + layer_button.layer_select.get_selected_layer() + '.' + layer_button.layer_select.get_selected_sub_layer());
		layer.value = layer_button.layer_select.get_selected_layer();
		sublayer.value = layer_button.layer_select.get_selected_sub_layer();
	}
	
	private void on_rotation_change(EventInfo@ event)
	{
		puts('on_rotation_change: ' + rotation_wheel.degrees);
		rotation.value = rotation_wheel.degrees;
	}
	
	private void on_other_ids_open(EventInfo@ event)
	{
		if(event.type != EventType::BEFORE_OPEN)
			return;
		
		populate_other_ids();
		
		other_ids_list_view.select.enabled = false;
		other_ids_list_view.select_none();
		
		const int other_id_index = Emitters::OtherEmitterIds.find(emitter_id.value);
		
		if(other_id_index != -1)
		{
			other_ids_list_view.set_selected_item(Emitters::OtherEmitterNames[other_id_index]);
		}
		
		other_ids_list_view.select.enabled = true;
	}
	
	private void on_other_ids_select(EventInfo@ event)
	{
		if(other_ids_list_view.selected_index == -1)
			return;
		
		other_ids_list_view.select.enabled = false;
		
		update_emitter_id(Emitters::OtherEmitterIds[other_ids_list_view.selected_index]);
		emitter_id_select.selected_value = event.value;
		
		other_ids_list_view.select.enabled = true;
	}
	
}