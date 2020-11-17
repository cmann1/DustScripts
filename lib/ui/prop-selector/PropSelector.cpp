#include '../../props/common.cpp';
#include '../../props/Prop.cpp';
#include '../../props/data.cpp';
#include '../UI.cpp';
#include '../Align.cpp';
#include '../ScrollView.cpp';
#include '../Button.cpp';
#include '../label.cpp';
#include '../shapes/ShapeCross.cpp';
#include 'GroupName.cpp';
#include 'PropIcon.cpp';

class PropSelector : ButtonClickHandler
{
	
	private float GROUPS_WIDTH = 175;
	private float TITLE_BAR_HEIGHT = 34;
	
	scene@ g;
	
	UI@ ui;
	Mouse@ mouse;
	ScrollView@ groupContainer;
	ScrollView@ propsContainer;
	ScrollView@ palettesContainer;
	Button@ close_button;
	Button@ clear_button;
	
	GroupName@ selected_group = null;
	PropIcon@ selected_prop_icon = null;
	PropIcon@ focused_prop = null;
	
	PropSelectorResult result = PropSelectorResult::Inactive;
	const PropIndex@ result_prop = null; 
	uint result_palette;
	
	bool visible = false;
	
	private const PropIndex@ palettes_selection = null;
	
	private array<GroupName@> groupNames;
	
	PropSelector(UI@ ui)
	{
		@g = get_scene();
		
		@this.ui = ui;
		@this.mouse = ui.mouse;
		
		@groupContainer = ScrollView(ui);
		@propsContainer = ScrollView(ui);
		@palettesContainer = ScrollView(ui);
		@close_button = Button(ui, ShapeCross(ui));
		@clear_button = Button(ui, Label(ui, 'Clear'));
		
		const float height = TITLE_BAR_HEIGHT - ui.padding * 2;
		close_button.width = close_button.height = height;
		clear_button.fit_to_height(height);
		
		@close_button.click_listener = this;
		@clear_button.click_listener = this;
		
		groupContainer.padding = ui.padding;
		
		groupNames.resize(PROP_INDICES.size());
		
		for(uint i = 0; i < PROP_INDICES.size(); i++)
		{
			const array<PropIndex>@ group = PROP_INDICES[i];
			string group_name = PROP_GROUP_NAMES[i];
			
			if(group_name == '')
				continue;
			
			GroupName@ groupName = GroupName(ui, @this, group_name, i);
			groupName.width = GROUPS_WIDTH;
			groupContainer.addChild(groupName);
			
			@groupNames[i] = groupName;
		}
		
		propsContainer.columns = 99999;
		propsContainer.padding = ui.padding;
		
		palettesContainer.padding = ui.padding;
		palettesContainer.direction = Direction::Horizontal;
	}
	
	void select_group(GroupName@ group)
	{
		if(@selected_group == @group)
			return;
		
		select_prop(null);
		
		if(@selected_group != null)
		{
			selected_group.selected = false;
		}
		
		@selected_group = group;
		propsContainer.clear();
		
		if(@group != null)
		{
			group.selected = true;
			
			const array<PropIndex>@ groupIndices = PROP_INDICES[group.id];
			
			for(uint i = 0; i < groupIndices.size(); i++)
			{
				const PropIndex@ prop_index = @groupIndices[i];
				PropIcon@ propIcon = PropIcon(ui, @this, prop_index);
				propsContainer.addChild(propIcon);
			}
			
			groupContainer.scroll_into_view = groupContainer.children.findByRef(group);
		}
	}
	
	void select_prop(PropIcon@ prop_icon, bool update_selected = true)
	{
		result = visible ? PropSelectorResult::Waiting : PropSelectorResult::Inactive;
		@result_prop = null; 
		result_palette = 0;
		
		if(@prop_icon == null || prop_icon.palette == -1)
		{
			@palettes_selection = null;
			palettesContainer.clear();
		}
		
		if(@selected_prop_icon == @prop_icon)
		{
			if(update_selected && @prop_icon !=  null)
			{
				result = PropSelectorResult::Selected;
				@result_prop = prop_icon.prop_index; 
				result_palette = prop_icon.palette == -1 ? 0 : prop_icon.palette;
			}
			
			return;
		}
		
		if(@selected_prop_icon != null)
		{
			selected_prop_icon.selected = false;
		}
		
		@selected_prop_icon = @prop_icon;
		
		if(@prop_icon != null)
		{
			prop_icon.selected = true;
			
			if(prop_icon.prop_index.palettes == 1)
			{
				if(update_selected)
				{
					result = PropSelectorResult::Selected;
					@result_prop = prop_icon.prop_index; 
					result_palette = prop_icon.palette == -1 ? 0 : prop_icon.palette;
				}
			}
			else
			{
				@palettes_selection = @prop_icon.prop_index;
			}
			
			if(update_selected && prop_icon.palette != -1)
			{
				result = PropSelectorResult::Selected;
				@result_prop = prop_icon.prop_index; 
				result_palette = prop_icon.palette;
			}
		}
		
		if(@palettes_selection != null && prop_icon.palette == -1)
		{
			for(uint i = 0; i < palettes_selection.palettes; i++)
			{
				PropIcon@ propIcon = PropIcon(ui, @this, palettes_selection, i);
				palettesContainer.addChild(propIcon);
			}
		}
	}
	
