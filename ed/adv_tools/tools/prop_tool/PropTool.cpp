#include 'PropToolState.cpp';
#include 'PropSortingData.cpp';
#include 'PropData.cpp';
#include '../../misc/SelectAction.cpp';
#include '../../settings/PropToolSettings.cpp';
#include '../../../../lib/tiles/common.cpp';
#include '../../../../lib/props/common.cpp';
#include '../../../../lib/props/outlines.cpp';

const string EMBED_spr_icon_prop_tool = SPRITES_BASE + 'icon_prop_tool.png';

class PropTool : Tool
{
	
	private PropToolState state = Idle;
	
	private Mouse@ mouse;
	private bool mouse_press_moved;
	private bool mouse_press_modified;
	
	private int prop_data_pool_size;
	private int prop_data_pool_count;
	private array<PropData@> prop_data_pool(prop_data_pool_size);
	
	private int props_under_mouse_size = 32;
	private array<PropSortingData> props_under_mouse(props_under_mouse_size);
	
	private int highlighted_props_size = 32;
	private int highlighted_props_count;
	private array<PropData@> highlighted_props(highlighted_props_size);
	private dictionary highlighted_props_map;
	
	private PropData@ hovered_prop;
	private PropData@ pressed_prop;
	private int selected_props_size = 32;
	private int selected_props_count;
	private array<PropData@> selected_props(selected_props_size);
	private bool temporary_selection;
	
	private int hover_index_offset;
	
	private BoolSetting@ pick_through_tiles;
	private BoolSetting@ pick_ignore_holes;
	private FloatSetting@ pick_radius;
	
	private float drag_start_x, drag_start_y;
	
	PropTool()
	{
		super('Prop Tool');
		
		init_shortcut_key(VK::Q);
	}
	
	void build_sprites(message@ msg) override
	{
		build_sprite(msg, 'icon_prop_tool');
	}
	
	void create(AdvToolScript@ script, ToolGroup@ group) override
	{
		Tool::create(script, group);
		
		set_icon('script', 'icon_prop_tool', 33, 33);
		
		@mouse = @script.mouse;
	}
	
	void on_init() override
	{
		@pick_through_tiles	= script.get_bool(this, 'pick_through_tiles', false);
		@pick_ignore_holes	= script.get_bool(this, 'pick_ignore_holes', true);
		@pick_radius		= script.get_float(this, 'pick_radius', 2);
	}
	
	// //////////////////////////////////////////////////////////
	// Callbacks
	// //////////////////////////////////////////////////////////
	
	Tool@ on_shortcut_key() override
	{
		return selected ? script.get_tool('Props') : @this;
	}
	
	protected void on_select_impl()
	{
		script.hide_gui(true);
	}
	
	void on_deselect_impl()
	{
		script.hide_gui(false);
		
		select_none();
		state = Idle;
		temporary_selection = false;
	}
	
	protected void step_impl() override
	{
		// Reset hover index offset when the mouse moves
		
		if(mouse.delta_x != 0 || mouse.delta_y != 0)
		{
			hover_index_offset = 0;
			mouse_press_moved = true;
		}
		else if(mouse.scroll != 0 && !script.space)
		{
			hover_index_offset -= mouse.scroll;
		}
		
		if(mouse.left_press)
		{
			mouse_press_moved = false;
		}
		
		switch(state)
		{
			case Idle: state_idle(); break;
			case Moving: state_moving(); break;
			case Rotating: state_rotating(); break;
			case Scaling: state_scaling(); break;
			case Selecting: state_selecting(); break;
		}
		
		for(int i = highlighted_props_count - 1; i >= 0; i--)
		{
			highlighted_props[i].step();
		}
	}
	
