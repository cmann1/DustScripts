#define uint unsigned int
#define int64 long int
#define uint64 unsigned long int
#define delete remove

template <class T>
class array
{
	
	uint length;
	uint size;

	array(const uint size);

	// Returns the length of the array.
	uint length() const;
	uint size() const;

	// Sets the new length of the array.
	void resize(uint);

	void reverse();
	// Reverses the order of the elements in the array.

	// Inserts a new element, or another array of elements, into the array at the specified index.
	void insertAt(uint index, const T value);
	void insertAt(uint index, const array<T>& arr);

	// Appends an element at the end of the array.
	void insertLast(const T& in);

	// Removes the element at the specified index.
	void removeAt(uint index);

	// Removes the last element of the array.
	void removeLast();

	// Removes count elements starting from start.
	void removeRange(uint start, uint count);

	// Sorts the elements in the array in ascending order. For object types, this will use the type's opCmp method.
	// The second variant will sort only the elements starting at index startAt and the following count elements.
	void sortAsc();
	void sortAsc(uint startAt, uint count);

	// These does the same thing as sortAsc except sorts the elements in descending order.
	void sortDesc();
	void sortDesc(uint startAt, uint count);

	// This method takes as input a callback function to use for comparing two elements when sorting the array.
	// The callback function should take as parameters two references of the same type of the array elements and it should return a bool. The return value should be true if the first argument should be placed before the second argument.
	void sort(const void* compareFunc, uint startAt=0, uint count);

	// These will return the index of the first element that has the same value as the wanted value.
	// For object types, this will use the type's opEquals or opCmp method to compare the value. For arrays of handles any null handle will be skipped.
	// If no match is found the methods will return a negative value.
	int find(const T in);
	int find(uint startAt, const T in);

	int findByRef(const T in);
	int findByRef(uint startAt, const T in);
  
  T& operator[] (int i);
	
};

class dictionary
{
	
	// Sets a key/value pair in the dictionary. If the key already exists, the value will be changed.
	void set(const string key, void* value);
	void set(const string key, int64 value);
	void set(const string key, double value);

	// Retrieves the value corresponding to the key. The methods return false if the key is not found, and in this case the value will maintain its default value based on the type.
	bool get(const string key, void* value) const;
	bool get(const string key, int64 value) const;
	bool get(const string key, double value) const;

	// This method returns an array with all of the existing keys in the dictionary. The order of the keys in the array is undefined.
	array<string> @getKeys() const;

	// Returns true if the key exists in the dictionary.
	bool exists(const string key) const;

	// Removes the key and the corresponding value from the dictionary. Returns false if the key wasn't found.
	bool delete(const string key);

	// Removes all entries in the dictionary.
	void deleteAll();

	// Returns true if the dictionary doesn't hold any entries.
	bool isEmpty() const;

	uint getSize() const;

};

class string
{
	
	uint length;
	
	// Returns the length of the string.
	uint length() const;

	// Sets the length of the string.
	void resize(uint);

	// Returns true if the string is empty, i.e. the length is zero.
	bool isEmpty() const;

	// Returns a string with the content starting at start and the number of bytes given by count. The default arguments will return the whole string as the new string.
	string substr(uint start = 0, int count = -1) const;

	// Inserts another string other at position pos in the original string.
	void insert(uint pos, const string other);

	// Erases a range of characters from the string, starting at position pos and counting count characters.
	void erase(uint pos, int count = -1);

	// Find the first occurrence of the value str in the string, starting at start. If no occurrence is found a negative value will be returned.
	int findFirst(const string str, uint start = 0) const;

	// Find the last occurrence of the value str in the string. If start is informed the search will begin at that position, i.e. any potential occurrence after that position will not be searched. If no occurrence is found a negative value will be returned.
	int findLast(const string str, int start = -1) const;

	// The first variant finds the first character in the string that matches on of the characters in chars, starting at start. If no occurrence is found a negative value will be returned.
	// The second variant finds the first character that doesn't match any of those in chars. The third and last variant are the same except they start the search from the end of the string.
	// These functions work on the individual bytes in the strings. They do not attempt to understand encoded characters, e.g. UTF-8 encoded characters that can take up to 4 bytes.
	int findFirstOf(const string chars, int start = 0) const;
	int findFirstNotOf(const string chars, int start = 0) const;
	int findLastOf(const string chars, int start = -1) const;
	int findLastNotOf(const string chars, int start = -1) const;
	
	// Splits the string in smaller strings where the delimiter is found
	array<string>@ split(const string delimiter) const;
	
};

// String functions
//{
	
	// Concatenates the strings in the array into a large string, separated by the delimiter.
	string join(const array<string> arr, const string delimiter);

	// Parses the string for an integer value. The base can be 10 or 16 to support decimal numbers or hexadecimal numbers. If byteCount is provided it will be set to the number of bytes that were considered as part of the integer value.
	int64 parseInt(const string str, uint base = 10, uint byteCount = 0);
	uint64 parseUInt(const string str, uint base = 10, uint byteCount = 0);

	// Parses the string for a floating point value. If byteCount is provided it will be set to the number of bytes that were considered as part of the value.
	double parseFloat(const string in, uint byteCount = 0);

	/**
	 * The format functions takes a string that defines how the number should be formatted. The string is a combination of the following characters:
	 * 		l = left justify
	 * 		0 = pad with zeroes
	 * 		+ = always include the sign, even if positive
	 * 		space = add a space in case of positive number
	 * 		h = hexadecimal integer small letters (not valid for formatFloat)
	 * 		H = hexadecimal integer capital letters (not valid for formatFloat)
	 * 		e = exponent character with small e (only valid for formatFloat)
	 * 		E = exponent character with capital E (only valid for formatFloat)
	 */
	string formatInt(int64 val, const string options, uint width = 0);
	string formatUInt(uint64 val, const string options, uint width = 0);
	string formatFloat(double val, const string options, uint width = 0, uint precision = 0);

//}

// Math functions
//{
	
	// Trigonometric functions
	float cos(float rad);
	float sin(float rad);
	float tan(float rad);
	
	// Inverse trigonometric functions
	float acos(float val);
	float asin(float val);
	float atan(float val);
	float atan2(float y, float x);
	
	// Hyperbolic functions
	float cosh(float rad);
	float sinh(float rad);
	float tanh(float rad);
	
	// Logarithmic functions
	float log(float val);
	float log10(float val);
	
	// Power to
	float pow(float val, float exp);
	
	// Square root
	float sqrt(float val);
	
	// Absolute value
	float abs(float val);
	
	// Ceil and floor functions
	float ceil(float val);
	float floor(float val);
	
	// Returns the fraction
	float fraction(float val);
	
	// Approximate float comparison, to deal with numeric imprecision
	bool closeTo(float a, float b, float epsilon = 0.00001f);
	bool closeTo(double a, double b, double epsilon = 0.0000000001);
	
	// Conversion between floating point and IEEE 754 representations
	float  fpFromIEEE(uint raw); 
	double fpFromIEEE(uint64 raw);
	uint   fpToIEEE(float fp);
	uint64 fpToIEEE(double fp);
	
//}
