#include '../../../tiles/common.cpp';
#include '../../../tiles/closest_point_on_tile.cpp';
#include '../../../tiles/TileEdge.cpp';

#include 'Constraint.cpp';
#include '../ITileProvider.cpp';

class TileConstraint : Constraint
{
	
	ITileProvider@ tile_provider;
	Particle@ particle;
	float friction;
	float radius;
	
	private raycast@ ray;
	
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
		tileinfo@ tile;
		
		if(radius <= 0)
		{
			@tile = tile_provider.get_tile(tx, ty);
			
			if(@tile == null || !tile.solid())
				return;
			
			float normal_x, normal_y;
			if(!point_in_tile(particle.x, particle.y, tx, ty, tile.type(),
				normal_x, normal_y))
				return;
		}
		
		tileinfo@ closest_tile = null;
		float closest_dist_sqr = 9999999.0;
		bool closest_inside = false;
		float closest_x;
		float closest_y;
		float closest_nx;
		float closest_ny;
		
		const int tx1 = int(floor((particle.x - 48 - radius) / 48));
		const int ty1 = int(floor((particle.y - 48 - radius) / 48));
		const int tx2 = int(floor((particle.x + 48 + radius) / 48));
		const int ty2 = int(floor((particle.y + 48 + radius) / 48));
		
		for(tx = tx1; tx <= tx2; tx++)
		{
			for(ty = ty1; ty <= ty2; ty++)
			{
				@tile = tile_provider.get_tile(tx, ty);
				if(@tile == null || !tile.solid())
					continue;
				
				float hit_x, hit_y;
				float normal_x, normal_y;
				const int type = tile.type();
				if(!closest_point_on_tile(
					particle.x, particle.y, tx, ty, type, tile,
					hit_x, hit_y, normal_x, normal_y))
						continue;
				
				const float dx = hit_x - particle.x;
				const float dy = hit_y - particle.y;
				const float dist_sqr = dx * dx + dy * dy;
				bool inside = true;
				
				if(dist_sqr >= closest_dist_sqr)
					continue;
				
				if(radius > 0)
				{
					float _;
					inside = point_in_tile(particle.x, particle.y, tx, ty, type, _, _);
					if(!inside && dist_sqr > radius * radius)
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
			//float dx = particle.x - particle.prev_x;
			//float dy = particle.y - particle.prev_y;
			//
			//if(!particle.has_contact || dx * dx + dy * dy > particle.contact_vx * particle.contact_vx + particle.contact_vy * particle.contact_vy)
			//{
			//	particle.contact_vx = dx;
			//	particle.contact_vy = dy;
			//	particle.contact_nx = closest_nx;
			//	particle.contact_ny = closest_ny;
			//}
			//
			//particle.has_contact = true;
			
			if(radius > 0)
			{
				const float dx = closest_inside ? particle.x - closest_x : closest_x - particle.x;
				const float dy = closest_inside ? particle.y - closest_y : closest_y - particle.y;
				const float l = sqrt(dx * dx + dy * dy);
				particle.x = closest_x - dx / l * radius;
				particle.y = closest_y - dy / l * radius;
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
	
}
