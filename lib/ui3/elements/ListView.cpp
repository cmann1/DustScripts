#include '../MouseButton.cpp';
#include '../events/Event.cpp';
#include '../layouts/GridLayout.cpp';
#include 'ScrollView.cpp';
#include 'ListViewItem.cpp';

namespace ListView { const string TYPE_NAME = 'ListView'; }

class ListView : ScrollView
{
	
	bool drag_select = false;
	bool allow_deselect = true;
	/// Force at least this many items to remain selected at all times
	uint min_select = 0;
	/// Which key to hold for multiple select. If set, clicking without holding
	/// this key will single select the clicked item
	int multi_select_key = 0;

	Event select;
	
	protected GridLayout@ grid_layout;
	protected bool _allow_multiple_selection;
	protected array<ListViewItem@> _items;
	protected array<ListViewItem@> _selected_items;
	protected uint _num_items;
	protected uint _num_selected_items;
	protected bool busy_updating_selection;
	protected bool internal_select;
	
	protected bool busy_drag_select;
	protected bool drag_select_select;
	protected bool check_multi_select_key;
	
	ListView(UI@ ui)
	{
		super(@ui);
		
		_width  = _set_width  = 200;
		_height = _set_height = 200;
		
		@grid_layout = GridLayout(ui, 1, 0, 0, FlowDirection::Row, FlowAlign::Stretch, FlowAlign::Stretch, FlowFit::MainAxis);
		grid_layout.row_spacing = 0;
		@_content.layout = @grid_layout;
	}
	
	string element_type { get const override { return ListView::TYPE_NAME; } }
	
	uint num_items { get const { return _num_items; } }
	
	uint num_selected_items { get const { return _num_selected_items; } }
	
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
		if(list.length() < _num_items)
			list.resize(_num_items);
		
		for(uint i = 0; i < _num_items; i++)
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
			return set_item_index(@item, _num_items);
		
		if(@item._list_view != null)
		{
			@item._list_view.remove_item(item);
		}
			
		if(index < 0 || index >= int(_num_items))
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
		if(index < 0 || index >= int(_num_items))
			return null;
		
