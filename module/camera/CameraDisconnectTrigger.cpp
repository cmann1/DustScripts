#include '../../lib/triggers/EnterExitTrigger.cpp';

/** Detaches the camera from the current node/path when the player enters.
 * If `connect_node` is set, will attempt to connect the camera to the path the given node is on.
 */
class CameraDisconnectTrigger : trigger_base, EnterExitTrigger
{
	
	[entity,camera] uint connect_node;
	
	private script@ script;
	private scripttrigger@ self;
	private editor_api@ editor;
	
	void init(script@ s, scripttrigger@ self)
	{
		@this.script = s;
		@this.self = self;
		
		@editor = get_editor_api();
		
		self.editor_colour_inactive(0x666b7254);
		self.editor_colour_circle(0xff6b7254);
		self.editor_colour_active(0xff6b7254);
	}
	
	void activate(controllable@ c)
	{
		activate_enter_exit(c);
	}
	
	void step()
	{
		step_enter_exit();
	}
	
	bool can_trigger_enter_exit(controllable@ c)
	{
		return c.player_index() != -1;
	}
	
	void on_trigger_enter(controllable@ c)
	{
		const int player_index = c.player_index();
		camera@ cam = get_camera(player_index);
		
		if(connect_node != 0)
		{
			entity@ e = entity_by_id(connect_node);
			camera_node@ node = @e != null ? e.as_camera_node() : null;
			
			if(@node != null)
			{
				// Check connected nodes. Can prevent jumping around interest nodes if the camera
				// is already on connect_node's path.
				camera_node@ current_node = cam.current_node();
				if(@current_node != null)
				{
					for(uint i = 0, count = node.num_edges(); i < count; i++)
					{
						if(node.connected_node_id(i) == current_node.id())
						{
							return;
						}
					}
				}
				
				if(cam.try_connect(node))
				{
					on_connect(player_index, node);
					return;
				}
			}
		}
		
		if(cam.camera_type() != 'player')
		{
			cam.force_disconnect();
			on_connect(player_index, null);
		}
	}
	
	protected void on_connect(const int player_index, camera_node@ node)
	{
		
	}
	
	void editor_draw(float)
	{
		if(@editor != null && !editor.triggers_visible())
			return;
		if(connect_node == 0)
			return;
		
		entity@ e = entity_by_id(connect_node);
		if(@e == null)
			return;
		
		camera_node@ node = e.as_camera_node();
		if(@node == null)
			return;
		
		scene@ g = get_scene();
		g.draw_line_world(22, 22, self.x(), self.y(), node.x(), node.y(), 3, 0xaaaa6600);
	}
	
}
