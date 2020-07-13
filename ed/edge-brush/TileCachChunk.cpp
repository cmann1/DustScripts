class TileCachChunk
{
	
	private array<tileinfo@> chunk;
	private int size;
	private int x;
	private int y;
	int start_tile_x;
	int start_tile_y;
	
	TileCachChunk(int size, int x, int y)
	{
		this.size = size;
		this.x = y;
		this.x = y;
		this.start_tile_x = x * size;
		this.start_tile_y = y * size;
		chunk.resize(size * size);
	}
	
	tileinfo@ get_tile(scene@ g, int layer, int tile_x, int tile_y)
	{
		const int index = (tile_y - start_tile_y) * size + (tile_x - start_tile_x);
		tileinfo@ tile = chunk[index];
		
		if(@tile == null)
		{
			@tile = g.get_tile(tile_x, tile_y, layer);
			@chunk[index] = @tile;
		}
		
		return @tile;
	}
	
}