	protected void draw_impl(const float sub_frame) override
	{
		if(state != Idle)
			return;
		
		float x1, y1, x2, y2;
		int select_index = 0;
		
		for(int i = 0; i < highlighted_props_count; i++)
		{
			PropData@ prop_data = @highlighted_props[i];
			prop_data.draw();
			
			if(!prop_data.selected)
				continue;
			
			if(select_index == 0)
			{
				x1 = prop_data.collision_x + prop_data.x1;
				y1 = prop_data.collision_y + prop_data.y1;
				x2 = prop_data.collision_x + prop_data.x2;
				y2 = prop_data.collision_y + prop_data.y2;
				select_index++;
			}
			else
			{
				if(prop_data.collision_x + prop_data.x1 < x1) x1 = prop_data.collision_x + prop_data.x1;
				if(prop_data.collision_y + prop_data.y1 < y1) y1 = prop_data.collision_y + prop_data.y1;
				if(prop_data.collision_x + prop_data.x2 > x2) x2 = prop_data.collision_x + prop_data.x2;
				if(prop_data.collision_y + prop_data.y2 > y2) y2 = prop_data.collision_y + prop_data.y2;
			}
		}
		
		if(select_index > 0)
		{
			outline_rect(script.g, 22, 22, x1, y1, x2, y2, PropToolSettings::SelectRectLineWidth / script.zoom, 0x55ffffff);
		}
	}
	
	// //////////////////////////////////////////////////////////
	// States
	// //////////////////////////////////////////////////////////
	
	private void state_idle()
	{
		// Clear hovered state
		
		for(int i = highlighted_props_count - 1; i >= 0; i--)
		{
			highlighted_props[i].hovered = false;
		}
		
		@hovered_prop = null;
		
		if(@pressed_prop != null && (mouse.delta_x != 0 || mouse.delta_y != 0))
		{
			if(!pressed_prop.selected)
			{
				select_prop(pressed_prop, SelectAction::Set);
				clear_highlighted_props();
				temporary_selection = true;
			}
			
			drag_start_x = mouse.x;
			drag_start_y = mouse.y;
			
			for(int i = 0; i < selected_props_count; i++)
			{
				selected_props[i].start_drag();
			}
			
			pressed_prop.hovered = true;
			@pressed_prop = null;
			state = Moving;
			return;
		}
		
		if(!script.space)
		{
			pick_props();
			do_mouse_selection();
		}
		else
		{
			@pressed_prop = null;
		}
		
		clear_highlighted_props();
	}
	
	private void state_moving()
	{
		if(script.space || !mouse.left_down)
		{
			if(temporary_selection)
			{
				select_none();
				temporary_selection = false;
			}
			
			state = Idle;
			return;
		}
		
		float start_x, start_y;
		float mouse_x, mouse_y;
		snap(drag_start_x, drag_start_y, start_x, start_y);
		snap(mouse.x, mouse.y, mouse_x, mouse_y);
		const float drag_delta_x = mouse_x - start_x;
		const float drag_delta_y = mouse_y - start_y;
		
		for(int i = 0; i < selected_props_count; i++)
		{
			selected_props[i].do_drag(drag_delta_x, drag_delta_y);
		}
	}
	
	private void state_rotating()
	{
	}
	
	private void state_scaling()
	{
	}
	
	private void state_selecting()
	{
		
	}
	
	// //////////////////////////////////////////////////////////
	// Private Methods
	// //////////////////////////////////////////////////////////
	
	private void do_mouse_selection()
	{
		if(mouse.left_press)
		{
			// Add or remove from selection on shift/ctrl press
			if(script.shift || script.ctrl)
			{
				select_prop(hovered_prop, script.shift ? SelectAction::Add : SelectAction::Remove);
				mouse_press_modified = true;
			}
			else
			{
				@pressed_prop = hovered_prop;
			}
		}
		else if(mouse.left_release)
		{
			// Deselect all on normal click in empty space
			if(!mouse_press_moved && !mouse_press_modified)
			{
				select_prop(hovered_prop, SelectAction::Set);
			}
			
			mouse_press_modified = false;
			@pressed_prop = null;
		}
	}
	
