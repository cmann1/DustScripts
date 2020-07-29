enum ImageSize
{
	
	/// Do not scale
	None,
	/// Scale to fit the width and height exactly
	Fit,
	/// Scale the image to fit as best as possible while maintaining the aspect ratio
	FitInside,
	/// Shrink the image, while maintaining the aspect ratio, only when it doesn't fit
	ConstrainInside,
	
}