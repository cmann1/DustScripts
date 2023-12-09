#include '../../lib/std.cpp';
#include '../../lib/drawing/tile.cpp';
#include '../../lib/enums/ColType.cpp';
#include '../../lib/enums/EmitterId.cpp';
#include '../../lib/enums/Team.cpp';
#include '../../lib/events/MessageSystem.cpp';
#include '../../lib/input/GVB.cpp';
#include '../../lib/tiles/TileShape.cpp';

class BreakableWall : enemy_base, callback_base
{
	
	[persist] bool active = true;
	[persist] float health = 5;
	[persist] int emitter_id = -1;
	[persist] int layer = -1;
	[persist] int sublayer = -1;
	
	/** The main `script` must implement `IMessageSystemSource` for these events to work.
	 * If not blank broadcasts an event when hit for the first time. */
	[persist] string first_hit_event = '';
	/** If not blank broadcasts an event when a tile is destroyed. */
	[persist] string tile_destroy_event = '';
	/** If not blank broadcasts an event when all tiles have been destroyed. */
	[persist] string destroy_event = '';
	
	[hidden] array<BreakableWallTile> tiles;
	
	private scriptenemy@ self;
	private scene@ g;
	
	private input_api@ input;
	private editor_api@ editor;
	private camera@ cam;
	private textfield@ txt;
	
	private float x1 = 0;
	private float y1 = 0;
	private float x2 = 0;
	private float y2 = 0;
	private string filter_id;
	
	private bool queue_update;
	
	private MessageSystem@ messages;
	
	void init(script@ s, scriptenemy@ self)
	{
		@this.self = self;
		
		@g = get_scene();
		
		self.auto_physics(false);
		
		if(is_playing())
		{
			filter_id = '_bw_filter_' + self.id();
			
			if(tiles.length == 0)
			{
				g.remove_entity(self.as_entity());
				return;
			}
			
			for(uint i = 0; i < tiles.length; i++)
			{
				BreakableWallTile@ t = @tiles[i];
				t.health = t.health == -1 ? health : t.health;
			}
			
			self.on_hurt_callback(this, 'on_hurt', 0);
			
			queue_update = true;
			
			IMessageSystemSource@ msg_src = cast<IMessageSystemSource@>(s);
			@messages = @msg_src != null ? msg_src.message_system : null;
		}
		else
		{
			@input = get_input_api();
			@editor = get_editor_api();
			@cam = get_active_camera();
			
			@txt = create_textfield();
			txt.set_font('envy_bold', 20);
			
			for(uint i = 0; i < tiles.length; i++)
			{
				BreakableWallTile@ t = @tiles[i];
				t.update_tile(g);
			}
			
			update_hitbox(false);
		}
	}
	
	private void update_hitbox(const bool update_collision=true)
	{
		if(tiles.length == 0)
		{
			if(update_collision)
			{
				self.hit_collision().remove();
				self.base_collision().remove();
			}
			return;
		}
		
		float cx = 0;
		float cy = 0;
		
		for(uint i = 0; i < tiles.length; i++)
		{
			BreakableWallTile@ t = @tiles[i];
			float x = t.x * 48;
			float y = t.y * 48;
			
			if(i == 0)
			{
				x1 = x;
				y1 = y;
				x2 = x + 48;
				y2 = y + 48;
			}
			else
			{
				x1 = min(x1, x);
				y1 = min(y1, y);
				x2 = max(x2, x + 48);
				y2 = max(y2, y + 48);
			}
			
			x += 24;
			y += 24;
			cx += x;
			cy += y;
		}
		
		if(update_collision)
		{
			cx /= tiles.length;
			cy /= tiles.length;
			self.set_xy(cx, cy);
			self.hit_rectangle(y1 - cy, y2 - cy, x1 - cx, x2 - cx);
			self.base_rectangle(y1 - cy, y2 - cy, x1 - cx, x2 - cx);
		}
	}
	