	void select_prop(uint set, uint group, uint index, uint palette = 0)
	{
		if(group < 0 || group >= PROP_INDICES.size())
			return;
		
		if(palette < 0)
			return;
		
		select_group(groupNames[group]);
		
		for(uint i = 0; i < propsContainer.children.size(); i++)
		{
			PropIcon @prop_icon = cast<PropIcon>(propsContainer.children[i]);
			
			if(prop_icon.prop_index.set == set && prop_icon.prop_index.index == index)
			{
				if(palette >= prop_icon.prop_index.palettes)
					break;
				
				select_prop(null, false);
				select_prop(prop_icon, false);
				
				if(palette < palettesContainer.children.size() && prop_icon.prop_index.palettes > 1)
				{
					select_prop(cast<PropIcon>(palettesContainer.children[palette]), false);
				}
				
				return;
			}
		}
		
		select_prop(null);
	}
	
	void reset()
	{
		select_prop(null);
		select_group(null);
		@focused_prop = null;
		@palettes_selection = null;
		
		result = visible ? PropSelectorResult::Waiting : PropSelectorResult::Inactive;
		@result_prop = null; 
		result_palette = 0;
		
		palettesContainer.clear();
	}
	
	void show()
	{
		visible = true;
	}
	
	void hide()
	{
		result = visible ? PropSelectorResult::Waiting : PropSelectorResult::Inactive;
		@result_prop = null; 
		result_palette = 0;
		
		visible = false;
	}
	
	void draw()
	{
		const float PADDING = ui.padding;
		
		const float width = 767;
		
		const Rect window(
			-width * 0.5, -300,
			 width * 0.5,  300);
		
		Rect rect(
			window.x1, window.y1,
			window.x2, window.y2);
		
		const float PALETTE_HEIGHT = 64;
		const float PALETTE_TOP = window.y2 - (PALETTE_HEIGHT + PADDING * 3 + ui.scrollbar_width);
		const bool show_palette = @palettes_selection != null;
		
		const float content_top = rect.y1 + TITLE_BAR_HEIGHT + PADDING;
		
		// Background
		// ----------------------------------------
		
		g.draw_rectangle_hud(22, 22, rect.x1, rect.y1, rect.x2, rect.y2, 0, ui.bg_colour);
		g.draw_glass_hud(22, 22, rect.x1, rect.y1, rect.x2, rect.y2, 0, 0x00000000);
		
		// Title bar
		// ----------------------------------------
		
		rect.set(
			window.x1, window.y1,
			window.x2, window.y1 + TITLE_BAR_HEIGHT);
		
		ui.text('Prop Selector', rect.x1 + PADDING, (rect.y1 + rect.y2 - 1) * 0.5, Align::Left, Align::Middle);
		g.draw_rectangle_hud(22, 22, rect.x1, rect.y2 - 0.5, rect.x2, rect.y2, 0, ui.border_colour);
		
		rect.set(
			window.x2 - PADDING - close_button.width, window.y1 + (TITLE_BAR_HEIGHT - close_button.height) * 0.5,
			window.x2 - PADDING, window.y1 + TITLE_BAR_HEIGHT);
		
		close_button.draw(g, rect);
		
		rect.set(
			rect.x1 - PADDING - clear_button.width, rect.y1,
			rect.x1 - PADDING, rect.y2);
		
		clear_button.draw(g, rect);
		
		// Groups
		// ----------------------------------------
		
		rect.set(
			window.x1 + PADDING,
			content_top,
			window.x1 + PADDING + GROUPS_WIDTH,
			window.y2 - PADDING);
		
		groupContainer.draw(g, rect);
		
		// Props
		// ----------------------------------------
		
		if(@selected_group != null)
		{
			rect.set(
				rect.x2 + PADDING * 2 + ui.scrollbar_width,
				content_top,
				window.x2 - PADDING * 2 - ui.scrollbar_width,
				show_palette ? PALETTE_TOP : window.y2 - PADDING);
			
			propsContainer.draw(g, rect);
		}
		
		// Palettes
		// ----------------------------------------
		
		if(show_palette)
		{
			rect.set(
				rect.x1,
				PALETTE_TOP + PADDING,
				window.x2 - PADDING,
				window.y2 - PADDING * 2 - ui.scrollbar_width);
			
			palettesContainer.draw(g, rect);
		}
		
		ui.post_draw();
		
		if(!mouse.right_down)
		{
			@focused_prop = null;
		}
	}
	
	void on_button_click(Button@ button)
	{
		if(@button == @close_button)
		{
			hide();
		}
		else if(@button == @clear_button)
		{
			result = None;
			@result_prop = null; 
			result_palette = 0;
		}
	}
	
}

enum PropSelectorResult
{
	
	// The prop selector window is not open
	Inactive,
	// The prop selector window is open but a prop is not selected
	Waiting,
	// "None" has been selected
	None,
	// A prop has been selected
	Selected,
	
}