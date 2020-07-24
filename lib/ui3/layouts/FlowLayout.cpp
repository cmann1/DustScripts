#include '../utils/Orientation.cpp';
#include 'Layout.cpp';

class FlowLayout : Layout
{
	
	Orientation orientation;
	
	FlowLayout(Orientation orientation)
	{
		this.orientation = orientation;
	}
	
	void do_layout(const array<Element@>@ elements, const float x1, const float y1, const float x2, const float y2)
	{
		const int num_children = elements.size();
	}
	
}