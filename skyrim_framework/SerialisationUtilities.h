#include <unordered_map>
class TESForm;
TESForm* GetFormByOldID(const std::unordered_map<UInt8, UInt8>& loadOrderTable, UInt32 formID);

#ifdef _DEBUG
void printBuffer(const UInt32*buf, const UInt32 length);
#endif