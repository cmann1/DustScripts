#include '../UI.cpp';
#include 'WindowAnchor.cpp';

/// Will manage element position inside of a UI.
/// Elements will be anchored to the closests screen edge and when the screen size changes, element positions will
/// be adjusted to attempt to keep the relative distance to that edge. Elements will also be clamped inside of the screen bounds
/// to ensure they are always visible.
/// 
/// To use, add the [hidden] annotation, call the `initialise` method during startup, then register elements with the `register_element` method.
/// 
/// Elements are tracked across sessions based on their id. As ids may change based on the number and order elements are created in, giving
/// unique names to elements before registering them is recommended.
///
/// Elements are assumed to be added directly to the UI, and should not be nested within other elements.
class WindowManager
{
	
	/// Will load positions stored between sessions
	bool restore_positions = true;
	/// If true anchor position will stretch relative to the screen width and anchor distance
	bool relative = false;
	
	private UI@ ui;
	
	[hidden]
	private array<WindowAnchor> anchors;
	
	private dictionary anchor_map;
	private array<WindowAnchor@> pending_anchors;
	
	private bool started_initalisation;
	private bool initalised;
	
	private bool pending_reposition_all;
	private int num_pending_update;
	private bool waiting_for_layout;
	
	private EventCallback@ after_layout_delegate;
	
	void initialise(UI@ ui)
	{
		@this.ui = ui;
		
		if(@ui == null)
		{
			puts('WindowManager: ui cannot be null');
			return;
		}
		
		for(int i = int(anchors.length()) - 1; i >= 0; i--)
		{
			WindowAnchor@ anchor = @anchors[i];
			anchor_map[anchor.id] = i;
		}
		
		@after_layout_delegate = EventCallback(on_after_layout);
		ui.screen_resize.on(EventCallback(on_screen_resize));
		
		initalised = true;
	}
	
	/// Registers the element with the WindowManager and automatically repositions it if the has been
	/// registered before.
	void register_element(Element@ element)
	{
		if(!initalised)
		{
			puts('WindowManager: Call initialise before registering elements.');
			return;
		}
		
		if(@element == null)
			return;
		
		const string id = element.name != '' ? element.name : element._id;
		
		WindowAnchor@ anchor;
		
		if(anchor_map.exists(id))
		{
			@anchor = anchors[int(anchor_map[id])];
			anchor.pending_reposition = true;
		}
		else
		{
			anchors.insertLast(WindowAnchor());
			@anchor = @anchors[anchors.length() - 1];
			anchor.id = id;
		}
		
		@anchor.element = element;
		anchor.initialise();
		pending_anchors.insertLast(anchor);
		num_pending_update++;
		wait_for_layout();
	}
	
	/// Resets anchor positions for all registered windows
	void update_all_anchors()
	{
		num_pending_update = int(anchors.length());
		pending_anchors.resize(num_pending_update);
		
		for(int i = 0; i < num_pending_update; i++)
		{
			@pending_anchors[i] = @anchors[i];
		}
		
		wait_for_layout();
	}
	
	/// Resets the anchor position for the specified element
	void update_anchors(Element@ element)
	{
		if(@element == null)
			return;
		
		const string id = element.name != '' ? element.name : element._id;
		
		if(!anchor_map.exists(id))
			return;
		
		pending_anchors.insertLast(@anchors[int(anchor_map[id])]);
		num_pending_update++;
		wait_for_layout();
	}
	
	/// Clears all stored anchors.
	void clear()
	{
		for(int i = int(anchors.length()) - 1; i >= 0; i--)
		{
			anchors[i].clear();
		}
	}
	
	/// Clears all stored positions for windows that have not yet been registered.
	void clear_unused()
	{
		for(int i = int(anchors.length()) - 1; i >= 0; i--)
		{
			WindowAnchor@ anchor = @anchors[i];
			
			if(@anchor.element != null)
				continue;
			
			anchors.removeAt(i);
			
			if(anchor_map.exists(anchor.id))
			{
				anchor_map.delete(anchor.id);
			}
		}
	}
	
	/// Clears the stored position for the given element
	void clear(Element@ element)
	{
		if(@element == null)
			return;
		
		const string id = element.name != '' ? element.name : element._id;
		
		if(!anchor_map.exists(id))
			return;
		
		const int index = int(anchor_map[id]);
		
		anchors.removeAt(index);
		anchor_map.delete(id);
	}
	
	private void wait_for_layout()
	{
		if(waiting_for_layout)
			return;
		
		ui.after_layout.on(after_layout_delegate);
		waiting_for_layout = true;
	}
	
	private void update_pending_anchors()
	{
		const float width = ui.region_width;
		const float height = ui.region_height;
		
		for(int i = num_pending_update - 1; i >= 0; i--)
		{
			WindowAnchor@ anchor = @pending_anchors[i];
			
			if(anchor.pending_reposition)
			{
				anchor.reposition(width, height, relative);
				anchor.pending_reposition = false;
			}
			else
			{
				anchor.update(width, height);
			}
		}
		
		pending_anchors.resize(0);
		num_pending_update = 0;
	}
	
	private void reposition_all()
	{
		const float width  = ui.region_width;
		const float height = ui.region_height;
		
		for(int i = int(anchors.length()) - 1; i >= 0; i--)
		{
			WindowAnchor@ anchor = @anchors[i];
			
			if(@anchor.element != null)
			{
				anchor.reposition(width, height, relative);
			}
		}
		
		pending_reposition_all = false;
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////////////////////////////
	
	private void on_screen_resize(EventInfo@ event)
	{
		pending_reposition_all = true;
		wait_for_layout();
	}
	
	private void on_after_layout(EventInfo@ event)
	{
		if(pending_reposition_all)
		{
			reposition_all();
		}
		
		if(num_pending_update > 0)
		{
			update_pending_anchors();
		}
		
		ui.after_layout.off(after_layout_delegate);
		waiting_for_layout = false;
	}
	
}