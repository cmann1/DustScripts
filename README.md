# [Lib]
A collection of common and useful classes and functions.
I recommend putting this in the `script_src` root so they can be easily included in any map.
At the very least I'll include "lib/std.cpp" in most projects, which contains some of the most common things I use, such as `puts` overloads for various types.

- **drawing** - Drawing related utilities.
- **easing** - A collection of easing functions.
- **enums** - Built-in constants defined as enums, such as entity and var types.
- **math** - Math and geometry related classes and functions. Use `math.cpp` for most commonly needed functionality, eg. `dot`, `distance`, `lerp`.
- **tiles** - Tile related utilities.
- **ui3** - A general purpose UI library with a variety of common controls.

# [Shared]
A collection of stand-alone scripts to add predefined functionality to a map, eg. outlining entities.
Once added to a map and compiled, left click the script name to save it as a preset so that it can easily be added to other maps in the future.
- **Entity outliner** - Draws configurable outlines around entities
- **Breakable wall** - Allows you to mark rectangular regions of tiles as breakable
- **Prop wind** - Will make certain props sway in the wind
- **Shadows** - Casts shadows from tiles

# [Editor Utils]

### General Installation
1. Copy **lib**  and **ed** to your **script_src** directory.
2. All scripts are contained in the **ed** directory, or a subdirectory with that.
2. Open the script tab in the editor.
3. Click the text box, type **ed/path/to/script.cpp** and hit enter

