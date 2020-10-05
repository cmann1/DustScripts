abstract class SelectableData : IWorldBoundingBox
{
	
	protected AdvToolScript@ script;
	
	string key;
	int scene_index;
	int is_mouse_inside;
	bool hovered;
	bool selected;
	bool primary_selected;
	int pending_selection;
	
	float aabb_x,  aabb_y;
	float aabb_x1, aabb_y1;
	float aabb_x2, aabb_y2;
	
	protected void init(AdvToolScript@ script, const string key, const int scene_index)
	{
		@this.script = script;
		this.key = key;
		this.scene_index = scene_index;
		
		hovered = false;
		selected = false;
		primary_selected = false;
		pending_selection = 0;
	}
	
	protected void aabb_from_rect(const float half_width, const float half_height, const float angle)
	{
		aabb_x1 = -half_width  * 0.5;
		aabb_y1 = -half_height * 0.5;
		aabb_x2 =  half_width  * 0.5;
		aabb_y2 =  half_height * 0.5;
		
		float x1, y1, x2, y2, x3, y3, x4, y4;
		rotate(aabb_x1, aabb_y1, angle, x1, y1);
		rotate(aabb_x2, aabb_y1, angle, x2, y2);
		rotate(aabb_x2, aabb_y2, angle, x3, y3);
		rotate(aabb_x1, aabb_y2, angle, x4, y4);
		
		aabb_x1 = min(min(x1, aabb_x + x2), min(x3, aabb_x + x4));
		aabb_y1 = min(min(y1, aabb_y + y2), min(y3, aabb_y + y4));
		aabb_x2 = max(max(x1, aabb_x + x2), max(x3, aabb_x + x4));
		aabb_y2 = max(max(y1, aabb_y + y2), max(y3, aabb_y + y4));
	}
	
	protected void get_colours(float &out line_width, uint &out line_colour, uint &out fill_colour)
	{
		line_width = pending_selection == 1 ? Settings::PendingAddLineWidth : pending_selection == -1 ? Settings::PendingRemoveLineWidth
			: hovered ? Settings::HoveredLineWidth
			: selected ? Settings::SelectedLineWidth : Settings::DefaultLineWidth;
		fill_colour = pending_selection == 1 ? Settings::PendingAddFillColour : pending_selection == -1 ? Settings::PendingRemoveFillColour
			: hovered ? Settings::HoveredFillColour
			: selected ? Settings::SelectedFillColour : Settings::DefaultFillColour;
		line_colour = pending_selection == 1 ? Settings::PendingAddLineColour : pending_selection == -1 ? Settings::PendingRemoveLineColour
			: hovered ? Settings::HoveredLineColour
			: selected ? Settings::SelectedLineColour : Settings::DefaultLineColour;
	}
	
	// IWorldBoundingBox
	
	float get_world_x1() override
	{
		return aabb_x + aabb_x1;
	}
	
	float get_world_y1() override
	{
		return aabb_y + aabb_y1;
	}
	
	float get_world_x2() override
	{
		return aabb_x + aabb_x2;
	}
	
	float get_world_y2() override
	{
		return aabb_y + aabb_y2;
	}
	
}