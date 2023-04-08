class ExtendedTileTool : Tool, IToolStepListener
{
	
	private ShortcutKey pick_key;
	
	ExtendedTileTool(AdvToolScript@ script)
	{
		super(script, 'Extended Tile Tool');
		
		selectable = false;
	}
	
	void on_init() override
	{
		Tool@ tool = script.get_tool('Tiles');
		
		if(@tool == null)
			return;
		
		tool.register_step_listener(this);
		pick_key.init(script).from_config('KeyPickTile', 'MiddleClick');
	}
	
	// //////////////////////////////////////////////////////////
	// Tool Callbacks
	// //////////////////////////////////////////////////////////
	
	void tool_step(Tool@ tool) override
	{
		if(script.mouse_in_scene && !script.space.down && !script.handles.mouse_over && pick_key.down())
		{
			pick_key.clear_gvb();
			
			for(int layer = 20; layer >= 6; layer--)
			{
				if(!script.editor.get_layer_visible(layer))
					continue;
				
				float layer_x, layer_y;
				script.mouse_layer(layer, layer_x, layer_y);
				const int tile_x = floor_int(layer_x / 48);
				const int tile_y = floor_int(layer_y / 48);
				tileinfo@ tile = script.g.get_tile(tile_x, tile_y, layer);
				
				if(tile.solid())
				{
					script.editor.set_tile_sprite(
						tile.sprite_set(), tile.sprite_tile(), tile.sprite_palette());
					script.show_info_popup(
						'Tile: ' + tile.sprite_set() + '.' +tile.sprite_tile() + '.' +tile.sprite_palette(),
						null, PopupPosition::Below, 2);
					break;
				}
			}
		}
	}
	
	// //////////////////////////////////////////////////////////
	// Methods
	// //////////////////////////////////////////////////////////
	
}