### AdvTools
Combines and improves some of the old editor scripts using the new editor api.
See [this map](https://www.dropbox.com/s/15mq10gethutkml/__AdvTools?dl=0) for an overview of everything it can do.  
Compiling as an editor plugin is recommended.

### Tiles (t.cpp)
- **CopyTileEdges** - Copies only tiles with one or more solid edges to the target layer.
- **MoveTiles** - Move or copy tiles to the target layer.
- **SetTileSprites** - Sets all tiles to the target sprite set and palette
- **MakeTilesInvisible** - Makes tiles invisible - not sure this is useful because invisible tiles are removed when exiting play mode or a checkpoint is loaded

### Edge Brush (edge-brush/main.cpp)
##### Basic usage:
- **Right mouse:** Draw.
- **Middle mouse:** Toggle **update_collision**.
- **Left mouse + Mouse wheel:** Change layer.
- **Left mouse + Right mouse:** Drag horizontally to change the brush size.
- **Left mouse + Middle mouse:** Toggle precision mode.
##### Properties:
- **enabled:** Turn the script on and off.
- **layer:** Which layer to edit.
- **size:** The size of the brush.
- **update_collision:**
    - **Keep:** Keep existing edge collision.
    - **Off:** Turn edge collision off.
    - **Off:** Turn edge collision on.
- **update_edge:** 
    - **Keep:** Keep existing edge sprite.
    - **Off:** Turn edge sprite off. An edge with collision will always render an edge sprite regardless of this setting.
    - **Off:** Turn edge sprite on.
- **update_top:** Should top edges be modified.
- **update_bottom:**  Should bottom edges be modified.
- **update_left:**  Should left edges be modified.
- **update_right:**  Should right edges be modified.
- **edge:** 
    - **External:** Only edges adjacent empty space will be modified.
    - **Internal:** Only edges shared by two tiles will be modified.
    - **Both:** All edges will be modified.
- **external_different_sprites:** When checked, internal edges which are shared by two tiles with different sprite sets will be considered external.
- **precision_mode:** In precision mode, only the closest edge on the closest tile can will be modified.
- **precision_inside_only:** If checked only the tile the mouse is positioned inside of will be modified.
- **precision_update_neighbour:** When turning the edge rendering on for a tile, should the edge on adjacent tile be turned off?
- **render_edges:** Will indicate which edges will be modified. Tiles with collision are marked with cyan, and tiles without are marked with a broken magenta line.
- **always_render_edges:** If unchecked the edge markers will only be rendered while the brush is active (the right mouse is held down).

### Prop Path (prop-path.cpp)
A script trigger for placing props along a user defined path.  
Place a PropPath trigger, use the controls to select a prop and create the path - a live preview will be rendered.  
When you're happy with the results, press the "place" button and delete the trigger.
* **place:** Use this "button" to place the props in the map.
* **hide_props:** Don't render props for testing.
* **hide_overlays:** Don't render the curves and handles. Useful for checking prop placement without other stuff in the way.
* **smart_handles:** If checked control points on this and adjacent curves will be kept parallel and will be moved along with vertices.
* **accurate:** Tries to place props more accurately. Useful if the curve has tight corners.
* **curves:** The path is made up of one or more bezier curves. Use the +/- buttons to and and remove segments.
	* Points 1 and 4 are start and end points
	* Points 1 and 2 control the shape of the curve
* **prop_def:** The prop's properties.
	* **prop_set/group/index:** Which prop to use. See https://github.com/cmann1/PropUtils/tree/master/files/prop_reference
	* **origin_x/y:** A range from 0 to 1. The point (relative to its bounding box) around which the prop is placed. 0,0 would be the top left corner, and 1,1 the bottom right
	* **spacing:** The amount of space on the path between each prop placed.
	* **layer:** The layer/sub_layer the prop is placed on.
	* **end_layer:** If not -1, props will slowly transition along the curve from **layer** to this layer.
	* **frame:** The prop's frame
	* **palette:** The prop's palette
	* **scale_x/y:** The scale of the prop. **NOTE:** This doesn't seem to work. Prop scales aren't remembered.
	* **rotation:** The prop's rotation
	* **scale_sx/y:** The starting scale of the prop. The prop's scale will change from this scale at the start of the path, to **scale_ex/y** at the end of the path.
	* **scale_ex/y:** The ending scale of the prop

##### Controls
* **Left mouse**: Move handles.
* **Middle mouse**: Move handles as if the **smart_handles** option is off.

### Prop Brush (prop-brush/PropBrush.cpp)
An editor script for quickly placing props.

##### Basic usage:
- Add a brush.
- Add a prop to the brush.
- Set the prop values, or press **select_prop** button to open the prop selection window.
- **Right mouse:** Place props.
- **Middle mouse:** Erase props.
- **Left mouse + Mouse wheel:** Change brush size.
- **Left mouse + Right mouse:** Drag horizontally to change the brush size.

##### Properties:
- **draw** - Disables the prop brush. Make sure to turn off when using other tools.
- **preview** - Shows a preview of the first brush.
- **spread_mul** - A global multiplier for brush spread.
- **angle_mul** - A global multiplier for brush angle.
- **smoothing** - Smooths the brush angle. Only relevant when **brush.rotate_to_dir** is set.
- **place_on_tiles** - If checked props snap to the nearest tile's surface.
- **place_on_tiles_distance** - How far to check for tiles when **place_on_tiles** is on.
- **place_on_tiles_layer** - Which layer to snap to when **place_on_tiles** is on. -1 will use the brush's layer.
- **brush**
    - **active** - Can be used to disable individual brushes.
    - **angle_min** - Placed props will have a random rotation between these values (relative to the mouse direction if **brush.rotate_to_dir** is checked)
    - **angle_max** - Same as **angle_min**
    - **angle_step** - If not equal to zero, the angle will be increased by this much after each prop is placed, otherwise a random angle is chosen. (All angles are always between **angle_min** and **angle_max**)
    - **rotate_to_dir** - If checked prop angle will be relative to the direction of the mouse.
    - **spread** - Props will be placed in a random position in a circle with this radius.
    - **density** - The number of props by distance moved, or by seconds if the **spray** option is checked.
    - **uniform** - Place props with a more uniform distribution.
    - **spray** - Props are placed based on distance moved, or continuously over time if **spray** is checked.
    - **layer** - The layer to place the prop on.
    - **sub_layer** - The sub-layer to place the prop on.
    - **flip_x** - Props will be randomly flipped horizontally.
    - **flip_y** - Props will be randomly flipped vertically.
    - **scale_min** - Scale range. Only works on Dustmod maps.
    - **scale_max** - Scale range. Only works on Dustmod maps.
    - **cycle_props** - If checked, instead of a random prop for the list, all props in the list will be cycled through sequentially.
    - **prop** - A single brush can have multiple props. One of these is selected randomly every time a prop is placed. 
        - **prop_set** - The prop set. These values can be set manually, but using the prop selector below is recommended.
        - **prop_group** - The prop group.
        - **prop_index** - The prop index.
        - **prop_palette** - The prop palette.
        - **select_prop** - Press to open the prop selector window. See **Prop Selection Window** section below.
        - **pivot** - Defines where the prop rotates around.
            - **Top, Left, etc.** - The prop rotates around the corresponding corner.
            - **Origin** - A per-prop, predefined pivot meant to be a more logical point for the prop to rotate around.
            - **Custom** - Use the custom values defined below.
        - **pivot_x** - A value in the range **0** - **1**. Only used when **pivot** is set to **Custom**.
        - **pivot_y** - Same as **pivot_x**.
        - **clone** - Click to clone this prop and add it to the end of the list.
    - **cluster_chance** - Each attempt to place a prop will have a chance to be placed in a larger cluster. Set to zero to never cluster.
    - **cluster_min** - The minimum number of props in a cluster. Set this and **cluster_max** to **1** to never cluster.
    - **cluster_max** - The maximum number of props in a cluster.
    - **clone** - Click to clone this brush and add it to the end of the list.

##### Prop Selection Window:
- Click a prop to select it.
- If there are alternate palettes, they are shown below, otherwise the prop is selected, and the window is closed.
- Click the selected prop again to quickly select the first palette, or click one of the palettes to select that.
