# Lib
A collection of common and useful classes and functions.
I recommend putting this in the `script_src` root so they can be easily included in any map.
At the very least I'll include "lib/std.cpp" in most projects, which contains some of the most common things I use, such as `puts` overloads for various types.

- **drawing** - Drawing related utilities.
- **easing** - A collection of easing functions.
- **enums** - Built-in constants defined as enums, such as entity and var types.
- **math** - Math and geometry related classes and functions. Use `math.cpp` for most commonly needed functionality, eg. `dot`, `distance`, `lerp`.
- **tiles** - Tile related utilities.
- **ui** - UI related code. Contains scrollbars, mouse handling, etc. used by `PropSelector`, currently probably not very general purpose or useful.

# Shared
A collection of stand-alone scripts to add predefined functionality to a map, eg. outlining entities.
Once added to a map and compiled, left click the script name to save it as a preset so that it can easily be added to other maps in the future.
- **Entity outliner** - Draws a configurable outlines around entities
- **Breakable wall** - Allows you to mark rectangular regions of tiles as breakable
- **Prop wind** - Will make certain props sway in the wind
- **Shadows** - Casts shadows from tiles

# Editor Utils

#### General Installation
1. Copy **lib**  and **ed** to your **script_src** directory.
2. Open the script tab in the editor.
3. Click the text box, type **ed/prop-path.cpp** and hit enter

#### Props (pp.cpp)
The main script has a utility for selecting and outputting code that can be used to draw them inside of a script.
- **PropMover** - A trigger that can be used to move props between layers and sub layers

#### Tiles (t.cpp)
- **RemoveTileEdges** - Removes edges from selected tiles

#### Emitters (em.cpp)
- **SetEmitterSubLayer** - allows setting emitter sub layers
- **SetEmitterRotation** - allows setting emitter rotation

## Prop Path (prop-path.cpp)
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

###### Controls
NOTE: These are now supported in-editor in the latest Dustmod.
* **Left mouse**: Move handles.
* **Middle mouse**: Move handles as if the **smart_handles** option is off.
