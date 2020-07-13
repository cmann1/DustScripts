#include '../../lib/std.cpp';
#include '../../lib/layer.cpp';
#include '../../lib/math/math.cpp';
#include '../../lib/math/Line.cpp';
#include '../../lib/Mouse.cpp';
#include '../../lib/drawing/common.cpp';
#include '../../lib/drawing/circle.cpp';
#include '../../lib/tiles/common.cpp';
#include '../../lib/tiles/get_tile_edge_points.cpp';
#include '../../lib/tiles/closest_point_on_tile.cpp';
#include '../../lib/tiles/EdgeFlags.cpp';
#include '../../lib/tiles/TileEdge.cpp';
#include 'OnOffProperty.cpp';
#include 'EdgeType.cpp';
#include 'DragSizeState.cpp';
#include 'TileCachChunkQueue.cpp';
#include 'TileCachChunk.cpp';

const uint ON_OFF_COLOUR = 0xaaffffff;
const uint ON_OFF_ALPHA = ON_OFF_COLOUR & 0xff000000;
const float KEEP_WIDTH = 3;
const float ON_OFF_WIDTH = 1.5;
const float ON_OFF_LENGTH = 8;
const uint EDGE_ON_COLOUR  = 0xff00ffff;
const uint EDGE_OFF_COLOUR = 0x88ff00ff;
const uint PRECISE_EDGE_CURSOR_COLOUR = 0xffffffff;
const uint PRECISE_EDGE_MARKER_COLOUR = 0xffffffff;
const uint PRECISE_EDGE_ARROW_COLOUR = 0x55ffffff;
const float EDGE_OFF_FACTOR = 0.4;
const float EDGE_RENDER_WIDTH = 2;
const float PRECISE_EDGE_RENDER_WIDTH = 1;
const float PRECISE_EDGE_MARKER_WIDTH = 4;
const float SCROLL_AMOUNT = 10;
const uint DISPLAY_TEXT_FILL_COLOUR = 0xffffffff;
const uint DISPLAY_TEXT_SHADOW_COLOUR = 0xee000000;
const int DISPLAY_MODE_TIME = 1 * 60;
const float DISPLAY_TEXT_OFFSET = 15;

const int TILES_CACHE_CHUNK_SIZE = 30;

class script
{
	
	[text] bool enabled = true;
	[text] int layer = 19;
	[text] float size = 24;
	[option,0:Keep,1:Off,2:On] OnOffProperty update_collision = OnOffProperty::Off;
	[option,0:Keep,1:Off,2:On] OnOffProperty update_edges = OnOffProperty::Off;
	[text] bool update_top = true;
	[text] bool update_bottom = true;
	[text] bool update_left = true;
	[text] bool update_right = true;
	[option,0:External,1:Internal,2:Both] EdgeType edges = EdgeType::External;
	[text] bool external_different_sprites = true;
	[text] bool precision_mode = false;
	[text] bool precision_inside_only = false;
	[text] bool precision_update_neighbour = false;
	[text] bool render_edges = true;
	[text] bool always_render_edges = true;
	
	scene@ g;
	Mouse@ mouse = Mouse();
	camera@ cam;
	float view_x;
	float view_y;
	float prev_mouse_x;
	float prev_mouse_y;
	bool force_mouse_update;
	
	DragSizeState drag_size = DragSizeState::Off;
	float drag_size_start;
	float drag_size_x;
	float drag_size_y;
	
	array<bool> edges_flags;
	
	/**
	 * [x1, y1, x2, y2, has_collision]
	 */
	array<float> draw_edges;
	int draw_edges_size;
	int num_draw_edges;
	float draw_edges_width;
	
	bool precision_edge = false;
	float precision_edge_x1;
	float precision_edge_y1;
	float precision_edge_x2;
	float precision_edge_y2;
	float precision_edge_marker_x1;
	float precision_edge_marker_y1;
	float precision_edge_marker_x2;
	float precision_edge_marker_y2;
	
	dictionary tiles_cache;
	int tiles_cache_size;
	int tile_cache_layer;
	TileCachChunkQueue tiles_cache_queue(20);
	
	textfield@ display_text;
	int display_mode_timer = 0;
	
	script()
	{
		@g = get_scene();
		@cam = get_active_camera();
		
		edges_flags.resize(4);
		
		@display_text = create_textfield();
        display_text.align_horizontal(0);
        display_text.align_vertical(1);
		display_text.colour(DISPLAY_TEXT_FILL_COLOUR);
	}
	
