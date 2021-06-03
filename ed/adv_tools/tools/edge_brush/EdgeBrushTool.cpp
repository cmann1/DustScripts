#include '../../../../lib/tiles/common.cpp';
#include '../../../../lib/tiles/get_tile_edge_points.cpp';
#include '../../../../lib/tiles/get_tile_quad.cpp';
#include '../../../../lib/tiles/closest_point_on_tile.cpp';
#include '../../../../lib/tiles/EdgeFlags.cpp';

#include 'EdgeBrushMode.cpp';
#include 'EdgeBrushRenderMode.cpp';
#include 'EdgeBrushState.cpp';
#include 'EdgeFacing.cpp';
#include 'TileEdgeData.cpp';

const string EDGE_BRUSH_SPRITES_BASE = SPRITES_BASE + 'edge_brush/';
const string EMBED_spr_icon_edge_brush = SPRITES_BASE + 'icon_edge_brush.png';

class EdgeBrushTool : Tool
{
	
	private EdgeBrushMode mode = EdgeBrushMode::Brush;
	private EdgeBrushState state = Idle;
	private Mouse@ mouse;
	private int layer;
	private float x, y;
	
	private float drag_radius_start;
	private float drag_radius_x, drag_radius_y;
	
	/// -1 = Off, 1 = On, 0 = None
	private int draw_mode = 0;
	
	private dictionary tile_chunks;
	private int tile_cache_layer = -1;
	
	private int draw_list_index;
	private int draw_list_size = 1024;
	private array<TileEdgeData@> draw_list(draw_list_size);
	
	// Settings
	
	/// Each edge has two bits/flags controlling whether an edge has collision and is rendered: collision and priority
	/// If the collision bit is on, or collision is off and priority is one, an edge is rendered.
	/// If the collision and priorty bit are both off no edge is rendered.
	bool update_collision = true;
	bool update_priority = true;
	//float brush_radius = 48;
	float brush_radius = 7500;
	EdgeBrushRenderMode render_mode = EdgeBrushRenderMode::Always;
	/// Which edges must be updated: Top, Bottom, Left, Right
	uint edge_mask = 0x8 | 0x4 | 0x2 | 0x1;
	/// Must internal edges (edges shared by two tiles) by updated
	// TODO: Set to external
	EdgeFacing edge_facing = EdgeFacing::External;
	/// If true, edges shared by tiles with different sprites will be considered "external"
	bool check_internal_sprites = true;
	
	private bool is_layer_valid { get const { return layer >= 6 && layer <= 20; } }
	
	EdgeBrushTool(AdvToolScript@ script)
	{
		super(script, 'Tiles', 'Edge Brush');
		
		// Set priority = -1 so W selects the tile tool first
		init_shortcut_key(VK::W, -1);
	}
	
	void build_sprites(message@ msg) override
	{
		build_sprite(msg, 'icon_edge_brush');
		
		//toolbar.build_sprites(msg);
	}
	
	void create(ToolGroup@ group) override
	{
		Tool::create(group);
		
		set_icon(SPRITE_SET, 'icon_edge_brush');
		
		@mouse = @script.mouse;
	}
	
	private void update_brush_radius(const float new_radius, const float overlay_time=0.1)
	{
		brush_radius = max(5.0, new_radius);
		
		if(overlay_time > 0)
		{
			if(state == EdgeBrushState::DragRadius)
			{
				script.info_overlay.show(drag_radius_x, drag_radius_y, int(brush_radius) + '', overlay_time);
			}
			else
			{
				script.info_overlay.show(mouse, int(brush_radius) + '', overlay_time);
			}
		}
	}
	
	private void clear_tile_cache()
	{
		tile_chunks.deleteAll();
		tile_cache_layer = layer;
	}
	
