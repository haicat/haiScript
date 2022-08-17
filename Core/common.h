#pragma once

#include "include/nlohmann/json.hpp"
using namespace System;
using json = nlohmann::json;

inline String^ jGetKeyS(json j) {
	if (!j.is_string()) {
		return nullptr;
	}
	return gcnew String(j.get<std::string>().c_str());
}

inline bool jGetKeyB(json j) {
	if (!j.is_boolean()) {
		return false;
	}
	return j.get<bool>();
}