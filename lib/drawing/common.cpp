#include '../math/math.cpp';
#include '../math/geom.cpp';

void draw_arrow(scene@ g, uint layer, uint sub_layer, float x1, float y1, float x2, float y2, float width=2, float head_size=20, float head_position=1, uint colour=0xFFFFFFFF, bool world=true)
{
	float dx = x2 - x1;
	float dy = y2 - y1;
	const float length = sqrt(dx * dx + dy * dy);
	
	if(length <= 0)
		return;
	
	const float x3 = x1 + dx * head_position;
	const float y3 = y1 + dy * head_position;
	
	dx = dx / length * head_size;
	dy = dy / length * head_size;
	const float x4  = x3 - dx;
	const float y4  = y3 - dy;
	
	if(world)
	{
		g.draw_line_world(layer, sub_layer, x1, y1, x2, y2, width, colour);
		g.draw_line_world(layer, sub_layer, x3, y3, x4 + dy, y4 - dx, width, colour);
		g.draw_line_world(layer, sub_layer, x3, y3, x4 - dy, y4 + dx, width, colour);
	}
	else
	{
		g.draw_line_hud(layer, sub_layer, x1, y1, x2, y2, width, colour);
		g.draw_line_hud(layer, sub_layer, x3, y3, x4 + dy, y4 - dx, width, colour);
		g.draw_line_hud(layer, sub_layer, x3, y3, x4 - dy, y4 + dx, width, colour);
	}
}

void draw_arrow_fill(
	scene@ g, uint layer, uint sub_layer, float x1, float y1, float x2, float y2, float width=2,
	float head_width=20, float head_length=20, float head_position=1, uint colour=0xFFFFFFFF, bool world=true)
{
	float dx = x2 - x1;
	float dy = y2 - y1;
	const float length = sqrt(dx * dx + dy * dy);
	
	if(length <= 0)
		return;
	
	const float x3 = x1 + dx * head_position;
	const float y3 = y1 + dy * head_position;
	
	dx = length > 0 ? dx / length : 1;
	dy = length > 0 ? dy / length : 0;
	const float x4  = x3 - dx * head_length;
	const float y4  = y3 - dy * head_length;
	
	if(world)
	{
		if(head_position == 1)
		{
			g.draw_line_world(layer, sub_layer, x1, y1, x4, y4, width, colour);
		}
		else
		{
			g.draw_line_world(layer, sub_layer, x1, y1, x2, y2, width, colour);
		}
		
		g.draw_quad_world(layer, sub_layer, false,
			x3, y3, x3, y3,
			x4 + dy * head_width, y4 - dx * head_width,
			x4 - dy * head_width, y4 + dx * head_width,
			colour, colour, colour, colour);
	}
	else
	{
		if(head_position == 1)
		{
			g.draw_line_hud(layer, sub_layer, x1, y1, x4, y4, width, colour);
		}
		else
		{
			g.draw_line_hud(layer, sub_layer, x1, y1, x2, y2, width, colour);
		}
		
		g.draw_quad_hud(layer, sub_layer, false,
			x3, y3, x3, y3,
			x4 + dy * head_width, y4 - dx * head_width,
			x4 - dy * head_width, y4 + dx * head_width,
			colour, colour, colour, colour);
	}
}

