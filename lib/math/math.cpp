#include '../std.cpp';

const float PI = 3.1415926535897932384626433832795;
const float PI2 = PI * 2;
const float HALF_PI = PI / 2;
const float DEG2RAD = 1.0 / 180.0 * PI;
const float RAD2DEG = 1.0 / PI * 180.0;
const float EPSILON = 5.3E-5;

float rand_rotation()
{
	return rand_range(0, 360);
}

float clamp01(float value)
{
	return value < 0.0
		? 0.0
		: value > 1.0
			? 1.0
			: value;
}

float clamp(const float value, const float min, const float max)
{
	if(value <= min)
		return min;
	if(value >= max)
		return max;
	
	return value;
}

int clamp(const int value, const int min, const int max)
{
	if(value <= min)
		return min;
	if(value >= max)
		return max;
	
	return value;
}

float dot(float x1, float y1, float x2, float y2)
{
	return x1 * x2 + y1 * y2;
}

float magnitude(float x, float y)
{
	return sqrt(x * x + y * y);
}

float distance(float x1, float y1, float x2, float y2)
{
	const float dx = x2 - x1;
	const float dy = y2 - y1;
	return sqrt(dx * dx + dy * dy);
}

float length_sqr(float x, float y)
{
	return x * x + y * y;
}

float dist_sqr(float x1, float y1, float x2, float y2)
{
	const float dx = x2 - x1;
	const float dy = y2 - y1;
	return dx * dx + dy * dy;
}

float lerp(float a, float b, float x)
{
	return a * (1.0 - x) + b * x;
}

/// Current and target are in radians
float shortest_angle(float current, float target)
{
	const float num = repeat(target - current, PI2);
	return num > PI
		? num - PI2
		: num;
	
//	float da = (a1 - a0) % PI2;
//	return 2 * da % PI2 - da;
}

/// Current and target are in radians
float shortest_angle_degrees(float current, float target)
{
	const float num = repeat(target - current, 360);
	return num > 180
		? num - 360
		: num;
}

float repeat(const float t, const float length)
{
	return clamp(t - floor(t / length) * length, 0.0f, length);
}

float lerp_angle(float a0, float a1, float t)
{
    return a0 + shortest_angle(a0, a1) * t;
}

float lerp_angle_degrees(float a0, float a1, float t)
{
    return a0 + shortest_angle_degrees(a0, a1) * t;
}

void normalize(float x, float y, float &out out_x, float &out out_y)
{
	const float len = sqrt(x * x + y * y);
	out_x = len != 0 ? x / len : 0;
	out_y = len != 0 ? y / len : 0;
}

float normalize_angle(const float theta)
{
	return theta - PI2 * floor((theta + PI) / PI2);
}

float normalize_degress(const float theta)
{
	return theta - 360 * floor((theta + 180) / 360);
}

void project(
	float ax, float ay, float bx, float by,
	float &out out_x, float &out out_y)
{
	const float dp = dot(ax, ay, bx, by);
	out_x = ( dp / (bx * bx + by * by) ) * bx;
	out_y = ( dp / (bx * bx + by * by) ) * by;
}

void reflect(
	float x, float y, float normal_x, float normal_y,
	float &out out_x, float &out out_y)
{
	const float d = dot(x, y, normal_x, normal_y);
	out_x = x - 2 * normal_x * d;
	out_y = y - 2 * normal_y * d;
}

void rotate(float x, float y, float angle, float &out out_x, float &out out_y)
{
	out_x = cos(angle) * x - sin(angle) * y;
	out_y = sin(angle) * x + cos(angle) * y;
}

void rotate(
	float x, float y, float centre_x, float centre_y, float angle,
	float &out out_x, float &out out_y)
{
	x -= centre_x;
	y -= centre_y;
	out_x = centre_x + cos(angle) * x - sin(angle) * y;
	out_y = centre_y + sin(angle) * x + cos(angle) * y;
}

float sign(float x)
{
	return x < -1e-9 ? -1 : (x > 1e-9 ? 1 : 0);
}

int sign(int x)
{
	return x < 0 ? -1 : (x > 0 ? 1 : 0);
}

int sign_to(float from, float to)
{
	return from <= to ? 1 : -1;
}

int mod(const int x, const int m)
{
	return (x % m + m) % m;
}

float mod(const float x, const float m)
{
	return (x % m + m) % m;
}

bool approximately(const float a, const float b)
{
	return abs(b - a) < EPSILON;
}

void vec2_limit(float x, float y, float limit, float &out out_x, float &out out_y)
{
	float length = x * x + y * y;

	if(length > limit * limit && length > 0)
	{
		length = sqrt(length);
		out_x = x / length * limit;
		out_y = y / length * limit;
	}
	else
	{
		out_x = x;
		out_y = y;
	}
}

float map(float value, float from_min, float from_max, float to_min, float to_max)
{
	value = (value - from_min) / (from_max - from_min);
	return to_min + value * (to_max - to_min);
}

float map_clamped(float value, float from_min, float from_max, float to_min, float to_max)
{
	value = clamp01((value - from_min) / (from_max - from_min));
	return to_min + value * (to_max - to_min);
}

/// Returns the z-component of the cross product of a and b
float cross_product_z(float a_x, float a_y, float b_x, float b_y)
{
	return a_x * b_y - a_y * b_x;
}

/// Orientation is positive if abc is counterclockwise, negative if clockwise, or 0 if colinear.
/// Note: Dustforce has positive y going down the screen, so clockwise becomes
/// counterclockwise and vice versa.
/// https://www.geeksforgeeks.org/orientation-3-ordered-points/
/// http://e-maxx.ru/algo/oriented_area
float orientation(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3)
{
	// Same thing but fewer steps
	return (y3 - y2) * (x2 - x1) - (y2 - y1) * (x3 - x2);
	// return
	// 	cross_product_z(a_x, a_y, b_x, b_y) +
	// 	cross_product_z(b_x, b_y, c_x, c_y) +
	// 	cross_product_z(c_x, c_y, a_x, a_y);
}

/// Does line ab intersect line cd?
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

/// A faster? line rectangle intersection that also returns both min and max intersection points
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

/// -1 = Left side
///  0 = The point is on the line
///  1 = Right side
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
