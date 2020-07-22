#include "../enums/VarType.cpp";

void copy_vars(entity@ src, entity@ dst)
{
	varstruct@ src_vars = src.vars();
	varstruct@ dst_vars = dst.vars();
	copy_vars(src_vars, dst_vars);
}

void copy_vars(varstruct@ src_vars, varstruct@ dst_vars)
{
	for(uint i = 0; i < src_vars.num_vars(); i++)
	{
		const string name = src_vars.var_name(i);
		varvalue@ src_var = src_vars.get_var(i);
		varvalue@ dst_var = dst_vars.get_var(name);
		copy_var(src_var, dst_var);
	}
}

void copy_var(varvalue@ src_var, varvalue@ dst_var)
{
	const int type_id = src_var.type_id();
	
	if(type_id == VarType::Bool)
	{
		dst_var.set_bool(src_var.get_bool());
	}
	else if(type_id == VarType::Int8)
	{
		dst_var.set_int8(src_var.get_int8());
	}
	else if(type_id == VarType::Int16)
	{
		dst_var.set_int16(src_var.get_int16());
	}
	else if(type_id == VarType::Int32)
	{
		dst_var.set_int32(src_var.get_int32());
	}
	else if(type_id == VarType::Int64)
	{
		dst_var.set_int64(src_var.get_int64());
	}
	else if(type_id == VarType::Float)
	{
		dst_var.set_float(src_var.get_float());
	}
	else if(type_id == VarType::String)
	{
		dst_var.set_string(src_var.get_string());
	}
	else if(type_id == VarType::Vec2)
	{
		dst_var.set_vec2(src_var.get_vec2_x(), src_var.get_vec2_y());
	}
	else if(type_id == VarType::Array)
	{
		vararray@ src_var_arr = src_var.get_array();
		vararray@ dst_var_arr = dst_var.get_array();
		
		int size = src_var_arr.size();
		dst_var_arr.resize(size);
		
		for(int i = 0; i < size; i++)
		{
			copy_var(src_var_arr.at(i), dst_var_arr.at(i));
		}
	}
	else if(type_id == VarType::Struct)
	{
		varstruct@ src_struct = src_var.get_struct();
		varstruct@ dst_struct = dst_var.get_struct();
		copy_vars(src_struct, dst_struct);
	}
}