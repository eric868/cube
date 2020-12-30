/******************************************************************************
Copyright (c) 2016. All Rights Reserved.

FileName: dtrans.cpp
Version: 1.0
Date: 2016.1.13

History:
base     2016.1.13   1.0     Create
******************************************************************************/

#include "dtrans.h"
#include "trace.h"
#include "jsoncpp/include/json/json.h"

namespace base
{

static Json::Value dict2jv(const base::dictionary& dict);
static Json::Value array2jv(const base::darray& dar);

static Json::Value array2jv(const base::darray& dar)
{
	Json::Value root;

	for (size_t index = 0; index < dar.count(); index++) 
	{
		const base::dobject& obj = dar[index];
		switch (obj.value_type()) {
		case base::DVT_BOOL:
			root.append(obj.bool_value());
			break;
		case base::DVT_INTEGER:
			root.append((Json::Value::Int64)obj.integer_value());
			break;
		case base::DVT_DOUBLE:
			root.append(obj.double_value());
			break;
		case base::DVT_STRING:
			root.append(obj.string_value());
			break;
		case base::DVT_ARRAY:
			root.append(array2jv(*obj.array_value()));
			break;
		case base::DVT_DICTIONARY:
			root.append(dict2jv(*obj.dictionary_value()));
			break;
		default:
			break;
		}
	}
	return root;
}

static Json::Value dict2jv(const base::dictionary& dict)
{
	Json::Value root;

	std::vector<std::string> keys = dict.all_keys();
	std::vector<std::string>::iterator it = keys.begin();
	for (; it != keys.end(); it++)
	{
		const base::dobject& obj = dict[*it];
		switch (obj.value_type()) {
		case base::DVT_BOOL:
			root[(*it).c_str()] = obj.bool_value();
			break;
		case base::DVT_INTEGER:
			root[(*it).c_str()] = (Json::Value::Int64)obj.integer_value();
			break;
		case base::DVT_DOUBLE:
			root[(*it).c_str()] = obj.double_value();
			break;
		case base::DVT_STRING:
			root[(*it).c_str()] = obj.string_value();
			break;
		case base::DVT_ARRAY:
			root[(*it).c_str()] = array2jv(*obj.array_value());
			break;
		case base::DVT_DICTIONARY:
			root[(*it).c_str()] = dict2jv(*obj.dictionary_value());
			break;
		default:
			break;
		}
	}
	return root;
}

std::string djson::dict2str(const base::dictionary* dict)
{
	return dict2str(*dict);
}

std::string djson::dict2str(const base::dictionary& dict)
{
	Json::Value root = dict2jv(dict);

	Json::FastWriter writer;
	std::string str = writer.write(root);

	return str;
}

/******************************************************************************/

static void jv2dict(base::dictionary& dict, const Json::Value& jv);
static void jv2dar(base::darray& dar, const Json::Value& jv);

static void jv2dict(base::dictionary& dict, const Json::Value& jv)
{
	assert(jv.isObject());

	Json::Value::Members keys = jv.getMemberNames();
	for (int i = 0; i < (int)keys.size(); i++) 
	{
		const Json::Value& mjv = jv[keys[i]];
		switch(mjv.type()) {
		case Json::nullValue:
			break;
		case Json::intValue:
		case Json::uintValue:
			dict[keys[i]] = (base::dint)mjv.asInt64();
			break;
		case Json::stringValue:
			dict[keys[i]] = mjv.asString();
			break;
		case Json::realValue:
			dict[keys[i]] = mjv.asDouble();
			break;
		case Json::booleanValue:
			dict[keys[i]] = mjv.asBool();
			break;
		case Json::arrayValue:
			{
				base::darray mdar;
				jv2dar(mdar, mjv);
				dict[keys[i]] = mdar;
			}
			break;
		case Json::objectValue:
			{
				base::dictionary mdict;
				jv2dict(mdict, mjv);
				dict[keys[i]] = mdict;
			}
			break;
		default:
			break;
		}
	}
}

static void jv2dar(base::darray& dar, const Json::Value& jv)
{
	assert(jv.isArray());

	for (size_t i = 0; i < jv.size(); i++) 
	{
		const Json::Value& mjv = jv[(int)i];
		switch (mjv.type()) {
		case Json::nullValue:
			break;
		case Json::intValue:
		case Json::uintValue:
			dar.append((base::dint)mjv.asInt64());
			break;
		case Json::stringValue:
			dar.append(mjv.asString());
			break;
		case Json::realValue:
			dar.append(mjv.asDouble());
			break;
		case Json::booleanValue:
			dar.append(mjv.asBool());
			break;
		case Json::arrayValue:
			{
				base::darray mdar;
				jv2dar(mdar, mjv);
				dar.append(mdar);
			}
			break;
		case Json::objectValue:
			{
				base::dictionary mdict;
				jv2dict(mdict, mjv);
				dar.append(mdict);
			}
			break;
		default:
			break;
		}
	}
}

base::dictionary* djson::str2dict(const std::string& json_str)
{
	return str2dict(json_str.c_str());
}

base::dictionary* djson::str2dict(const char* json_str)
{
	Json::Reader reader;
	Json::Value root;
	const char* root_name = "root";

	if (!reader.parse(json_str, root)) {
		TRACE_ERROR("base", 0, "djson: parse json string error, %s", json_str);
		return NULL;
	}

	base::dictionary* dict = new base::dictionary();
	switch (root.type()) 
	{
	case Json::nullValue:
		break;
	case Json::intValue:
	case Json::uintValue:
		(*dict)[root_name] = (base::dint)root.asInt64();
		break;
	case Json::stringValue:
		(*dict)[root_name] = root.asString();
		break;
	case Json::realValue:
		(*dict)[root_name] = root.asDouble();
		break;
	case Json::booleanValue:
		(*dict)[root_name] = root.asBool();
		break;
	case Json::arrayValue:
		{
			base::darray dar;
			jv2dar(dar, root);
			(*dict)[root_name] = dar;
		}
		break;
	case Json::objectValue:
		jv2dict(*dict, root);
		break;
	default:
		break;
	}
	
	return dict;
}

/*********************************************************************/

std::string dxml::dict2str(const base::dictionary* dict)
{
	return dict2str(*dict);
}

std::string dxml::dict2str(const base::dictionary& dict)
{
	return "";
}

base::dictionary* dxml::str2dict(const std::string& xml_str)
{
	return str2dict(xml_str.c_str());
}

base::dictionary* dxml::str2dict(const char* xml_str)
{
	return NULL;
}

}