	void editor_step()
	{
		if(!enabled)
			return;
		
		prev_mouse_x = mouse.x;
		prev_mouse_y = mouse.y;
		mouse.layer = layer = clamp(layer, 6, 20);
		mouse.step();
		
		view_x = cam.x();
		view_y = cam.y();
		
		if(mouse.left_down && mouse.middle_press)
		{
			precision_mode = !precision_mode;
			display_mode_timer = DISPLAY_MODE_TIME;
			force_mouse_update = true;
		}
		
		update_drag_size();
		
		int scroll;
		
		if(!precision_mode && mouse.left_down && mouse.scrolled(scroll))
		{
			size = clamp(size - scroll * SCROLL_AMOUNT, 0.0, 9999.0);
			force_mouse_update = true;
		}
		
		if(mouse.middle_press && !mouse.left_down)
		{
			switch(update_collision)
			{
				case OnOffProperty::Keep:
				case OnOffProperty::On:
					update_collision = OnOffProperty::Off;
					break;
				case OnOffProperty::Off:
					update_collision = OnOffProperty::On;
					break;
			}
		}
		
		if(mouse.right_press)
		{
			force_mouse_update = true;
		}
		
		if(tile_cache_layer != layer || mouse.right_release)
		{
			if(tiles_cache_size > 0)
			{
				tiles_cache.deleteAll();
				tiles_cache_queue.clear();
				tiles_cache_size = 0;
			}
			
			tile_cache_layer = layer;
		}
		
		if(!force_mouse_update && mouse.x == prev_mouse_x && mouse.y == prev_mouse_y)
		{
			force_mouse_update = false;
			return;
		}
		
		num_draw_edges = 0;
		precision_edge = false;
		draw_edges_width = precision_mode ? PRECISE_EDGE_RENDER_WIDTH : EDGE_RENDER_WIDTH;
		edges_flags[TileEdge::Top]		= update_top;
		edges_flags[TileEdge::Bottom]	= update_bottom;
		edges_flags[TileEdge::Left]		= update_left;
		edges_flags[TileEdge::Right]	= update_right;
		
		if(precision_mode)
		{
			precise_mode();
		}
		else if((render_edges && always_render_edges) || (mouse.right_down && drag_size == DragSizeState::Off))
		{
			brush_mode();
		}
	}
	
	void update_drag_size()
	{
		if(!precision_mode)
		{
			if(drag_size == DragSizeState::On)
			{
				if(!mouse.left_down && !mouse.right_down)
				{
					drag_size = DragSizeState::Off;
				}
				else
				{
					float x = g.mouse_x_world(0, 19);
					size = max(1, drag_size_start + (x - drag_size_x));
				}
			}
			else if(mouse.left_down && mouse.right_press)
			{
				drag_size = DragSizeState::On;
				display_mode_timer = 0;
				drag_size_start = size;
				drag_size_x = g.mouse_x_world(0, 19);
				drag_size_y = g.mouse_y_world(0, 19);
			}
		}
	}
	
