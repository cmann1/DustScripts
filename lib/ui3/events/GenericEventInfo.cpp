#include '../elements/Element.cpp';
#include 'IGenericEventTarget.cpp';
#include 'EventType.cpp';

class GenericEventInfo
{
	
	string type;
	IGenericEventTarget@ target;
	Element@ target_element;
	
	void reset(const string type, IGenericEventTarget@ target=null, Element@ target_element=null)
	{
		this.type = type;
		@this.target = target;
		@this.target_element = target_element;
	}
	
}