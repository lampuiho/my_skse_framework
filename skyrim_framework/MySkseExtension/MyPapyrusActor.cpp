#include "MyPapyrusActor.h"
#include "MyPapyrusForm.h"
#include "MyGameApi.h"
#include "skse\GameTypes.h"
#include "skse\PapyrusNativeFunctions.h"
#include<algorithm>

namespace papyrusActor
{

	struct HasEquippedArmourType : WornArmorFinder{
		UInt32 armourClass;
		HasEquippedArmourType(Actor* ref, UInt32 armourClass) : WornArmorFinder(ref), armourClass(armourClass){}
		bool conditionFunc(TESObjectARMO* armo){
			return armo->bipedObject.data.weightClass == armourClass ? true : false;
		}
	};

	struct WornArmorHasKeywordString : WornArmorFinder{
		BSFixedString keywordString;
		WornArmorHasKeywordString(Actor* ref, BSFixedString i_keywordString) : WornArmorFinder(ref)
			, keywordString(i_keywordString) {}
		bool conditionFunc(TESObjectARMO* armo){
			return papyrusForm::HasKeywordString(armo, keywordString);
		}
	};

	struct IsWorn{
		bool Accept(BaseExtraList* xList){
			if (xList && xList->HasType(kExtraData_Worn))
				return true;
			return false;
		}
	};

	struct IsFavourite{
		bool Accept(BaseExtraList* xList){
			if (xList && xList->HasType(kExtraData_Hotkey))
				return true;
			return false;
		}
	};

	struct IsWornOrFavourite : IsWorn, IsFavourite{
		bool Accept(BaseExtraList* xList){
			return IsWorn::Accept(xList) || IsFavourite::Accept(xList); //need optimisation
		}
	};

	ActiveEffect* GetNthEffect(Actor* ref, UInt32 index){
		return ref->magicTarget.GetActiveEffects()->GetNthItem(index);
	}
	void ModAVEX(Actor* ref, UInt32 type, UInt32 index, float mag){
		ref->actorValueOwner.Unk_06(type, index, mag);
	}
	void AddSpellRange(Actor* ref, BGSListForm* toAdd){
		if (ref){
			if (toAdd->forms[0]->formType == kFormType_Spell)
				for (auto i = 0; i< toAdd->forms.count; ++i)
					CALL_MEMBER_FN_EX(ref, AddSpell)(reinterpret_cast<SpellItem*>(toAdd->forms.arr.entries[i]));
		}
	}
	void RemovePerkRange(Actor* ref, BGSListForm* toRemove){
		if (ref){
			if (toRemove->forms[0]->formType == kFormType_Perk)
				for (UInt32 i = 0; i< toRemove->GetSize(); ++i)
					ref->RemovePerk(reinterpret_cast<BGSPerk*>(toRemove->forms.arr.entries[i]));
		}
	}
	void RemoveSpellRange(Actor* ref, BGSListForm* toRemove){
		if (ref){
			if (toRemove->forms[0]->formType == kFormType_Spell){
				for (auto i = 0; i< toRemove->GetSize(); ++i)
					PapyrusRemoveSpell(0, 0, ref, reinterpret_cast<SpellItem*>(toRemove->forms.arr.entries[i]));
			}
		}
	}
	void DispelRange(Actor* ref, BGSListForm* toRemove){
		if (ref){
			if (toRemove->forms[0]->formType == kFormType_Spell)
				for (auto i = 0; i< toRemove->GetSize(); ++i)
					PapyrusDispelSpell(0, 0, ref, reinterpret_cast<SpellItem*>(toRemove->forms.arr.entries[i]));
		}
	}
	void RemoveSpellRange(Actor* ref, VMArray<SpellItem*> toRemove){
		if (ref){
			for (UInt32 i = 0; i< toRemove.Length(); ++i){
				SpellItem* get;
				toRemove.Get(&get, i);
				if (get)
					PapyrusRemoveSpell(0, 0, ref, get);
			}
		}
	}
	void RemovePerkRange(Actor* ref, VMArray<BGSPerk*> toRemove){
		if (ref){
			for (UInt32 i = 0; i< toRemove.Length(); ++i){
				BGSPerk* get;
				toRemove.Get(&get, i);
				if (get)
					ref->RemovePerk(get);
			}
		}
	}
	void AddPerkRange(Actor* ref, VMArray<BGSPerk*> toAdd){
		if (ref){
			for (UInt32 i = 0; i< toAdd.Length(); ++i){
				BGSPerk* get;
				toAdd.Get(&get, i);
				if (get)
					ref->AddPerk(get, 0);
			}
		}
	}
	bool FindWornBaseList(InventoryEntryData* pEntryData){
		ExtendDataList* pExtendList = pEntryData->extendDataList;
		if (pExtendList != nullptr) return pExtendList->Find(IsWorn());
		return false;
	}
	bool FindFavouriteBaseList(InventoryEntryData* pEntryData){
		ExtendDataList* pExtendList = pEntryData->extendDataList;
		if (pExtendList != nullptr)  return pExtendList->Find(IsFavourite());
		return false;
	}
	bool FindWornOrFavouriteBaseList(InventoryEntryData* pEntryData){
		ExtendDataList* pExtendList = pEntryData->extendDataList;
		if (pExtendList != nullptr) return pExtendList->Find(IsWornOrFavourite());
		return false;
	}
	bool WornHasArmorType(Actor* ref, UInt32 armourClass){
		return HasEquippedArmourType(ref, armourClass)();
	}
	bool HasWornArmorWithKeywordString(Actor* ref, BSFixedString keywordString){
		return WornArmorHasKeywordString(ref, keywordString)();
	}
	struct GetEquippedArmor : WornArmorVisiter<void>{
		std::vector<TESObjectARMO*>* equippedItemArray;
		GetEquippedArmor(Actor* thisActor, std::vector<TESObjectARMO*>& equippedItemArray): WornArmorVisiter(thisActor), equippedItemArray(&equippedItemArray){}
		bool conditionFunc(TESObjectARMO* armo)
		{
			equippedItemArray->push_back(reinterpret_cast<TESObjectARMO*>(armo));
			return true;
		}
	};
	struct FillEquippedArmorArray : WornArmorVisiter<void>{
		VMArray<TESObjectARMO*>* equippedItemArray;
		FillEquippedArmorArray(Actor* thisActor, VMArray<TESObjectARMO*>& iequippedItemArray): WornArmorVisiter(thisActor), equippedItemArray(&iequippedItemArray){}
		bool conditionFunc(TESObjectARMO* armo)
		{
			equippedItemArray->Set(&armo, log2(armo->bipedObject.data.parts)-1);
			return true;
		}
	};

