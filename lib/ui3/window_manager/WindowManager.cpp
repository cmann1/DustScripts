#include '../UI.cpp';
#include 'WindowAnchor.cpp';

/// Will manage element position inside of a UI.
/// Elements will be anchored to the closests screen edge and when the screen size changes, element positions will
/// be adjusted to attempt to keep the relative distance to that edge. Elements will also be clamped inside of the screen bounds
/// to ensure they are always visible.
/// 
/// To use add the [hidden] annotation, call the initialise method  during startup, register elements with the register_element method, and
/// make sure to finish by calling complete_registration.
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
	private array<WindowAnchor> stored_anchors;
	
	private array<WindowAnchor> pending_anchors;
	private dictionary anchor_map;
	private int num_anchors;
	
	private bool started_initalisation;
	private bool initalised;
	
	private bool pending_reposition_all;
	private bool pending_initialise;
	private bool pending_update_all;
	private array<int> pending_update;
	private int num_pending_update;
	private int size_pending_update;
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
		
		@after_layout_delegate = EventCallback(on_after_layout);
		started_initalisation= true;
	}
	
	void register_element(Element@ element)
	{
		if(!started_initalisation)
		{
			puts('WindowManager: Call initialise before registering elements.');
			return;
		}
		
		if(initalised)
		{
			puts('WindowManager: register_element must be called before complete_registration.');
			return;
		}
		
		if(@element == null)
			return;
		
		const string id = element.name != '' ? element.name : element._id;
		
		WindowAnchor anchor;
		anchor.id = id;
		@anchor.element = element;
		pending_anchors.insertLast(anchor);
	}
	
	void complete_registration()
	{
		initalised = true;
		pending_initialise = true;
		
		ui.screen_resize.on(EventCallback(on_screen_resize));
		wait_for_layout();
	}
	
	void update_all_anchors()
	{
		pending_update_all = true;
		num_pending_update = 0;
		wait_for_layout();
	}
	
	void update_anchors(Element@ element)
	{
		if(@element == null)
			return;
		
		const string id = element.name != '' ? element.name : element._id;
		
		if(!anchor_map.exists(id))
			return;
		
		if(num_pending_update == size_pending_update)
		{
			pending_update.resize(size_pending_update += 16);
		}
		
		pending_update[num_pending_update++] = int(anchor_map[id]);
	}
	
	private void wait_for_layout()
	{
		if(waiting_for_layout)
			return;
		
		ui.after_layout.on(after_layout_delegate);
		waiting_for_layout = true;
	}
	
	private void initialise_all_anchors()
	{
		const float width = ui.region_width;
		const float height = ui.region_height;
		
		// Step 1.
		
		dictionary stored_anchors_map;
		
		if(restore_positions)
		{
			const int num_stored = int(stored_anchors.length());
			
			for(int i = 0; i < num_stored; i++)
			{
				WindowAnchor@ anchor = @stored_anchors[i];
				stored_anchors_map[anchor.id] = anchor;
			}
		}
		
		// Step 2.
		
		num_anchors = int(pending_anchors.length());
		stored_anchors.resize(num_anchors);
		
		for(int i = num_anchors - 1; i >= 0; i--)
		{
			WindowAnchor@ anchor;
			
			if(restore_positions && stored_anchors_map.exists(pending_anchors[i].id))
			{
				stored_anchors[i] = cast<WindowAnchor>(stored_anchors_map[pending_anchors[i].id]);
				@anchor = @stored_anchors[i];
				@anchor.element = @pending_anchors[i].element;
				anchor.initialise();
				anchor.reposition(width, height, relative);
			}
			else
			{
				stored_anchors[i] = pending_anchors[i];
				@anchor = @stored_anchors[i];
				@anchor.element = @pending_anchors[i].element;
				anchor.initialise();
				anchor.update(width, height);
			}
		}
		
		pending_initialise = false;
	}
	
	private void update_all_anchors_impl()
	{
		const float width = ui.region_width;
		const float height = ui.region_height;
		
		num_anchors = int(pending_anchors.length());
		stored_anchors.resize(num_anchors);
		
		for(int i = num_anchors - 1; i >= 0; i--)
		{
			WindowAnchor@ anchor = @pending_anchors[i];
			anchor.update(width, height);
			stored_anchors[i] = anchor;
			anchor_map[anchor.id] = i;
		}
		
		pending_update_all = false;
	}
	
	private void update_pending_anchors()
	{
		const float width = ui.region_width;
		const float height = ui.region_height;
		
		for(int i = num_pending_update - 1; i >= 0; i--)
		{
			WindowAnchor@ anchor = @stored_anchors[pending_update[i]];
			anchor.update(width, height);
		}
		
		num_pending_update = 0;
	}
	
	private void reposition_all()
	{
		const float width = ui.region_width;
		const float height = ui.region_height;
		
		for(int i = num_anchors - 1; i >= 0; i--)
		{
			WindowAnchor@ anchor = @stored_anchors[i];
			anchor.reposition(width, height, relative);
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
		if(pending_initialise)
		{
			initialise_all_anchors();
		}
		else if(pending_update_all)
		{
			update_all_anchors_impl();
		}
		
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