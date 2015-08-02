#include <unordered_map>
#include <string>

namespace Requiem{
	UInt32 SerialiseArmour(std::unordered_map<UInt8, char*>& loadordertable, UInt32*& buf);
	void DeSerialiseArmour(const std::unordered_map<UInt8, UInt8>& loadOrderTable, const UInt32* buf, const UInt8 length);
	void ClearWornArmour();
	void RegisterPluginEvents();
}