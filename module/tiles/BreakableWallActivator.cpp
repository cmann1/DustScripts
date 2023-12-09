#include 'BreakableWall.cpp';

class BreakableWallActivator : trigger_base
{
	
	[entity,enemy] uint wall_id;
	
	scripttrigger@ self;
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
	}
	
	void activate(controllable@ c)
	{
		if(c.player_index() == -1)
			return;
		
		scriptenemy@ se = scriptenemy_by_id(wall_id);
		BreakableWall@ bw = @se != null ? cast<BreakableWall@>(se.get_object()) : null;
		if(@bw != null)
		{
			bw.active = true;
		}
		
		get_scene().remove_entity(self.as_entity());
	}
	
}
