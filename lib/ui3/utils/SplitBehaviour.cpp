/// How the SplitPane divides the space
enum SplitBehaviour
{
	
	/// `position` indicates the size of the left element, and the right element will fill the remaining space.
	FixedLeft,
	/// `position` indicates the size of the right element, and the left element will fill the remaining space.
	FixedRight,
	/// `position` is a value between 0 and 1 indicating the position of the split
	Percentage,
	
}