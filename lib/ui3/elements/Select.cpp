#include '../popups/PopupOptions.cpp';
#include 'SelectBase.cpp';
#include 'ListView.cpp';
#include 'shapes/Arrow.cpp';

namespace Select { const string TYPE_NAME = 'Select'; }

class Select : SelectBase, IStepHandler
{
	
	float button_size = 20;
	
	protected Arrow@ _arrow;
	protected ListView@ list_view;
	protected PopupOptions@ popup;
	
	protected bool open;
	
	Select(UI@ ui, const string placeholder_text='',
		const string placeholder_icon_set='', const string placeholder_icon_name='',
		const float placeholder_icon_width=-1, const float placeholder_icon_height=-1,
		const float placeholder_icon_offset_x=0, const float placeholder_icon_offset_y=0)
	{
		super(ui, placeholder_text,
			placeholder_icon_set, placeholder_icon_name,
			placeholder_icon_width, placeholder_icon_height,
			placeholder_icon_offset_x, placeholder_icon_offset_y);
		
		@_arrow = Arrow(ui);
		_arrow.padding = 2;
		_arrow.rotation = 90;
		Container::add_child(_arrow);
		
		@list_view = ListView(ui);
		list_view.border_size = 0;
		list_view.border_colour = 0;
		list_view.background_colour = 0;
		list_view.select.on(EventCallback(on_list_view_select));
		
		@popup = PopupOptions(ui, list_view, true, PopupPosition::Below, PopupTriggerType::Manual, PopupHideType::MouseDownOutside, false);
		popup.wait_for_mouse = true;
		popup.allow_target_overlap = false;
		popup.stretch = true;
		popup.spacing = 2;
		popup.padding = 0;
		popup.hide.on(EventCallback(on_popup_hide));
	}
	
	string element_type { get const override { return Select::TYPE_NAME; } }
	
	bool set_icon(int index, const string set, const string name, const float width, const float height, const float offset_x=0, const float offset_y=0) override
	{
		if(!SelectBase::set_icon(index, set, name, width, height, offset_x, offset_y))
			return false;
		
		ListViewItem@ item = list_view.get_item(index < 0 || index >= _num_values ? _num_values - 1 : index);
		
		if(@item != null)
		{
			item.set(set, name, width, width, offset_x, offset_y);
		}
		
		return true;
	}
	
	void add_value(const string value, const string text, int index=-1) override
	{
		SelectBase::add_value(value, text, index);
		list_view.add_item(value, text, TextAlign::Left, index);
	}
	
	void add_value(const string value, const string text,
		const string icon_set, const string icon_name,
		const float icon_width=-1, const float icon_height=-1,
		const float icon_offset_x=0, const float icon_offset_y=0,
		int index=-1) override
	{
		SelectBase::add_value(value, text,
			icon_set, icon_name,
			icon_width, icon_height,
			icon_offset_x, icon_offset_y,
			index);
		
		ListViewItem@ item = list_view.get_item(index < 0 || index >= _num_values ? _num_values - 1 : index);
		item.set(icon_set, icon_name, text, icon_width, icon_width, icon_offset_x, icon_offset_y);
	}
	
	bool remove_value(int index) override
	{
		if(SelectBase::remove_value(index))
		{
			list_view.remove_item(index);
			return true;
		}
		
		return false;
	}
	
	bool set_value_index(int index, int new_index) override
	{
		if(SelectBase::set_value_index(index, new_index))
		{
			if(new_index < 0)
				new_index = 0;
			else if(new_index > _num_values)
				new_index = _num_values;
			
			list_view.set_item_index(index, new_index);
			
			return true;
		}
		
		return false;
	}
	
	void clear() override
	{
		SelectBase::clear();
		
		list_view.clear();
	}
	
	void close()
	{
		if(@tooltip != null)
		{
			tooltip.enabled = true;
		}
		
		ui.hide_tooltip(popup);
		open = false;
	}
	
	//
	
	bool ui_step()
	{
		if(ui.has_input && input_api::consume_gvb_press(ui.input, GVB::Escape))
		{
			ui.hide_tooltip(popup);
			open = false;
			return false;
		}
		
		return true;
	}
	
	void _do_layout(LayoutContext@ ctx) override
	{
		const float border_size = max(0.0, ui.style.border_size);
		
		_label._x = border_size;
		_label._y = border_size;
		_label._width = _width - button_size - border_size * 2;
		_label._height = _height - border_size * 2;
		_label.validate_layout = true;
		
		if(_show_icons && _icon.visible)
		{
			_icon._x = border_size;
			_icon._y = border_size;
			_icon._width = min(_height, _width) - border_size * 2;
			_icon._height = _icon._width;
			
			_label._x += _icon._width;
			_label._width -= _icon._width;
			_icon.validate_layout = true;
		}
		
		_arrow._x = _width - border_size - button_size;
		_arrow._y = border_size;
		_arrow._width  = button_size;
		_arrow._height = _label._height;
		_arrow.validate_layout = true;
	}
	
	void _draw(Style@ style, DrawingContext@ ctx)
	{
		style.draw_interactive_element(
			x1, y1, x2, y2,
			hovered || pressed,
			open,
			pressed, disabled);
	}
	
	protected void update_custom_value(const string &in value, const bool has_value) override
	{
		SelectBase::update_custom_value(value, has_value);
		
		if(has_value)
		{
			list_view.select_none();
		}
	}
	
	// Events
	
	void _mouse_press(EventInfo@ event)
	{
		if(event.button != ui.primary_button)
			return;
		
		if(open)
		{
			close();
			return;
		}
		
		list_view.fit_to_contents(true);
		
		if(selected_index == -1)
		{
			list_view.select_none();
			list_view.scroll_y = 0;
		}
		else
		{
			ListViewItem@ selected_item = list_view.get_item(selected_index);
			
			if(@selected_item != null)
			{
				selected_item.selected = true;
				@list_view.content.scroll_into_view = selected_item;
			}
		}
		
		if(@tooltip != null)
		{
			tooltip.enabled = false;
		}
		
		ui._step_subscribe(this);
		open = true;
		ui.show_tooltip(popup, this);
	}
	
	protected void on_list_view_select(EventInfo@ event)
	{
		if(!open)
			return;
		
		if(list_view.num_selected_items > 0)
		{
			selected_index = list_view.selected_index;
		}
		
		close();
	}
	
	protected void on_popup_hide(EventInfo@ event)
	{
		on_list_view_select(event);
	}
	
}
