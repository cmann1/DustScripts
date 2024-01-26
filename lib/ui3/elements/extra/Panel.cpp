#include '../Button.cpp';
#include '../Container.cpp';
#include '../Label.cpp';
#include '../shapes/Arrow.cpp';

namespace Panel { const string TYPE_NAME = 'Panel'; }

/**
 * @class Panel
 * @brief A container that can also display a title and can be collapsed.
 */
class Panel : Container
{
	
	Event collapse;
	
	protected string _title;
	protected bool _collapsible;
	protected bool _show_collapse_arrow;
	protected bool _collapsed;
	protected bool _only_title_border;
	
	protected Button@ _title_button;
	protected Container@ _title_button_content;
	protected Arrow@ _collapse_arrow_icon;
	protected Label@ _title_label;
	
	protected bool has_title_contents;
	
	Panel(UI@ ui)
	{
		super(ui);
	}
	
	string element_type { get const override { return Panel::TYPE_NAME; } }
	
	// ///////////////////////////////////////////////////////////////////
	// Basic properties
	// ///////////////////////////////////////////////////////////////////
	
	/**
	 * @brief If not empty displays a title at the top of this panel.
	 */
	string title
	{
		get const { return _title; }
		set
		{
			if(value == _title)
				return;
			
			_title = value;
			update_title_button();
		}
	}
	
	/**
	 * @brief Access to the title label. Changes made to this may require calling `invalidate_layout()` on this panel.
	 */
	Label@ title_label
	{
		get { return _title_label; }
	}
	
	/**
	 * @brief If true only the top border around the title is drawn.
	 */
	bool only_title_border
	{
		get { return _only_title_border; }
		set
		{
			if(value == _only_title_border)
				return;
			
			_only_title_border = value;
		}
	}
	
	/**
	 * @brief If true the panel can be collapsed by clicking on its title.
	 *        Even if this is false, the panel can till be collapsed
	 */
	bool collapsible
	{
		get { return _collapsible; }
		set
		{
			if(value == _collapsible)
				return;
			
			_collapsible = value;
			update_title_button();
		}
	}
	
	/**
	 * @brief If true an arrow is displayed next to the title.
	 */
	bool show_collapse_arrow
	{
		get { return _show_collapse_arrow; }
		set
		{
			if(value == _show_collapse_arrow)
				return;
			
			_show_collapse_arrow = value;
			update_title_button();
		}
	}
	