void draw_glowing_line(
	scene@ g, uint layer, uint sub_layer,
	float x1, float y1, float x2, float y2,
	float thickness, float glow_width,
	uint colour, uint glow_inner_colour, uint glow_outer_colour,
	bool start_cap=true, bool end_cap=true,
	bool world=true)
{
	float dx = x2 - x1;
	float dy = y2 - y1;
	const float length = sqrt(dx * dx + dy * dy);
	
	const float mx = (x1 + x2) * 0.5;
	const float my = (y1 + y2) * 0.5;
	
	const float angle = atan2(-dx, dy);
	
	normalize(-dy, dx, dx, dy);
	const float outer_dx = dx * (glow_width + thickness * 0.5);
	const float outer_dy = dy * (glow_width + thickness * 0.5);
	const float inner_dx = dx * thickness * 0.5;
	const float inner_dy = dy * thickness * 0.5;
	const float cap_factor = 0.7;
	const float cap_dx = dx * glow_width;
	const float cap_dy = dy * glow_width;
	
	if(world)
	{
		// Bottom
		g.draw_quad_world(layer, sub_layer, false,
			x1 + inner_dx - dx,
			y1 + inner_dy - dy,
			x2 + inner_dx - dx,
			y2 + inner_dy - dy,
			x2 + outer_dx, y2 + outer_dy, x1 + outer_dx, y1 + outer_dy,
			glow_inner_colour, glow_inner_colour, glow_outer_colour, glow_outer_colour);
		// Top
		g.draw_quad_world(layer, sub_layer, false,
			x1 - inner_dx + dx,
			y1 - inner_dy + dy,
			x2 - inner_dx + dx,
			y2 - inner_dy + dy,
			x2 - outer_dx, y2 - outer_dy, x1 - outer_dx, y1 - outer_dy,
			glow_inner_colour, glow_inner_colour, glow_outer_colour, glow_outer_colour);
		
		if(start_cap)
		{
			// Bottom
			g.draw_quad_world(layer, sub_layer, false,
				x1 + inner_dx, y1 + inner_dy,
				x1 + outer_dx, y1 + outer_dy,
				x1 + outer_dx * cap_factor - cap_dy * cap_factor,
				y1 + outer_dy * cap_factor + cap_dx * cap_factor,
				x1 + inner_dx - cap_dy,
				y1 + inner_dy + cap_dx,
				glow_inner_colour, glow_outer_colour, glow_outer_colour, glow_outer_colour);
			// Top
			g.draw_quad_world(layer, sub_layer, false,
				x1 - inner_dx, y1 - inner_dy,
				x1 - outer_dx, y1 - outer_dy,
				x1 - outer_dx * cap_factor - cap_dy * cap_factor,
				y1 - outer_dy * cap_factor + cap_dx * cap_factor,
				x1 - inner_dx - cap_dy,
				y1 - inner_dy + cap_dx,
				glow_inner_colour, glow_outer_colour, glow_outer_colour, glow_outer_colour);
			// Middle
			g.draw_quad_world(layer, sub_layer, false,
				x1 - inner_dx + dy, y1 - inner_dy - dx,
				x1 + inner_dx + dy, y1 + inner_dy - dx,
				x1 + inner_dx - cap_dy, y1 + inner_dy + cap_dx,
				x1 - inner_dx - cap_dy, y1 - inner_dy + cap_dx,
				glow_inner_colour, glow_inner_colour, glow_outer_colour, glow_outer_colour);
		}
		
		if(end_cap)
		{
			// Bottom
			g.draw_quad_world(layer, sub_layer, false,
				x2 + inner_dx, y2 + inner_dy,
				x2 + outer_dx, y2 + outer_dy,
				x2 + outer_dx * cap_factor + cap_dy * cap_factor,
				y2 + outer_dy * cap_factor - cap_dx * cap_factor,
				x2 + inner_dx + cap_dy,
				y2 + inner_dy - cap_dx,
				glow_inner_colour, glow_outer_colour, glow_outer_colour, glow_outer_colour);
			// Top
			g.draw_quad_world(layer, sub_layer, false,
				x2 - inner_dx, y2 - inner_dy,
				x2 - outer_dx, y2 - outer_dy,
				x2 - outer_dx * cap_factor + cap_dy * cap_factor,
				y2 - outer_dy * cap_factor - cap_dx * cap_factor,
				x2 - inner_dx + cap_dy, y2 - inner_dy - cap_dx,
				glow_inner_colour, glow_outer_colour, glow_outer_colour, glow_outer_colour);
			// Middle
			g.draw_quad_world(layer, sub_layer, false,
				x2 - inner_dx - dy, y2 - inner_dy + dx,
				x2 + inner_dx - dy, y2 + inner_dy + dx,
				x2 + inner_dx + cap_dy, y2 + inner_dy - cap_dx,
				x2 - inner_dx + cap_dy, y2 - inner_dy - cap_dx,
				glow_inner_colour, glow_inner_colour, glow_outer_colour, glow_outer_colour);
		}
		
		g.draw_rectangle_world(layer, sub_layer,
			mx - thickness, my - length * 0.5,
			mx + thickness, my + length * 0.5, angle * RAD2DEG, colour);
	}
	else
	{
		// Bottom
		g.draw_quad_hud(layer, sub_layer, false,
			x1 + inner_dx - dx,
			y1 + inner_dy - dy,
			x2 + inner_dx - dx,
			y2 + inner_dy - dy,
			x2 + outer_dx, y2 + outer_dy, x1 + outer_dx, y1 + outer_dy,
			glow_inner_colour, glow_inner_colour, glow_outer_colour, glow_outer_colour);
		// Top
		g.draw_quad_hud(layer, sub_layer, false,
			x1 - inner_dx + dx,
			y1 - inner_dy + dy,
			x2 - inner_dx + dx,
			y2 - inner_dy + dy,
			x2 - outer_dx, y2 - outer_dy, x1 - outer_dx, y1 - outer_dy,
			glow_inner_colour, glow_inner_colour, glow_outer_colour, glow_outer_colour);
		
		if(start_cap)
		{
			// Bottom
			g.draw_quad_hud(layer, sub_layer, false,
				x1 + inner_dx, y1 + inner_dy,
				x1 + outer_dx, y1 + outer_dy,
				x1 + outer_dx * cap_factor - cap_dy * cap_factor,
				y1 + outer_dy * cap_factor + cap_dx * cap_factor,
				x1 + inner_dx - cap_dy,
				y1 + inner_dy + cap_dx,
				glow_inner_colour, glow_outer_colour, glow_outer_colour, glow_outer_colour);
			// Top
			g.draw_quad_hud(layer, sub_layer, false,
				x1 - inner_dx, y1 - inner_dy,
				x1 - outer_dx, y1 - outer_dy,
				x1 - outer_dx * cap_factor - cap_dy * cap_factor,
				y1 - outer_dy * cap_factor + cap_dx * cap_factor,
				x1 - inner_dx - cap_dy,
				y1 - inner_dy + cap_dx,
				glow_inner_colour, glow_outer_colour, glow_outer_colour, glow_outer_colour);
			// Middle
			g.draw_quad_hud(layer, sub_layer, false,
				x1 - inner_dx + dy, y1 - inner_dy - dx,
				x1 + inner_dx + dy, y1 + inner_dy - dx,
				x1 + inner_dx - cap_dy, y1 + inner_dy + cap_dx,
				x1 - inner_dx - cap_dy, y1 - inner_dy + cap_dx,
				glow_inner_colour, glow_inner_colour, glow_outer_colour, glow_outer_colour);
		}
		
		if(end_cap)
		{
			// Bottom
			g.draw_quad_hud(layer, sub_layer, false,
				x2 + inner_dx, y2 + inner_dy,
				x2 + outer_dx, y2 + outer_dy,
				x2 + outer_dx * cap_factor + cap_dy * cap_factor,
				y2 + outer_dy * cap_factor - cap_dx * cap_factor,
				x2 + inner_dx + cap_dy,
				y2 + inner_dy - cap_dx,
				glow_inner_colour, glow_outer_colour, glow_outer_colour, glow_outer_colour);
			// Top
			g.draw_quad_hud(layer, sub_layer, false,
				x2 - inner_dx, y2 - inner_dy,
				x2 - outer_dx, y2 - outer_dy,
				x2 - outer_dx * cap_factor + cap_dy * cap_factor,
				y2 - outer_dy * cap_factor - cap_dx * cap_factor,
				x2 - inner_dx + cap_dy, y2 - inner_dy - cap_dx,
				glow_inner_colour, glow_outer_colour, glow_outer_colour, glow_outer_colour);
			// Middle
			g.draw_quad_hud(layer, sub_layer, false,
				x2 - inner_dx - dy, y2 - inner_dy + dx,
				x2 + inner_dx - dy, y2 + inner_dy + dx,
				x2 + inner_dx + cap_dy, y2 + inner_dy - cap_dx,
				x2 - inner_dx + cap_dy, y2 - inner_dy - cap_dx,
				glow_inner_colour, glow_inner_colour, glow_outer_colour, glow_outer_colour);
		}
		
		g.draw_rectangle_hud(layer, sub_layer,
			mx - thickness, my - length * 0.5,
			mx + thickness, my + length * 0.5, angle * RAD2DEG, colour);
	}
}

