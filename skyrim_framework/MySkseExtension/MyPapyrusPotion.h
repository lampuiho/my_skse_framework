#include <skse/PapyrusPotion.h>
#include <skse/GameObjects.h>

namespace papyrusPotion
{
	bool IsDrink(AlchemyItem* thisPotion);
	bool IsPoison(AlchemyItem* thisPotion);
	void MyRegisterFuncs(VMClassRegistry* registry);
}