	private void on_hurt(controllable@ attacked, controllable@ attacker, hitbox@ hb, int arg)
	{
		if(!active)
			return;
		
		const int damage = hb.damage();
		if(damage <= 0)
			return;
		
		rectangle@ r = hb.base_rectangle();
		float x = hb.x();
		float y = hb.y();
		const float x1 = x + r.left();
		const float y1 = y + r.top();
		const float x2 = x + r.right();
		const float y2 = y + r.bottom();
		
		if(damage == 10000)
		{
			x = attacker.centre_x();
			y = attacker.centre_y();
		}
		
		int break_count = 0;
		
		for(int i = int(tiles.length) - 1; i >= 0; i--)
		{
			BreakableWallTile@ t = @tiles[i];
			const float tx1 = t.x * 48;
			const float ty1 = t.y * 48;
			const float tx2 = tx1 + 48;
			const float ty2 = ty1 + 48;
			
			if(tx1 > x2 || ty1 > y2 || tx2 < x1 || ty2 < y1)
				continue;
			
			t.health -= damage;
			
			trigger_event(first_hit_event , 'first_hit', t);
			first_hit_event = '';
			
			if(t.health <= 0)
			{
				int emitter_id = this.emitter_id;
				if(emitter_id == -1)
				{
					tileinfo@ tile = g.get_tile(t.x, t.y);
					emitter_id = tile.solid()
						? g.get_emitter_type_id('cleansed_fb_' + tile.sprite_set())
						: EmitterId::CleansedFb1;
				}
				
				int layer = this.layer < 0 ? 19 : this.layer;
				int sublayer = this.sublayer < 0 ? 12 : this.sublayer;
				
				g.add_particle_burst(emitter_id, tx1 + 24, ty1 + 24, 48, 48, layer, sublayer);
				
				g.set_tile(t.x, t.y, 19, false, TileShape::Full, 0, 0, 0);
				tiles.removeAt(i);
				
				trigger_event(tile_destroy_event, 'tile_destroy', t);
				
				break_count++;
			}
		}
		
		if(break_count > 0)
		{
			if(tiles.length > 0)
			{
				update_hitbox();
			}
			else
			{
				trigger_event(destroy_event, 'destroy', null);
				g.remove_entity(self.as_entity());
			}
		}
	}
	
	void step()
	{
		if(queue_update)
		{
			update_hitbox();
			queue_update = false;
		}
		
		int i = g.get_entity_collision(y1, y2, x1, x2, ColType::Hitbox);
		while(--i >= 0)
		{
			hitbox@ hb = g.get_hitbox_collision_index(i);
			if(@hb == null || hb.team() == Team::Filth)
				continue;
			message@ meta = hb.metadata();
			if(meta.has_int(filter_id))
				continue;
			
			meta.set_int(filter_id, 1);
			hb.on_hit_filter_callback(this, 'on_hit_filter', 0);
		}
	}
	
	private bool on_hit_filter(hitbox@ hb, hittable@ h, int)
	{
		if(!h.is_same(self.as_entity()))
			return true;
		
		rectangle@ r = hb.base_rectangle();
		const float x = hb.x();
		const float y = hb.y();
		const float x1 = x + r.left();
		const float y1 = y + r.top();
		const float x2 = x + r.right();
		const float y2 = y + r.bottom();
		
		int hit_count = 0;
		
		for(int i = int(tiles.length) - 1; i >= 0; i--)
		{
			BreakableWallTile@ t = @tiles[i];
			const float tx1 = t.x * 48;
			const float ty1 = t.y * 48;
			const float tx2 = tx1 + 48;
			const float ty2 = ty1 + 48;
			
			if(tx1 <= x2 && ty1 <= y2 && tx2 >= x1 && ty2 >= y1)
				return true;
		}
		
		return false;
	}
	
	private void trigger_event(const string event, const string type, BreakableWallTile@ t=null)
	{
		if(@messages == null)
			return;
		if(event == '')
			return;
		
		message@ msg = create_message();
		msg.set_string('name', event);
		msg.set_string('type', type);
		msg.set_entity('breakable_wall', self.as_entity());
		if(@t != null)
		{
			msg.set_int('x', t.x);
			msg.set_int('y', t.y);
		}
		messages.broadcast(event, msg);
	}
	
	void editor_step()
	{
		if(editor.mouse_in_gui())
			return;
		if(input.key_check_gvb(GVB::Space))
			return;
		if(!self.is_same(editor.get_selected_entity()))
			return;
		
		const bool left_mouse_down = input.key_check_gvb(GVB::LeftClick);
		const bool right_mouse_down = input.key_check_gvb(GVB::RightClick);
		const uint mouse_state = input.mouse_state();
		const int scroll = mouse_state & 0x1 != 0 ? 1 : mouse_state & 0x2 != 0 ? -1 : 0;
		
		if(input.key_check_gvb(GVB::Shift) && (left_mouse_down || right_mouse_down) || scroll != 0)
		{
			if(edit_tile(left_mouse_down, right_mouse_down, scroll))
			{
				update_hitbox(false);
			}
			
			if(left_mouse_down)
				input.key_clear_gvb(GVB::LeftClick);
			if(right_mouse_down)
				input.key_clear_gvb(GVB::RightClick);
		}
	}
	