void draw_dot(scene@ g, int layer, int sub_layer, float x, float y, float size=1, uint colour=0xFFFFFFFF, float rotation=0)
{
	g.draw_rectangle_world(layer, sub_layer, x-size, y-size, x+size, y+size, rotation, colour);
}
void draw_dot_hud(scene@ g, int layer, int sub_layer, float x, float y, float size=1, uint colour=0xFFFFFFFF, float rotation=0)
{
	g.draw_rectangle_hud(layer, sub_layer, x-size, y-size, x+size, y+size, rotation, colour);
}

void outline_rect(scene@ g, uint layer, uint sub_layer, float x1, float y1, float x2, float y2, float thickness=2, uint colour=0xFFFFFFFF)
{
	thickness *= 0.5;
	// Top
	g.draw_rectangle_world(layer, sub_layer,
		x1 - thickness, y1 - thickness,
		x2 + thickness, y1 + thickness,
		0, colour);
	// Bottom
	g.draw_rectangle_world(layer, sub_layer,
		x1 - thickness, y2 - thickness,
		x2 + thickness, y2 + thickness,
		0, colour);
	// Left
	g.draw_rectangle_world(layer, sub_layer,
		x1 - thickness, y1 - thickness,
		x1 + thickness, y2 + thickness,
		0, colour);
	// Right
	g.draw_rectangle_world(layer, sub_layer,
		x2 - thickness, y1 - thickness,
		x2 + thickness, y2 + thickness,
		0, colour);
}
void outline_rect_hud(scene@ g, uint layer, uint sub_layer, float x1, float y1, float x2, float y2, float thickness=2, uint colour=0xFFFFFFFF)
{
	thickness *= 0.5;
	// Top
	g.draw_rectangle_hud(layer, sub_layer,
		x1 - thickness, y1 - thickness,
		x2 + thickness, y1 + thickness,
		0, colour);
	// Bottom
	g.draw_rectangle_hud(layer, sub_layer,
		x1 - thickness, y2 - thickness,
		x2 + thickness, y2 + thickness,
		0, colour);
	// Left
	g.draw_rectangle_hud(layer, sub_layer,
		x1 - thickness, y1 - thickness,
		x1 + thickness, y2 + thickness,
		0, colour);
	// Right
	g.draw_rectangle_hud(layer, sub_layer,
		x2 - thickness, y1 - thickness,
		x2 + thickness, y2 + thickness,
		0, colour);
}

