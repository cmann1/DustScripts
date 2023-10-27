#include '../emitters/EmitterBurstManager.cpp';
#include '../utils/RandomStream.cpp';

/// Manages cleaning dustblocks similar to how it's done by the engine/dustman.
/// Call clear on `cechkpoint_load` and `update` in step.
class DustblockCleaner
{
	
	/// If set will use this instead of the built in rand() method.
	RandomStream@ rng;
	/// If set will create emitter bursts using the built in particle burst API when blocks are cleared.
	bool create_particles = true;
	/// If set will create emitter bursts when blocks are cleared.
	EmitterBurstManager@ emitter_bursts;
	/// If true will play sounds when blocks are cleared.
	bool play_sounds = true;
	// If true only dusblocks can be cleared.
	bool only_dustblocks = true;
	/// Controls how quickly the timer for invisible cleared tiles runs down.
	float clear_increment = 30;
	
	EmitterBurstSettings emitter_settings(
		0, 18, 3, 48, 48, 12, 1);
	
	protected scene@ g;
	
	protected int pending_tiles_size = 32;
	protected int pending_tiles_count = 0;
	protected array<DustblockCleanerData> pending_tiles(pending_tiles_size);
	
	protected int sound_count;
	
	DustblockCleaner(scene@ g)
	{
		@this.g = g;
	}
	
	void clear()
	{
		pending_tiles_count = 0;
		sound_count = 0;
	}
	
	int clean_rect(const float x1, const float y1, const float x2, const float y2, const float time = 0)
	{
		const int tx1 = int(floor(x1 / 48.0));
		const int ty1 = int(floor(y1 / 48.0));
		const int tx2 = int(floor(x2 / 48.0));
		const int ty2 = int(floor(y2 / 48.0));
		
		int clean_count = 0;
		
		for(int tx = tx1; tx <= tx2; tx++)
		{
			for(int ty = ty1; ty <= ty2; ty++)
			{
				if(clean_tile(tx, ty, time))
				{
					clean_count++;
				}
			}
		}
		
		return clean_count;
	}
	
	int clean_circle(const float x, const float y, const float radius, const float time = 0)
	{
		const int tx1 = int(floor((x - radius) / 48.0));
		const int ty1 = int(floor((y - radius) / 48.0));
		const int tx2 = int(floor((x + radius) / 48.0));
		const int ty2 = int(floor((y + radius) / 48.0));
		
		int clean_count = 0;
		const float r2 = radius * radius;
		
		for(int tx = tx1; tx <= tx2; tx++)
		{
			for(int ty = ty1; ty <= ty2; ty++)
			{
				const float dx = (tx * 48 + 24) - x;
				const float dy = (ty * 48 + 24) - y;
				
				if((dx * dx + dy * dy) > r2)
					continue;
				
				if(clean_tile(tx, ty, time))
				{
					clean_count++;
				}
			}
		}
		
		return clean_count;
	}
	
	/// `time` - How long the invisible tiles will persist before being removed.
	///          Setting this to -1 will clear block immediatelly instead of at the end of the frame when time expires.
	///          A value of 10 is the default used to clear dust blocks the player touches.
	bool clean_tile(const int x, const int y, const float time = 0)
	{
		tileinfo@ tile = g.get_tile(x, y);
		
		if(!tile.solid())
			return false;
		
		DustblockCleanerData@ d;
		int di = -1;
		
		for(int i = 0; i < pending_tiles_count; i++)
		{
			DustblockCleanerData@ cd = @pending_tiles[i];
			if(cd.x == x && cd.y == y)
			{
				@d = cd;
				di = i;
				break;
			}
		}
		
		if(@d != null && tile.sprite_set() == 0 && tile.sprite_tile() == 0)
		{
			if(time >= 0)
			{
				d.time = min(d.time, time);
			}
			else
			{
				tile.solid(false);
				g.set_tile(d.x, d.y, 19, tile, true);
				
				pending_tiles[di] = pending_tiles[--pending_tiles_count];
			}
			
			return false;
		}
		
		if(only_dustblocks && !tile.is_dustblock())
			return false;
		
		if(@d == null)
		{
			if(time >= 0)
			{
				if(pending_tiles_count >= pending_tiles_size)
				{
					pending_tiles.resize(pending_tiles_size *= 2);
				}
				
				@d = @pending_tiles[pending_tiles_count++];
				d.x = x;
				d.y = y;
				d.time = time;
				
				tile.sprite_tile(0);
				g.set_tile(d.x, d.y, 19, tile, true);
			}
			else
			{
				tile.solid(false);
				g.set_tile(d.x, d.y, 19, tile, true);
			}
			
			const uint8 sprite_set = tile.sprite_set();
			
			if(play_sounds && sound_count < 4)
			{
				string t_name;
				switch(sprite_set)
				{
					case 2: t_name = 'leaf'; break;
					case 3: t_name = 'trash'; break;
					case 4: t_name = 'slime'; break;
					case 5: t_name = 'poly'; break;
					default: t_name = 'dust'; break;
				}
				
				const uint val = (@rng != null ? rng.nexti() : rand()) % 4;
				sound_count++;
				g.play_sound(
					val == 0
						? 'sfx_' + t_name + '_med'
						: 'sfx_' + t_name + '_light_' + val,
					x * 48 + 24, y * 48 + 24, 1, false, true);
			}
			
			if(create_particles)
			{
				g.add_particle_burst(
					g.get_emitter_id('cleansed_fb_' + sprite_set),
					x * 48 + 24, y * 48 + 24,
					emitter_settings.size_x, emitter_settings.size_y,
					emitter_settings.layer, emitter_settings.sub_layer);
			}
			else if(@emitter_bursts != null)
			{
				emitter_settings.emitter_id = g.get_emitter_id('cleansed_fb_' + sprite_set);
				emitter_bursts.add(emitter_settings, x * 48 + 24, y * 48 + 24);
			}
		}
		else if(time >= 0)
		{
			d.time = min(d.time, time);
		}
		else
		{
			tile.solid(false);
			g.set_tile(d.x, d.y, 19, tile, true);
			
			pending_tiles[di] = pending_tiles[--pending_tiles_count];
		}
		
		return true;
	}
	
	void update(const float time_scale)
	{
		sound_count = 0;
		
		for(int i = pending_tiles_count - 1; i >= 0; i--)
		{
			DustblockCleanerData@ d = @pending_tiles[i];
			d.time -= clear_increment * DT * time_scale;
			if(d.time <= 0)
			{
				tileinfo@ tile = g.get_tile(d.x, d.y);
				
				if(tile.solid())
				{
					tile.solid(false);
					g.set_tile(d.x, d.y, 19, tile, true);
				}
				
				pending_tiles[i] = pending_tiles[--pending_tiles_count];
			}
		}
	}
	
}

class DustblockCleanerData
{
	
	int x, y;
	float time;
	
}