	void brush_mode()
	{
		bool do_update = mouse.right_down && drag_size == DragSizeState::Off;
		float mouse_x;
		float mouse_y;
		
		if(drag_size == DragSizeState::On)
		{
			mouse_x = drag_size_x;
			mouse_y = drag_size_y;
		}
		else
		{
			mouse_x = mouse.x;
			mouse_y = mouse.y;
		}
		
		Line line;
		array<bool>@ edges_flags = @this.edges_flags;
		
		const float size_sqr = size * size;
		
		const float layer_size = size * get_layer_scale(19, layer);
		int start_x = floor_int((mouse_x - layer_size) * PIXEL2TILE);
		int start_y = floor_int((mouse_y - layer_size) * PIXEL2TILE);
		int end_x = floor_int((mouse_x + layer_size) * PIXEL2TILE);
		int end_y = floor_int((mouse_y + layer_size) * PIXEL2TILE);
		
		const int chunk_start_x = floor_int(float(start_x) / TILES_CACHE_CHUNK_SIZE);
		const int chunk_start_y = floor_int(float(start_y) / TILES_CACHE_CHUNK_SIZE);
		const int chunk_end_x = floor_int(float(end_x) / TILES_CACHE_CHUNK_SIZE);
		const int chunk_end_y = floor_int(float(end_y) / TILES_CACHE_CHUNK_SIZE);
		
		for(int chunk_y = chunk_start_y; chunk_y <= chunk_end_y; chunk_y++)
		{
			for(int chunk_x = chunk_start_x; chunk_x <= chunk_end_x; chunk_x++)
			{
				TileCachChunk@ chunk = get_tiles_cache_chunk(chunk_x, chunk_y);
				
				int c_start_x = int(max(chunk.start_tile_x, start_x));
				int c_start_y = int(max(chunk.start_tile_y, start_y));
				int c_end_x = int(min(chunk.start_tile_x + TILES_CACHE_CHUNK_SIZE - 1, end_x));
				int c_end_y = int(min(chunk.start_tile_y + TILES_CACHE_CHUNK_SIZE - 1, end_y));
				
//				outline_rect(g,
//					(c_start_x) * TILE2PIXEL + 5,
//					(c_start_y) * TILE2PIXEL + 5,
//					(c_end_x + 1) * TILE2PIXEL - 5,
//					(c_end_y + 1) * TILE2PIXEL - 5,
//					22, 24, 2, 0xff00ff00);
//				
//				outline_rect(g,
//					chunk_x * TILES_CACHE_CHUNK_SIZE * TILE2PIXEL,
//					chunk_y * TILES_CACHE_CHUNK_SIZE * TILE2PIXEL,
//					(chunk_x * TILES_CACHE_CHUNK_SIZE + TILES_CACHE_CHUNK_SIZE) * TILE2PIXEL,
//					(chunk_y * TILES_CACHE_CHUNK_SIZE + TILES_CACHE_CHUNK_SIZE) * TILE2PIXEL,
//					22, 23, 2, 0xffff0000);
				
				for(int tile_y = c_start_y; tile_y <= c_end_y; tile_y++)
				{
					for(int tile_x = c_start_x; tile_x <= c_end_x; tile_x++)
					{
						tileinfo@ tile = chunk.get_tile(g, layer, tile_x, tile_y);
						
						if(!tile.solid())
							continue;
						
						int type = tile.type();
						float x = tile_x * TILE2PIXEL;
						float y = tile_y * TILE2PIXEL;
						bool requires_update = false;
						
						for(int edge = TileEdge::Top; edge <= TileEdge::Right; edge++)
						{
							if(!check_edge(tile_x, tile_y, tile, type, edge))
								continue;
							
							/*
							 * Get the end points of this edge, and calculate if it is within the brush radius
							 */
							
							get_edge_points(type, edge, line.x1, line.y1, line.x2, line.y2, x, y);
							
							float px, py;
							line.closest_point(mouse_x, mouse_y, px, py);
							
							if(dist_sqr(mouse_x, mouse_y, px, py) > size_sqr)
								continue;
							
							/*
							 * Update the edge flags if necessary
							 */
							
							uint8 edge_bits = get_tile_edge(tile, edge);
							
							if(do_update)
							{
								if(update_edge_bits(tile, edge, edge_bits, edge_bits))
								{
									requires_update = true;
								}
							}
							
							/*
							 * Store for rendering
							 */
							
							if(render_edges)
							{
								add_draw_edge(line.x1, line.y1, line.x2, line.y2, (edge_bits & EdgeFlags::Collision) != 0);
							}
						} // edge
						
						if(requires_update)
						{
							g.set_tile(tile_x, tile_y, layer, tile, false);
						}
						
					} // tile x
				} // tile y
				
			} // chunk x
		} // chunk y
	}
	
