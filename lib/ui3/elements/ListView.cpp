#include '../MouseButton.cpp';
#include '../events/Event.cpp';
#include '../layouts/GridLayout.cpp';
#include 'ScrollView.cpp';
#include 'ListViewItem.cpp';

namespace ListView { const string TYPE_NAME = 'ListView'; }

class ListView : ScrollView
{
	
	bool drag_select = false;
	
	Event select;
	
	protected GridLayout@ grid_layout;
	protected bool _allow_multiple_selection;
	protected array<ListViewItem@> _items;
	protected array<ListViewItem@> _selected_items;
	protected int _num_items;
	protected int _num_selected_items;
	protected bool busy_updating_selection;
	
	protected bool busy_drag_select;
	protected bool drag_select_select;
	
	ListView(UI@ ui)
	{
		super(@ui);
		
		border_size = ui.style.border_size;
		border_colour = ui.style.normal_border_clr;
		background_colour = ui.style.normal_bg_clr;
		
		_width  = _set_width  = 200;
		_height = _set_height = 200;
		
		@grid_layout = GridLayout(ui, 1, 0, 0, FlowDirection::Row, FlowAlign::Stretch, FlowAlign::Stretch, FlowFit::MainAxis);
		grid_layout.row_spacing = 0;
		@_content.layout = @grid_layout;
	}
	
	string element_type { get const override { return ListView::TYPE_NAME; } }
	
	int num_items { get const { return _num_items; } }
	
	int num_selected_items { get const { return _num_selected_items; } }
	
	bool allow_multiple_selection
	{
		get const { return _allow_multiple_selection; }
		set
		{
			if(_allow_multiple_selection == value)
				return;
			
			_allow_multiple_selection = value;
			
			if(!_allow_multiple_selection && _num_selected_items > 1)
			{
				ListViewItem@ item = @_selected_items[_num_selected_items - 1];
				
				busy_updating_selection = true;
				
				for(int i = _num_selected_items - 2; i >= 0; i++)
				{
					_selected_items[i].selected = false;
				}
				
				@_selected_items[0] = @item;
				_selected_items.resize(1);
				_num_selected_items = 1;
				
				busy_updating_selection = false;
				
				dispatch_select_event(item);
			}
		}
	}
	
	int get_items(array<ListViewItem@>@ list)
	{
		if(int(list.length()) < _num_items)
			list.resize(_num_items);
		
		for(int i = 0; i < _num_items; i++)
		{
			@list[i] = @_items[i];
		}
		
		return _num_items;
	}
	
	// Add
	
	ListViewItem@ add_item(ListViewItem@ item, const int index=-1)
	{
		if(@item == null)
			return null;
		
		if(@item._list_view == @this)
		{
			return set_item_index(@item, _num_items);
		}
		
		if(@item._list_view != null)
		{
			@item._list_view.remove_item(item);
		}
			
		if(index < 0 || index >= _num_items)
		{
			_items.insertLast(@item);
		}
		else
		{
			_items.insertAt(index, item);
		}
		
		if(item.selected)
		{
			item.selected = false;
			select_item(item);
		}
		
		@item._list_view = @this;
		_content.add_child(item, index);
		_num_items++;
		return @item;
	}
	
	ListViewItem@ add_item(const string value, const int index=-1)
	{
		return add_item(ListViewItem(ui, value), index);
	}
	
	ListViewItem@ add_item(const string value, Element@ content, const int index=-1)
	{
		return add_item(ListViewItem(ui, value, content), index);
	}
	
	ListViewItem@ add_item(const string value, const string text, const TextAlign text_align_h=TextAlign::Left, const int index=-1)
	{
		return add_item(ListViewItem(ui, value, text, text_align_h), index);
	}
	
	ListViewItem@ add_item(const string value,
		const string sprite_set, const string sprite_name, const float width=-1, const float height=-1, const float offset_x=0, const float offset_y=0,
		const int index=-1)
	{
		return add_item(ListViewItem(ui, value, sprite_set, sprite_name, width, height, offset_x, offset_y), index);
	}
	