void outline_rect_inside(scene@ g, uint layer, uint sub_layer, float x1, float y1, float x2, float y2, float thickness, uint colour)
{
	thickness *= 0.5;
	// Left
	g.draw_rectangle_world(layer, sub_layer,
		x1,             y1 + thickness,
		x1 + thickness, y2 - thickness, 0, colour);
	// Right
	g.draw_rectangle_world(layer, sub_layer,
		x2 - thickness, y1 + thickness,
		x2,             y2 - thickness, 0, colour);
	// Top
	g.draw_rectangle_world(layer, sub_layer,
		x1, y1,
		x2, y1 + thickness, 0, colour);
	// Bottom
	g.draw_rectangle_world(layer, sub_layer,
		x1, y2 - thickness,
		x2, y2, 0, colour);
}
void outline_rect_inside_hud(scene@ g, uint layer, uint sub_layer, float x1, float y1, float x2, float y2, float thickness, uint colour)
{
	thickness *= 0.5;
	// Left
	g.draw_rectangle_hud(layer, sub_layer,
		x1,             y1 + thickness,
		x1 + thickness, y2 - thickness, 0, colour);
	// Right
	g.draw_rectangle_hud(layer, sub_layer,
		x2 - thickness, y1 + thickness,
		x2,             y2 - thickness, 0, colour);
	// Top
	g.draw_rectangle_hud(layer, sub_layer,
		x1, y1,
		x2, y1 + thickness, 0, colour);
	// Bottom
	g.draw_rectangle_hud(layer, sub_layer,
		x1, y2 - thickness,
		x2, y2, 0, colour);
}