	private void select_prop(PropData@ prop_data, const SelectAction action)
	{
		if(action == SelectAction::Set)
		{
			while(selected_props_count > 0)
			{
				PropData@ selected_pro_data = @selected_props[--selected_props_count];
				selected_pro_data.selected = false;
			}
		}
		
		if(
			@prop_data == null ||
			action == SelectAction::Remove && !prop_data.selected ||
			action == SelectAction::Add && prop_data.selected
		)
			return;
		
		if(action == SelectAction::Add || action == SelectAction::Set)
		{
			if(selected_props_count >= selected_props_size)
			{
				selected_props.resize(selected_props_size += 32);
			}
			
			@selected_props[selected_props_count++] = prop_data;
			prop_data.selected = true;
		}
		else
		{
			selected_props.removeAt(selected_props.findByRef(@prop_data));
			prop_data.selected = false;
			selected_props_count--;
		}
	}
	
	private void select_none()
	{
		select_prop(null, SelectAction::Set);
	}
	
	private PropData@ highlight_prop(prop@ prop, const array<array<float>>@ outline)
	{
		const string key = prop.id() + '';
		PropData@ prop_data;
		
		if(highlighted_props_map.exists(key))
			return cast<PropData@>(highlighted_props_map[key]);
		
		@prop_data = prop_data_pool_count > 0
			? @prop_data_pool[--prop_data_pool_count]
			: PropData();
		
		@prop_data.prop = prop;
		prop_data.key = key;
		@prop_data.outline = @outline;
		
		if(highlighted_props_count >= highlighted_props_size)
		{
			highlighted_props.resize(highlighted_props_size += 32);
		}
		
		@highlighted_props[highlighted_props_count++] = @prop_data;
		@highlighted_props_map[key] = @prop_data;
		
		prop_data.init(script);
		
		return prop_data;
	}
	
	/// Return PropData that is no longer higlighted to the pool
	private void clear_highlighted_props()
	{
		for(int i = highlighted_props_count - 1; i >= 0; i--)
		{
			PropData@ prop_data = @highlighted_props[i];
			
			if(prop_data.hovered || prop_data.selected)
				continue;
			
			if(prop_data_pool_count >= prop_data_pool_size)
			{
				prop_data_pool.resize(prop_data_pool_size += 32);
			}
			
			@prop_data_pool[prop_data_pool_count++] = @prop_data;
			@highlighted_props[i] = @highlighted_props[--highlighted_props_count];
			highlighted_props_map.delete(prop_data.key);
		}
	}
	
	private void snap(const float x, const float y, float &out out_x, float &out out_y)
	{
		const float snap = get_snap_size();
		
		if(snap != 0)
		{
			out_x = round(x / snap) * snap;
			out_y = round(y / snap) * snap;
		}
		else
		{
			out_x = x;
			out_y = y;
		}
	}
	
	private float get_snap_size()
	{
		if(script.shift)
			return 48;
		
		if(script.ctrl && script.alt)
			return 5;
		
		if(script.ctrl)
			return 24;
		
		if(script.alt)
			return 1;
		
		return 0;
	}
	
