class EmitterData : IWorldBoundingBox
{
	
	AdvToolScript@ script;
	EmitterTool@ tool;
	entity@ emitter;
	string key;
	int scene_index;
	int is_mouse_inside;
	bool hovered;
	bool selected;
	bool primary_selected;
	bool visible;
	bool modified;
	
	private varstruct@ vars;
	private varvalue@ width_var;
	private varvalue@ height_var;
	private varvalue@ rotation_var;
	private varvalue@ sublayer_var;
	
	int layer, sublayer;
	float x, y;
	float width, height;
	float rotation;
	
	float world_x, world_y;
	float world_size_x, world_size_y;
	float x1, y1, x2, y2;
	float local_mouse_x, local_mouse_y;
	
	float min_x, min_y;
	float max_x, max_y;
	
	private float drag_start_x, drag_start_y;
	
	void init(AdvToolScript@ script, EmitterTool@ tool, entity@ emitter, const int scene_index)
	{
		@this.script = script;
		@this.tool = tool;
		@this.emitter = emitter;
		this.scene_index = scene_index;
		
		key = emitter.id() + '';
		
		@vars = emitter.vars();
		@width_var = vars.get_var('width');
		@height_var = vars.get_var('height');
		@rotation_var = vars.get_var('e_rotation');
		@sublayer_var = vars.get_var('draw_depth_sub');
		
		x = emitter.x();
		y = emitter.y();
		layer = emitter.layer();
		sublayer = sublayer_var.get_int32();
		width = width_var.get_int32();
		height = height_var.get_int32();
		rotation = rotation_var.get_int32();
		
		hovered = false;
		selected = false;
		primary_selected = false;
		modified = false;
	}
	
	void update()
	{
		script.transform(x, y, layer, 22, world_x, world_y);
		script.transform_size(width, height, layer, 22, world_size_x, world_size_y);
		
		x1 = world_x - world_size_x * 0.5;
		y1 = world_y - world_size_y * 0.5;
		x2 = world_x + world_size_x * 0.5;
		y2 = world_y + world_size_y * 0.5;
		
		rotate(script.mouse.x - world_x, script.mouse.y - world_y, -rotation * DEG2RAD, local_mouse_x, local_mouse_y);
		local_mouse_x += world_x;
		local_mouse_y += world_y;
		
		is_mouse_inside = 
			local_mouse_x >= x1 && local_mouse_x <= x2 &&
			local_mouse_y >= y1 && local_mouse_y <= y2
				? 1 : 0;
		
		min_x = -world_size_x * 0.5;
		min_y = -world_size_y * 0.5;
		max_x =  world_size_x * 0.5;
		max_y =  world_size_y * 0.5;
		
		float x1, y1, x2, y2, x3, y3, x4, y4;
		rotate(min_x, min_y, rotation * DEG2RAD, x1, y1);
		rotate(max_x, min_y, rotation * DEG2RAD, x2, y2);
		rotate(max_x, max_y, rotation * DEG2RAD, x3, y3);
		rotate(min_x, max_y, rotation * DEG2RAD, x4, y4);
		
		min_x = min(min(world_x + x1, world_x + x2), min(world_x + x3, world_x + x4));
		min_y = min(min(world_y + y1, world_y + y2), min(world_y + y3, world_y + y4));
		max_x = max(max(world_x + x1, world_x + x2), max(world_x + x3, world_x + x4));
		max_y = max(max(world_y + y1, world_y + y2), max(world_y + y3, world_y + y4));
	}
	
	/// Changing an emitter's properties does not reflect in the editor for some reason.
	/// And simply adding and removing does not seem to work either. At the end of the frame remove the emitter.
	/// Then at the start of the next, remove it again, and then add it.
	void pre_step_validate()
	{
		if(!modified)
			return;
		
		script.g.remove_entity(emitter);
		script.g.add_entity(emitter);
		modified = false;
	}
	
	void post_step_validate()
	{
		if(!modified)
			return;
		
		script.g.remove_entity(emitter);
	}
	
	void step()
	{
		if(primary_selected)
		{
			const float nx = cos(rotation * DEG2RAD);
			const float ny = sin(rotation * DEG2RAD);
			const float ox = world_size_x * 0.5;
			const float oy = world_size_y * 0.5;
			
			const array<float>@ o = Settings::ScaleHandleOffsets;
			
			for(int i = 0; i < 32; i += 4)
			{
				if(script.handles.square(
					world_x + nx * ox * o[i + 0] - ny * oy * o[i + 1],
					world_y + ny * ox * o[i + 2] + nx * oy * o[i + 3],
					Settings::ScaleHandleSize, rotation, Settings::RotateHandleColour, Settings::RotateHandleHighlightColour))
				{
//					hovered_handle_index = i / 4;
				}
			}
		}
	}
	
	void draw()
	{
		const uint fill = hovered ? EmitterToolSettings::HoveredFillColour
			: selected
				? EmitterToolSettings::SelectedFillColour
				: EmitterToolSettings::VisibleFillColour;
		const uint line = hovered ? EmitterToolSettings::HoveredLineColour
			: selected
				? EmitterToolSettings::SelectedLineColour
				: EmitterToolSettings::VisibleLineColour;
		const float line_width = hovered ? EmitterToolSettings::HoveredLineWidth
			: selected
				? EmitterToolSettings::SelectedLineWidth
				: EmitterToolSettings::LineWidth;
		
		if(fill != 0)
		{
			script.g.draw_rectangle_world(22, 22,
				x1, y1, x2, y2, rotation, fill);
		}
		
		outline_rotated_rect(script.g, 22, 22,
			world_x, world_y, world_size_x * 0.5, world_size_y * 0.5,
			rotation, line_width / script.zoom,
			line);
	}
	
	int opCmp(const EmitterData &in other)
	{
		// Emitters that the mouse is inside of take priority over ones that the mouse is close to
		
		if(is_mouse_inside != other.is_mouse_inside)
			return is_mouse_inside - other.is_mouse_inside;
		
		// Compare layers
		
		if(layer != other.layer)
			return layer - other.layer;
		
		if(sublayer != other.sublayer)
			return sublayer - other.sublayer;
		
		return scene_index - other.scene_index;
	}
	
	// Moving
	
	void start_drag()
	{
		drag_start_x = x;
		drag_start_y = y;
	}
	
	void do_drag(const float drag_delta_x, const float drag_delta_y)
	{
		x = drag_start_x + drag_delta_x;
		y = drag_start_y + drag_delta_y;
		
		emitter.set_xy(x, y);
	}
	
	void stop_drag(const bool accept)
	{
		if(!accept)
		{
			x = drag_start_x;
			y = drag_start_y;
			
			emitter.set_xy(x, y);
		}
	}
	
	// Layer/Sublar
	
	void shift_layer(const int dir, const bool sublayer=false)
	{
		if(sublayer)
		{
			this.sublayer = clamp(this.sublayer + dir, 0, 24);
			sublayer_var.set_int32(this.sublayer);
		}
		else
		{
			layer = clamp(layer + dir, 0, 20);
			emitter.layer(layer);
		}
		
		modified = true;
		update();
	}
	
	// IWorldBoundingBox
	
	float get_world_x1() override
	{
		return min_x;
	}
	
	float get_world_y1() override
	{
		return min_y;
	}
	
	float get_world_x2() override
	{
		return max_x;
	}
	
	float get_world_y2() override
	{
		return max_y;
	}
	
}