class script
{
	
	scene@ g;
	bool in_game = false;
	
	controllable@ player = null;
	camera@ cam;
	
	script()
	{
		@g = get_scene();
		@cam = get_camera(0);
	}
	
	void on_level_start()
	{
		in_game = true;
	}
	
}

class SetEmitterVarBaseTrigger : trigger_base
{
	
	scene@ g;
	scripttrigger @self;
	
	[text] bool run = false;
	[text] int filter_layer = -1;
	
	bool run_prev = false;
	
	SetEmitterVarBaseTrigger()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
		self.square(true);
		run_prev = run;
	}
	
	void _run()
	{
		const float x = self.x();
		const float y = self.y();
		const float r = self.radius();
		int count = g.get_entity_collision(y - r, y + r, x - r, x + r, 13);
		
		array<entity@> to_update;
		int to_update_count = 0;
		
		for(int i = 0; i < count; i++)
		{
			entity@ emitter = g.get_entity_collision_index(i);
			if(emitter is null)
			{
				puts('NULL?');
				continue;
			}
			varstruct@ vars = emitter.vars();
			
//			print_vars(emitter);
			
			if(filter_layer != -1 && emitter.layer() != filter_layer)
				continue;
			
			to_update.insertLast(@emitter);
			to_update_count++;
			setProperties(vars);
		}
		
		for(int i = to_update_count - 1; i >= 0; --i)
		{
			entity@ emitter = to_update[i];
			varstruct@ vars = emitter.vars();
			entity@ new_emitter = create_entity('entity_emitter');
			new_emitter.set_xy(emitter.x(), emitter.y());
			new_emitter.layer(emitter.layer());
			varstruct@ new_vars = new_emitter.vars();
			new_vars.get_var('draw_depth_sub').set_int32(vars.get_var('draw_depth_sub').get_int32());
			new_vars.get_var('e_rotation').set_int32(vars.get_var('e_rotation').get_int32());
			new_vars.get_var('emitter_id').set_int32(vars.get_var('emitter_id').get_int32());
			new_vars.get_var('height').set_int32(vars.get_var('height').get_int32());
			new_vars.get_var('r_area').set_bool(vars.get_var('r_area').get_bool());
			new_vars.get_var('r_rotation').set_bool(vars.get_var('r_rotation').get_bool());
			new_vars.get_var('width').set_int32(vars.get_var('width').get_int32());
			
			g.add_entity(new_emitter);
			g.remove_entity(emitter);
		}
	}
	
	void setProperties(varstruct@ vars) {  }
	
	void editor_step()
	{
		if(run_prev != run)
		{
			_run();
			run_prev = run;
		}
	}
	
	void activate(controllable @e) {}
	
}

class SetEmitterSubLayer : SetEmitterVarBaseTrigger
{
	
	[text] int target_sub_layer = 12;
	
	void setProperties(varstruct@ vars)
	{
		vars.get_var('draw_depth_sub').set_int32(target_sub_layer);
	}
	
}

class SetEmitterRotation : SetEmitterVarBaseTrigger
{
	
	[angle] int rotation = 0;
	
	void setProperties(varstruct@ vars)
	{
		vars.get_var('e_rotation').set_int32(rotation);
		vars.get_var('r_rotation').set_bool(true);
	}
	
}


class EmitterTrigger : trigger_base
{
	
	scene@ g;
	scripttrigger @self;
	
	bool run_prev = false;
	[text] bool run = false;
	
	EmitterTrigger()
	{
		@g = get_scene();
	}
	
	void _run()
	{
		const float x = self.x();
		const float y = self.y();
		const float r = self.radius();
		int entity_count = g.get_entity_collision(y-r, y+r, x-r, x+r, 13);
		
		for(int i = 0; i < entity_count; i++)
		{
			entity@ emitter = g.get_entity_collision_index(i);
			
			varstruct@ vars = emitter.vars();
			
			int layer = emitter.layer();
			int id = vars.get_var("emitter_id").get_int32();
			int width = vars.get_var("width").get_int32();
			int height = vars.get_var("height").get_int32();
			int sub_layer = vars.get_var("draw_depth_sub").get_int32();
			float ex = emitter.x();
			float ey = emitter.y();
			
			puts('create_emitter(' + id + ', ' + ex + ', ' + ey + ', ' + width + ', ' + height + ', ' + layer + ', ' + sub_layer + ');');
		}
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
		self.square(true);
	}
	
	void editor_step()
	{
		if(run_prev != run)
		{
			_run();
			run_prev = run;
		}
	}
	
	void activate(controllable @e) {}
	
}