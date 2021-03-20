#include '../drawing/common.cpp';

class EntityRemover : trigger_base
{
	
	scene@ g;
	scripttrigger@ self;
	
	entity@ entity_to_remove = null;
	
	[entity] uint id;
	[text] bool player_only = true;
	
	EntityRemover()
	{
		@g = get_scene();
	}
	
	EntityRemover(entity@ e)
	{
		if(@e != null)
			id = e.id();
		
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
		@entity_to_remove = entity_by_id(id);
	}
	
	void activate(controllable@ e)
	{
		if(player_only && e.player_index() == -1)
			return;
		
		@entity_to_remove = entity_by_id(id);
		
		if(@entity_to_remove != null)
			g.remove_entity(entity_to_remove);
		
		g.remove_entity(self.as_entity());
	}
	
	void editor_step()
	{
		if(entity_to_remove is null || entity_to_remove.id() != id)
		{
			@entity_to_remove = entity_by_id(id);
		}
	}
	
	void editor_draw(float sub_frame)
	{
		if(@entity_to_remove != null)
		{
			draw_arrow(g, 22, 22, self.x(), self.y(), entity_to_remove.x(), entity_to_remove.y(), 2, 20, 0.5, 0x99FF0000);
		}
	}
	
}