	void precise_mode()
	{
		bool do_update = mouse.right_down && drag_size == DragSizeState::Off;
		
		Line line;
		
		const float max_distance = 48 * 48;
		float closest_px;
		float closest_py;
		float closest_distance = 9999999;
		float closest_tile_distance = 9999999;
		int closest_edge = -1;
		int closest_tile_x = -1;
		int closest_tile_y = -1;
		tileinfo@ closest_tile;
		
		const int mouse_tile_x = floor_int(mouse.x * PIXEL2TILE);
		const int mouse_tile_y = floor_int(mouse.y * PIXEL2TILE);
		
		const float distance = precision_inside_only ? 0 : 48;
		const int start_x = floor_int((mouse.x - distance) * PIXEL2TILE);
		const int start_y = floor_int((mouse.y - distance) * PIXEL2TILE);
		const int end_x = floor_int((mouse.x + distance) * PIXEL2TILE);
		const int end_y = floor_int((mouse.y + distance) * PIXEL2TILE);
		
		for(int tile_x = start_x; tile_x <= end_x; tile_x++)
		{
			for(int tile_y = start_y; tile_y <= end_y; tile_y++)
			{
				tileinfo@ tile = g.get_tile(tile_x, tile_y, layer);
				
				if(!tile.solid())
					continue;
				
				int type = tile.type();
				float x = tile_x * TILE2PIXEL;
				float y = tile_y * TILE2PIXEL;
				
				/*
				 * Find the closest edge
				 */
				
				for(int edge = TileEdge::Top; edge <= TileEdge::Right; edge++)
				{
					if(!check_edge(tile_x, tile_y, tile, type, edge))
						continue;
					
					/*
					 * Get the end points of this edge, and calculate the distance to the mouse
					 */
					
					get_edge_points(type, edge, line.x1, line.y1, line.x2, line.y2, x, y);
					
					const float shrink_factor = 0.002;
					float px, py;
					
					// Shrink the edge very slightly so that when two edges share an end point, the edge closer to the mouse will take priority
					float dx = (line.x2 - line.x1) * shrink_factor;
					float dy = (line.y2 - line.y1) * shrink_factor;
					line.x1 += dx;
					line.y1 += dy;
					line.x2 -= dx;
					line.y2 -= dy;
					
					line.closest_point(mouse.x, mouse.y, px, py);
					
					float dist = dist_sqr(mouse.x, mouse.y, px, py);
					float tile_dist = dist_sqr(mouse.x, mouse.y, x + 24, y + 24);
					
					// Prioritise the tile the mouse is inside of
					if(tile_x == mouse_tile_x && tile_y == mouse_tile_y)
					{
						dist = clamp(dist - 0.01, 0, 9999.0);
					}
					// or the tile closer to the mouse
					else if(tile_dist < closest_tile_distance && closest_edge >= 0)
					{
						dist = clamp(dist - 0.009, 0, 9999.0);
					}
					
					if(dist <= max_distance && dist < closest_distance)
					{
						closest_distance = dist;
						closest_edge = edge;
						closest_px = px;
						closest_py = py;
						closest_tile_x = tile_x;
						closest_tile_y = tile_y;
						closest_tile_distance = tile_dist;
						@closest_tile = tile;
					}
				} // edges
				
			} // y
		} // x
		
		/*
		 * Update that edge
		 */
		
		if(closest_edge == -1)
			return;
		
		int type = closest_tile.type();
		float x = closest_tile_x * TILE2PIXEL;
		float y = closest_tile_y * TILE2PIXEL;
		
		get_edge_points(type, closest_edge, line.x1, line.y1, line.x2, line.y2, x, y);
		
		precision_edge = true;
		precision_edge_marker_x1 = mouse.x;
		precision_edge_marker_y1 = mouse.y;
		precision_edge_marker_x2 = closest_px;
		precision_edge_marker_y2 = closest_py;
		precision_edge_x1 = line.x1;
		precision_edge_y1 = line.y1;
		precision_edge_x2 = line.x2;
		precision_edge_y2 = line.y2;
		
		if(do_update)
		{
			uint8 edge_bits = get_tile_edge(closest_tile, closest_edge);
			
			if(update_edge_bits(closest_tile, closest_edge, edge_bits, edge_bits) || true)
			{
				g.set_tile(closest_tile_x, closest_tile_y, layer, closest_tile, false);
				
				/*
				 * Remove the neighbouring edge if this one has been set
				 */
				
				if(precision_update_neighbour && (edge_bits & (EdgeFlags::Priority | EdgeFlags::Collision)) != 0 && is_full_edge(type, closest_edge))
				{
					int n_x;
					int n_y;
					int n_edge = opposite_tile_edge(closest_edge);
					edge_adjacent_tile(closest_edge, closest_tile_x, closest_tile_y, n_x, n_y);
					
					tileinfo@ neightbour_tile = g.get_tile(n_x, n_y, layer);
					
					if(is_full_edge(neightbour_tile.type(), n_edge) && neightbour_tile.solid())
					{
						uint8 n_edge_bits = get_tile_edge(neightbour_tile, n_edge);
						n_edge_bits &= ~(EdgeFlags::Priority | EdgeFlags::Collision);
						set_tile_edge(neightbour_tile, n_edge, n_edge_bits);
						g.set_tile(n_x, n_y, layer, neightbour_tile, false);
					}
				}
			}
		}
		
		if(render_edges)
		{
			for(int edge = TileEdge::Top; edge <= TileEdge::Right; edge++)
			{
				if(!check_edge(closest_tile_x, closest_tile_y, closest_tile, type, edge))
					continue;
				
				uint8 edge_bits = get_tile_edge(closest_tile, edge);
				get_edge_points(type, edge, line.x1, line.y1, line.x2, line.y2, x, y);
				add_draw_edge(line.x1, line.y1, line.x2, line.y2, (edge_bits & EdgeFlags::Collision) != 0);
			}
		}
	}
	
