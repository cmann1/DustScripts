#include '../elements/Container.cpp';
#include 'LayoutContext.cpp';

interface ILayoutParentHandler
{
	
	void do_child_layout(LayoutContext@ ctx, Container@ container);
	
}