	/**
	 * @brief Set to collapse/uncollapse this panel.
	 */
	bool collapsed
	{
		get { return _collapsed; }
		set
		{
			if(value == _collapsed)
				return;
			
			_collapsed = value;
			children_visible = !_collapsed;
			
			if(@_collapse_arrow_icon != null)
			{
				_collapse_arrow_icon.rotation = _collapsed ? 0 : 90;
			}
			
			invalidate_layout();
			ui._dispatch_event(@collapse, _collapsed ? EventType::CLOSE : EventType::OPEN, this);
		}
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Methods
	// ///////////////////////////////////////////////////////////////////
	
	protected void update_title_button()
	{
		has_title_contents = false;
		
		if(@_title_button == null)
		{
			@_title_button_content = Container(ui);
			@_title_button = Button(ui, _title_button_content);
			_title_button.draw_border = DrawOption::Never;
			_title_button.draw_background = DrawOption::Never;
			_title_button.set_padding(0);
			_title_button.mouse_click.on(EventCallback(on_title_click));
		}
		
		_title_button._visible = title != '' || _show_collapse_arrow || _collapsible;
		_title_button.disabled = !_collapsible;
		
		if(_show_collapse_arrow)
		{
			if(@_collapse_arrow_icon == null)
			{
				@_collapse_arrow_icon = Arrow(ui);
				_collapse_arrow_icon.use_highlight_colour = true;
				@_collapse_arrow_icon.hover_delegate = _title_button;
			}
			
			_title_button_content.add_child(_collapse_arrow_icon, 0);
			_collapse_arrow_icon.rotation = _collapsed ? 0 : 90;
			
			has_title_contents = true;
		}
		else if(@_collapse_arrow_icon != null)
		{
			_title_button_content.remove_child(_collapse_arrow_icon);
		}
		
		if(_title != '')
		{
			if(@_title_label == null)
			{
				@_title_label = Label(ui);
			}
			
			_title_label.text = _title;
			_title_label.fit_to_contents();
			_title_button_content.add_child(_title_label);
			
			has_title_contents = true;
		}
		else
		{
			_title_button_content.remove_child(_title_label);
		}
		
		if(@_title_button != null && _title_button._visible)
		{
			_title_button_content.fit_to_contents();
			_title_button.fit_to_contents();
		}
		
		invalidate_layout();
	}
	
	protected float calc_title_height() const
	{
		return has_title_contents ? _title_button._height : 0;
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Element
	// ///////////////////////////////////////////////////////////////////
	
	void _get_subtree_insets(float &out inset_x, float &out inset_y)
	{
		inset_x = 0;
		inset_y = calc_title_height();
	}
	
	void _queue_children_for_layout(ElementStack@ stack) override
	{
		if(!_collapsed)
		{
			stack.push_reversed(@children);
		}
		
		if(has_title_contents)
		{
			stack.push(_title_button);
		}
	}
	
	void _do_layout(LayoutContext@ ctx) override
	{
		if(_defer_layout)
			return;
		
		if(has_title_contents)
		{
			_title_button_content._x = 0;
			_title_button_content._y = 0;
			_title_button_content.fit_to_contents();
			_title_button.fit_to_contents();
			_title_button_content._width = _width;
			_title_button._width = _width;
			_title_button._y = -_title_button._height;
			
			float x = ui.style.spacing * 2.5;
			const float title_height = calc_title_height();
			
			if(_show_collapse_arrow)
			{
				_collapse_arrow_icon._x = x;
				_collapse_arrow_icon._y = ceil((_title_button_content._height - _collapse_arrow_icon._height) * 0.5) + 1;
				x += _collapse_arrow_icon._width + ui.style.spacing;
			}
			
			if(_title != '')
			{
				_title_label._x = x;
				_title_label._y = ceil((_title_button_content._height - _title_label._height) * 0.5);
			}
		}
		
		Container::_do_layout(ctx);
	}
	
	void _draw(Style@ style, DrawingContext@ ctx)
	{
		if(has_title_contents)
		{
			const bool has_border = border_colour != 0 && border_size != 0;
			
			if(background_blur)
			{
				style.draw_glass(x1 + blur_inset, y1 + blur_inset, x2 - blur_inset, y2 - blur_inset, 0);
			}
			
			if(background_colour != 0)
			{
				const float inset = has_border ? max(0.0, border_size) : 0;
				
				style.draw_rectangle(
					x1 + inset, y1 + inset, x2 - inset, y2 - inset,
					0, background_colour);
			}
			
			if(has_border)
			{
				const float ty1 = _title_button.y1 + _title_button._height * 0.5;
				
				if(!_only_title_border && !_collapsed)
				{
					//Left
					style.draw_rectangle(
						x1,
						ty1 + border_size,
						x1 + border_size,
						y2 - border_size, 0, border_colour);
					// Right
					style.draw_rectangle(
						x2 - border_size,
						ty1 + border_size,
						x2,
						y2 - border_size, 0, border_colour);
					// Bottom
					style.draw_rectangle(
						x1,
						y2 - border_size,
						x2,
						y2, 0, border_colour);
				}
				
				const float tox = _title_button.x1;
				const float tx1 = tox + (_show_collapse_arrow
					? _collapse_arrow_icon.x - ui.style.spacing
					: _title != '' ? _title_label.x - ui.style.spacing : 0);
				const float tx2 = tox + (_show_collapse_arrow
					? _title != '' ? _title_label._x + _title_label._width + ui.style.spacing
					: _collapse_arrow_icon._x + _collapse_arrow_icon._width + ui.style.spacing : 0);
				
				// Top Left
				style.draw_rectangle(
					x1,
					ty1,
					tx1,
					ty1 + border_size, 0, border_colour);
				// Top Right
				style.draw_rectangle(
					tx2,
					ty1,
					x2,
					ty1 + border_size, 0, border_colour);
			}
		}
		else
		{
			Container::_draw(style, ctx);
		}
	}
	
	
	protected float layout_padding_top
	{
		get const { return !_collapsed ? Container::get_layout_padding_top() : 0; }
	}
	
	protected float layout_padding_bottom
	{
		get const { return !_collapsed ? Container::get_layout_padding_bottom() : 0; }
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////////////////////////////
	
	private void on_title_click(EventInfo@ event)
	{
		if(!_collapsible)
			return;
		
		collapsed = !_collapsed;
	}
	
}
