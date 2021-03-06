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
	
	private int tx1, ty1, tx2, ty2;
	private array<TileData@> tiles;
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
			
			const int c = (tx2 - tx1) * (ty2 - ty1);
			tiles.resize(c);
			for(int i = 0; i < c; i++)
			{
				@tiles[i] = null;
			}
		}
		
		const int idx = (ty - ty1) * (tx2 - tx1) + (tx - tx1);
		
		TileData@ t = @tiles[idx];
		
		if(@t != null)
		{
			if(frame - t.age <= max_age)
				return t;
		}
		
		@t = @tiles[idx] = TileData(g.get_tile(tx, ty, collision_layer), frame);
		return t;
	}
	
}