	ListViewItem@ add_item(const string value,
		const string sprite_set, const string sprite_name,
		const string text,
		const float width=-1, const float height=-1, const float offset_x=0, const float offset_y=0,
		const TextAlign text_align_h=TextAlign::Left, const int index=-1)
	{
		return add_item(ListViewItem(ui, value, sprite_set, sprite_name, text, width, height, offset_x, offset_y, text_align_h), index);
	}
	
	
	// Remove
	
	ListViewItem@ remove_item(ListViewItem@ item)
	{
		if(@item == null || @item._list_view != @this)
			return @item;
		
		int index = _items.findByRef(@item);
		
		if(index == -1)
			return @item;
		
		_items.removeAt(index);
		@item._list_view = null;
		
		if(item.selected)
		{
			deselect_item(item);
		}
		
		_num_items--;
		_content.remove_child(item);
		return @item;
	}
	
	ListViewItem@ remove_item(const string value, const bool first=false)
	{
		int index;
		return remove_item(get_item(value, index, first));
	}
	
	ListViewItem@ remove_item(const int index)
	{
		if(index < 0 || index >= _num_items)
			return null;
		
		return remove_item(@_items[index]);
	}
	
	// Order
	
	ListViewItem@ set_item_index(int old_index, int index)
	{
		if(old_index < 0 || old_index >= _num_items)
			return null;
		
		if(index < 0)
			index = 0;
		else if(index >= _num_items)
			index = _num_items - 1;
		
		if(old_index == index)
			return _items[old_index];
		
		ListViewItem@ item = _items[old_index];
		_items.removeAt(old_index);
		_items.insertAt(index > old_index ? index - 1 : index, item);
		
		_content.set_child_index(item, index);
		return @item;
	}
	
	ListViewItem@ set_item_index(ListViewItem@ item, int index)
	{
		if(@item == null || @item._list_view != @this)
			return @item;
		
		int old_index = _items.findByRef(@item);
		
		if(old_index == -1)
			return @item;
		
		return set_item_index(old_index, index);
	}
	
	ListViewItem@ move_item_to_front(ListViewItem@ item)
	{
		if(_num_items == 0 || @_items[_num_items - 1] == @item)
			return @item;
		
		return set_item_index(item, _num_items + 1);
	}
	
	ListViewItem@ move_item_to_back(ListViewItem@ item)
	{
		if(_num_items == 0 || @_items[0] == @item)
			return @item;
		
		return set_item_index(item, 0);
	}
	
	ListViewItem@ move_item_up(ListViewItem@ item)
	{
		if(@item == null || @item._list_view != @this || @_items[_num_items - 1] == @item)
			return @item;
		
		int index = _items.findByRef(@item);
		
		if(index == -1 || index == _num_items - 1)
			return @item;
		
		@_items[index] = @_items[index + 1];
		@_items[index + 1] = item;
		
		_content.move_up(item);
		return @item;
	}
	
	ListViewItem@ move_item_down(ListViewItem@ item)
	{
		if(@item == null || @item.parent != @this || @_items[0] == @item)
			return @item;
		
		int index = _items.findByRef(@item);
		
		if(index == -1 || index == 0)
			return @item;
		
		@_items[index] = @_items[index - 1];
		@_items[index - 1] = @item;
		
		_content.move_down(item);
		return @item;
	}
	
	// Get
	
	ListViewItem@ get_item(const int index)
	{
		if(index < 0 || index >= _num_items)
			return null;
		
		return @_items[index];
	}
	
	ListViewItem@ get_item(const string value, int &out index, const bool first=false)
	{
		ListViewItem@ item;
		index = -1;
		
		if(first)
		{
			for(int i = 0; i < _num_items; i++)
			{
				if(_items[i].value == value)
				{
					@item = @_items[i];
					index = i;
					break;
				}
			}
		}
		else
		{
			for(int i = _num_items - 1; i >= 0; i--)
			{
				if(_items[i].value == value)
				{
					@item = @_items[i];
					index = i;
					break;
				}
			}
		}
		
		return @item;
	}
	
	int get_item_index(ListViewItem@ item)
	{
		return _items.findByRef(item);
	}
	
	// Selection
	
	void select_item(ListViewItem@ item)
	{
		if(busy_updating_selection || @item == null || @item._list_view != @this || item.selected)
			return;
		
		busy_updating_selection = true;
		
		if(!_allow_multiple_selection && _num_selected_items == 1)
		{
			_selected_items[0].selected = false;
			@_selected_items[0] = @item;
		}
		else
		{
			_selected_items.insertLast(@item);
			_num_selected_items++;
		}
		
		item.selected = true;
		
		busy_updating_selection = false;
		
		dispatch_select_event(item);
	}
	
