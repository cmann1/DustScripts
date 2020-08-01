#include '../UI.cpp';
#include '../../math/math.cpp';
#include '../utils/ElementStack.cpp';
#include 'Element.cpp';
#include 'LockedContainer.cpp';

class SingleContainer : LockedContainer
{
	
	/**
	 * @class LockedContainer
	 * @brief A Container that can only hold a single child
	 */
	
	protected Element@ _content;
	
	SingleContainer(UI@ ui, Element@ content)
	{
		super(ui);
		
		@this.content = content;
	}
	
	string element_type { get const override { return 'SingleContainer'; } }
	
	Element@ content
	{
		get { return @_content; }
		set
		{
			if(@_content == @value)
				return;
			
			if(@_content != null)
			{
				Container::remove_child(_content);
			}
			
			@_content = @value;
			
			if(@_content != null)
			{
				Container::add_child(_content);
			}
			
			_validate_layout = true;
		}
	}
	
	void fit_to_contents(const float padding=NAN)
	{
		fit_to_contents(true, padding, padding, padding, padding);
	}
	
	void fit_to_contents(const float padding_x, const float padding_y)
	{
		fit_to_contents(true, padding_x, padding_x, padding_y, padding_y);
	}
	
	void fit_to_contents(const bool include_border, const float padding)
	{
		fit_to_contents(include_border, padding, padding, padding, padding);
	}
	
	void fit_to_contents(const bool include_border, const float padding_x, const float padding_y)
	{
		fit_to_contents(include_border, padding_x, padding_x, padding_y, padding_y);
	}
	
	void fit_to_contents(bool include_border, float padding_left=NAN, float padding_right=NAN, float padding_top=NAN, float padding_bottom=NAN)
	{
		if(is_nan(padding_left))
			padding_left = default_left_padding;
		if(is_nan(padding_right))
			padding_right = default_right_padding;
		if(is_nan(padding_top))
			padding_top = default_top_padding;
		if(is_nan(padding_bottom))
			padding_bottom = default_bottom_padding;
		
		if(@_content == null)
		{
			width  = padding_left + padding_right;
			height = padding_top + padding_bottom;
			return;
		}
		
		float width  = _content._width  + padding_left + padding_right;
		float height = _content._height + padding_top + padding_bottom;
		
		if(include_border)
		{
			width  += border_size * 2;
			height += border_size * 2;
		}
		
		this.width  = width;
		this.height = height;
	}
	
	void _queue_children_for_layout(ElementStack@ stack) override
	{
		if(@_content != null)
		{
			stack.push(_content);
		}
	}
	
	bool add_child(Element@ child, int index=-1) override
	{
		return false;
	}
	
	bool remove_child(Element@ child) override
	{
		if(Container::remove_child(child))
		{
			@_content = null;
			return true;
		}
		
		return false;
	}
	
	protected float border_size { get const { return 0; } }
	
	protected float default_left_padding { get const { return ui.style.spacing; } }
	protected float default_right_padding { get const { return ui.style.spacing; } }
	protected float default_top_padding { get const { return ui.style.spacing; } }
	protected float default_bottom_padding { get const { return ui.style.spacing; } }
	
}