	private bool check_edge(const int tx, const int ty, TileEdgeData@ data, const TileEdge edge,
		const bool fast,
		const float cx=0, const float cy=0, const float radius=0)
	{
		if((data.valid_edges & (1 << edge)) == 0)
			return false;
		
		if(!fast && radius > 0)
		{
			data.select_edge(edge);
			if(!line_circle_intersect(cx, cy, brush_radius, data.cx1, data.cy1, data.cx2, data.cy2))
				return false;
		}
		
		if(edge_facing == EdgeFacing::Both)
			return true;
		
		const EdgeFacing facing = (data.edges_facing >> edge) & 0x1 == 1
			? EdgeFacing::External : EdgeFacing::Internal;
		
		return facing == edge_facing;
	}
	
	// //////////////////////////////////////////////////////////
	// Callbacks
	// //////////////////////////////////////////////////////////
	
	Tool@ on_shortcut_key() override
	{
		return selected ? script.get_tool('Tiles') : @this;
	}
	
	protected void on_select_impl()
	{
		script.editor.hide_panels_gui(true);
		
		//toolbar.show(script, this);
	}
	
	protected void on_deselect_impl()
	{
		script.editor.hide_panels_gui(false);
		
		state = Idle;
		draw_list_index = 0;
		clear_tile_cache();
		//toolbar.hide();
	}
	
	protected void step_impl() override
	{
		// TODO: Make sure both settings aren't set to Auto
		// TODO: Add mouse/key shortcuts for toggling update_collision/priority
		
		layer = script.editor.selected_layer;
		x = mouse.x;
		y = mouse.y;
		
		if(tile_cache_layer != layer)
		{
			clear_tile_cache();
		}
		
		draw_list_index = 0;
		
		// TODO: Remove Temp
		if(script.editor.key_check_pressed_vk(VK::N))
			update_collision = !update_collision;
		if(script.editor.key_check_pressed_vk(VK::M))
			update_priority = !update_priority;
		if(script.editor.key_check_pressed_vk(VK::N) || script.editor.key_check_pressed_vk(VK::M))
			puts(update_collision, update_priority);
		
		switch(state)
		{
			case EdgeBrushState::Idle: state_idle(); break;
			case EdgeBrushState::Draw: state_draw(); break;
			case EdgeBrushState::DragRadius: state_drag_radius(); break;
		}
	}
	
	protected void draw_impl(const float sub_frame) override
	{
		if(!is_layer_valid)
			return;
		
		const float line_width = min(1.5 / script.zoom, 10.0);
		
		for(int i = 0; i < draw_list_index; i++)
		{
			TileEdgeData@ data = draw_list[i];
			
			for(TileEdge edge = TileEdge::Top; edge <= TileEdge::Right; edge++)
			{
				if(data.draw_edges & (1 << edge) == 0)
					continue;
				
				data.select_edge(edge);
				
				float x1, y1, x2, y2;
				script.transform(data.cx1, data.cy1, layer, 22, x1, y1);
				script.transform(data.cx2, data.cy2, layer, 22, x2, y2);
				
				const bool collision_on = data.edge & Collision != 0;
				const bool priority_on = data.edge & Priority != 0;
				const uint clr = collision_on ? Settings::EdgeOnColour
					: (priority_on ? Settings::EdgeVisibleColour : Settings::EdgeOffColour);
				
				draw_line(script.g, 22, 22, x1, y1, x2, y2, line_width, clr, true);
			}
		}
		
		switch(mode)
		{
			case EdgeBrushMode::Brush: draw_brush(sub_frame); break;
			case EdgeBrushMode::Precision: draw_precision(sub_frame); break;
		}
		
		draw_cursor();
	}
	
	private void draw_cursor()
	{
		drawing::fill_circle(script.g, 22, 22,
			x, y, min(brush_radius, 3 / script.zoom), 16,
			Settings::CursorLineColour, Settings::CursorLineColour);
	}
	
