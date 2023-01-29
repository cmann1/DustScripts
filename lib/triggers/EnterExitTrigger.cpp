/// Tracks entities to provide enter and exit events for triggers.
mixin class EnterExitTrigger
{
	
	protected dictionary entities_enter_exit_map;
	protected array<controllable@> entities_enter_exit_list;
	protected array<int> timers_enter_exit;
	protected int entities_enter_exit_list_count;
	protected int entities_enter_exit_list_size;
	
	/// Must be called during normal trigger activation.
	void activate_enter_exit(controllable@ c)
	{
		if(!can_trigger_enter_exit(c))
			return;
		
		const string id = (c.player_index() != -1 ? -c.player_index() : c.id()) + '';
		
		if(entities_enter_exit_map.exists(id))
		{
			entities_enter_exit_map[id] = 3;
			return;
		}
		
		if(entities_enter_exit_list_count >= entities_enter_exit_list_size)
		{
			entities_enter_exit_list.resize(entities_enter_exit_list_size += 8);
		}
		
		// Persist for 3 frames because the player won't activate triggers when starting a dash
		entities_enter_exit_map[id] = 3;
		@entities_enter_exit_list[entities_enter_exit_list_count++] = c;
		
		on_trigger_enter(c);
	}
	
	/// Must be called during normal trigger step.
	void step_enter_exit()
	{
		for(int i = entities_enter_exit_list_count - 1; i >= 0 ; i--)
		{
			controllable@ c = entities_enter_exit_list[i];
			const string id = (c.player_index() != -1 ? -c.player_index() : c.id()) + '';
			const int timer = int(entities_enter_exit_map[id]);
			
			if(timer == 0)
			{
				@entities_enter_exit_list[i] = @entities_enter_exit_list[--entities_enter_exit_list_count];
				entities_enter_exit_map.delete(id);
				on_trigger_exit(c);
				continue;
			}
			
			entities_enter_exit_map[id] = timer - 1;
		}
	}
	
	/// Can be implemented to filter which entities will activate the trigger.
	bool can_trigger_enter_exit(controllable@ c)
	{
		return true;
	}
	
	/// Implement to receive events when entities enter the trigger for the first time.
	void on_trigger_enter(controllable@ c)
	{
		
	}
	
	/// Implement to receive events when entities exit the trigger.
	void on_trigger_exit(controllable@ c)
	{
		
	}
	
}
