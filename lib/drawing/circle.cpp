void draw_circle(scene@ g, float x, float y, float radius, uint segments, uint layer, uint sub_layer, float thickness=2, uint colour=0xFFFFFFFF, bool world=true)
{
	if(segments < 3) segments = 3;
	
	float angle = 0;
	float angle_step = PI * 2 / segments;
	
	for(uint i = 0; i < segments; i++)
	{
		if(world)
		{
			g.draw_line(
				layer, sub_layer, 
				x + cos(angle) * radius, y + sin(angle) * radius,
				x + cos(angle + angle_step) * radius, y + sin(angle + angle_step) * radius,
				thickness, colour);
		}
		else
		{
			g.draw_line_hud(
				layer, sub_layer, 
				x + cos(angle) * radius, y + sin(angle) * radius,
				x + cos(angle + angle_step) * radius, y + sin(angle + angle_step) * radius,
				thickness, colour);
		}
		
		angle += angle_step;
	}
}

void fill_circle(scene@ g, uint layer, uint sub_layer, const float x, const float y, const float radius, int segments, uint colour1, uint colour2)
{
	float prev_x = x + radius;
	float prev_y = y;
	const float step = PI2 / segments;
	float angle = step;
	
	for(int i = 0; i < segments; i += 2)
	{
		const float x2 = prev_x;
		const float y2 = prev_y;
		const float x3 = x + cos(angle) * radius;
		const float y3 = y + sin(angle) * radius;
		angle += step;
		const float x4 = i == segments - 1 ? x3 : x + cos(angle) * radius;
		const float y4 = i == segments - 1 ? y3 : y + sin(angle) * radius;
		angle += step;
		
		g.draw_quad_world(layer, sub_layer, false,
			x, y, x2, y2, x3, y3, x4, y4,
			colour1, colour2, colour2, colour2);
		
		prev_x = x4;
		prev_y = y4;
	}
}

void fill_circle_outside(scene@ g, uint layer, uint sub_layer, float x, float y, float radius, int segments, uint inside_colour, uint outside_colour, float outside_size=7000)
{
	float prev_x = x + radius;
	float prev_y = y;
	const float step = PI2 / segments;
	float angle = step;
	const float rf = 1 / radius * outside_size;
	
	for(int i = 0; i < segments; i++)
	{
		const float cx = x + cos(i == segments - 1 ? 0 : angle) * radius;
		const float cy = y + sin(i == segments - 1 ? 0 : angle) * radius;
		
		g.draw_quad_world(layer, sub_layer, false,
			prev_x, prev_y, cx, cy,
			cx + (cx - x) * rf, cy + (cy - y) * rf,
			prev_x + (prev_x - x) * rf, prev_y + (prev_y - y) * rf,
			inside_colour, inside_colour, outside_colour, outside_colour);
			
		prev_x = cx;
		prev_y = cy;
		angle += step;
	}
}