#include '../../math/math.cpp';
#include '../UI.cpp';
#include '../Style.cpp';
#include '../events/Event.cpp';
#include 'SingleContainer.cpp';

class MultiButton : SingleContainer
{
	
	Event select;
	
	int selected_index { get const { return _selected_index; } }
	string selected_name { get const { return _selected_name; } }
	Image@ selected_image { get { return @_selected_image; } }
	
	protected array<string> item_names;
	protected array<Image@> images;
	protected string _selected_name;
	protected Image@ _selected_image;
	protected int _selected_index = -1;
	
	protected bool pressed;
	
	MultiButton(UI@ ui)
	{
		super(ui, null, 'mbtn');
		
		children_mouse_enabled = false;
		width  = 40;
		height = 40;
	}
	
	Image@ add(const string name, Image@ image, int index=-1)
	{
		if(@image == null || item_names.find(name) != -1)
			return null;
		
		int insert_index;
		
		if(index == -1 || index >= int(item_names.length()))
		{
			item_names.insertLast(name);
			images.insertLast(image);
			insert_index = item_names.length() - 1;
		}
		else
		{
			item_names.insertAt(index, name);
			images.insertAt(index, @image);
			insert_index = index;
		}
		
		if(@_selected_image == null)
		{
			set_selected(insert_index, name, image);
		}
		
		return image;
	}
	
	Image@ add(const string name, const string sprite_text, const string sprite_name, const float width=-1, const float height=-1, const float offset_x=-0.5, const float offset_y=-0.5)
	{
		return add(name, Image(ui, sprite_text, sprite_name, width, height, offset_x, offset_y));
	}
	
	Image@ remove(const string name)
	{
		const int index = item_names.find(name);
		
		if(index == -1)
			return null;
		
		return _remove(index, images[index]);
	}
	
	Image@ remove(Image@ image)
	{
		if(@image == null)
			return null;
		
		const int index = images.findByRef(@image);
		
		if(index == -1)
			return null;
		
		return _remove(index, image);
	}
	
	void do_layout(const float parent_x, const float parent_y) override
	{
		Element::do_layout(parent_x, parent_y);
		
		if(hovered && ui.mouse.primary_press)
		{
			pressed = true;
		}
		else if(pressed)
		{
			if(hovered && ui.mouse.primary_release)
			{
				const int num_items = int(item_names.length());
				
				if(num_items > 0)
				{
					const int new_index = (_selected_index + 1) % num_items;
					
					if(new_index != _selected_index)
					{
						set_selected(new_index, item_names[new_index], images[new_index]);
					}
				}
			}
			
			if(!ui.mouse.primary_down)
			{
				pressed = false;
			}
		}
		
		if(@_content != null)
		{
			_content.x = (width  - _content.width)  * 0.5;
			_content.y = (height - _content.height) * 0.5;
			
			if(pressed)
			{
				_content.x += ui.style.button_pressed_icon_offset;
				_content.y += ui.style.button_pressed_icon_offset;
			}
		}
	}
	
	void draw(Style@ style, const float sub_frame) override
	{
		if(alpha != 1)
			style.multiply_alpha(alpha);
		
		style.draw_interactive_element(this, hovered, false, disabled);
		
		if(@_content != null)
		{
			if(disabled)
				style.disable_alpha();
			
			_content.draw(style, sub_frame);
			
			if(disabled)
				style.restore_alpha();
		}
		
		if(alpha != 1)
			style.restore_alpha();
	}
	
	private Image@ _remove(const int index, Image@ image)
	{
		int new_index;
		string new_name = '';
		Image@ new_image = null;
		
		if(@image == @_selected_image)
		{
			if(num_children == 1)
			{
				@content = null;
				set_selected(-1, '', null);
			}
			else
			{
				const int next_index = index == num_children - 1 ? index - 1 : index + 1;
				new_name = item_names[next_index];
				@new_image = @images[next_index];
				@content = @new_image;
				new_index = next_index > index ? next_index - 1 : next_index;
			}
		}
		
		item_names.removeAt(index);
		images.removeAt(index);
		
		if(@new_image != null)
		{
			set_selected(new_index, new_name, new_image);
		}
		
		return image;
	}
	
	private void set_selected(const int index, const string name, Image@ image)
	{
		_selected_index = index;
		@_selected_image = @image;
		_selected_name = name;
		
		@content = @image;
		
		ui._event_info.reset(EventType::SELECT, this);
		select.dispatch(ui._event_info);
	}
	
	protected float border_size { get const override { return ui.style.border_size; } }
	
}