	void deselect_item(ListViewItem@ item)
	{
		if(busy_updating_selection || @item == null || @item._list_view != @this || !item.selected)
			return;
		
		busy_updating_selection = true;
		
		int index = _selected_items.findByRef(@item);
		
		if(index != -1)
		{
			_selected_items.removeAt(index);
			item.selected = false;
			_num_selected_items--;
		}
		
		busy_updating_selection = false;
		
		dispatch_select_event(item);
	}
	
	void select_all()
	{
		if(_num_selected_items == _num_items || !_allow_multiple_selection)
			return;
		
		busy_updating_selection = true;
		
		for(int i = 0; i < _num_items; i++)
		{
			ListViewItem@ item = @_items[i];
			
			if(!item.selected)
			{
				item.selected = true;
				_selected_items.insertLast(@item);
			}
		}
		
		busy_updating_selection = false;
		
		_num_selected_items = _num_items;
		dispatch_select_event();
	}
	
	void select_none()
	{
		if(_num_selected_items == 0)
			return;
		
		busy_updating_selection = true;
		
		for(int i = 0; i < _num_selected_items; i++)
		{
			_selected_items[i].selected = false;
		}
		
		busy_updating_selection = false;
		
		_num_selected_items = 0;
		_selected_items.resize(0);
		dispatch_select_event();
	}
	
	void set_selected_item(ListViewItem@ item)
	{
		if(@item._list_view != @this || @item != null || item.selected)
			return;
		
		busy_updating_selection = true;
		
		for(int i = 0; i < _num_selected_items; i++)
		{
			_selected_items[i].selected = false;
		}
		
		_selected_items.resize(0);
		_num_selected_items = 0;
		
		if(@item != null)
		{
			_selected_items.insertLast(@item);
			_num_selected_items++;
			item.selected = true;
		}
		
		busy_updating_selection = false;
		
		dispatch_select_event();
	}
	
	ListViewItem@ selected_item
	{
		get const { return _num_selected_items > 0 ? _selected_items[0] : null; }
	}
	
	int selected_index
	{
		get const
		{
			return _num_selected_items > 0 ? _items.findByRef(_selected_items[0]) : -1;
		}
	}
	
	int get_selected_items(array<ListViewItem@>@ list, const bool ordered=true)
	{
		if(int(list.length()) < _num_selected_items)
			list.resize(_num_selected_items);
		
		if(ordered)
		{
			int index = 0;
			
			for(int i = 0; i < _num_items; i++)
			{
				ListViewItem@ item = @_items[i];
				
				if(item.selected)
				{
					@list[index++] = @item;
				}
			}
		}
		else
		{
			for(int i = 0; i < _num_selected_items; i++)
			{
				@list[i] = @_selected_items[i];
			}
		}
		
		return _num_selected_items;
	}
	
	//
	
	protected void dispatch_select_event(ListViewItem@ item=null)
	{
		ui._dispatch_event(@select, EventType::SELECT, this, @item != null ? item.value : '');
	}
	
	// Events
	
	void _mouse_press(EventInfo@ event) override
	{
		if(event.button != ui.primary_button)
			return;
		
		if(drag_select)
		{
			ListViewItem@ item = cast<ListViewItem>(@ui.mouse_over_element);
			
			if(@item != null && @item._list_view == @this)
			{
				busy_drag_select = true;
				item.selected = !item.selected;
				drag_select_select = item.selected;
			}
		}
	}
	
	void _mouse_move(EventInfo@ event) override
	{
		if(busy_drag_select)
		{
			if(!ui.mouse.primary_down)
			{
				busy_drag_select = false;
			}
			else
			{
				ListViewItem@ item = cast<ListViewItem>(@ui.mouse_over_element);
				
				if(@item != null && @item._list_view == @this)
				{
					item.selected = drag_select_select;
				}
			}
		}
	}
	
	void _mouse_release(EventInfo@ event) override
	{
		if(event.button != ui.primary_button)
			return;
		
		busy_drag_select = false;
	}
	
}