	void add_draw_edge(float x1, float y1, float x2, float y2, bool has_collision)
	{
		if(num_draw_edges >= draw_edges_size)
		{
			draw_edges.insertLast(x1);
			draw_edges.insertLast(y1);
			draw_edges.insertLast(x2);
			draw_edges.insertLast(y2);
			draw_edges.insertLast(has_collision ? 1 : 0);
			num_draw_edges += 5;
			draw_edges_size += 5;
		}
		else
		{
			draw_edges[num_draw_edges++] = x1;
			draw_edges[num_draw_edges++] = y1;
			draw_edges[num_draw_edges++] = x2;
			draw_edges[num_draw_edges++] = y2;
			draw_edges[num_draw_edges++] = has_collision ? 1 : 0;
		}
	}
	
	bool check_edge(int tile_x, int tile_y, tileinfo@ tile, int tile_type, int edge)
	{
		return	edges_flags[edge] &&
				is_valid_edge(tile_type, edge) &&
				(
					edges == EdgeType::Both ||
					is_external_edge(g, layer, tile_x, tile_y, tile, tile_type, edge, external_different_sprites) == (edges == EdgeType::External)
				);
	}
	
	bool update_edge_bits(tileinfo@ tile, int edge, uint8 edge_bits, uint8 &out result)
	{
		uint8 old_edge_bits = edge_bits;
		
		if(update_collision == OnOffProperty::Off)
		{
			edge_bits &= ~EdgeFlags::Collision;
		}
		else if(update_collision == OnOffProperty::On)
		{
			edge_bits |= (EdgeFlags::Priority | EdgeFlags::Collision);
		}
		
		if(update_edges == OnOffProperty::Off)
		{
			edge_bits &= ~EdgeFlags::Priority;
		}
		else if(update_edges == OnOffProperty::On)
		{
			edge_bits |= EdgeFlags::Priority;
		}
		
		result = edge_bits;
		
		if(edge_bits != old_edge_bits)
		{
			set_tile_edge(tile, edge, edge_bits);
			return true;
		}
		
		return false;
	}
	
	TileCachChunk@ get_tiles_cache_chunk(int chunk_x, int chunk_y)
	{
		string key = chunk_x + ',' + chunk_y;
		
		if(tiles_cache.exists(key))
		{
			return cast<TileCachChunk@>(@tiles_cache[key]);
		}
		
		if(tiles_cache_queue.is_full())
		{
			string remove_key;
			tiles_cache_queue.dequeue(remove_key);
			
			tiles_cache.delete(remove_key);
			tiles_cache_size--;
		}
		
		TileCachChunk@ chunk = TileCachChunk(TILES_CACHE_CHUNK_SIZE, chunk_x, chunk_y);
		tiles_cache_queue.enqueue(key);
		@tiles_cache[key] = @chunk;
		tiles_cache_size++;
		
		return @chunk;
	}
	
