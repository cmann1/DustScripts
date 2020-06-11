void draw_arrow(scene@ g, uint layer, uint sub_layer, float x1, float y1, float x2, float y2, float width=2, float head_size=20, float head_position=1, uint colour=0xFFFFFFFF, bool world=true)
{
	float dx = x2 - x1;
	float dy = y2 - y1;
	float length = sqrt(dx * dx + dy * dy);
	
	if(length <= 0)
		return;
	
	const float x3  = x1 + dx * head_position;
	const float y3  = y1 + dy * head_position;
	
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

void draw_dot(scene@ g, int layer, int sub_layer, float x, float y, float size=1, uint colour=0xFFFFFFFF, float rotation=0)
{
	g.draw_rectangle_world(layer, sub_layer, x-size, y-size, x+size, y+size, rotation, colour);
}
void draw_dot_hud(scene@ g, int layer, int sub_layer, float x, float y, float size=1, uint colour=0xFFFFFFFF, float rotation=0)
{
	g.draw_rectangle_hud(layer, sub_layer, x-size, y-size, x+size, y+size, rotation, colour);
}

void outline_rect(scene@ g, float x1, float y1, float x2, float y2, uint layer, uint sub_layer, float thickness=2, uint colour=0xFFFFFFFF)
{
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
void outline_rect_hud(scene@ g, float x1, float y1, float x2, float y2, uint layer, uint sub_layer, float thickness=2, uint colour=0xFFFFFFFF)
{
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