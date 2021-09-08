#include '../../lib/enums/ColType.cpp';
#include '../../lib/enums/EntityState.cpp';
#include '../../lib/enums/AttackType.cpp';
#include '../../lib/math/math.cpp';
#include '../../lib/utils/colour.cpp';

#include 'EntityOutlinerSettings.cpp';
#include 'Triggers.cpp';

class EntityOutliner : callback_base
{
	
	private EntityOutlinerSettings@ defaults;
	private array<EntityOutlinerSettings> settings;
	private array<EntityOutlinerSettings> from_settings;
	private array<EntityOutlinerSettings> target_settings;
	private EntityOutlinerSettings@ active_settings;
	
	private array<EntityOutlinerSettings> checkpoint_settings;
	private array<EntityOutlinerSettings> checkpoint_from_settings;
	private array<EntityOutlinerSettings> checkpoint_target_settings;
	
	private EntityOutlinerTrigger@ selected_trigger;
	
	private scene@ g;
	private camera@ cam;
	private bool initialised;
	
	private int entity_list_size = 32;
	private array<ControllableOutlineData> entity_list(entity_list_size);
	private int num_entities;
	private int num_players;
	private float scale = 1;
	
	EntityOutliner()
	{
		@g = get_scene();
		num_players = num_cameras();
		settings.resize(num_players);
		from_settings.resize(num_players);
		target_settings.resize(num_players);
		@active_settings = @this.settings[0];
	}
	
	void init_settings(EntityOutlinerSettings@ settings)
	{
		@this.defaults = settings;
	}
	
	private void initialise()
	{
		for(int i = 0; i < num_players; i++)
		{
			settings[i] = defaults;
		}
		
		add_broadcast_receiver('', this, 'on_message');
		add_broadcast_receiver('EntityOutliner.editor', this, 'on_editor_message');
	}
	
	void checkpoint_load(camera@ cam)
	{
		settings = checkpoint_settings;
		from_settings = checkpoint_from_settings;
		target_settings = checkpoint_target_settings;
		
		for(int i = 0; i < num_players; i++)
		{
			EntityOutlinerSettings@ settings = @this.settings[i];
			if(settings.t > 0)
			{
				this.settings[i] = target_settings[i];
				settings.t = 0;
			}
		}
		
		// Add all entities on screen because step is not run for a few frames after a checkpoint is loaded
		get_entities_on_screen(cam);
	}
	
	void checkpoint_save()
	{
		checkpoint_settings = settings;
		checkpoint_from_settings = from_settings;
		checkpoint_target_settings = target_settings;
	}
	
	void editor_var_changed(var_info@ info, EntityOutlinerSettings@ settings)
	{
		defaults = settings;
		
		for(int i = 0; i < num_players; i++)
		{
			this.settings[i] = defaults;
		}
	}
	
	void step(camera@ cam)
	{
		if(!initialised)
		{
			initialise();
			initialised = true;
		}
		
		for(int i = 0; i < num_players; i++)
		{
			EntityOutlinerSettings@ settings = @this.settings[i];
			if(settings.t > 0)
			{
				settings.update_transition(@this.from_settings[i], @this.target_settings[i]);
			}
		}
		
		@active_settings = @this.settings[cam.player()];
		
		if(!active_settings.enabled)
			return;
		
		// Old way - iterate all entities
		/*num_entities = 0;
		while(entity_list_size < entities)
		{
			entity_list_size *= 2;
			entity_list.resize(entity_list_size);
		}
		
		for(int i = 0; i < entities; i++)
		{
			controllable@ c = entity_by_index(i).as_controllable();
			
			if(@c == null || !outline_prisms && c.type_name().substr(0, 14) == 'enemy_tutorial')
				continue;4
			
			@entity_list[num_entities++] = c;
		}*/
		
		@this.cam = cam;
		get_entities_on_screen(cam);
		scale = active_settings.scale_with_camera ? cam.screen_height() / 1080 : 1;
	}
	
	void editor_step(camera@ cam)
	{
		step(cam);
		scale = active_settings.scale_with_camera ? 1 / cam.editor_zoom() : 1;
	}
	
