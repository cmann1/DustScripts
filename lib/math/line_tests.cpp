#include 'math.cpp';

/** Returns true if line ab intersect line cd. */
bool lines_intersect(
	float a_x, float a_y, float b_x, float b_y,
	float c_x, float c_y, float d_x, float d_y)
{
	// Before expanding cross products:
	// return (
	//     cross_product_z(a_x - c_x, a_y - c_y, d_x - c_x, d_y - c_y) < 0 and
	//     cross_product_z(b_x - c_x, b_y - c_y, d_x - c_x, d_y - c_y) > 0 and
	//     cross_product_z(d_x - a_x, d_y - a_y, b_x - a_x, b_y - a_y) < 0 and
	//     cross_product_z(c_x - a_x, c_y - a_y, b_x - a_x, b_y - a_y) > 0
	// );

	return (
		(a_x - c_x) * (d_y - c_y) - (a_y - c_y) * (d_x - c_x) < 0 and
		(b_x - c_x) * (d_y - c_y) - (b_y - c_y) * (d_x - c_x) > 0 and
		(d_x - a_x) * (b_y - a_y) - (d_y - a_y) * (b_x - a_x) < 0 and
		(c_x - a_x) * (b_y - a_y) - (c_y - a_y) * (b_x - a_x) > 0
	);
}

bool line_line_intersection(
	float ax, float ay, float bx, float by,
	float cx, float cy, float dx, float dy,
	float &out x, float &out y, float & out t)
{
	const float s1x = bx - ax;
	const float s1y = by - ay;
	const float s2x = dx - cx;
	const float s2y = dy - cy;
	const float det = (-s2x * s1y + s1x * s2y);
	
	// Not sure why but could get a divide by zero error her without the equality check
	if(det < EPSILON && det > -EPSILON || det == 0)
	{
		x = 0;
		y = 0;
		t = 0;
		return false;
	}
	
	t = (s2x * (ay - cy) - s2y * (ax - cx)) / det;

	if(t < 0 || t > 1)
	{
		x = 0;
		y = 0;
		return false;
	}
	
	const float s = (-s1y * (ax - cx) + s1x * (ay - cy)) / det;
	
	if(s < 0 || s > 1)
	{
		x = 0;
		y = 0;
		return false;
	}
	
	x = ax + s1x * t;
	y = ay + s1y * t;
	return true;
}

bool ray_ray_intersection(
	const float ax1, const float ay1, const float ax2, const float ay2,
	const float bx1, const float by1, const float bx2, const float by2,
	float &out x, float &out y, float &out t)
{
	const float dx = ax2 - ax1;
	const float dy = ay2 - ay1;
	const float bdx = bx2 - bx1;
	const float bdy = by2 - by1;
	
	float det = (-bdx * dy + dx * bdy);
	
	if(det < EPSILON && det > -EPSILON)
	{
		x = 0;
		y = 0;
		t = 0;
		return false;
	}
	
	t = (bdx * (ay1 - by1) - bdy * (ax1 - bx1)) / det;

	x = ax1 + dx * t;
	y = ay1 + dy * t;
	return true;
}

bool line_rectangle_intersection(
	float ax, float ay, float bx, float by,
	float r1x, float r1y, float r2x, float r2y,
	float &out x, float &out y, float &out t)
{
	// Top
	if(line_line_intersection(ax, ay, bx, by, r1x, r1y, r2x, r1y, x, y, t))
		return true;
	// Bottom
	if(line_line_intersection(ax, ay, bx, by, r1x, r2y, r2x, r2y, x, y, t))
		return true;
	// Left
	if(line_line_intersection(ax, ay, bx, by, r1x, r1y, r1x, r2y, x, y, t))
		return true;
	// Right
	if(line_line_intersection(ax, ay, bx, by, r2x, r1y, r2x, r2y, x, y, t))
		return true;
	
	return false;
}

/** A faster? line rectangle intersection that also returns both min and max intersection points. */
bool line_aabb_intersection(
	const float lx1, const float ly1, const float lx2, const float ly2,
	const float ax1, const float ay1, const float ax2, const float ay2,
	float &out t_min, float &out t_max)
{
	const float dx = lx2 - lx1;
	const float dy = ly2 - ly1;
	const float inv_delta_x = dx != 0 ? 1 / dx : 1;
	const float inv_delta_y = dy != 0 ? 1 / dy : 1;
	float t1 = (ax1 - lx1) * inv_delta_x;
	float t2 = (ax2 - lx1) * inv_delta_x;

	t_min = min(t1, t2);
	t_max = max(t1, t2);

	t1 = (ay1 - ly1) * inv_delta_y;
	t2 = (ay2 - ly1) * inv_delta_y;

	t_min = max(t_min, min(t1, t2));
	t_max = min(t_max, max(t1, t2));
	
	return t_min <= 1 && t_max >= 0 && t_max >= t_min;
}

/**
 * @param p1x
 * @param p1y
 * @param p2x
 * @param p2y
 * @param px
 * @param py
 * @return
 *   -1 = Left side.
 *    0 = The point is on the line.
 *    1 = Right side.
 */
int line_side(
	const float p1x, const float p1y, const float p2x, const float p2y,
	const float px, const float py)
{
	const float d = (px - p1x) * (p2y - p1y) - (py - p1y) * (p2x - p1x);
	
	return d > EPSILON
		? -1
		: (d < -EPSILON
			? 1 : 0);
}
