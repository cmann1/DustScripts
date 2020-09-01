#include '../LockedContainer.cpp';

namespace PropSelector { const string TYPE_NAME = 'PropSelector'; }

class PropSelector : LockedContainer
{
	
	PropSelector(UI@ ui)
	{
		super(ui);
	}
	
	string element_type { get const override { return PropSelector::TYPE_NAME; } }
	
}