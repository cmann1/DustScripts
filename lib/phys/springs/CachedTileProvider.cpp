/// API calls can be slow, so use this to automatically cache an area of tiles
/// Tiles will be recached after "max_age" to allow for changing geometry
/// One of these should be used per object/local grouping of particles
/// Make sure to set increment the frame property each step
class CachedTileProvider : ITileProvider
{
	
	scene@ g;
	int collision_layer = 19;
	float frame;
	
	int max_age = 6;
	/// Keep solid tiles cached for longer.
	int max_age_solid = 24;
	/// Keep dustblocks cached for shorter since it's more likely they'll change.
	int max_age_dustblock = 3;
	
	private int tx1, ty1, tx2, ty2;
	private int tiles_size = 32;
	private int tiles_count;
	private array<TileData@> tiles(tiles_size);
	private bool init = false;
	
	TileData@ get_tile(const int tx, const int ty) override
	{
		if(tx < tx1 || tx >= tx2 || ty < ty1 || ty >= ty2)
		{
			// An upper limit just to make sure the array doesn't get too big
			// if something moves around a lot
			if((tx2 - tx1) * (ty2 - ty1) > 5000)
				init = false;
			
			if(!init)
			{
				init = true;
				tx1 = tx - 5;
				ty1 = ty - 5;
				tx2 = tx + 6;
				ty2 = ty + 6;
			}
			else
			{
				if(tx < tx1) tx1 = tx - 5;
				if(ty < ty1) ty1 = ty - 5;
				if(tx >= tx2) tx2 = tx + 6;
				if(ty >= ty2) ty2 = ty + 6;
			}
			
			const int new_count = (tx2 - tx1) * (ty2 - ty1);
			while(new_count >= tiles_size)
			{
				tiles.resize(tiles_size *= 2);
			}
			
			for(int i = 0; i < tiles_count; i++)
			{
				tiles[i].type = -1;
			}
			for(int i = tiles_count; i < new_count; i++)
			{
				@tiles[i] = TileData();
			}
			tiles_count = new_count;
		}
		
		TileData@ t = @tiles[(ty - ty1) * (tx2 - tx1) + (tx - tx1)];
		
		if(t.type != -1)
		{
			if(frame - t.age <= max_age)
				return t;
		}
		
		tileinfo@ tile = g.get_tile(tx, ty, collision_layer);
		t.init(tile, frame);
		
		if(t.solid)
		{
			if(tile.is_dustblock())
			{
				t.age += max_age_dustblock - max_age;
			}
			else
			{
				t.age += max_age_solid - max_age;
			}
		}
		
		return t;
	}
	
	/// Clears all cached tiles
	void clear()
	{
		tiles_count = 0;
		init = false;
		tx1 = ty1 = tx2 = ty2 = 0;
	}
	
	/// Clears all cached tiles in the region inclusive.
	void clear(const int x1, const int y1, const int x2, const int y2)
	{
		for(int x = x1; x <= x2; x++)
		{
			if(x < tx1 || x >= tx2)
				continue;
			
			for(int y = y1; y <= y2; y++)
			{
				if(y < ty1 || y > ty2)
					continue;
				
				tiles[(y - ty1) * (tx2 - tx1) + (x - tx1)].type = -1;
			}
		}
	}
	
}