void outline_rect_outside(scene@ g, uint layer, uint sub_layer, float x1, float y1, float x2, float y2, float thickness, uint colour)
{
	thickness *= 0.5;
	// Left
	g.draw_rectangle_world(layer, sub_layer,
		x1 - thickness, y1,
		x1,             y2, 0, colour);
	// Right
	g.draw_rectangle_world(layer, sub_layer,
		x2,             y1,
		x2 + thickness, y2, 0, colour);
	// Top
	g.draw_rectangle_world(layer, sub_layer,
		x1 - thickness, y1 - thickness,
		x2 + thickness, y1, 0, colour);
	// Bottom
	g.draw_rectangle_world(layer, sub_layer,
		x1 - thickness, y2,
		x2 + thickness, y2 + thickness, 0, colour);
}
void outline_rect_outside_hud(scene@ g, uint layer, uint sub_layer, float x1, float y1, float x2, float y2, float thickness, uint colour)
{
	thickness *= 0.5;
	// Left
	g.draw_rectangle_hud(layer, sub_layer,
		x1 - 1, y1,
		x1,     y2, 0, colour);
	// Right
	g.draw_rectangle_hud(layer, sub_layer,
		x2,     y1,
		x2 + 1, y2, 0, colour);
	// Top
	g.draw_rectangle_hud(layer, sub_layer,
		x1 - 1, y1 - 1,
		x2 + 1, y1, 0, colour);
	// Bottom
	g.draw_rectangle_hud(layer, sub_layer,
		x1 - 1, y2,
		x2 + 1, y2 + 1, 0, colour);
}

void outline_rotated_rect(scene@ g, uint layer, uint sub_layer, float x, float y, float size_x, float size_y, float rotation, float thickness=2, uint colour=0xFFFFFFFF)
{
	thickness *= 0.5;
	float p1_x, p1_y, p2_x, p2_y;
	float p3_x, p3_y, p4_x, p4_y;
	
	calculate_rotated_rectangle(
		x, y, size_x, size_y, rotation,
		p1_x, p1_y, p2_x, p2_y,
		p3_x, p3_y, p4_x, p4_y);
	
	const float tx = (p1_x + p2_x) * 0.5;
	const float ty = (p1_y + p2_y) * 0.5;
	const float bx = (p3_x + p4_x) * 0.5;
	const float by = (p3_y + p4_y) * 0.5;
	const float lx = (p1_x + p4_x) * 0.5;
	const float ly = (p1_y + p4_y) * 0.5;
	const float rx = (p2_x + p3_x) * 0.5;
	const float ry = (p2_y + p3_y) * 0.5;
	
	// Top
	g.draw_rectangle_world(layer, sub_layer,
		tx - size_x, ty - thickness,
		tx + size_x, ty + thickness,
		rotation, colour);
	// Bottom
	g.draw_rectangle_world(layer, sub_layer,
		bx - size_x, by - thickness,
		bx + size_x, by + thickness,
		rotation, colour);
	// Left
	g.draw_rectangle_world(layer, sub_layer,
		lx - thickness, ly - size_y,
		lx + thickness, ly + size_y,
		rotation, colour);
	// Right
	g.draw_rectangle_world(layer, sub_layer,
		rx - thickness, ry - size_y,
		rx + thickness, ry + size_y,
		rotation, colour);
}
void outline_rotated_rect_hud(scene@ g, uint layer, uint sub_layer, float x, float y, float size_x, float size_y, float rotation, float thickness=2, uint colour=0xFFFFFFFF)
{
	thickness *= 0.5;
	float p1_x, p1_y, p2_x, p2_y;
	float p3_x, p3_y, p4_x, p4_y;
	
	calculate_rotated_rectangle(
		x, y, size_x, size_y, rotation,
		p1_x, p1_y, p2_x, p2_y,
		p3_x, p3_y, p4_x, p4_y);
	
	const float tx = (p1_x + p2_x) * 0.5;
	const float ty = (p1_y + p2_y) * 0.5;
	const float bx = (p3_x + p4_x) * 0.5;
	const float by = (p3_y + p4_y) * 0.5;
	const float lx = (p1_x + p4_x) * 0.5;
	const float ly = (p1_y + p4_y) * 0.5;
	const float rx = (p2_x + p3_x) * 0.5;
	const float ry = (p2_y + p3_y) * 0.5;
	
	// Top
	g.draw_rectangle_hud(layer, sub_layer,
		tx - size_x, ty - thickness,
		tx + size_x, ty + thickness,
		rotation, colour);
	// Bottom
	g.draw_rectangle_hud(layer, sub_layer,
		bx - size_x, by - thickness,
		bx + size_x, by + thickness,
		rotation, colour);
	// Left
	g.draw_rectangle_hud(layer, sub_layer,
		lx - thickness, ly - size_y,
		lx + thickness, ly + size_y,
		rotation, colour);
	// Right
	g.draw_rectangle_hud(layer, sub_layer,
		rx - thickness, ry - size_y,
		rx + thickness, ry + size_y,
		rotation, colour);
}

