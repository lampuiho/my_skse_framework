#include "LevelersTower.h"
#include "skse\GameData.h"
#include "skse\GameRTTI.h"
#include "skse\GameForms.h"
#include "skse\GameObjects.h"
#include "LevelersTowerConst.h"
#include "skyrim_framework\common_const.h"
#include "skyrim_framework\MySkseExtension\MyPapyrusForm.h"
#include "skyrim_framework\MySkseExtension\MyPapyrusPotion.h"
#include "skyrim_framework\MySkseExtension\MyPapyrusActor.h"
#include "skyrim_framework\MySkseExtension\Utilities.h"
#include "skyrim_framework\MySkseExtension\MyGameApi.h"

namespace LevelersTower{
#define haskey(key) papyrusForm::KeywordFinder(keywordForm, papyrusForm::HasKeywordStringFunc(keywords[key]))

	bool MyRegisterFuncs(VMClassRegistry* registry){
		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, bool, TESObjectREFR*>("SortIngred", "LevelersTower", LevelersTower::SortIngred, registry));
		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, bool, TESObjectREFR*>("SortAllItems", "LevelersTower", LevelersTower::SortAllItems, registry));
		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, void, TESObjectREFR*>("TakeAllIngred", "LevelersTower", LevelersTower::TakeAllIngred, registry));
		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, void, TESObjectREFR*>("TakeEverything", "LevelersTower", LevelersTower::TakeEverything, registry));
		return true;
	}

	SInt32 GetWeaponContainer(TESObjectWEAP* akBaseItem){
		if (papyrusForm::IsFromMod(akBaseItem, dawnguard))
			return LevelersContDLC1WeaponREF;
		else if (papyrusForm::IsFromMod(akBaseItem, dragonborn))
			return LevelersContDLC2WeaponREF;
		else
		{
			if (akBaseItem->formID == woodChoppingAxe || akBaseItem->formID == pickaxe)
				return LevelersContMConsumeREF;

			if (akBaseItem->fullName.name.data)
				if (strstr(akBaseItem->fullName.name.data, forsworn))
					return LevelersContWpnForswornREF;

			BGSKeywordForm* keywordForm = &akBaseItem->keyword;
			if (keywordForm->numKeywords > 0)
				if (haskey(weaptypestaff) || akBaseItem->keyword.HasKeyword((BGSKeyword*)(void*)Common::FormContainer::Forms[Common::FormContainer::WeapMaterialWood]))
					return LevelersContWpnStavesREF;
				else if (haskey(weapmaterialimperial) || akBaseItem->keyword.HasKeyword((BGSKeyword*)(void*)Common::FormContainer::Forms[Common::FormContainer::WeapMaterialIron]))
					return LevelersContWpnIronREF;
				else if (akBaseItem->keyword.HasKeyword((BGSKeyword*)(void*)Common::FormContainer::Forms[Common::FormContainer::WeapMaterialSteel]))
					return LevelersContWpnSteelREF;
				else if (haskey(weapmaterialdraugr) || haskey(weapmaterialdraugrhoned))
					return LevelersContWpnDraugrREF;
				else if (akBaseItem->keyword.HasKeyword((BGSKeyword*)(void*)Common::FormContainer::Forms[Common::FormContainer::WeapMaterialOrcish]))
					return LevelersContWpnOrcishREF;
				else if (akBaseItem->keyword.HasKeyword((BGSKeyword*)(void*)Common::FormContainer::Forms[Common::FormContainer::WeapMaterialElven]))
					return LevelersContWpnElvenREF;
				else if (akBaseItem->keyword.HasKeyword((BGSKeyword*)(void*)Common::FormContainer::Forms[Common::FormContainer::WeapMaterialDwarven]))
					return LevelersContWpnDwarvenREF;
				else if (akBaseItem->keyword.HasKeyword((BGSKeyword*)(void*)Common::FormContainer::Forms[Common::FormContainer::WeapMaterialGlass]))
					return LevelersContWpnGlassREF;
				else if (haskey(weapmaterialfalmer) || haskey(weapmaterialfalmerhoned))
					return LevelersContWpnFalmerREF;
				else if (haskey(weapmaterialsilver))
					return LevelersContWpnSilverREF;
				else if (akBaseItem->keyword.HasKeyword((BGSKeyword*)(void*)Common::FormContainer::Forms[Common::FormContainer::WeapMaterialEbony]))
					return LevelersContWpnEbonyREF;
				else if (akBaseItem->keyword.HasKeyword((BGSKeyword*)(void*)Common::FormContainer::Forms[Common::FormContainer::WeapMaterialDaedric]))
					return LevelersContWpnDaedricREF;
			
			return LevelersContWpnUniqueREF;
		}
	}

	SInt32 GetArmorContainer(TESObjectARMO* akBaseItem){
		if (papyrusForm::IsFromMod(akBaseItem, dawnguard))
			return LevelersContDLC1ArmorREF;
		else if (papyrusForm::IsFromMod(akBaseItem, dragonborn))
			return LevelersContDLC2ArmorREF;
		else if (papyrusForm::IsFromMod(akBaseItem, hearthfires))
			return LevelersContBYOHArmorREF;
		else{
			if (akBaseItem->fullName.name.data)
				if (strstr(akBaseItem->fullName.name.data, guard) || strstr(akBaseItem->fullName.name.data, imperial))
					return LevelersContArmCityGuardREF;

			BGSKeywordForm* keywordForm = &akBaseItem->keyword;
			if (keywordForm->numKeywords > 0)
				if (haskey(armormaterialstormcloak))
					return LevelersContArmCityGuardREF;
				else if (haskey(clothingring))
					return LevelersStrongBoxRingREF;
				else if (haskey(clothingnecklace))
					return LevelersStrongBoxNecklaceREF;
				else if (haskey(clothingcirclet))
					return LevelersStrongBoxCircletREF;
				else if (haskey(armorclothing))
					return LevelersContMClothREF;
				else if (haskey(vendoritemanimalhide) || haskey(armormaterialstudded) || haskey(armormaterialscaled) || haskey(armormaterialhide) || haskey(armormaterialleather) || haskey(vendoritemclothing) || haskey(armormaterialforsworn))
					return LevelersContArmSkinsREF;
				else if (haskey(armormaterialsteel) || haskey(dlc1armormaterielfalmerheavyoriginal) || haskey(armormaterialiron) || haskey(armormaterialironbanded) || haskey(armormaterialsteelplate) || haskey(armormaterialblades))
					return LevelersContArmSteelREF;
				else if (haskey(armormaterialorcish))
					return LevelersContArmOrcishREF;
				else if (haskey(armormaterialelven) || haskey(armormaterialelvengilded))
					return LevelersContArmElvenREF;
				else if (haskey(armormaterialglass))
					return LevelersContArmGlassREF;
				else if (haskey(armormaterialdwarven))
					return LevelersContArmDwarvenREF;
				else if (haskey(armormaterialebony))
					return LevelersContArmEbonyREF;
				else if (haskey(armormaterialdragonplate) || haskey(armormaterialdragonscale))
					return LevelersContArmDragonREF;
				else if (haskey(armormaterialdaedric))
					return LevelersContArmDaedricREF;

				return LevelersContArmUniqueREF;
		}
	}

	SInt32 GetBookContainer(TESObjectBOOK* akBaseItem){
		if (papyrusForm::IsFromMod(akBaseItem, dawnguard))
			return LevelersContDLC1BookREF;
		else if (papyrusForm::IsFromMod(akBaseItem, dragonborn))
			return LevelersContDLC2BookREF;
		else {
#ifdef _DEBUG
			std::cout << "Sorting Book: " << std::hex << akBaseItem->formID << " Flags: " << static_cast<UInt32>(akBaseItem->data.flags)
				<< " Teaches: " << akBaseItem->data.teaches.skill << std::endl;
#endif
			if ((akBaseItem->data.flags & TESObjectBOOK::Data::kType_Spell) != 0 || akBaseItem->data.teaches.skill > 5 && akBaseItem->data.teaches.skill < 24)
				return LevelersContMBookTomeREF;
			else if (akBaseItem->data.type == 0xFF || papyrusForm::HasKeywordString(akBaseItem, keywords[VendorItemRecipe]))
				return LevelersContMNotesREF;
			return LevelersContMBooksREF;
		}
	}

	SInt32 GetMiscContainer(TESObjectMISC* akBaseItem){
		if (akBaseItem->formID == Lockpick || akBaseItem->formID == Torch01)
			return LevelersContMConsumeREF;
		else if (papyrusForm::IsFromMod(akBaseItem, dawnguard))
			return LevelersContDLC1MiscREF;
		else if (papyrusForm::IsFromMod(akBaseItem, dragonborn))
			return LevelersContDLC2MiscREF;
		else if (papyrusForm::IsFromMod(akBaseItem, hearthfires))
			return LevelersContBYOHMiscREF;
		else if (akBaseItem->formID == Gold001)
			return LevelersStrongBoxGoldREF;
		else {
			if (akBaseItem->fullName.name.data)
				if (strstr(akBaseItem->fullName.name.data, dwemer))
					return LevelersContMDwarvenREF;

			BGSKeywordForm* keywordForm = &akBaseItem->keyword;
			if (keywordForm->numKeywords)
				if (haskey(vendoritemanimalpart) || (haskey(vendoritemanimalhide)))
					return LevelersContMPeltREF;
				else if (haskey(vendoritemoreingot))
					return LevelersContMOreREF;
				else if (haskey(vendoritemgem))
					return LevelersStrongBoxGemREF;

				return LevelersContMMiscREF;
		}
	}

	SInt32 GetPotionContainer(AlchemyItem* akBaseItem){
		if (papyrusForm::IsFromMod(akBaseItem, dawnguard))
			return LevelersContDLC1FoodREF;
		else if (papyrusForm::IsFromMod(akBaseItem, dragonborn))
			return LevelersContDLC2FoodREF;
		else if (papyrusForm::IsFromMod(akBaseItem, hearthfires))
			return LevelersContBYOHFoodREF;
		else
		{
			AlchemyItem* thisPotion = (AlchemyItem*)akBaseItem;
			if (thisPotion){
				if (papyrusPotion::IsDrink(thisPotion))
					return LevelersContMPotionREF;
				else if (thisPotion->IsFood())
					return LevelersContMFoodREF;
				else if ((thisPotion->itemData.flags & thisPotion->kFlag_Poison) != 0)
					return LevelersContMPoisonREF;
			}
			return LevelersContMOtherPREF;
		}
	}

	SInt32 GetIngredContainer(IngredientItem* akBaseItem){
		UInt32 formID = akBaseItem->formID;
		formID = formID > 0xFFFFFF ? formID | 0xFF000000 : formID;
		std::map<UInt32, UInt32>::const_iterator it = IngredContainerMap.find(formID);
		formID = it == IngredContainerMap.end() ? LevelersExtraIngred : it->second;
		return formID == LevelersExtraIngred ? (papyrusForm::HasKeywordString(akBaseItem, keywords[vendoritemfood]) ? LevelersContMFoodREF : formID) : formID;
	}

	SInt32 GetContainerCode(StaticFunctionTag* thisObjRef, TESForm* akBaseItem){
		if (akBaseItem)
		{
			switch (akBaseItem->GetFormType()) //meh, hash table jump is more coding...
			{
			case kFormType_Ingredient:
				return GetIngredContainer(reinterpret_cast<IngredientItem*>(akBaseItem));
			case kFormType_Weapon:
				return GetWeaponContainer(reinterpret_cast<TESObjectWEAP*>(akBaseItem));
			case kFormType_Armor:
				return GetArmorContainer(reinterpret_cast<TESObjectARMO*>(akBaseItem));
			case kFormType_Ammo:
				return LevelersContWpnArrowREF;
			case kFormType_Book:
				return GetBookContainer(reinterpret_cast<TESObjectBOOK*>(akBaseItem));
			case kFormType_Note:
				return LevelersContMNotesREF;
			case kFormType_ScrollItem:
				return LevelersContMScrollsREF;
			case kFormType_SoulGem:
				return LevelersContMSoulREF;
			case kFormType_Misc:
				return GetMiscContainer(reinterpret_cast<TESObjectMISC*>(akBaseItem));
			case kFormType_Key:
				return LevelersContMKeysREF;
			case kFormType_Potion:
				return GetPotionContainer(reinterpret_cast<AlchemyItem*>(akBaseItem));
			}
		}
		return -1;
	}

	TESContainer* getContainer(TESObjectREFR* akActionRef){
		TESContainer* pContainer = NULL;
		TESForm* pBaseForm = akActionRef->baseForm;
		if (pBaseForm) {
			pContainer = DYNAMIC_CAST(pBaseForm, TESForm, TESContainer);
			return pContainer;
		}
		return NULL;
	}

	BGSListForm* getContainerFormList(){
		UInt32 ModIndex = static_cast<UInt32>(DataHandler::GetSingleton()->GetModIndex("LevelersTower.esp")) << 24;
		ModIndex |= 0x00FFFFFF;
		return DYNAMIC_CAST(LookupFormByID(containerFLST & ModIndex), TESForm, BGSListForm);
	}

	bool SortAllItems(StaticFunctionTag* thisObjRef, TESObjectREFR* akActionRef){
		BGSListForm* containerRefList = getContainerFormList();
		if (akActionRef && containerRefList)
		{
			TESContainer* pContainer = getContainer(akActionRef);
			if (pContainer) {
				Actor* m_akActionRef = DYNAMIC_CAST(akActionRef, TESObjectREFR, Actor);
				std::unique_ptr<ContainerSorter> sorter;
				if (m_akActionRef){
					ClassFactory(sorter, ActorSorter, m_akActionRef, containerRefList);
				}
				else{
					ClassFactory(sorter, ContainerSorter, akActionRef, containerRefList);
				}
			}
		}
		return false;
	}

	bool SortIngred(StaticFunctionTag* thisObjRef, TESObjectREFR* akActionRef){
		BGSListForm* containerRefList = getContainerFormList();
		if (akActionRef && containerRefList)
		{
			TESContainer* pContainer = getContainer(akActionRef);
			if (pContainer) {
				IngredSorter sorter(akActionRef, containerRefList);
				sorter.afterConstruction();
			}
		}
		return false;
	}

	void TakeFromContainers(UInt32 min, UInt32 max, TESObjectREFR* akActionRef){
		BGSListForm* containerRefList = getContainerFormList();
		if (akActionRef && containerRefList){
			for (SInt32 i = min; i <= max; ++i){
				TESObjectREFR* targetCont = DYNAMIC_CAST(containerRefList->forms[i], TESForm, TESObjectREFR);
				if (targetCont) {
					ContainerCleaner sorter(targetCont, akActionRef);
					sorter.afterConstruction();
				}
			}
		}
	}

	void TakeAllIngred(StaticFunctionTag* thisObjRef, TESObjectREFR* akActionRef){
		TakeFromContainers(LevPot01REF, LevelersExtraIngred, akActionRef);
	}

	void TakeEverything(StaticFunctionTag* thisObjRef, TESObjectREFR* akActionRef){
		TakeFromContainers(LevelersContArmCityGuardREF, LevelersContBYOHMiscREF, akActionRef);
	}

	template <typename Op>
	void ContainerInfo::Visit(Op& op) const {
		for (ContainerInfoVector::const_iterator it = m_vec.begin(); it != m_vec.end(); ++it) {
			op.Accept(it);
		}
	}

	void ContainerInfo::Init(TESObjectREFR*container){
		if (container) {
			UInt32 vecSize = 0;

			TESContainer* pContainer = getContainer(container);
			if (pContainer){
				vecSize += pContainer->numEntries;
				m_map.reserve(vecSize);
				pContainer->Visit(*this);
			}

			ExtraContainerChanges* pXContainerChanges = static_cast<ExtraContainerChanges*>
				(container->extraData.GetByType(kExtraData_ContainerChanges));
			if (pXContainerChanges){
				vecSize += pXContainerChanges->data->objList->Count();
				m_vec.reserve(vecSize);
				pXContainerChanges->data->objList->Visit(*this);
			}
			m_map.clear();
		}
	}

	bool ContainerInfo::ValidateEntry(TESForm* form){
		if (form && (form->flags & 0x04) == 0 && form->formType != kFormType_LeveledItem)
			return true;
		return false;
	}

	bool ContainerInfo::Accept(TESContainer::Entry* pEntry)
	{
		if (ValidateEntry(pEntry->form))
			m_map.emplace(pEntry->form, RemovalInfo(pEntry->count, 0));
		return true;
	}

	bool ContainerInfo::Accept(InventoryEntryData* pEntry)
	{
		if (ValidateEntry(pEntry->type) && !CALL_MEMBER_FN_EX(pEntry, IsQuestItem)())
			registerRemovalInfo(pEntry->type, pEntry->countDelta, 0);
		return true;
	}

	void ContainerInfo::registerRemovalInfo(TESForm* form, SInt32 count, UInt8 equipState){
		ContainerInfoMap::iterator it = m_map.find(form);
		if (it != m_map.end()){
			count += it->second.count;
		}
		if (count > 0)
			m_vec.push_back(RemovalInfo(form, count, equipState));
	}
	
	UInt32 ContainerInfo::Count() const { return m_map.size(); }

	void ContainerRemovalInfoWithException::Accept(ContainerInfoVector::const_iterator it){
		exceptionList.push_back(it->form);
	}

	bool ContainerRemovalInfoWithException::Accept(InventoryEntryData* pEntry){
		if (ValidateEntry(pEntry->type)){
			if (CALL_MEMBER_FN_EX(pEntry, IsQuestItem)())
				return true;

			SInt32 count = pEntry->countDelta;
			UInt8 equipState = 0;

			if (pEntry->type->IsWeapon()){
				BaseExtraList* leftList = NULL;
				BaseExtraList* equipList = NULL;
				pEntry->GetExtraWornBaseLists(&equipList, &leftList);
				if (equipList){
					count -= 1;
					equipState |= 1;
				}
				if (leftList){
					count -= 1;
					equipState |= 2;
				}
				if (equipState == 0)
				{
					if (papyrusActor::FindFavouriteBaseList(pEntry))
						count -= 1;
				}
			}
			else if (pEntry->type->IsAmmo() && papyrusActor::FindWornOrFavouriteBaseList(pEntry))
				return true;
			else if (pEntry->type->IsArmor() && papyrusActor::FindWornBaseList(pEntry)){
				count -= 1;
				equipState = 1;
			}
			else if (papyrusActor::FindFavouriteBaseList(pEntry))
				count -= 1;

			registerRemovalInfo(pEntry->type, count, equipState);
		}
		return true;
	}

	ContainerRemovalInfoWithException::ContainerRemovalInfoWithException(TESObjectREFR* listItemContainer)
	{
		ContainerInfo exeptionContainerInfo;
		exeptionContainerInfo.Init(listItemContainer);
		exceptionList.reserve(exeptionContainerInfo.Count());
		exeptionContainerInfo.Visit(*this);
	}

	ContainerCleaner::ContainerCleaner(TESObjectREFR* akActionRef, TESObjectREFR* targetContainer) :
		m_akActionRef(akActionRef), m_targetContainer(targetContainer){
		m_info.reset(new ContainerInfo());
	}
	
	void ContainerCleaner::Accept(ContainerInfo::ContainerInfoVector::const_iterator pEntry)
	{
		RemoveItem(pEntry->form, pEntry->count, m_targetContainer);
	}
	
	void ContainerCleaner::afterConstruction(){
		m_info->Init(m_akActionRef);
		m_info->Visit(*this);
	}

	void ContainerCleaner::RemoveItem(TESForm* akBaseItem, SInt32 aiCount, TESObjectREFR* targetContainer){
		if (targetContainer){
			UInt32 errorCode;
			CALL_MEMBER_FN_OFFSET(m_akActionRef, RemoveItem)(errorCode, akBaseItem, aiCount, 4, 0, targetContainer, 0, 0);
		}
	}

	void ContainerSorter::RemoveItem(TESForm* akBaseItem, SInt32 aiCount, SInt32 containerCode){
		ContainerCleaner::RemoveItem(akBaseItem, aiCount
			, reinterpret_cast<TESObjectREFR*>(m_containerRefList->forms[containerCode]));
	}

	void ContainerSorter::SortItem(TESForm* akBaseItem, SInt32 aiCount){
		SInt32 containerCode = GetContainerCode(NULL, akBaseItem);
		if (containerCode == -1)
			return;
		if (containerCode == LevelersStrongBoxGoldREF)
		{
			aiCount -= 10000;
			if (aiCount <= 0)
				return;
		}
		else if (containerCode == LevelersContMConsumeREF){
			aiCount -= 1;
			if (aiCount <= 0)
				return;
		}
		
		RemoveItem(akBaseItem, aiCount, containerCode);
	}

	void ContainerSorter::Accept(ContainerInfo::ContainerInfoVector::const_iterator pEntry)
	{
		SortItem(pEntry->form, pEntry->count);
	}

	ContainerSorter::ContainerSorter(TESObjectREFR* akActionRef, BGSListForm* containerRefList) :
		ContainerCleaner(akActionRef, NULL), m_containerRefList(containerRefList){}

	void ActorSorter::EquipItem(TESForm* akBaseItem, SInt8 equipState){
		if (equipState > 1 && m_akActionRef_Actor && akBaseItem){
			if (equipState & 1)
					papyrusActor::EquipItemEx(m_akActionRef_Actor, akBaseItem, 0, false, false);
			if (equipState & 2)
					papyrusActor::EquipItemEx(m_akActionRef_Actor, akBaseItem, 2, false, false);
		}
	}

	void ActorSorter::Accept(ContainerInfo::ContainerInfoVector::const_iterator pEntry)
	{
		ContainerSorter::Accept(pEntry);
		EquipItem(pEntry->form, pEntry->equipState);
	}
	
	ActorSorter::ActorSorter(Actor* akActionRef, BGSListForm* containerRefList)
		: ContainerSorter(akActionRef, containerRefList), m_akActionRef_Actor(akActionRef){
		m_info.reset(new ContainerRemovalInfoWithException(
			DYNAMIC_CAST(containerRefList->forms[LevelersTempCont], TESForm, TESObjectREFR)));
	}
	
	void IngredSorter::SortItem(TESForm* akBaseItem, SInt32 aiCount){
		if (akBaseItem->formType == kFormType_Ingredient){
			SInt32 containerCode = GetIngredContainer(reinterpret_cast<IngredientItem*>(akBaseItem));
			RemoveItem(akBaseItem, aiCount, containerCode);
		}
	}

	IngredSorter::IngredSorter(TESObjectREFR* akActionRef, BGSListForm* containerRefList)
		: ContainerSorter(akActionRef, containerRefList){}

}