	void draw(float sub_frame)
	{
		const EntityOutlinerSettings@ settings = @active_settings;
		
		if(!settings.enabled)
			return;
		
		for(int i = 0; i < num_entities; i++)
		{
			ControllableOutlineData@ data = @entity_list[i];
			controllable@ c = data.c;
			if(c.destroyed())
				continue;
			
			sprites@ spr = c.get_sprites();
			
			string sprite_name;
			uint frame;
			float face;
			float draw_offset_x = c.draw_offset_x();
			float draw_offset_y = c.draw_offset_y();
			float rotation = c.rotation();
			
			const string type_name = c.type_name();
			
			if(c.attack_state() != AttackType::Idle && type_name != 'enemy_stoneboss')
			{
				sprite_name = c.attack_sprite_index();
				frame = uint(max(c.attack_timer(), 0.0));
				face = c.attack_face();
				
				if(c.state() == EntityState::WallRun)
				{
					draw_offset_x = draw_offset_y = 0;
				}
			}
			else
			{
				sprite_name = c.sprite_index();
				frame = uint(max(c.state_timer(), 0.0));
				face = c.face();
			}
			
			frame = frame % spr.get_animation_length(sprite_name);
			
			const float x = lerp(c.prev_x(), c.x(), sub_frame) + draw_offset_x;
			const float y = lerp(c.prev_y(), c.y(), sub_frame) + draw_offset_y;
			
			if(type_name == 'enemy_flag')
			{
				rotation = 0;
				// TODO: Fix stun offset for flags (I think it requires being able to get draw offset1 and 2 separately)
				// draw_offset_x = draw_offset_y = 0;
			}
			
			int layer1, sublayer1;
			int layer2, sublayer2;
			
			if(type_name == 'hittable_apple')
			{
				layer1 = settings.apple_layer1;
				sublayer1 = settings.apple_sublayer1;
				layer2 = settings.apple_layer2;
				sublayer2 = settings.apple_sublayer2;
			}
			else if(type_name.substr(0, 5) == 'dust_')
			{
				if(!settings.outline_player)
					continue;
				
				layer1 = settings.player_layer1;
				sublayer1 = settings.player_sublayer1;
				layer2 = settings.player_layer2;
				sublayer2 = settings.player_sublayer2;
			}
			else
			{
				layer1 = settings.enemy_layer1;
				sublayer1 = settings.enemy_sublayer1;
				layer2 = settings.enemy_layer2;
				sublayer2 = settings.enemy_sublayer2;
			}
			
			float offset1_x = settings.offset1_x;
			float offset1_y = settings.offset1_y;
			float scale1 = settings.scale1;
			uint colour1 = settings.colour1;
			
			if(data.t > 0)
			{
				if(sublayer1 != data.sub_layer)
				{
					spr.draw_world(
						layer1, data.sub_layer,
						sprite_name, frame, 0,
						x + data.offset_x * scale, y + data.offset_y * scale,
						rotation, c.scale() * data.scale * face, c.scale() * data.scale,
						multiply_alpha(data.colour, data.t));
					colour1 = multiply_alpha(colour1, 1 - data.t);
				}
				else
				{
					offset1_x = lerp(offset1_x, data.offset_x, data.t);
					offset1_y = lerp(offset1_y, data.offset_y, data.t);
					scale1 = lerp(scale1, data.scale, data.t);
					colour1 = colour::lerp(colour1, data.colour, data.t);
				}
			}
			
			spr.draw_world(
				layer1, sublayer1,
				sprite_name, frame, 0,
				x + offset1_x * scale, y + offset1_y * scale,
				rotation, c.scale() * scale1 * face, c.scale() * scale1, colour1);
			
			if(settings.draw_double)
			{
				if(layer2 < 0)
					layer2 = layer1;
				if(sublayer2 < 0)
					sublayer2 = sublayer1;
				const float offset_x = settings.offset2_x != 0 ? settings.offset2_x : settings.offset1_x;
				const float offset_y = settings.offset2_y != 0 ? settings.offset2_y : settings.offset1_y;
				
				spr.draw_world(
					layer2, sublayer2,
					sprite_name, frame, 0,
					x + offset_x * scale, y + offset_y * scale,
					rotation, c.scale() * settings.scale2 * face, c.scale() * settings.scale2, settings.colour2);
			}
		}
	}
	
