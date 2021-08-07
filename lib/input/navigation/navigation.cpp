#include 'INavigable.cpp';

namespace navigation
{
	
	NavigateOn get(const NavigateOn navigate_on, INavigable@ parent)
	{
		if((navigate_on & NavigateOn::Inherit) == 0 || @parent == null)
			return navigate_on;
		
		return NavigateOn(navigate_on | parent.navigate_on);
	}
	
	bool consume(input_api@ input, const NavigateOn navigate_on, const NavigateOn type, const int gvb, const bool consume_gvb)
	{
		if((navigate_on & type) == 0 || !input.key_check_pressed_gvb(gvb))
			return false;
		
		if(consume_gvb)
		{
			input.key_clear_gvb(gvb);
		}
		
		return true;
	}
	
}
