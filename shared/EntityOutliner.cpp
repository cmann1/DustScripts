const int ATTACK_TYPE_IDLE = 0;
const int ENTITY_STATE_WALLRUN = 11;

float lerp(float a, float b, float x)
{
	return a * (1.0 - x) + b * x;
}

class script : callback_base
{
	
	EntityOutliner defaults;
	
	[text] EntityOutliner entity_outliner;
	
	script()
	{
		add_broadcast_receiver("EntityOutliner.update", this, "on_update");
	}
	
	void on_update(string id, message@ msg)
	{
		if(msg.get_int("defaults") == 1)
		{
			entity_outliner.enabled = defaults.enabled;
			entity_outliner.colour = defaults.colour;
			entity_outliner.draw_double = defaults.draw_double;
			entity_outliner.offset_x = defaults.offset_x;
			entity_outliner.offset_y = defaults.offset_y;
		}
		else
		{
			entity_outliner.enabled = msg.get_int("enabled") == 1;
			entity_outliner.colour = uint(msg.get_int("colour"));
			entity_outliner.draw_double = msg.get_int("draw_double") == 1;
			entity_outliner.offset_x = msg.get_float("offset_x");
			entity_outliner.offset_y = msg.get_float("offset_y");
		}
	}
	
	void on_level_start()
	{
		defaults = entity_outliner;
	}
	
	void step_post(int entities)
	{
		if(entity_outliner.enabled)
			entity_outliner.step(entities);
	}
	
	void draw(float sub_frame)
	{
		if(entity_outliner.enabled)
			entity_outliner.draw(sub_frame);
	}
	
}

class EntityOutliner
{
	
	[text] bool enabled = true;
	[colour,alpha] uint colour = 0xFFFFFFFF;
	[text] bool draw_double = false;
	[text] bool ignore_prisms = false;
	[text] bool outline_player = true;
	
	[text] int apple_layer = 18;
	[text] int apple_sublayer = 6;
	
	[text] int enemy_layer = 18;
	[text] int enemy_sublayer = 7;
	
	[text] int player_layer = 18;
	[text] int player_sublayer = 9;
	
	[text] float offset_x = 2;
	[text] float offset_y = -2;
	
	array<controllable@> entity_list;
	
	EntityOutliner()
	{
		
	}
	
	int f = 0;
	int fdraw = 0;
	void step(int entities)
	{
		entity_list.resize(0);
		
		for(int i = 0; i < entities; i++)
		{
			controllable@ c = entity_by_index(i).as_controllable();
			
			if(@c != null and c.life() > 0)
			{
				dustman@ dm = c.as_dustman();
				if(dm !is null and dm.dead())
					continue;
				
				if(ignore_prisms && c.type_name().substr(0, 14) == "enemy_tutorial")
					continue;
				
				entity_list.insertLast(c);
			}
		}
		f++;
		fdraw=f;
	}
	
	void draw(float sub_frame)
	{
		for(uint count = entity_list.length(), i = 0; i < count; i++)
		{
			controllable@ c = entity_list[i];
			sprites@ spr = c.get_sprites();
			
			string sprite_name;
			uint frame;
			float face;
			float draw_offset_x = c.draw_offset_x();
			float draw_offset_y = c.draw_offset_y();
			
			if(c.attack_state() == ATTACK_TYPE_IDLE)
			{
				sprite_name = c.sprite_index();
				frame = uint(c.state_timer());
				face = c.face();
			}
			else
			{
				sprite_name = c.attack_sprite_index();
				frame = uint(c.attack_timer());
				face = c.attack_face();
				
				if(c.state() == ENTITY_STATE_WALLRUN)
				{
					draw_offset_x = draw_offset_y = 0;
				}
			}
			
			frame = frame % spr.get_animation_length(sprite_name);
			
			const float x = lerp(c.prev_x(), c.x(), sub_frame) + draw_offset_x;
			const float y = lerp(c.prev_y(), c.y(), sub_frame) + draw_offset_y;
			
			const string type_name = c.type_name();
			int layer, sublayer;
			
			if(type_name == "hittable_apple")
			{
				layer = apple_layer;
				sublayer = apple_sublayer;
			}
			else if(type_name.substr(0, 5) == "dust_")
			{
				if(!outline_player)
					continue;
				
				layer = player_layer;
				sublayer = player_sublayer;
			}
			else
			{
				layer = enemy_layer;
				sublayer = enemy_sublayer;
			}
			
			if(layer == -1 or sublayer == -1)
				continue;
			
			spr.draw_world(
				layer, sublayer,
				sprite_name, frame, 0,
				x + offset_x, y + offset_y,
				c.rotation(), c.scale() * face, c.scale(), colour);
			
			if(draw_double)
			{
				spr.draw_world(
					layer, sublayer,
					sprite_name, frame, 0,
					x - offset_x, y - offset_y,
					c.rotation(), c.scale() * face, c.scale(), colour);
			}
		}
		fdraw++;
	}
	
}

class EntityOutlinerTrigger : trigger_base
{
	
	scripttrigger@ self;
	
	[text] bool defaults = false;
	[text] bool enabled = true;
	[colour,alpha] uint colour = 0xFFFFFFFF;
	[text] bool draw_double = false;
	
	[text] float offset_x = 2;
	[text] float offset_y = -2;
	
	[text] bool only_once = false;
	
	EntityOutlinerTrigger()
	{
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
	}
	
	void activate(controllable@ e)
	{
		for(uint i = 0; i < num_cameras(); i++)
		{
			entity@ player = controller_entity(i);
			if(@player != null and e.is_same(player))
			{
				message@ msg = create_message();
				msg.set_int("defaults", defaults ? 1 : 0);
				msg.set_int("enabled", enabled ? 1 : 0);
				msg.set_int("colour", colour);
				msg.set_int("draw_double", draw_double ? 1 : 0);
				msg.set_float("offset_x", offset_x);
				msg.set_float("offset_y", offset_y);
				broadcast_message("EntityOutliner.update", msg);
				
				if(only_once)
				{
					get_scene().remove_entity(self.as_entity());
				}
				
				return;
			}
		}
	}
	
}