	void editor_draw(float sub_frame)
	{
		if(!enabled)
			return;
		
		/*
		 * Setup
		 */
		
		float x, y;
		
		if(drag_size == DragSizeState::On)
		{
			x = drag_size_x;
			y = drag_size_y;
		}
		else
		{
			transform_layer_position(view_x, view_y, mouse.x, mouse.y, layer, 22, x, y);
		}
		
		/*
		 * Render tile edges
		 */
		
		if(render_edges || precision_mode)
		{
			for(int i = 0; i < num_draw_edges; i += 5)
			{
				float x1 = draw_edges[i + 0];
				float y1 = draw_edges[i + 1];
				float x2 = draw_edges[i + 2];
				float y2 = draw_edges[i + 3];
				bool has_collision = draw_edges[i + 4] == 1;
				
				transform_layer_position(view_x, view_y, x1, y1, layer, 22, x1, y1);
				transform_layer_position(view_x, view_y, x2, y2, layer, 22, x2, y2);
				
				if(has_collision)
				{
					draw_line(g, 22, 25, x1, y1, x2, y2, draw_edges_width, EDGE_ON_COLOUR);
				}
				else
				{
					float dx = x2 - x1;
					float dy = y2 - y1;
					draw_line(g, 22, 23, x1, y1, x1 + dx * EDGE_OFF_FACTOR, y1 + dy * EDGE_OFF_FACTOR, draw_edges_width, EDGE_OFF_COLOUR);
					draw_line(g, 22, 23, x2, y2, x2 - dx * EDGE_OFF_FACTOR, y2 - dy * EDGE_OFF_FACTOR, draw_edges_width, EDGE_OFF_COLOUR);
				}
			}
		}
		
		/*
		 * Render cursor
		 */
		
		const float cursor_x = drag_size == DragSizeState::On ? drag_size_x : x;
		const float cursor_y = drag_size == DragSizeState::On ? drag_size_y : y;
		
		if(precision_mode)
		{
			if(precision_edge)
			{
				/*
				 * Arrow pointing to edge
				 */
				
				float edge_marker_x1;
				float edge_marker_y1;
				float edge_marker_x2;
				float edge_marker_y2;
				transform_layer_position(view_x, view_y, precision_edge_marker_x1, precision_edge_marker_y1, layer, 22, edge_marker_x1, edge_marker_y1);
				transform_layer_position(view_x, view_y, precision_edge_marker_x2, precision_edge_marker_y2, layer, 22, edge_marker_x2, edge_marker_y2);
				
				draw_line(g, 22, 23,
					edge_marker_x1, edge_marker_y1, edge_marker_x2, edge_marker_y2,
					PRECISE_EDGE_RENDER_WIDTH, PRECISE_EDGE_ARROW_COLOUR);
				
				/*
				 * Highlight edge
				 */
				
				float edge_x1;
				float edge_y1;
				float edge_x2;
				float edge_y2;
				transform_layer_position(view_x, view_y, precision_edge_x1, precision_edge_y1, layer, 22, edge_x1, edge_y1);
				transform_layer_position(view_x, view_y, precision_edge_x2, precision_edge_y2, layer, 22, edge_x2, edge_y2);
				
				draw_line(g, 22, 22,
					edge_x1, edge_y1, edge_x2, edge_y2,
					PRECISE_EDGE_MARKER_WIDTH, PRECISE_EDGE_MARKER_COLOUR);
			}
		}
		else
		{
			float layer_size, _lsy;
			transform_layer_position(view_x, view_y, mouse.x, mouse.y, layer, 22, layer_size, _lsy);
			draw_circle(g, cursor_x, cursor_y, size, 64, 22, 24, 3, 0xaaffffff);
			
			if(drag_size == DragSizeState::On)
			{
				display_text.text(int(size) + '');
				display_text.draw_world(22, 24, cursor_x, cursor_y - DISPLAY_TEXT_OFFSET, 1, 1, 0);
			}
		}
		
		if(display_mode_timer > 0)
		{
			display_text.text(precision_mode ? 'Precision' : 'Brush');
			display_text.draw_world(22, 24, cursor_x, cursor_y - DISPLAY_TEXT_OFFSET, 1, 1, 0);
			
			display_mode_timer--;
		}
		
		/*
		 * Render on/off icon
		 */
		
		if(update_collision == OnOffProperty::Keep)
		{
			g.draw_rectangle_world(22, 24, x - KEEP_WIDTH, y - KEEP_WIDTH, x + KEEP_WIDTH, y + KEEP_WIDTH, 0, ON_OFF_COLOUR);
		}
		else
		{
			uint colour = update_collision == OnOffProperty::On ? EDGE_ON_COLOUR : EDGE_OFF_COLOUR;
			colour = (colour & 0x00ffffff) | ON_OFF_ALPHA;
			g.draw_rectangle_world(22, 24, x - ON_OFF_LENGTH, y - ON_OFF_WIDTH, x + ON_OFF_LENGTH, y + ON_OFF_WIDTH, 0, colour);
			
			if(update_collision == OnOffProperty::On)
			{
				g.draw_rectangle_world(22, 24, x - ON_OFF_LENGTH, y - ON_OFF_WIDTH, x + ON_OFF_LENGTH, y + ON_OFF_WIDTH, 90, colour);
			}
		}
	}
	
}
