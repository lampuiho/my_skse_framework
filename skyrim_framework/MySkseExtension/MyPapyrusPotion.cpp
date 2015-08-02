#include "MyPapyrusPotion.h"
#include "skse/PapyrusNativeFunctions.h"

namespace papyrusPotion
{
	bool IsDrink(AlchemyItem* thisPotion)
	{
		return (thisPotion) ? thisPotion->itemData.useSound ? (thisPotion->itemData.flags & thisPotion->kFlag_Food) != 0 && thisPotion->itemData.useSound->formID == 0x000B6435 : false : false;
	}

	bool IsPoison(AlchemyItem* thisPotion)
	{
		return (thisPotion) ? (thisPotion->itemData.flags & thisPotion->kFlag_Poison) != 0 : false;
	}
	/*
	void MyRegisterFuncs(VMClassRegistry* registry){
		registry->RegisterFunction(
			new NativeFunction0 <AlchemyItem, bool>("IsDrink", "Potion", papyrusPotion::IsDrink, registry));

		registry->RegisterFunction(
			new NativeFunction0 <AlchemyItem, bool>("IsPoison", "Potion", papyrusPotion::IsPoison, registry));
	}*/
}