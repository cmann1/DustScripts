class ShortcutKeySorter
{
	
	Tool@ tool;
	int index;
	
	ShortcutKeySorter()
	{
		
	}
	
	/// Sort by priority then index
	int opCmp(const ShortcutKeySorter &in other)
	{
		if(tool.shortcut_key_priority != other.tool.shortcut_key_priority)
			return tool.shortcut_key_priority - other.tool.shortcut_key_priority;
		
		return index - other.index;
	}
	
}
