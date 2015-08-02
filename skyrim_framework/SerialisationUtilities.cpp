#include "skyrim_framework\MySkseExtension\Utilities.h"
#include "SerialisationUtilities.h"

typedef TESForm * (*_LookupFormByID)(UInt32 id);
static const _LookupFormByID LookupFormByID = (_LookupFormByID)0x00451A30;

TESForm* GetFormByOldID(const std::unordered_map<UInt8, UInt8>& loadOrderTable, UInt32 formID){
	UInt8 loadOrder = formID >> 24;
	if (loadOrder > 0){
		auto it = loadOrderTable.find(loadOrder);
		if (it != loadOrderTable.end())
		{
			UInt32 newLoadOrderID = static_cast<UInt32>(it->second);
			UInt32 newFormID = ((newLoadOrderID << 24) | 0x00FFFFFF) & formID;
			return LookupFormByID(formID);
		}
	}
	else
		return LookupFormByID(formID);
	return NULL;
}
#ifdef _DEBUG
void printBuffer(const UInt32*buf, const UInt32 length){
	std::cout << std::dec << "Size of buf: " << sizeof(*buf) * length;
	if (length > 0){
		std::cout << "\nArmour buff: ";
		UInt32 i = 0;
		while (i < length)
			std::cout << std::hex << buf[i++] << "-";
	}
	std::cout << std::endl;
}
#endif