#include '../UI.cpp';
#include '../Style.cpp';
#include '../utils/Orientation.cpp';
#include '../utils/IOrientationParent.cpp';
#include 'Element.cpp';

class Divider : Element
{
	
	Orientation orientation = Orientation::Horizontal;
	IOrientationParent@ orientation_parent;
	
	private float size;
	
	Divider(UI@ ui, Orientation orientation)
	{
		super(ui, 'div');
		
		this.orientation = orientation;
		fit();
	}
	
	Divider(UI@ ui, IOrientationParent@ orientation_parent=null)
	{
		super(ui, 'div');
		
		@this.orientation_parent = orientation_parent;
		fit();
	}
	
	void fit()
	{
		const Orientation orientation = @orientation_parent != null ? orientation_parent.orientation : this.orientation;
		const float size = ui.style.divider_thickness + ui.style.divider_margin * 2;
		
		if(orientation == Orientation::Horizontal)
		{
			width = size;
			height = 20;
		}
		else
		{
			width = 20;
			height = size;
		}
		
		this.size = size;
		this.orientation = orientation;
	}
	
	void _do_layout() override
	{
		if(@orientation_parent != null && orientation != orientation_parent.orientation)
		{
			if(orientation_parent.orientation == Orientation::Horizontal)
			{
				height = width;
				width = size;
			}
			else
			{
				width = height;
				height = size;
			}
			
			orientation = orientation_parent.orientation;
		}
		
		if(orientation == Orientation::Horizontal)
		{
			size = _width;
		}
		else
		{
			size = _height;
		}
	}
	
	void _draw(Style@ style) override
	{
		style.draw_rectangle(
			x1 + ui.style.divider_margin, y1 + ui.style.divider_margin,
			x2 - ui.style.divider_margin, y2 - ui.style.divider_margin,
			0, ui.style.normal_border_clr);
	}
	
}