#include <unordered_map>
class VMClassRegistry;
class TESObjectARMO;

namespace Requiem{
	namespace papyrusRequiem{
		extern std::unordered_map<UInt8, TESObjectARMO*> equippedArmour; //only a singleton is needed for the player
		bool MyRegisterFuncs(VMClassRegistry* registry);
	}
}