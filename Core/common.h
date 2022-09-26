#pragma once

#include "include/nlohmann/json.hpp"
#include "config.h"
using namespace System;
using json = nlohmann::json;

/// <summary>
/// safely grabs a string from a json object
/// </summary>
inline String^ jGetKeyS(json j, String^ nodeName = "", String^ defaultValue = nullptr) {
	if (!j.is_string()) {
		if (defaultValue != nullptr) { return defaultValue; }
		throw gcnew config::JSONParseException(
			"Attempted to get string value from JSON node " + nodeName + " (type " + gcnew String(j.type_name()) + ")"
		);
	}
	return gcnew String(j.get<std::string>().c_str());
}

/// <summary>
/// safely grabs a bool from a json object
/// </summary>
inline bool jGetKeyB(json j, String^ nodeName = "", Boolean^ defaultValue = nullptr) {
	if (!j.is_boolean()) {
		if (defaultValue != nullptr) { return defaultValue?true:false; }
		throw gcnew config::JSONParseException(
			"Attempted to get boolean value from JSON node " + nodeName + " (type " + gcnew String(j.type_name()) + ")"
		);
	}
	return j.get<bool>();
}