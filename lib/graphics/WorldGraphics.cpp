#include '../math/math.cpp';
#include 'Graphics.cpp';

class WorldGraphics : Graphics
{
	
	WorldGraphics(scene@ g, uint layer, uint sub_layer)
	{
		super(g, layer, sub_layer);
	}
	
	void draw_rectangle(
		const float &in x1, const float &in y1, const float &in x2, const float &in y2,
		const float &in rotation, const uint &in colour) const override
	{
		g.draw_rectangle_world(layer, sub_layer, x1, y1, x2, y2, rotation, colour);
	}
	
	void draw_glass(
		const float &in x1, const float &in y1, const float &in x2, const float &in y2,
		const float &in rotation, const uint &in colour) const override
	{
		g.draw_glass_world(layer, sub_layer, x1, y1, x2, y2, rotation, colour);
	}

	void draw_gradient(
		const float &in x1, const float &in y1, const float &in x2, const float &in y2,
		const uint &in c00, const uint &in c10, const uint &in c11, const uint &in c01) const override
	{
		g.draw_gradient_world(layer, sub_layer, x1, y1, x2, y2, c00, c10, c11, c01);
	}

	void draw_line(
		const float &in x1, const float &in y1, const float &in x2, const float &in y2,
		const float &in width, uint colour) const override
	{
		const float dx = x2 - x1;
		const float dy = y2 - y1;
		const float length = sqrt(dx * dx + dy * dy);
		
		const float mx = (x1 + x2) * 0.5;
		const float my = (y1 + y2) * 0.5;
		
		g.draw_rectangle_world(layer, sub_layer,
			mx - width, my - length * 0.5,
			mx + width, my + length * 0.5, atan2(-dx, dy) * RAD2DEG, colour);
	}

	void draw_quad(
		const bool &in is_glass,
		const float &in x1, const float &in y1, const float &in x2, const float &in y2,
		const float &in x3, const float &in y3, const float &in x4, const float &in y4,
		const uint &in c1, const uint &in c2, const uint &in c3, const uint &in c4) const override
	{
		g.draw_quad_world(layer, sub_layer, is_glass, x1, y1, x2, y2, x3, y3, x4, y4, c1, c2, c3, c4);
	}
	
	void draw_sprite(
		sprites@ sprite,
		const string &in sprite_name, const uint &in frame, const uint &in palette,
		const float &in x, const float &in y, const float &in rotation,
		const float &in scale_x, const float &in scale_y,
		const uint &in colour) const override
	{
		sprite.draw_world(layer, sub_layer, sprite_name, frame, palette, x, y, rotation, scale_x, scale_y, colour);
	}
	
}