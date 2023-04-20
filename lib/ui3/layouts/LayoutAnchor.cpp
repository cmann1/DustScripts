#include '../utils/Position.cpp';
#include 'LayoutAnchorType.cpp';

class LayoutAnchor
{
	
	// TODO: Add object/sibling anchors.
	// Would complicate AnchorLayout since we'd want to layout the siblings first
	
	private Position side;
	
	LayoutAnchorType type = None;
	float position;
	float _padding;
	Element@ element;
	
	LayoutAnchor(const Position side)
	{
		this.side = side;
	}
	
	LayoutAnchor@ clear()
	{
		type = None;
		return this;
	}
	
	LayoutAnchor@ pixel(const float position, const float padding)
	{
		type = Pixel;
		this.position = position;
		this._padding = padding;
		return this;
	}
	
	LayoutAnchor@ pixel(const float position)
	{
		return pixel(position, _padding);
	}
	
	LayoutAnchor@ percent(const float position, const float padding)
	{
		type = Percent;
		this.position = position;
		this._padding = padding;
		return this;
	}
	
	LayoutAnchor@ percent(const float position)
	{
		return percent(position, _padding);
	}
	
	LayoutAnchor@ sibling(Element@ element, const float position=0)
	{
		@this.element = element;
		this.position = position;
		
		if(type == None && @element != null)
		{
			type = Percent;
		}
		
		return this;
	}
	
	/**
	 * @brief Anchors after the givven sibling with the given padding.
	 * @param sibling The sibling to anchor to.
	 * @param padding The amount of padding. Leave out to set to the default style spacing.
	 * @return This anchor.
	 */
	LayoutAnchor@ after(Element@ sibling, const float padding=NAN)
	{
		return this.sibling(sibling, 0.0).padding(padding);
	}
	
	LayoutAnchor@ padding(const float padding=0)
	{
		this._padding = padding;
		return this;
	}
	
	bool calculate(Element@ element, float anchor_p, float anchor_size, const bool pass1, float &out x)
	{
		Element@ sibling =
			@this.element != null && @this.element.parent == @element.parent &&
			this.element._visible
				? this.element : null;
			
		if(@sibling != null && pass1)
			return false;
		
		if(@sibling != null)
		{
			switch(side)
			{
				case Position::Left:
					anchor_p = sibling._x + sibling._width;
					anchor_size = -sibling._width;
					break;
				case Position::Right:
					anchor_p = sibling._x;
					anchor_size = -sibling._width;
					break;
				case Position::Top:
					anchor_p = sibling._y + sibling._height;
					anchor_size = -sibling._height;
					break;
				case Position::Bottom:
					anchor_p = sibling._y;
					anchor_size = -sibling._height;
					break;
			}
		}
		
		// Ignore the padding if anchored to a sibling that isn't visible
		const float padding = @this.element != null && @sibling == null
			? 0 : is_nan(_padding) ? element.ui.style.spacing : _padding;
		x = anchor_p + (type == Percent
			? anchor_size * position + padding
			: position + padding) * (side == Position::Left || side == Position::Top ? 1 : -1);
		return true;
	}
	
}
