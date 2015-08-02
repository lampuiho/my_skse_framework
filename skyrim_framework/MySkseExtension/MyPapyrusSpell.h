#include "skse/PapyrusSpell.h"
#include "skse/GameObjects.h"
#include "skse/PapyrusNativeFunctions.h"

namespace papyrusSpell
{
	UInt32 GetCastType(SpellItem* thisSpell){ return thisSpell ? thisSpell->data.unk10 : 0; }

	void MyRegisterFuncs(VMClassRegistry* registry){
		registry->RegisterFunction(
			new NativeFunction0 <SpellItem, UInt32>("GetCastType", "Spell", papyrusSpell::GetCastType, registry));
	}
}