	private void get_entities_on_screen(camera@ cam)
	{
		float view1_x, view1_y, view1_w, view1_h;
		float view2_x, view2_y, view2_w, view2_h;
		cam.get_layer_draw_rect(0, 19, view1_x, view1_y, view1_w, view1_h);
		cam.get_layer_draw_rect(1, 19, view2_x, view2_y, view2_w, view2_h);
		const float padding = 96;
		view1_x -= padding; view1_y -= padding;
		view2_x -= padding; view2_y -= padding;
		view1_w += padding * 2; view1_h += padding * 2;
		view2_w += padding * 2; view2_h += padding * 2;
		
		float view_x1 = min(view1_x, view2_x);
		float view_y1 = min(view1_y, view2_y);
		float view_x2 = max(view1_x + view1_w, view2_x + view2_w);
		float view_y2 = max(view1_y + view1_h, view2_y + view2_h);
		
		num_entities = 0;
		
		add_entities_type(view_y1, view_y2, view_x1, view_x2, ColType::Enemy);
		add_entities_type(view_y1, view_y2, view_x1, view_x2, ColType::Player);
	}
	
	private void add_entities_type(const float view_y1, const float view_y2, const float view_x1, const float view_x2, const ColType type)
	{
		const EntityOutlinerSettings@ settings = @active_settings;
		
		const int count = g.get_entity_collision(view_y1, view_y2, view_x1, view_x2, type);
		
		while(entity_list_size < num_entities + count + 4)
		{
			entity_list_size *= 2;
			entity_list.resize(entity_list_size);
		}
		
		for(int i = 0; i < count; i++)
		{
			controllable@ c = g.get_entity_collision_index(i).as_controllable();
			if(@c == null)
				continue;
			if(c.life() <= 0)
				continue;
			if(!settings.outline_prisms && c.type_name().substr(0, 14) == 'enemy_tutorial')
				continue;
			
			entity_list[num_entities++].reset(c);
		}
	}
	
	void on_message(string id, message@ msg)
	{
		if(id == 'EntityOutlinerSource')
		{
			const int c_id = msg.get_int('id');
			
			for(int i = 0; i < num_entities; i++)
			{
				ControllableOutlineData@ data = @entity_list[i];
				if(data.id != c_id)
					continue;
				
				data.sub_layer = msg.get_int('sub_layer');
				data.colour = uint(msg.get_int('colour'));
				data.offset_x = msg.get_float('offset_x');
				data.offset_y = msg.get_float('offset_y');
				data.scale = msg.get_float('scale');
				data.t = msg.get_float('t');
				break;
			}
			return;
		}
		
		if(id != 'EntityOutliner')
			return;
		
		entity@ e = msg.get_entity('trigger');
		scripttrigger@ st = @e != null ? e.as_scripttrigger() : null;
		EntityOutlinerReset@ et = @st != null ? cast<EntityOutlinerReset@>(st.get_object()) : null;
		if(@et == null)
			return;
		
		const int player = msg.get_int('player', -1);
		if(player < 0 || player >= num_players)
			return;
		
		EntityOutlinerSettings@ outline;
		
		EntityOutlinerTrigger@ ett = cast<EntityOutlinerTrigger@>(et);
		if(@ett != null)
		{
			@outline = @ett.outliner;
		}
		else
		{
			@outline = defaults;
		}
		
		if(et.transition > 0)
		{
			from_settings[player] = settings[player];
			target_settings[player] = outline;
			settings[player].transition = et.transition;
			settings[player].t = et.transition;
		}
		else
		{
			settings[player] = outline;
			settings[player].t = 0;
		}
	}
	
	void on_editor_message(string id, message@ msg)
	{
		const string event = msg.get_string('event');
		
		if(event == 'trigger_select')
		{
			entity@ e = msg.get_entity('trigger');
			scripttrigger@ st = @e != null ? e.as_scripttrigger() : null;
			@selected_trigger = @st != null ? cast<EntityOutlinerTrigger@>(st.get_object()) : null;
			
			if(@selected_trigger != null)
			{
				settings[0] = selected_trigger.outliner;
			}
		}
		else if(event == 'trigger_deselect')
		{
			@selected_trigger = null;
		}
		else if(event == 'trigger_update')
		{
			if(@selected_trigger != null)
			{
				settings[0] = selected_trigger.outliner;
			}
		}
	}
	
}

class ControllableOutlineData
{
	
	controllable@ c;
	int id;
	int sub_layer;
	float offset_x, offset_y;
	float scale;
	uint colour;
	float t;
	
	void reset(controllable@ c)
	{
		@this.c = c;
		id = c.id();
		t = 0;
	}
	
}
