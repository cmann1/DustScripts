#include '../lib/std.cpp';

class script
{
	
	scene@ g;
	bool in_game = false;
	
	controllable@ player = null;
	camera@ cam;
	
	script()
	{
		@g = get_scene();
		@cam = get_camera(0);
	}
	
	void on_level_start()
	{
		in_game = true;
	}
	
}

class TileBaseTrigger : trigger_base
{
	
	[text] bool run = false;
	[text] bool run_continuous = false;
	[text] int layer = 19;
	
	bool run_prev = false;
	
	[hidden] float prev_x = 0;
	[hidden] float prev_y = 0;
	
	scene@ g;
	scripttrigger @self;
	
	TileBaseTrigger()
	{
		@g = get_scene();
	}
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.self = self;
		self.square(true);
		run_prev = run;
	}
	
	void _run()
	{
		const float sx = self.x();
		const float sy = self.y();
		const float r = self.radius();
		const int x1 = int(floor((sx - r) / 48));
		const int y1 = int(floor((sy - r) / 48));
		const int x2 = int(floor((sx + r) / 48));
		const int y2 = int(floor((sy + r) / 48));
		
		for(int x = x1; x <= x2; x++)
		{
			for(int y = y1; y <= y2; y++)
			{
				tileinfo@ tile = g.get_tile(x, y, layer);
				this.updateTile(x, y, tile);
			}
		}
	}
	
	void updateTile(int x, int y, tileinfo@ tile)
	{
		
	}
	
	void editor_step()
	{
		if(run_prev != run or run_continuous)
		{
			if(run_continuous)
			{
				if(prev_x == self.x() and prev_y == self.y())
					return;
				prev_y = self.y();
				prev_x = self.x();
			}
			
			_run();
			run_prev = run;
		}
	}
	
	void activate(controllable @e) {}
	
}

class RemoveTileEdges: TileBaseTrigger
{
	
	[text] bool solid = false;
	[text] bool top = true;
	[text] bool bottom = true;
	[text] bool left = true;
	[text] bool right = true;
	
	protected uint8 updateDrawEdge(uint8 edgeBits, bool solid)
	{
		edgeBits &= ~8;
		if(solid)
			edgeBits |= 8;
		else
			edgeBits = 0;
		
		return edgeBits;
	}
	
	void updateTile(int x, int y, tileinfo@ tile)
	{
		if(!tile.solid()) return;
		
		if(top)
			tile.edge_top(updateDrawEdge(tile.edge_top(), solid));
		if(bottom)
			tile.edge_bottom(updateDrawEdge(tile.edge_bottom(), solid));
		if(left)
			tile.edge_left(updateDrawEdge(tile.edge_left(), solid));
		if(right)
			tile.edge_right(updateDrawEdge(tile.edge_right(), solid));
		g.set_tile(x, y, layer, tile, false);
	}
	
}

class CopyTileEdges: TileBaseTrigger
{
	
	[text] bool ignore_dustblocks = false;
	[text] int target_layer = 17;
	[text] int sprite_set = 2;
	[text] int sprite_tile = 8;
	[text] int sprite_palette = 1;
	
	void updateTile(int x, int y, tileinfo@ tile)
	{
		if(!tile.solid())
			return;
		
		if(ignore_dustblocks && tile.is_dustblock())
			return;
		
		if(tile.edge_top() & 8 == 0 && tile.edge_bottom() & 8 == 0 && tile.edge_left() & 8 == 0 && tile.edge_right() & 8 == 0)
			return;
		
		tile.sprite_set(sprite_set);
		tile.sprite_tile(sprite_tile);
		tile.sprite_palette(sprite_palette);
		
		g.set_tile(x, y, target_layer, tile, false);
	}
	
}

class MoveTiles: TileBaseTrigger
{
	
	[text] bool copy = false;
	[text] bool ignore_dustblocks = false;
	[text] int target_layer = 20;
	
	void updateTile(int x, int y, tileinfo@ tile)
	{
		if(!tile.solid())
			return;
			
		if(ignore_dustblocks && tile.is_dustblock())
			return;
		
		g.set_tile(x, y, target_layer, tile, false);
		
		if(!copy)
		{
			tile.solid(false);
			g.set_tile(x, y, layer, tile, false);
		}
	}
	
}

class SetTileSprites: TileBaseTrigger
{
	
	[text] bool ignore_dustblocks = false;
	
	[text] int filter_set = -1;
	[text] int filter_tile = -1;
	[text] int filter_palette = -1;
	
	[text] int target_set = -1;
	[text] int target_tile = -1;
	[text] int target_palette = -1;
	
	void updateTile(int x, int y, tileinfo@ tile)
	{
		if(!tile.solid())
			return;
			
		if(ignore_dustblocks && tile.is_dustblock())
			return;
		
		if(
			(filter_set == -1 || filter_set == int(tile.sprite_set())) &&
			(filter_tile == -1 || filter_tile == int(tile.sprite_tile())) &&
			(filter_palette == -1 || filter_palette == int(tile.sprite_palette()))
		)
		{
			if(target_set != -1)
				tile.sprite_set(target_set);
				
			if(target_tile != -1)
				tile.sprite_tile(target_tile);
				
			if(target_palette != -1)
				tile.sprite_palette(target_palette);
			
			g.set_tile(x, y, layer, tile, false);
		}
	}
	
}

class MakeTilesInivisible: TileBaseTrigger
{
	
	void updateTile(int x, int y, tileinfo@ tile)
	{
		if(!tile.solid()) return;
		
		tile.sprite_tile(0);
		
		g.set_tile(x, y, layer, tile, false);
	}
	
}