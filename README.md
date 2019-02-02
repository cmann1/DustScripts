# Editor Utils

#### General Installation
1. Copy **lib**  and **ed** to your **script_src** directory.
2. Open the script tab in the editor.
3. Click the text box, type **ed/prop-path.cpp** and hit enter

#### Prop Path
* **place:** Use this "button" to palce the props in the map before deleting the trigger.
* **hide_props:** Don't render props for testing.
* **hide_overlays:** Don't render the curves and handles. Useful for checking prop placement without other stuff in the way.
* **smart_handles:** If checked control points on this and adjacent curves will be kept parallel and will be moved along with vertices.
* **curves:** The path is made up of one or more bezier curves. Use the +/- buttons to and and remove segments.
	* Points 1 and 4 are start and end points
	* Points 1 and 2 control the shape of the curve
* **prop_def:** The prop's properties.
	* **prop_set/group/index:** Which prop to use. See https://github.com/cmann1/PropUtils/tree/master/files/prop_reference
	* **origin_x/y:** A range from 0 to 1. The point (relative to its bounding box) around which the prop is placed. 0,0 would be the top left corner, and 1,1 the bottom right
	* **spacing:** The amount of space on the path between each prop placed.
	* **layer:** The layer/sub_layer the prop is placed on.
	* **frame:** The prop's frame
	* **palette:** The prop's palette
	* **scale_x/y:** The scale of the prop. **NOTE:** This doesn't seem to work. Prop scales aren't remembered.
	* **rotation:** The prop's rotation
	* **scale_sx/y:** The starting scale of the prop. The prop's scale will change from this scale at the start of the path, to scale_ex/y at the end of the path.
	* **scale_ex/y:** The ending scale of the prop

##### Controls
* **Left mouse**: Move handles.
* **Middle mouse**: Move handles as if the **smart_handles** option is off.
** Right now these only work in game, and not in the editor.
