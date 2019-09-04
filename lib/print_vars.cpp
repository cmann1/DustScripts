#include "enums/VarType.cpp";

void print_vars(entity@ e)
{
	varstruct@ vars = e.vars();
	puts(e.type_name() + "[" + vars.num_vars() + "]");
	
	for(uint i = 0; i < vars.num_vars(); i++)
	{
//		string name = vars.var_name(i);
//		varvalue@ value = vars.get_var(i);
		puts( print_var_value(vars.var_name(i), vars.get_var(i), "   ") );
	}
}

string print_var_value(string name, varvalue@ value, string indent="", int array_max=25, bool print_type=true)
{
	const int id = value.type_id();
	
	string value_str = "";
	
	if(id == VarType::Bool)
	{
		value_str = "" + value.get_bool();
	}
	else if(id == VarType::Int8)
	{
		value_str = "" + value.get_int8();
	}
	else if(id == VarType::Int16)
	{
		value_str = "" + value.get_int16();
	}
	else if(id == VarType::Int32)
	{
		value_str = "" + value.get_int32();
	}
	else if(id == VarType::Int64)
	{
		value_str = "" + value.get_int64();
	}
	else if(id == VarType::Float)
	{
		value_str = "" + value.get_float();
	}
	else if(id == VarType::String)
	{
		value_str = "\"" + value.get_string() +"\"";
	}
	else if(id == VarType::Vec2)
	{
		value_str = "<" + value.get_vec2_x() + ", " + value.get_vec2_y() + ">";
	}
	else if(id == VarType::Array)
	{
		vararray@ arr_value = value.get_array();
		int size = arr_value.size();
		value_str = var_type_string(arr_value.element_type_id()) + "[" + size + "]";
		
		for(int i = 0; i < size; i++)
		{
			varvalue@ item = arr_value.at(i);
			value_str += "\n" + print_var_value(i + "", item, indent + "   ", array_max, false);
			
			if(array_max > 0 and i > array_max and  i + 1 < size)
			{
				value_str += "\n   " + indent + "... " + (size - array_max) + " more";
				break;
			}
		}
	}
	else if(id == VarType::Struct)
	{
//		value_str = "<" + value.get_vec2_x() + ", " + value.get_vec2_y() + ">";
	}
	
	return indent + name + (print_type ? "[" + var_type_string(id) + "]" : "") + " = " + value_str;
}

string var_type_string(const int id)
{
	if(id == VarType::None)
		return "None";
	if(id == VarType::Bool)
		return "Bool";
	if(id == VarType::Int8)
		return "Int8";
	if(id == VarType::Int16)
		return "Int16";
	if(id == VarType::Int32)
		return "Int32";
	if(id == VarType::Int64)
		return "Int64";
	if(id == VarType::Float)
		return "Float";
	if(id == VarType::String)
		return "String";
	if(id == VarType::Array)
		return "Array";
	if(id == VarType::Struct)
		return "Struct";
	if(id == VarType::Vec2)
		return "Vec2";
	
	return "Unknown";
}