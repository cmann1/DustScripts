#include 'ClippingMode.cpp';

class DrawingContext
{
	
	DrawingContext@ parent = null;
	Element@ root = null;
	int num_children = 0;
	float alpha = 1;
	
	int clipping_mode  = ClippingMode::None;
	float x1;
	float y1;
	float x2;
	float y2;
	
}