		return remove_item(@_items[index]);
	}
	
	// Order
	
	ListViewItem@ set_item_index(int old_index, int index)
	{
		if(old_index < 0 || old_index >= int(_num_items))
			return null;
		
		if(index < 0)
			index = 0;
		else if(index >= int(_num_items))
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
		
		if(index == -1 || index == int(_num_items) - 1)
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
		if(index < 0 || index >= int(_num_items))
			return null;
		
		return @_items[index];
	}
	
	ListViewItem@ get_item(const string value, int &out index, const bool first=false)
	{
		ListViewItem@ item;
		index = -1;
		
		if(first)
		{
			for(uint i = 0; i < _num_items; i++)
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
	
	void clear() override
	{
		_content.clear();
		
		_items.resize(0);
		_selected_items.resize(0);
		_num_items = 0;
		_num_selected_items = 0;
	}
	
	int get_item_index(ListViewItem@ item)
	{
		return _items.findByRef(item);
	}
	
	// Selection
	
	void select_item(ListViewItem@ item)
	{
		if((busy_updating_selection && !internal_select) || @item == null || @item._list_view != @this)
			return;

		if(item.selected)
		{
			if(multi_select_key == 0 || !ui._has_editor)
				return;
			if(ui._editor.key_check_vk(multi_select_key))
				return;
		}

		busy_updating_selection = true;

		if(!do_multiple_selection())
		{
			for(uint i = 0; i < _num_selected_items; i++)
			{
				_selected_items[i]._selected = false;
			}

			_num_selected_items = 1;
			_selected_items.resize(1);
			@_selected_items[0] = @item;
			item._selected = true;
		}
		else if(!item._selected)
		{
			_selected_items.insertLast(@item);
			_num_selected_items++;
			item._selected = true;
		}
		
		busy_updating_selection = false;
		dispatch_select_event(item);
	}
	
	void select_item(const string value)
	{
		int index;
		select_item(get_item(value, index));
	}
	
	void deselect_item(ListViewItem@ item)
	{
		if((busy_updating_selection && !internal_select) || @item == null || @item._list_view != @this || !item.selected)
			return;
		if(_num_selected_items <= min_select)
			return;
		
		if(!internal_select)
		{
			busy_updating_selection = true;
		}
		
		int index = _selected_items.findByRef(@item);
		
		if(index != -1)
		{
			_selected_items.removeAt(index);
			item._selected = false;
			_num_selected_items--;
		}
		
		if(!internal_select)
		{
			busy_updating_selection = false;
			dispatch_select_event(item);
		}
	}
	
	void select_all()
	{
		if(_num_selected_items == _num_items || !do_multiple_selection())
			return;
		
		busy_updating_selection = true;
		internal_select = true;
		
		for(uint i = 0; i < _num_items; i++)
		{
			ListViewItem@ item = @_items[i];
			
			if(!item.selected)
			{
				select_item(item);
			}
		}
		
		busy_updating_selection = false;
		internal_select = false;
		dispatch_select_event();
	}
	
	void select_none()
	{
		if(_num_selected_items == 0)
			return;
		
		busy_updating_selection = true;
		internal_select = true;
		
		for(int i = int(_num_selected_items) - 1; i >= 0; i--)
		{
			deselect_item(_selected_items[i]);
		}
		
		busy_updating_selection = false;
		internal_select = false;
		dispatch_select_event();
	}
	
	ListViewItem@ selected_item
	{
		get { return _num_selected_items > 0 ? _selected_items[0] : null; }
	}
	
	int selected_index
	{
		get const
		{
			return _num_selected_items > 0 ? _items.findByRef(_selected_items[0]) : -1;
		}
		set
		{
			if(_num_items == 0 || value < 0 || value >= int(_num_items))
				return;

			if(_num_selected_items == 1 && _items[_num_items]._selected)
				return;

			busy_updating_selection = true;

			for(uint i = 0; i < _num_selected_items; i++)
			{
				_selected_items[i]._selected = false;
			}

			_num_selected_items = 1;
			_selected_items.resize(1);
			@_selected_items[0] = _items[value];
			_selected_items[0]._selected = true;

			busy_updating_selection = false;
			dispatch_select_event(_selected_items[0]);
		}
	}
	
	int get_selected_items(array<ListViewItem@>@ list, const bool ordered=true)
	{
		if(list.length < _num_selected_items)
			list.resize(_num_selected_items);
		
		if(ordered)
		{
			int index = 0;
			
			for(uint i = 0; i < _num_items; i++)
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
			for(uint i = 0; i < _num_selected_items; i++)
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

	protected bool do_multiple_selection()
	{
		if(!check_multi_select_key || multi_select_key == 0)
			return _allow_multiple_selection;

		return
			!ui._has_editor || ui._editor.key_check_vk(multi_select_key);
	}

	// Events
	//
	void _mouse_click(EventInfo@ event) override
	{
		if(event.src.element_type != ListViewItem::TYPE_NAME)
			return;

		if(drag_select)
			return;
		
		check_multi_select_key = true;
		ListViewItem@ item = cast<ListViewItem@>(event.src);
		
		if(
			item._selected && _allow_multiple_selection && ui._has_editor &&
			(multi_select_key != 0 || !ui._editor.key_check_vk(multi_select_key)))
		{
			select_item(item);
			drag_select_select = true;
		}
		else
		{
			drag_select_select = allow_deselect &&
					(!item.selected || _num_selected_items > min_select)
				? !item.selected : true;
			item.selected = drag_select_select;
		}

		check_multi_select_key = false;
	}
	
	void _mouse_press(EventInfo@ event) override
	{
		if(event.button != ui.primary_button)
			return;

		if(!drag_select)
			return;
		
		ListViewItem@ item = cast<ListViewItem>(@ui.mouse_over_element);

		if(@item != null && @item._list_view == @this)
		{
			busy_drag_select = true;
			if(multi_select_key == 0 || !ui._has_editor || ui._editor.key_check_vk(multi_select_key))
			{
				drag_select_select = allow_deselect && (_num_selected_items > min_select)
					? !item.selected : true;
			}
			else
			{
				drag_select_select = true;
			}

			check_multi_select_key = true;
			if(drag_select_select)
				select_item(item);
			else
				deselect_item(item);
			check_multi_select_key = false;
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
				
				if(@item != null && @item._list_view == @this && (!item.selected || _num_selected_items > min_select))
				{
					check_multi_select_key = true;
					if(drag_select_select)
						select_item(item);
					else
						deselect_item(item);
					check_multi_select_key = false;
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
