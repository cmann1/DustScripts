#include 'PopupButton.cpp';

namespace SelectButton { const string TYPE_NAME = 'SelectButton'; }

/// PopupButton that provides a ListView popup
class SelectButton : PopupButton
{
	
	protected ListView@ _list_view;
	
	SelectButton(UI@ ui, Element@ content)
	{
		super(ui, content);
	}
	
	SelectButton(UI@ ui, const string text, const TextAlign text_align_h=TextAlign::Left)
	{
		super(ui, text, text_align_h);
	}
	
	SelectButton(UI@ ui, const string sprite_set, const string sprite_name, const float width=-1, const float height=-1, const float offset_x=0, const float offset_y=0)
	{
		super(ui, sprite_set, sprite_name, width, height, offset_x, offset_y);
	}
	
	protected void init() override
	{
		PopupButton::init();
		
		@_list_view = ListView(ui);
		_list_view.allow_deselect = false;
		_list_view.border_size = 0;
		_list_view.border_colour = 0;
		_list_view.background_colour = 0;
		_list_view.select.on(EventCallback(on_select));
		
		@_popup.content_element = _list_view;
	}
	
	string element_type { get const override { return SelectButton::TYPE_NAME; } }
	
	// ///////////////////////////////////////////////////////////////////
	// Basic Properties
	// ///////////////////////////////////////////////////////////////////
	
	ListView@ list_view
	{
		get { return _list_view; }
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Internal
	// ///////////////////////////////////////////////////////////////////
	
	protected void before_popup_show() override
	{
		PopupButton::before_popup_show();
		
		_list_view.fit_to_contents(true);
		
		ListViewItem@ item = _list_view.get_selected_item();
		
		if(@item != null)
		{
			@_list_view.content.scroll_into_view = item;
		}
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////////////////////////////
	
	protected void on_select(EventInfo@ event)
	{
		if(!_list_view.allow_multiple_selection)
		{
			close();
		}
	}
	
}