	void GetEquippedArmors(Actor* thisActor, std::vector<TESObjectARMO*>& equippedItemArray){
		GetEquippedArmor filler(thisActor, equippedItemArray); filler();
	}
	void GetEquippedArmors(Actor* thisActor, VMArray<TESObjectARMO*> equippedItemArray){
		FillEquippedArmorArray filler(thisActor, equippedItemArray); filler();
	}
	void EquipItems(Actor* thisActor, VMArray<TESForm*> ItemToEquipArray)
	{
		if (thisActor && ItemToEquipArray.arr)
		{
			TESForm* ItemToEquip;
			for (UInt8 i = 0; i< ItemToEquipArray.Length(); ++i){
				ItemToEquipArray.Get(&ItemToEquip, i);
				EquipItemEx(thisActor, ItemToEquip, 0, false, false);
			}
		}
	}
	
	void MyRegisterFuncs(VMClassRegistry* registry){
		registry->RegisterFunction(
			new NativeFunction1<Actor, ActiveEffect*, UInt32>("GetNthEffect", "Actor", papyrusActor::GetNthEffect, registry));
		registry->RegisterFunction(
			new NativeFunction3<Actor, void, UInt32, UInt32, float>("ModAVEX", "Actor", papyrusActor::ModAVEX, registry));
		registry->RegisterFunction(
			new NativeFunction1<Actor, bool, UInt32>("WornHasArmorType", "Actor", papyrusActor::WornHasArmorType, registry));
		registry->RegisterFunction(
			new NativeFunction1<Actor, void, BGSListForm*>("AddSpellRange", "Actor", papyrusActor::AddSpellRange, registry));
		registry->RegisterFunction(
			new NativeFunction1<Actor, void, VMArray<BGSPerk*>>("AddPerkRange", "Actor", papyrusActor::AddPerkRange, registry));
		registry->RegisterFunction(
			new NativeFunction1<Actor, void, VMArray<BGSPerk*>>("RemovePerkRange", "Actor", papyrusActor::RemovePerkRange, registry));
		registry->RegisterFunction(
			new NativeFunction1<Actor, void, BGSListForm*>("RemovePerksFromFormList", "Actor", papyrusActor::RemovePerkRange, registry));
		registry->RegisterFunction(
			new NativeFunction1<Actor, void, BGSListForm*>("DispelRange", "Actor", papyrusActor::DispelRange, registry));
		registry->RegisterFunction(
			new NativeFunction1<Actor, void, VMArray<SpellItem*>>("RemoveSpellRange", "Actor", papyrusActor::RemoveSpellRange, registry));
		registry->RegisterFunction(
			new NativeFunction1<Actor, void, BGSListForm*>("RemoveSpellsFromFormList", "Actor", papyrusActor::RemoveSpellRange, registry));
		registry->RegisterFunction(
			new NativeFunction1<Actor, bool, BSFixedString>("HasWornArmorWithKeywordString", "Actor", papyrusActor::HasWornArmorWithKeywordString, registry));
	}
}