	private bool edit_tile(const bool add, const bool remove, const int scroll)
	{
		float mouse_x, mouse_y;
		input.mouse_world(19, 10, mouse_x, mouse_y);
		
		if(!add && (mouse_x < x1 || mouse_x > x2 || mouse_y < y1 || mouse_y > y2))
			return false;
		
		const int tile_x = floor_int(mouse_x / 48);
		const int tile_y = floor_int(mouse_y / 48);
		
		for(int i = int(tiles.length) - 1; i >= 0; i--)
		{
			BreakableWallTile@ t = @tiles[i];
			if(t.x != tile_x || t.y != tile_y)
				continue;
			
			if(add)
				return false;
			
			if(remove)
			{
				tiles[i] = tiles[tiles.length - 1];
				tiles.resize(tiles.length - 1);
				return true;
			}
			
			if(scroll != 0)
			{
				t.health += scroll < 0 ? -1 : 1;
				if(t.health == 0)
				{
					t.health = scroll < 0 ? -1 : 1;
				}
				else if(t.health < 0)
				{
					t.health = -1;
				}
			}
			
			return false;
		}
		
		if(add)
		{
			tileinfo@ tile = g.get_tile(tile_x, tile_y);
			if(tile.solid())
			{
				tiles.resize(tiles.length + 1);
				BreakableWallTile@ t = @tiles[tiles.length - 1];
				t.init(tile_x, tile_y, tile.solid(), tile.type());
				return true;
			}
		}
		
		return false;
	}
	
	void editor_draw(float sub_frame)
	{
		const uint base_clr = 0xda5050;
		const uint fill_alpha = 0x11000000;
		const uint fill_empty_alpha = 0x22000000;
		const uint selected_fill_alpha = 0x22000000;
		const uint selected_fill_empty_alpha = 0x44000000;
		const uint outline_alpha = 0x77000000;
		
		const bool is_selected = self.is_same(editor.get_selected_entity());
		const float zoom = 1 / cam.editor_zoom();
		const bool update_tile = timestamp_now() % 2 == 0;
		
		float mouse_x, mouse_y;
		input.mouse_world(19, 10, mouse_x, mouse_y);
		const int tx = floor_int(mouse_x / 48);
		const int ty = floor_int(mouse_y / 48);
		
		for(uint i = 0; i < tiles.length; i++)
		{
			BreakableWallTile@ t = @tiles[i];
			
			if(update_tile)
			{
				t.update_tile(g);
			}
			
			const float x = t.x * 48;
			const float y = t.y * 48;
			
			fill_tile(g, 22, 22, t.shape, x, y, base_clr | (t.solid
				? is_selected ? selected_fill_alpha : fill_alpha
				: is_selected ? selected_fill_empty_alpha : fill_empty_alpha));
			
			if(t.solid)
			{
				outline_tile(g, 22, 22, t.shape, x, y, 2 * zoom, base_clr | outline_alpha);
			}
			
			if(is_selected && t.x == tx && t.y == ty)
			{
				txt.align_horizontal(0);
				txt.align_vertical(0);
				txt.text(t.health + '');
				txt.colour(base_clr | 0xff000000);
				txt.draw_world(22, 22, x + 24, y + 24, 0.8, 0.8, 0);
			}
		}
		
		const float x = self.x();
		const float y = self.y();
		fill_tile(g, 22, 22, 0, x - 24, y - 24, 0xffffff | (is_selected ? 0x33000000 : 0x22000000));
		outline_tile(g, 22, 22, 0, x - 24, y - 24, 2 * zoom, 0xffffff | (is_selected ? 0x99000000 : 0x66000000));
		
		if(is_selected)
		{
			txt.align_horizontal(0);
			txt.align_vertical(1);
			txt.text(
				'Add tile: [Shift+LeftMouse]\n'+
				'Remove tile: [Shift+RightMouse]\n'+
				'Adjust tile health: [MouseWheel]');
			txt.colour(0xffffffff);
			txt.draw_hud(15, 15, 0, SCREEN_BOTTOM - 10, 0.8, 0.8, 0);
		}
	}
	
}

class BreakableWallTile
{
	
	[persist] float health = -1;
	[persist] int x;
	[persist] int y;
	
	int shape;
	bool solid = true;
	
	BreakableWallTile() {}
	
	void init(const int x, const int y, const bool solid, const int shape)
	{
		this.x = x;
		this.y = y;
		this.solid = solid;
		this.shape = shape;
	}
	
	void update_tile(scene@ g)
	{
		tileinfo@ tile = g.get_tile(x, y);
		solid = tile.solid();
		shape = solid ? tile.type() : TileShape::Full;
	}
	
}
