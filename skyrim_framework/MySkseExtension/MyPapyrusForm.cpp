#include "MyPapyrusForm.h"
#include <algorithm>
#include "Utilities.h"
#include "MyGameApi.h"
#include "skse/GameRTTI.h"
#include "skse/PapyrusNativeFunctions.h"

namespace papyrusForm
{/*
	bool IsWeapon(TESForm* thisForm) { return (thisForm) ? thisForm->IsWeapon() : false; }
	bool IsAmmo(TESForm* thisForm) { return (thisForm) ? thisForm->IsAmmo() : false; }
	bool IsArmor(TESForm* thisForm) { return (thisForm) ? thisForm->IsArmor() : false; }
	bool IsFromMod(TESForm* thisForm, BSFixedString modName){ return thisForm && modName.data ? IsFromMod(thisForm, modName.data) : false; }*/
		
	template<typename T>
	bool KeywordFinder(TESForm* form, T &func){
		if (form){
			BGSKeywordForm* keywordForm = GetKeywordForm(form);
			if (keywordForm){
				KeywordFinder(keywordForm, func);
			}
		}
		return false;
	}

	HasKeywordStringFunc::HasKeywordStringFunc(const char* string) : m_string(string){}
	bool HasKeywordStringFunc::operator()(BGSKeyword* pKey){
		return _stricmp(pKey->keyword.Get(), m_string) == 0;
	}

	bool HasKeywordAny(TESForm* thisForm, const std::vector<UInt32*>& keywords){
		return std::find_if(keywords.begin(), keywords.end(), [thisForm](UInt32* p_keyword) -> bool{
			if (HasKeywordFormID(thisForm, *p_keyword))
				return true;
			return false;
		}) != keywords.end();
	}
	UInt8 GetModIndex(TESForm* thisForm){
		return thisForm ? thisForm->formID >> 24 : NULL;
	}
	bool IsFromMod(TESForm* thisForm, const char* modName){ return GetModIndex(thisForm) == DataHandler::GetSingleton()->GetModIndex(modName); }
	bool HasKeywordString(TESForm* thisForm, BSFixedString keywordString){
		return KeywordFinder(thisForm, HasKeywordStringFunc(keywordString.data));
	}
	bool HasKeywordContainingString(TESForm* thisForm, const char* sub_string){
		return KeywordFinder(thisForm, [sub_string](BGSKeyword* pKey){
			return Substring(pKey->keyword.Get(), sub_string) >= 0; });
	}
	bool HasKeywordFormID(TESForm* thisForm, UInt32 FormID){
		return KeywordFinder(thisForm, [FormID](BGSKeyword* pKey){
			return pKey->formID == FormID; });
	}
	void MyRegisterFuncs(VMClassRegistry* registry){/*
		registry->RegisterFunction(
			new NativeFunction0 <TESForm, bool>("IsWeapon", "Form", papyrusForm::IsWeapon, registry));

		registry->RegisterFunction(
			new NativeFunction0 <TESForm, bool>("IsAmmo", "Form", papyrusForm::IsAmmo, registry));

		registry->RegisterFunction(
			new NativeFunction0 <TESForm, bool>("IsArmor", "Form", papyrusForm::IsArmor, registry));

		registry->RegisterFunction(
			new NativeFunction0 <TESForm, UInt32>("GetModIndex", "Form", papyrusForm::GetModIndex, registry));

		registry->RegisterFunction(
			new NativeFunction1 <TESForm, bool, BSFixedString>("IsFromMod", "Form", papyrusForm::IsFromMod, registry));

		registry->RegisterFunction(
			new NativeFunction1 <TESForm, bool, BSFixedString>("HasKeywordString", "Form", papyrusForm::HasKeywordString, registry));
			*/
	}
}