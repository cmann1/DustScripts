#include '../elements/Element.cpp';

abstract class Layout
{
	
	/// The amount of space around the outside. Leave as NAN to use the default style spacing.
	float padding_left = NAN;
	/// Same as padding_left.
	float padding_right = NAN;
	/// Same as padding_left.
	float padding_top = NAN;
	/// Same as padding_left.
	float padding_bottom = NAN;
	
	void do_layout(const array<Element@>@ elements,
		const float x1, const float y1, const float x2, const float y2,
		float &out out_x1, float &out out_y1, float &out out_x2, float &out out_y2)
	{
		
	}
	
	void set_padding(const float padding)
	{
		padding_left = padding_right = padding_top = padding_bottom = padding;
	}
	
	void set_padding(const float padding_left_right, const float padding_top_bottom)
	{
		padding_left	= padding_left_right;
		padding_right	= padding_left_right;
		padding_top		= padding_top_bottom;
		padding_bottom	= padding_top_bottom;
	}
	
	void set_padding(const float padding_left, const float padding_right, const float padding_top, const float padding_bottom)
	{
		this.padding_left	= padding_left;
		this.padding_right	= padding_right;
		this.padding_top	= padding_top;
		this.padding_bottom	= padding_bottom;
	}
	
}