	private void draw_brush(const float sub_frame)
	{
		if(!script.shift)
		{
			script.circle(22, 22, x, y, brush_radius, 64,
				Settings::CursorLineWidth, Settings::CursorLineColour);
		}
		else
		{
			outline_rect(script.g, 22, 22,
				x - brush_radius, y - brush_radius,
				x + brush_radius, y + brush_radius,
				Settings::CursorLineWidth / script.zoom, Settings::CursorLineColour);
		}
	}
	
	private void draw_precision(const float sub_frame)
	{
		
	}
	
	// //////////////////////////////////////////////////////////
	// States
	// //////////////////////////////////////////////////////////
	
	private void state_idle()
	{
		// Change layer with mouse wheel
		if(script.ctrl && mouse.scroll != 0)
		{
			const int prev_layer = layer;
			layer = clamp(layer + mouse.scroll, 6, 20);
			layer += layer == 18 ? mouse.scroll : 0;
			script.editor.selected_layer = layer;
			
			if(layer != prev_layer)
			{
				clear_tile_cache();
			}
			
			script.info_overlay.show(mouse, 'LAYER: ' + layer + '', 0.5);
		}
		
		switch(mode)
		{
			case Brush: brush_state_idle(); break;
			case Precision: precision_state_idle(); break;
		}
	}
	
	private void brush_state_idle()
	{
		// Change brush size mouse wheel
		if(!script.ctrl && mouse.scroll != 0)
		{
			update_brush_radius(brush_radius - mouse.scroll * 10, 0.5);
		}
		// Change brush size by dragging
		if(!script.ctrl && !script.shift && script.alt && mouse.left_press)
		{
			drag_radius_start = brush_radius;
			drag_radius_x = mouse.x;
			drag_radius_y = mouse.y;
			state = DragRadius;
			return;
		}
		
		if(render_mode == Always)
		{
			do_brush_mode(0);
		}
		
		if(mouse.left_press || mouse.right_press)
		{
			draw_mode = mouse.left_press ? 1 : -1;
			clear_tile_cache();
			state = Draw;
			return;
		}
	}
	
	private void precision_state_idle()
	{
		
	}
	
	private void state_draw()
	{
		switch(mode)
		{
			case Brush: brush_state_draw(); break;
			case Precision: precision_state_draw(); break;
		}
	}
	
	private void brush_state_draw()
	{
		do_brush_mode(draw_mode);
		
		if(draw_mode == 1 && !mouse.left_down || draw_mode == -1 && !mouse.right_down)
		{
			state = Idle;
			return;
		}
	}
	
	private void precision_state_draw()
	{
		
	}
	
	private void state_drag_radius()
	{
		if(!mouse.left_down)
		{
			state = Idle;
			return;
		}
		
		x = drag_radius_x;
		y = drag_radius_y;
		
		update_brush_radius(
			(mouse.x - drag_radius_x) +
			drag_radius_start);
	}
	
