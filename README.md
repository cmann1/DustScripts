# [Lib]
A collection of common and useful classes and functions.
I recommend putting this in the `script_src` root so they can be easily included in any map.
At the very least I'll include "lib/std.cpp" in most projects, which contains some of the most common things I use, such as `puts` overloads for various types.

- **drawing** - Drawing related utilities.
- **easing** - A collection of easing functions.
- **enums** - Built-in constants defined as enums, such as entity and var types.
- **math** - Math and geometry related classes and functions. Use `math.cpp` for most commonly needed functionality, eg. `dot`, `distance`, `lerp`.
- **tiles** - Tile related utilities.
- **ui** - UI related code. Contains scrollbars, mouse handling, etc. used by `PropSelector`, currently probably not very general purpose or useful.

# [Shared]
A collection of stand-alone scripts to add predefined functionality to a map, eg. outlining entities.
Once added to a map and compiled, left click the script name to save it as a preset so that it can easily be added to other maps in the future.
- **Entity outliner** - Draws a configurable outlines around entities
- **Breakable wall** - Allows you to mark rectangular regions of tiles as breakable
- **Prop wind** - Will make certain props sway in the wind
- **Shadows** - Casts shadows from tiles

# [Editor Utils]

### General Installation
1. Copy **lib**  and **ed** to your **script_src** directory.
2. Open the script tab in the editor.
3. Click the text box, type **ed/prop-path.cpp** and hit enter

### Props (pp.cpp)
The main script has a utility for selecting and outputting code that can be used to draw them inside of a script.
- **PropMover** - A trigger that can be used to move props between layers and sub layers

### Tiles (t.cpp)
- **RemoveTileEdges** - Removes edges from selected tiles

### Emitters (em.cpp)
- **SetEmitterSubLayer** - allows setting emitter sub layers
- **SetEmitterRotation** - allows setting emitter rotation

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
NOTE: These are now supported in-editor in the latest Dustmod.
* **Left mouse**: Move handles.
* **Middle mouse**: Move handles as if the **smart_handles** option is off.

### Prop Brush (prop-brush.cpp)
An editor script for quickly placing props.

##### Basic usage:
- Add a brush.
- Add a prop to the brush.
- Set the prop values, or press **select_prop** button to open the prop selection window.
- Right click and drag to place props.
- Middle click to erase props.

##### Properties:
- **draw** - Disables the prop brush. Make sure to turn off when using other tools.
- **preview** - Shows a preview of the first brush.
- **spread_mul** - A global multiplier for brush spread.
- **angle_mul** - A global multiplier for brush angle.
- **smoothing** - Smooths the brush angle. Only relevant when **brush.rotate_to_dir** is set.
- **brush**
    - **brush.active** - Can be used to disable individual brushes.
    - **brush.angle_min** - Placed props will have a random rotation between these values (relative to the mouse direction if **brush.rotate_to_dir** is checked)
    - **brush.angle_max** - Same as **angle_min**
    - **brush.angle_step** - If larger than zero, the angle will be increased by this much after each prop is placed, otherwise a random angle is chosen.
    - **brush.rotate_to_dir** - If checked prop angle will be relative to the direction of the mouse.
    - **brush.spread** - Props will be placed in a random position in a circle with this radius.
    - **brush.density** - The number of props by distance moved, or by seconds if the **spray** option is checked.
    - **brush.uniform** - Place props with a more uniform distribution.
    - **brush.spray** - Props are placed based on distance moved, or continuously over time if **spray** is checked.
    - **brush.layer** - The layer to place the prop on.
    - **brush.sub_layer** - The sub-layer to place the prop on.
    - **brush.prop** - A single brush can have multiple props. One of these is selected randomly everytime a prop is placed. 
        - **brush.prop.prop_set** - The prop set. These valeus can be set manually, but using the prop selector below is recommended.
        - **brush.prop.prop_group** - The prop group.
        - **brush.prop.prop_index** - The prop index.
        - **brush.prop.prop_palette** - The prop palette.
        - **brush.prop.select_prop** - Press to open the prop selector window. See **Prop Selection Window** section below.
        - **brush.prop.pivot** - Defines where the prop rotates around.
            - **Top, Left, etc.** - The prop rotates around the corresponding corner.
            - **Origin** - A per-prop, predefined pivot meant to be a more logical point for the prop to rotate around.
            - **Custom** - Use the custom values defined below.
        - **brush.prop.pivot_x** - A value in the range **0** - **1**. Only used when **pivot** is set to **Custom**.
        - **brush.prop.pivot_y** - Same as **pivot_x**.
    - **brush.cluster_chance** - Each attempt to place a prop will have a chance to be placed in a larger cluster. Set to zero to never cluster.
    - **brush.cluster_min** - The minimum number of props in a cluster. Set this and **cluster_max** to **1** to never cluster.
    - **brush.cluster_max** - The maximum number of props in a cluster.

##### Prop Selection Window:
- Click a prop to select it.
- If there are alternate palettes, they are shown below, otherwise the prop is selected and the window closed.
- Click the selected prop again to quickly select the first palette, or click one of the palettes to select that.
- **IMPORTANT:** Mouse hud values are not reported properly by Dustmod, if the mouse is not aligning with the hud correctly, move it to the edge of the Dustmod window to "calibrate" it.
- **IMPORTANT:** There's an issue with Dustforce that causes rendering glitches when there are too many textures on screen. Some prop icons might display incorrectly because of this - when that happens hold the middle mouse button to show only that icon. 
