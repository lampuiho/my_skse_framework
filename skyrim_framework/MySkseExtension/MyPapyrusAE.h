#pragma once
#include <unordered_map>
class VMClassRegistry;
class ActiveEffect;

namespace papyrusActiveMagicEffect{
	UInt32 SerialiseRegisteredActiveEffects(std::unordered_map<UInt8, char*>& loadordertable, UInt32*&buf);
	void DeserialiseRegisteredActiveEffects(const std::unordered_map<UInt8, UInt8>& loadordertable, const UInt32*buf, const UInt32 length);
	extern std::unordered_map<UInt32, ActiveEffect*> activeEffects;
	void MyRegisterFuncs(VMClassRegistry* registry);
}