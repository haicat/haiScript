#pragma once

#include "include/nlohmann/json.hpp"
using namespace System;
using json = nlohmann::json;

/// <summary>
/// safely grabs a string from a json object
/// </summary>
inline String^ jGetKeyS(json j) {
	if (!j.is_string()) {
		return nullptr;
	}
	return gcnew String(j.get<std::string>().c_str());
}

/// <summary>
/// safely grabs a bool from a json object
/// </summary>
inline bool jGetKeyB(json j) {
	if (!j.is_boolean()) {
		return false;
	}
	return j.get<bool>();
}