class RemoveTimer : trigger_base
{
	
	scripttrigger@ self;
	
	int timer = 0;
	int duration = 0;
	entity@ e;
	
	RemoveTimer()
	{
	}
	
	RemoveTimer(entity@ e, int duration)
	{
		@this.e = e;
		this.duration = duration;
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
	}
	
	void step()
	{
		if(++timer >= duration)
		{
			if(@e != null)
				get_scene().remove_entity(e);
			get_scene().remove_entity(self.as_entity());
		}
	}
	
}

RemoveTimer@ remove_timer(entity@ e, int duration)
{
	RemoveTimer@ timer = RemoveTimer(e, duration);
	scripttrigger@ st = create_scripttrigger(timer);
	st.set_xy(e.x(), e.y());
	get_scene().add_entity(@st.as_entity(), false);
	
	return timer;
}