	private void do_brush_mode(const int update_edges)
	{
		if(!is_layer_valid)
			return;
		
		const bool render_edges = render_mode == Always || render_mode == DrawOnly && update_edges != 0;
		const bool reset_edges = update_edges == 1 && script.ctrl;
		
		float mx, my;
		script.mouse_layer(layer, mx, my);
		
		const float layer_radius = script.transform_size(brush_radius, mouse.layer, layer);
		const float radius_sqr = layer_radius * layer_radius;
		const int tx1 = floor_int((mx - layer_radius) * PIXEL2TILE);
		const int ty1 = floor_int((my - layer_radius) * PIXEL2TILE);
		const int tx2 = floor_int((mx + layer_radius) * PIXEL2TILE);
		const int ty2 = floor_int((my + layer_radius) * PIXEL2TILE);
		
		array<TileEdgeData>@ chunk;
		
		const int start_time = get_time_us();
		int stats_tile_update_count = 0;
		int stats_tile_skip_count = 0;
		
		// Found all chunks the brush is touching
		const int chunks_x1 = floor_int(float(tx1) / Settings::TileChunkSize);
		const int chunks_y1 = floor_int(float(ty1) / Settings::TileChunkSize);
		const int chunks_x2 = floor_int(float(tx2) / Settings::TileChunkSize);
		const int chunks_y2 = floor_int(float(ty2) / Settings::TileChunkSize);
		
		for(int chunk_y = chunks_y1; chunk_y <= chunks_y2; chunk_y++)
		{
			for(int chunk_x = chunks_x1; chunk_x <= chunks_x2; chunk_x++)
			{
				// Retrieve or create the TileData array cache for this chunk
				const string chunk_key = chunk_x + ',' + chunk_y;
				
				if(tile_chunks.exists(chunk_key))
				{
					@chunk = cast<array<TileEdgeData>@>(tile_chunks[chunk_key]);
				}
				else
				{
					@chunk = array<TileEdgeData>(Settings::TileChunkSize * Settings::TileChunkSize);
					@tile_chunks[chunk_key] = @chunk;
				}
				
				const int chunk_tx = chunk_x * Settings::TileChunkSize;
				const int chunk_ty = chunk_y * Settings::TileChunkSize;
				const int c_tx1 = tx1 > chunk_tx ? tx1 : chunk_tx;
				const int c_ty1 = ty1 > chunk_ty ? ty1 : chunk_ty;
				const int c_tx2 = tx2 < chunk_tx + Settings::TileChunkSize - 1
					? tx2 : chunk_tx + Settings::TileChunkSize - 1;
				const int c_ty2 = ty2 < chunk_ty + Settings::TileChunkSize - 1
					? ty2 : chunk_ty + Settings::TileChunkSize - 1;
				
				// Iterate all tiles within this chunk that are also inside the brush bounding box
				for(int tx = c_tx1; tx <= c_tx2; tx++)
				{
					for(int ty = c_ty1; ty <= c_ty2; ty++)
					{
						const float x = tx * 48;
						const float y = ty * 48;
						
						// Get and cache the tile data
						TileEdgeData@ data = @chunk[(ty - chunk_ty) * Settings::TileChunkSize + (tx - chunk_tx)];
						if(@data.tile == null)
						{
							data.init(script.g, tx, ty, layer, edge_mask, check_internal_sprites);
						}
						
						if(!data.solid)
						{
							stats_tile_skip_count++;
							continue;
						}
						
						data.draw_edges = 0;
						bool tile_requires_update = false;
						
						for(TileEdge edge = TileEdge::Top; edge <= TileEdge::Right; edge++)
						{
							if(!check_edge(tx, ty, data, edge, script.shift, mx, my, brush_radius))
								continue;
							
							if(
								update_edges != 0 && (data.updated_edges & (1 << edge)) == 0 &&
								data.update_edge(edge, update_edges, update_collision, update_priority))
							{
								tile_requires_update = true;
							}
							else if(reset_edges)
							{
								tile_requires_update = true;
							}
							
							// Mark edge for rendering
							if(render_edges)
							{
								if(data.draw_edges == 0)
								{
									while(draw_list_index + 1 >= draw_list_size)
									{
										draw_list.resize(draw_list_size *= 2);
									}
									
									@draw_list[draw_list_index++] = data;
								}
								
								data.draw_edges |= (1 << edge);
							}
						} // edge
						
						if(tile_requires_update)
						{
							script.g.set_tile(tx, ty, layer, data.tile, reset_edges);
							stats_tile_update_count++;
						}
					} // tile y
				} // tile x
			} // chunk x
		} // chunk y
		
		const int total_time = get_time_us() - start_time;
		
		int idx = 0;
		const int tw = tx2 - tx1 + 1;
		const int th = ty2 - ty1 + 1;
		script.debug.print((total_time / 1000) + 'ms', idx++);
		script.debug.print('NotSolid: ' + stats_tile_skip_count, idx++);
		script.debug.print('Updated: ' + stats_tile_update_count, idx++);
		script.debug.print('Tiles: ' + tw + ',' + th + ' ' + (tw * th), idx++);
	}
	
}
