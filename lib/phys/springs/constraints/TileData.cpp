#include '../../../tiles/get_tile_quad.cpp';

class TileData
{
	
	int type;
	bool solid;
	uint8 edge_top;
	uint8 edge_bottom;
	uint8 edge_left;
	uint8 edge_right;
	float x1, y1, x2, y2, x3, y3, x4, y4;
	
	float age;
	
	TileData(tileinfo@ t, const float age=0)
	{
		type = t.type();
		solid = t.solid();
		edge_top = t.edge_top();
		edge_bottom = t.edge_bottom();
		edge_left = t.edge_left();
		edge_right = t.edge_right();
		this.age = age;
		
		get_tile_quad(type, x1, y1, x2, y2, x3, y3, x4, y4);
	}
	
}
