#include '../../../tiles/common.cpp';
#include '../../../tiles/TileEdge.cpp';

#include 'Constraint.cpp';
#include 'TileData.cpp';
#include '../ITileProvider.cpp';

class TileConstraint : Constraint
{
	
	ITileProvider@ tile_provider;
	Particle@ particle;
	float friction;
	
	private raycast@ ray;
	private Line line;
	
	TileConstraint(ITileProvider@ tile_provider, Particle@ particle, const float friction=0.0)
	{
		@this.tile_provider = tile_provider;
		@this.particle = particle;
		this.friction = friction;
	}
	
	void resolve(const float time_scale, const int iteration) override
	{
		int tx = floor_int(particle.x / 48);
		int ty = floor_int(particle.y / 48);
		TileData@ tile;
		
		if(particle.radius <= 0)
		{
			@tile = tile_provider.get_tile(tx, ty);
			
			if(@tile == null || !tile.solid)
				return;
			
			float normal_x, normal_y;
			if(!point_in_tile(particle.x, particle.y, tx, ty, tile.type,
				normal_x, normal_y))
				return;
		}
		
		TileData@ closest_tile = null;
		float closest_dist_sqr = 9999999.0;
		bool closest_inside = false;
		float closest_x;
		float closest_y;
		float closest_nx;
		float closest_ny;
		
		const int tx1 = int(floor((particle.x - 48 - particle.radius) / 48));
		const int ty1 = int(floor((particle.y - 48 - particle.radius) / 48));
		const int tx2 = int(floor((particle.x + 48 + particle.radius) / 48));
		const int ty2 = int(floor((particle.y + 48 + particle.radius) / 48));
		
		for(tx = tx1; tx <= tx2; tx++)
		{
			for(ty = ty1; ty <= ty2; ty++)
			{
				@tile = tile_provider.get_tile(tx, ty);
				if(@tile == null || !tile.solid)
					continue;
				
				float hit_x, hit_y;
				float normal_x, normal_y;
				if(!closest_point_on_tile(
					particle.x, particle.y, tx, ty, tile,
					hit_x, hit_y, normal_x, normal_y))
						continue;
				
				const float dx = hit_x - particle.x;
				const float dy = hit_y - particle.y;
				const float dist_sqr = dx * dx + dy * dy;
				bool inside = true;
				
				if(dist_sqr >= closest_dist_sqr)
					continue;
				
				if(particle.radius > 0)
				{
					float _;
					inside = point_in_tile(particle.x, particle.y, tx, ty, tile.type, _, _);
					if(!inside && dist_sqr > particle.radius * particle.radius)
						continue;
				}
				
				@closest_tile = tile;
				closest_x = hit_x;
				closest_y = hit_y;
				closest_nx = normal_x;
				closest_ny = normal_y;
				closest_dist_sqr = dist_sqr;
				closest_inside = inside;
			}
		}
		
		if(@closest_tile != null)
		{
			particle.set_contact(closest_nx, closest_ny);
			
			if(particle.radius > 0)
			{
				const float dx = closest_inside ? particle.x - closest_x : closest_x - particle.x;
				const float dy = closest_inside ? particle.y - closest_y : closest_y - particle.y;
				const float l = sqrt(dx * dx + dy * dy);
				if(l != 0)
				{
					particle.x = closest_x - dx / l * particle.radius;
					particle.y = closest_y - dy / l * particle.radius;
				}
			}
			else
			{
				particle.x = closest_x;
				particle.y = closest_y;
			}
			
			const float x_ratio = 1 / (1 + (time_scale * DT * friction));
			particle.prev_x = particle.x - (particle.x - particle.prev_x) * x_ratio;
			particle.prev_y = particle.y - (particle.y - particle.prev_y) * x_ratio;
		}
	}
	
	bool closest_point_on_tile(
		float x, float y, int tile_x, int tile_y, TileData@ tile,
		float &out out_x, float &out out_y, float &out normal_x, float &out normal_y)
	{
		const float tx = tile_x * TILE2PIXEL;
		const float ty = tile_y * TILE2PIXEL;
		const float local_x = x - tx;
		const float local_y = y - ty;
		bool result = false;
		float closest_dist = MAX_FLOAT;
		
		for(int side = 0; side < 4; side++)
		{
			if(!is_valid_edge(tile.type, side))
				continue;
			
			switch(side)
			{
				case TileEdge::Top:
					if(tile.edge_top & 0x8 == 0)
						continue;
					line.x1 = tile.x1;
					line.y1 = tile.y1;
					line.x2 = tile.x2;
					line.y2 = tile.y2;
					break;
				case TileEdge::Bottom:
					if(tile.edge_bottom & 0x8 == 0)
						continue;
					line.x1 = tile.x3;
					line.y1 = tile.y3;
					line.x2 = tile.x4;
					line.y2 = tile.y4;
					break;
				case TileEdge::Left:
					if(tile.edge_left & 0x8 == 0)
						continue;
					line.x1 = tile.x4;
					line.y1 = tile.y4;
					line.x2 = tile.x1;
					line.y2 = tile.y1;
					break;
				case TileEdge::Right:
					if(tile.edge_right & 0x8 == 0)
						continue;
					line.x1 = tile.x2;
					line.y1 = tile.y2;
					line.x2 = tile.x3;
					line.y2 = tile.y3;
					break;
			}
			
			float check_x, check_y;
			line.closest_point(local_x, local_y, check_x, check_y);
			
			const float dx = tx + check_x - x;
			const float dy = ty + check_y - y;
			const float dist = dx * dx + dy * dy;
			
			if(dist < closest_dist)
			{
				out_x = tx + check_x;
				out_y = ty + check_y;
				closest_dist = dist;
				normal_x = line.y2 - line.y1;
				normal_y = -(line.x2 - line.x1);
				result = true;
			}
		}
		
		if(result)
		{
			const float l = sqrt(normal_x * normal_x + normal_y * normal_y);
			normal_x /= l;
			normal_y /= l;
		}
		
		return result;
	}
	
}