void shadowed_text_world(textfield@ tf, int layer, int sub_layer, float x, float y, float scale_x=1, float scale_y=1, float rotation=0, uint shadow_colour=0x77000000, float ox=5, float oy=5)
{
	const uint colour = tf.colour();
	tf.colour(shadow_colour);
	tf.draw_world(layer, sub_layer, x + ox, y + oy, scale_x, scale_y, rotation);
	tf.colour(colour);
	tf.draw_world(layer, sub_layer, x, y, scale_x, scale_y, rotation);
}
void shadowed_text_hud(textfield@ tf, int layer, int sub_layer, float x, float y, float scale_x=1, float scale_y=1, float rotation=0, uint shadow_colour=0x77000000, float ox=5, float oy=5)
{
	const uint colour = tf.colour();
	tf.colour(shadow_colour);
	tf.draw_hud(layer, sub_layer, x + ox, y + oy, scale_x, scale_y, rotation);
	tf.colour(colour);
	tf.draw_hud(layer, sub_layer, x, y, scale_x, scale_y, rotation);
}

void outlined_text_world(textfield@ tf, int layer, int sub_layer, float x, float y, float scale_x=1, float scale_y=1, float rotation=0, uint shadow_colour=0x77000000, float thickness=2)
{
	const uint colour = tf.colour();
	tf.colour(shadow_colour);
	tf.draw_world(layer, sub_layer, x - thickness, y, scale_x, scale_y, rotation);
	tf.draw_world(layer, sub_layer, x + thickness, y, scale_x, scale_y, rotation);
	tf.draw_world(layer, sub_layer, x, y - thickness, scale_x, scale_y, rotation);
	tf.draw_world(layer, sub_layer, x, y + thickness, scale_x, scale_y, rotation);
	tf.colour(colour);
	tf.draw_world(layer, sub_layer, x, y, scale_x, scale_y, rotation);
}
void outlined_text_hud(textfield@ tf, int layer, int sub_layer, float x, float y, float scale_x=1, float scale_y=1, float rotation=0, uint shadow_colour=0x77000000, float thickness=2)
{
	const uint colour = tf.colour();
	tf.colour(shadow_colour);
	tf.draw_hud(layer, sub_layer, x - thickness, y, scale_x, scale_y, rotation);
	tf.draw_hud(layer, sub_layer, x + thickness, y, scale_x, scale_y, rotation);
	tf.draw_hud(layer, sub_layer, x, y - thickness, scale_x, scale_y, rotation);
	tf.draw_hud(layer, sub_layer, x, y + thickness, scale_x, scale_y, rotation);
	tf.colour(colour);
	tf.draw_hud(layer, sub_layer, x, y, scale_x, scale_y, rotation);
}
