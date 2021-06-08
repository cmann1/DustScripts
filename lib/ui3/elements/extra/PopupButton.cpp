#include '../Button.cpp';

namespace PopupButton { const string TYPE_NAME = 'PopupButton'; }

/// A button with a popup attached that can be opened and closed by clicking the button.
class PopupButton : Button, IStepHandler
{
	
	/// Dispatched jsut before the popup opens, once it has opened, and when it closes
	Event open;
	
	protected PopupOptions@ _popup;
	
	PopupButton(UI@ ui, Element@ content)
	{
		super(ui, content);
	}
	
	PopupButton(UI@ ui, const string text, const TextAlign text_align_h=TextAlign::Left)
	{
		super(ui, text, text_align_h);
	}
	
	PopupButton(UI@ ui, const string sprite_set, const string sprite_name, const float width=-1, const float height=-1, const float offset_x=0, const float offset_y=0)
	{
		super(ui, sprite_set, sprite_name, width, height, offset_x, offset_y);
	}
	
	protected void init() override
	{
		Button::init();
		
		Button::set_selectable(true);
		Button::set_user_selectable(false);
		
		@_popup = PopupOptions(ui, null, true, PopupPosition::Below, PopupTriggerType::Manual, PopupHideType::MouseDownOutside);
		_popup.wait_for_mouse = true;
		_popup.allow_target_overlap = false;
		_popup.spacing = ui.style.spacing;
		_popup.padding = 0;
		_popup.show.on(EventCallback(on_popup_show));
		_popup.hide.on(EventCallback(on_popup_hide));
	}
	
	string element_type { get const override { return PopupButton::TYPE_NAME; } }
	
	// ///////////////////////////////////////////////////////////////////
	// Basic properties
	// ///////////////////////////////////////////////////////////////////
	
	bool selectable
	{
		get const override { return true; }
		set override { puts('PopupButton.selectable cannot be changed'); }
	}
	
	bool user_selectable
	{
		get const override { return false; }
		set override { puts('PopupButton.user_selectable cannot be changed'); }
	}
	
	bool selected
	{
		set override
		{
			puts('PopupButton.selected cannot be set');
		}
	}
	
	PopupOptions@ popup
	{
		get { return _popup; }
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Methods
	// ///////////////////////////////////////////////////////////////////
	
	void close()
	{
		if(!_selected)
			return;
		
		ui.hide_tooltip(_popup);
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Internal
	// ///////////////////////////////////////////////////////////////////
	
	bool ui_step()
	{
		if(ui._has_editor && (@popup == null || !popup.locked) && editor_api::consume_gvb_press(ui._editor, GVB::Escape))
		{
			close();
			return false;
		}
		
		return true;
	}
	
	protected void before_popup_show()
	{
		
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////////////////////////////
	
	void _mouse_click(EventInfo@ event) override
	{
		if(_selected)
		{
			close();
			return;
		}
		
		ui._dispatch_event(@open, EventType::BEFORE_OPEN, this);
		before_popup_show();
		
		ui._step_subscribe(this);
		Button::set_selected(true);
		ui.show_tooltip(_popup, this);
	}
	
	protected void on_popup_show(EventInfo@ event)
	{
		if(@tooltip != null)
		{
			tooltip.enabled = false;
		}
		
		ui._dispatch_event(@open, EventType::OPEN, this);
	}
	
	protected void on_popup_hide(EventInfo@ event)
	{
		Button::set_selected(false);
		
		if(@tooltip != null)
		{
			tooltip.enabled = true;
		}
		
		ui._dispatch_event(@open, EventType::CLOSE, this);
	}
	
}
