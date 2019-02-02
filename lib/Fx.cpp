class Fx : trigger_base
{
	scene@ g;
	script@ script;
	scripttrigger@ self;
	
	sprites@ sprite;
	[text] string sprite_set;
	[text] string sprite_name;
	[text] uint palette;
	
	float rotation;
	float scale_x = 1;
	float scale_y = 1;
	uint colour;
	
	int layer = 18;
	int sublayer = 14;
	
	float start_frame = 0;
	float end_frame = -1;
	float frame = 0;
	float frame_rate;
	int frame_count;
	int loop = -1;
	
	Fx()
	{
	}
	
	Fx(string sprite_set, string sprite_name, uint palette=0, float fps=15, float rotation=0, float scale_x=1, float scale_y=1, uint colour=0xFFFFFFFF)
	{
		this.sprite_set = sprite_set;
		this.sprite_name = sprite_name;
		this.palette = palette;
		this.rotation = rotation;
		this.scale_x = scale_x;
		this.scale_y = scale_y;
		this.colour = colour;
		
		frame_rate = fps / 60.0;
	}
	
	void init(script@ script, scripttrigger@ self)
	{
		@this.script = @script;
		@this.self = @self;
		
		@sprite = create_sprites();
		sprite.add_sprite_set(sprite_set);
		frame_count = sprite.get_animation_length(sprite_name);
		
		if(frame_rate < 0) frame = end_frame;
		
		if(end_frame == -1)
		{
			end_frame = frame_count;
		}
	}
	
	Fx@ set_layer(int layer, int sub_layer)
	{
		this.layer = layer;
		this.sublayer = sub_layer;
		return this;
	}
	
	void step()
	{
		frame += frame_rate;
		
		if(frame_rate > 0)
		{
			if(frame > end_frame)
			{
				if(loop > 0)
				{
					frame -= end_frame - start_frame;
					loop--;
				}
				else
				{
					get_scene().remove_entity(self.as_entity());
				}
			}
		}
		else
		{
			if(frame < start_frame)
			{
				if(loop > 0)
				{
					frame = end_frame + frame;
					loop--;
				}
				else
				{
					get_scene().remove_entity(self.as_entity());
				}
			}
		}
	}
	
	void draw(float sub_frame)
	{
		sprite.draw_world(layer, sublayer, sprite_name,
				uint(frame), palette, self.x(), self.y(), rotation,
				scale_x, scale_y, colour);
	}
	
}

Fx@ spawn_fx(float x, float y, string sprite_set, string sprite_name, uint palette=0, float fps=15, float rotation=0, float scale_x=1, float scale_y=1, uint colour=0xFFFFFFFF)
{
	Fx@ fx = Fx(sprite_set, sprite_name, palette, fps, rotation, scale_x, scale_y, colour);
	scripttrigger@ st = create_scripttrigger(fx);
	st.x(x);
	st.y(y);
	get_scene().add_entity(@st.as_entity(), false);
	
	return fx;
}