	private void pick_props()
	{
		// Find all props under the mouse
		
		const float radius = max(PropToolSettings::SmallPropRadius, pick_radius.value) / script.zoom;
		int i = script.g.get_prop_collision(mouse.y - radius, mouse.y + radius, mouse.x - radius, mouse.x + radius);
		
		array<PropSortingData>@ props_under_mouse = @this.props_under_mouse;
		int num_props_under_mouse = 0;
		
		if(props_under_mouse_size < i)
		{
			props_under_mouse.resize(props_under_mouse_size = i + 32);
		}
		
		while(i-- > 0)
		{
			prop@ p = script.g.get_prop_collision_index(i);
			
			const float prop_x = p.x();
			const float prop_y = p.y();
			const float rotation = p.rotation() * DEG2RAD * sign(p.scale_x()) * sign(p.scale_y());
			const float layer_scale = p.layer() <= 5 ? script.g.layer_scale(p.layer()) : 1.0;
			const float backdrop_scale = p.layer() <= 5 ? 2.0 : 1.0;
			const float scale_x = p.scale_x() / layer_scale * backdrop_scale;
			const float scale_y = p.scale_y() / layer_scale * backdrop_scale;
			
			// Calculate mouse "local" position relative to prop rotation and scale
			
			float local_x, local_y;
			const float layer_mx = script.g.mouse_x_world(0, p.layer());
			const float layer_my = script.g.mouse_y_world(0, p.layer());
			
			rotate(
				(layer_mx - prop_x) / scale_x,
				(layer_my - prop_y) / scale_y,
				-rotation, local_x, local_y);
			
			// Check for overlap with tiles
			
			if(!pick_through_tiles.value && hittest_tiles(p.layer(), p.sub_layer()))
				continue;
			
			// Check if the mouse is inside to the prop
			
			const array<array<float>>@ outline = @PROP_OUTLINES[p.prop_set() - 1][p.prop_group()][p.prop_index() - 1];
			const array<bool>@ holes = @PROP_OUTLINES_HOLES_INFO[p.prop_set() - 1][p.prop_group()][p.prop_index() - 1];
			
			bool is_inside = hittest_prop(p, local_x, local_y, outline, holes);
			
			// If the mouse is not directly inside the prop outline, check if it close by, within some threshold
			
			const bool is_close = !is_inside
				? check_prop_distance(p, local_x, local_y, outline, holes)
				: false;
			
			if(is_inside || is_close)
			{
				PropSortingData@ data = @props_under_mouse[num_props_under_mouse++];
				@data.prop = @p;
				@data.outline = @outline;
				data.is_inside = is_inside ? 1 : 0;
				data.scene_index = i;
			}
		}
		
		if(num_props_under_mouse == 0)
			return;
		
		// Sort to find the top most prop
		
		props_under_mouse.sortAsc(0, num_props_under_mouse);
		const int selected_index = (num_props_under_mouse - (hover_index_offset % num_props_under_mouse) - 1) % num_props_under_mouse;
		
		PropSortingData@ sorting_data = @props_under_mouse[selected_index];
		PropData@ prop_data = highlight_prop(@sorting_data.prop, @sorting_data.outline);
		prop_data.hovered = true;
		@hovered_prop = prop_data;
	}
	
	private bool hittest_tiles(const int prop_layer, const int prop_sublayer)
	{
		for(int layer = prop_layer; layer <= 20; layer++)
		{
			if(layer == prop_layer && prop_sublayer >= 10)
				continue;
			
			const float mx = script.g.mouse_x_world(0, layer);
			const float my = script.g.mouse_y_world(0, layer);
			const int tx = floor_int(mx / 48);
			const int ty = floor_int(my / 48);
			tileinfo@ tile = script.g.get_tile(tx, ty, layer);
			
			if(!tile.solid())
				continue;
			
			float normal_x, normal_y;
			
			if(point_in_tile(mx, my, tx, ty, tile.type(), normal_x, normal_y, layer))
				return true;
		}
		
		return false;
	}
	
	private bool hittest_prop(prop@ p, const float local_x, const float local_y,
		const array<array<float>>@ outline,
		const array<bool>@ holes)
	{
		bool inside = false;
		
		for(uint i = 0; i < outline.length(); i++)
		{
			if(point_in_polygon(local_x, local_y, @outline[i]))
			{
				if(pick_ignore_holes.value)
					return true;
				
				inside = holes[i] ? !inside : true;
			}
		}
		
		return inside;
	}
	
	private bool check_prop_distance(prop@ p, const float local_x, const float local_y,
		const array<array<float>>@ outline,
		const array<bool>@ holes)
	{
		const string prop_key = p.prop_set() + '.' + p.prop_group() + '.' + p.prop_index();
		const float prop_radius = (PropToolSettings::PropRadii.exists(prop_key)
			? float(PropToolSettings::PropRadii[prop_key])
			: pick_radius.value) / script.zoom;
		
		for(uint i = 0; i < outline.length(); i++)
		{
			if(distance_to_polygon_sqr(local_x, local_y, @outline[0]) <= prop_radius * prop_radius)
				return true;
			
			if(pick_ignore_holes.value)
				break;
		}
		
		return false;
	}
	
}