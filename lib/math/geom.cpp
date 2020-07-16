void calculate_rotated_rectangle(
	float x, float y,
	float size_x, float size_y,
	float rotation,
	float &out p1_x, float &out p1_y, float &out p2_x, float &out p2_y,
	float &out p3_x, float &out p3_y, float &out p4_x, float &out p4_y)
{
	float angle = rotation * DEG2RAD;
	float c = cos(angle);
	float s = sin(angle);
	float rx1 = -size_x * c + size_y * s;
	float ry1 = -size_x * s - size_y * c;
	float rx2 =  size_x * c + size_y * s;
	float ry2 =  size_x * s - size_y * c;

	p1_x = x + rx1;
	p1_y = y + ry1;
	p2_x = x + rx2;
	p2_y = y + ry2;
	p3_x = x - rx1;
	p3_y = y - ry1;
	p4_x = x - rx2;
	p4_y = y - ry2;
}