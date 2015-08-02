#include "skse\PapyrusActor.h"
#include "skse\GameExtraData.h"
#include "skse\GameForms.h"
#include "MyPapyrusArgs.h"
#include <vector>

namespace papyrusActor
{
	void AddSpellRange(Actor* ref, BGSListForm* toAdd);
	void DispelRange(Actor* ref, BGSListForm* toRemove);
	void RemoveSpellRange(Actor* ref, VMArray<SpellItem*> toRemove);
	void RemoveSpellRange(Actor* ref, BGSListForm* toRemove);
	void AddPerkRange(Actor* ref, VMArray<BGSPerk*> toAdd);
	void RemovePerkRange(Actor* ref, VMArray<BGSPerk*> toRemove);
	void RemovePerkRange(Actor* ref, BGSListForm* toRemove);
	bool FindWornBaseList(InventoryEntryData* pEntryData);
	bool FindFavouriteBaseList(InventoryEntryData* pEntryData);
	bool FindWornOrFavouriteBaseList(InventoryEntryData* pEntryData);
	bool WornHasArmorType(Actor* ref, UInt32 armourClass);
	bool HasWornArmorWithKeywordString(Actor* ref, BSFixedString keywordString);
	void GetEquippedArmors(Actor* thisActor, std::vector<TESObjectARMO*>& equippedItemArray);
	UInt32 GetHeavyArmorCount(Actor* thisActor);
	//void EquipItems(Actor* thisActor, VMArray<TESForm*> ItemToEquipArray);

	struct WornArmorFinder{
		Actor* ref;
		WornArmorFinder(Actor* i_ref) :ref(i_ref){}
		virtual bool conditionFunc(TESObjectARMO* armo) = 0;
		bool Accept(InventoryEntryData* pEntryData)
		{
			if (pEntryData && pEntryData->type->IsArmor())
			{
				if (FindWornBaseList(pEntryData))
					return conditionFunc(reinterpret_cast<TESObjectARMO*>(pEntryData->type));
			}
			return false;
		}
		bool operator ()(){
			if (ref){
				ExtraContainerChanges* pContainerChanges = static_cast<ExtraContainerChanges*>(ref->extraData.GetByType(kExtraData_ContainerChanges));
				if (pContainerChanges&&pContainerChanges->data&&pContainerChanges->data->objList){
					if (pContainerChanges->data->objList->Find(*this))
						return true;
				}
			}
			return false;
		}
	};

	template<typename Result>
	struct WornArmorVisiter : WornArmorFinder{
		Result result;
		WornArmorVisiter(Actor* ref) :WornArmorFinder(ref), result(0){}
		bool Accept(InventoryEntryData* pEntryData)
		{
			if (pEntryData && pEntryData->type->IsArmor())
			{
				if (FindWornBaseList(pEntryData)){
					return conditionFunc(reinterpret_cast<TESObjectARMO*>(pEntryData->type));
				}
			}
			return true;
		}
		Result operator()(){
			if (ref){
				ExtraContainerChanges* pContainerChanges = static_cast<ExtraContainerChanges*>(ref->extraData.GetByType(kExtraData_ContainerChanges));
				if (pContainerChanges&&pContainerChanges->data&&pContainerChanges->data->objList){
					pContainerChanges->data->objList->Visit(*this);
					return result;
				}
			}
			return static_cast<Result>(0);
		}
	};

	template<> struct WornArmorVisiter<void> : WornArmorFinder{
		WornArmorVisiter(Actor* ref) :WornArmorFinder(ref){}
		bool Accept(InventoryEntryData* pEntryData)
		{
			if (pEntryData && pEntryData->type->IsArmor())
			{
				if (FindWornBaseList(pEntryData)){
					return conditionFunc(reinterpret_cast<TESObjectARMO*>(pEntryData->type));
				}
			}
			return true;
		}
		void operator()(){
			if (ref){
				ExtraContainerChanges* pContainerChanges = static_cast<ExtraContainerChanges*>(ref->extraData.GetByType(kExtraData_ContainerChanges));
				if (pContainerChanges&&pContainerChanges->data&&pContainerChanges->data->objList){
					pContainerChanges->data->objList->Visit(*this);
				}
			}
		}
	};

	void MyRegisterFuncs(VMClassRegistry* registry);
}