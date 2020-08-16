enum NavigateOn
{
	
	/// Never navigate
	None		= 0,
	/// If set the navigable will inherit any set navigation options from its parent
	Inherit		= 1,
	/// Navigate when tab is pressed
	Tab			= 2,
	/// Navigate when return is pressed
	Return		= 4,
	/// Navigate when ctrl+return is pressed
	CtrlReturn	= 8,
	
}