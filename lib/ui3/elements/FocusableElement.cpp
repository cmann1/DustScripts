#include '../../input/Keyboard.cpp';
#include '../../input/navigation/INavigable.cpp';
#include 'Element.cpp';

/// 
abstract class FocusableElement : Element, IKeyboardFocus, INavigable
{
	
	protected bool focused;
	protected NavigationGroup@ _navigation_parent;
	protected NavigateOn _navigate_on = NavigateOn(Inherit | Tab);
	
	FocusableElement(UI@ ui)
	{
		super(ui);
	}
	
	void set_focus()
	{
		@ui.focus = @this;
	}
	
	// ///////////////////////////////////////////////////////////////////
	// INavigable
	// ///////////////////////////////////////////////////////////////////
	
	/// Internal - don't set explicitly.
	NavigationGroup@ navigation_parent
	{
		get { return @_navigation_parent; }
		set { @_navigation_parent = @value; }
	}
	
	NavigateOn navigate_on
	{
		get const { return navigation::get(_navigate_on, _navigation_parent); }
		set { _navigate_on = value; }
	}
	
	INavigable@ previous_navigable(INavigable@ from)
	{
		return @_navigation_parent != null ? _navigation_parent.previous_navigable(@this) : null;
	}
	
	INavigable@ next_navigable(INavigable@ from)
	{
		return @_navigation_parent != null ? _navigation_parent.next_navigable(@this) : null;
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////////////////////////////
	
	void on_focus(Keyboard@ keyboard) override
	{
		focused = true;
	}
	
	void on_blur(Keyboard@ keyboard, const BlurAction type) override
	{
		focused = false;
	}
	
	void on_key_press(Keyboard@ keyboard, const int key, const bool is_gvb, const string text) { }
	
	void on_key(Keyboard@ keyboard, const int key, const bool is_gvb, const string text) { }
	
	void on_key_release(Keyboard@ keyboard, const int key, const bool is_gvb) { }
	
	
}