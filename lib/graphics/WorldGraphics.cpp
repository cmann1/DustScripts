#include '../math/math.cpp';
#include 'Graphics.cpp';

class WorldGraphics : Graphics
{
	
	WorldGraphics(scene@ g, uint layer, uint sub_layer)
	{
		super(g, layer, sub_layer);
	}
	
	void draw_rectangle(
		const float x1, const float y1, const float x2, const float y2,
		const float rotation, const uint colour) const override
	{
		g.draw_rectangle_world(layer, sub_layer, x1, y1, x2, y2, rotation, colour);
	}
	
	void draw_glass(
		const float x1, const float y1, const float x2, const float y2,
		const float rotation, const uint colour) const override
	{
		g.draw_glass_world(layer, sub_layer, x1, y1, x2, y2, rotation, colour);
	}

	void draw_gradient(
		const float x1, const float y1, const float x2, const float y2,
		const uint c00, const uint c10, const uint c11, const uint c01) const override
	{
		g.draw_gradient_world(layer, sub_layer, x1, y1, x2, y2, c00, c10, c11, c01);
	}

	void draw_line(
		const float x1, const float y1, const float x2, const float y2,
		const float width, uint colour) const override
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
		const bool is_glass,
		const float x1, const float y1, const float x2, const float y2,
		const float x3, const float y3, const float x4, const float y4,
		const uint c1, const uint c2, const uint c3, const uint c4) const override
	{
		g.draw_quad_world(layer, sub_layer, is_glass, x1, y1, x2, y2, x3, y3, x4, y4, c1, c2, c3, c4);
	}
	
	void draw_sprite(
		sprites@ sprite,
		const string sprite_name, const uint frame, const uint palette,
		const float x, const float y, const float rotation,
		const float scale_x, const float scale_y,
		const uint colour) const override
	{
		sprite.draw_world(layer, sub_layer, sprite_name, frame, palette, x, y, rotation, scale_x, scale_y, colour);
	}
	
	void draw_text(
		textfield@ text_field,
		const float x, const float y,
		const float scale_x, const float scale_y,
		const float rotation) const override
	{
		text_field.draw_world(layer, sub_layer, x, y, scale_x, scale_y, rotation);
	}
	
}