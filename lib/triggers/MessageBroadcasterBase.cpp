mixin class MessageBroadcasterBase
{
	
	scripttrigger@ self;
	script@ script;
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = script;
		@this.self = self;
	}
	
	void on_add()
	{
		if(remove_event != '')
		{
			add_remove_event();
		}
	}
	
	void on_remove()
	{
		if(remove_event != '')
		{
			remove_remove_event();
		}
	}
	
	void activate(controllable@ c)
	{
		activate_enter_exit(c);
	}
	
	void step()
	{
		step_enter_exit();
	}
	
	bool can_trigger_enter_exit(controllable@ c)
	{
		if(c.player_index() != -1 && !players)
			return false;
		
		const string type = c.type_name();
		
		if(!apples && type == 'hittable_apple')
			return false;
		
		if(!enemies && (
			type == 'enemy_tutorial_square' ||
			type == 'enemy_tutorial_hexagon' ||
			type.substr(0, 6) == 'enemy_'))
			return false;
		
		return true;
	}
	
	void on_trigger_enter(controllable@ c)
	{
		if(group && entities_enter_exit_list_count > 1)
			return;
		
		message@ msg = create_message();
		msg.set_entity('broadcaster', self.as_entity());
		msg.set_entity('entity', c.as_entity());
		
		if(key != '')
		{
			msg.set_int(key, value);
		}
		
		entity@ e = entity_id != 0 ? entity_by_id(entity_id) : null;
		
		if(@e != null)
		{
			e.send_message(id, msg);
		}
		else
		{
			broadcast(id, msg);
		}
		
		if(once)
		{
			get_scene().remove_entity(self.as_entity());
		}
	}
	
	//void on_trigger_exit(controllable@ c)
	//{
	//	puts('on_trigger_exit ' + (c.player_index() != -1 ? -c.player_index() : c.id()));
	//}
	
	void do_remove(const string &in, message@)
	{
		get_scene().remove_entity(self.as_entity());
	}
	
}
