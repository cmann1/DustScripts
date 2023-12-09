/** Removes one or more entities when the trigger is activated. */
class EntityRemover : trigger_base
{
	
	[entity] array<uint> entity_ids;
	[persist] bool player_only = true;
	
	scene@ g;
	scripttrigger@ self;
	
	EntityRemover()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
	}
	
	void activate(controllable@ c)
	{
		if(player_only && c.player_index() == -1)
			return;
		
		for(uint i = 0; i < entity_ids.length; i++)
		{
			entity@ e = entity_by_id(entity_ids[i]);
			
			if(@e != null)
			{
				g.remove_entity(e);
			}
		}
		
		g.remove_entity(self.as_entity());
	}
	
	void editor_draw(float sub_frame)
	{
		const float x = self.x();
		const float y = self.y();
		
		for(uint i = 0; i < entity_ids.length; i++)
		{
			entity@ e = entity_by_id(entity_ids[i]);
			if(@e == null)
				continue;
			
			g.draw_line_world(22, 22, x, y, e.x(), e.y(), 2, 0x99FF0000);
		}
	}
	
}
