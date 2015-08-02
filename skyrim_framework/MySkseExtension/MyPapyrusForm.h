#include "skse/PapyrusForm.h"
#include "skse/GameData.h"
#include "Utilities.h"
#include <vector>

//DEFINE_MEMBER_FN_LONG(TESForm, IsQuestItem, bool, 0x00418FE0);

namespace papyrusForm
{/*
	bool IsWeapon(TESForm* thisForm);
	bool IsAmmo(TESForm* thisForm);
	bool IsArmor(TESForm* thisForm);
	bool IsFromMod(TESForm* thisForm, BSFixedString modName);*/

	template<typename T>
	bool KeywordFinder(BGSKeywordForm* keywordForm, T &func){
		for (int i = 0; i < keywordForm->numKeywords; i++)
		{
			BGSKeyword* pKey = keywordForm->keywords[i];
			if (pKey)
			{
				if (func(pKey)) return true;
			}
		}
		return false;
	}

	struct HasKeywordStringFunc{
		const char* m_string;
		HasKeywordStringFunc(const char* string);
		bool operator()(BGSKeyword* pKey);
	};

	bool HasKeywordAny(TESForm* thisForm, const std::vector<UInt32*>& keywords);
	bool IsFromMod(TESForm* thisForm, const char* modName);
	bool HasKeywordString(TESForm* thisForm, BSFixedString keywordString);
	bool HasKeywordContainingString(TESForm* thisForm, const char* sub_string);
	bool HasKeywordFormID(TESForm* thisForm, UInt32 FormID);
	void MyRegisterFuncs(VMClassRegistry* registry);
}
