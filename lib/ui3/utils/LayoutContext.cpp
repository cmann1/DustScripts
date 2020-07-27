class LayoutContext
{
	
	LayoutContext@ parent;
	Element@ root;
	int num_children;
	bool mouse_active = true;
	
	int clipping_mode  = ClippingMode::None;
	float x1;
	float y1;
	float x2;
	float y2;
	
	float subtree_x1;
	float subtree_y1;
	float subtree_x2;
	float subtree_y2;
	
}