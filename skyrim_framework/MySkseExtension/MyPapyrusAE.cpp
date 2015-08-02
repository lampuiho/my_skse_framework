#include "MyPapyrusAE.h"
#include "MyGameApi.h"
#include "skse\GameObjects.h"
#include "skse\GameReferences.h"
#include "skse\PapyrusVM.h"
#include "skse\PapyrusNativeFunctions.h"
#include "skyrim_framework\SerialisationUtilities.h"

namespace papyrusActiveMagicEffect{

	std::unordered_map<UInt32, ActiveEffect*> activeEffects;

	UInt32 GetActorValueIndex(ActiveEffect* effect){ return effect->actorValue; }

	struct EffectMatcher{
		ActiveEffect* effect;
		EffectMatcher(ActiveEffect* effect) : effect(effect){}
		bool Accept(ActiveEffect* e){ return effect == e; }
	};

	UInt32 RequestEffectID(ActiveEffect* effect){
		auto ret = activeEffects.emplace(reinterpret_cast<UInt32>(effect), effect);
		if (ret.second)
			return reinterpret_cast<UInt32>(effect);
		else
			return 0;
	}

	ActiveEffect* GetEffectByID(StaticFunctionTag* obj, UInt32 id){
		auto it = activeEffects.find(id);
		if (it != activeEffects.end())
			return it->second;
		else
			return NULL;
	}
	
	void InvalidateEffect(StaticFunctionTag* obj, UInt32 id){
		activeEffects.erase(id);
	}

	//void InvalidateAllRegisteredEffects(StaticFunctionTag* obj){
	//	activeEffects.clear();
	//}

	SInt32 GetEffectIndex(ActiveEffect* effect){
		Actor* ref;
		if (ref = GetTargetActor(effect->reference)){
			 return ref->magicTarget.GetActiveEffects()->GetIndexOf(EffectMatcher(effect));
		}
		return -1;
	}

	void SetMagnitude(ActiveEffect* effect, float mag){ effect->magnitude = mag; }

	/*
	bool DispelEX(ActiveEffect* effect){
		if (effect->item && effect->item->formType == kFormType_Spell){
			Actor* ref = effect->reference->Unk_02();
			return PapyrusRemoveSpell(0, 0, ref, reinterpret_cast<SpellItem*>(effect->item));
		}
		return false;
	}*/

	UInt32 SerialiseRegisteredActiveEffects(std::unordered_map<UInt8, char*>& loadordertable, UInt32*&buf){
		UInt16 i = activeEffects.size();
		if (i > 0){
			buf = new UInt32[i * 3];
			i = 0;
			for (auto p : activeEffects){
				buf[i++] = p.first;
				Actor* ref = GetTargetActor(p.second->reference);
				buf[i++] = ref->formID;
				UInt8 loadOrder = ref->formID >> 24;
				if (loadOrder > 0)
					loadordertable.emplace(loadOrder, reinterpret_cast<char*>(NULL));
				buf[i++] = ref->magicTarget.GetActiveEffects()->GetIndexOf(EffectMatcher(p.second));
			}
#ifdef _DEBUG
			printBuffer(buf, i);
#endif
			return i * sizeof(*buf);
		}
		return 0;
	}

	void DeserialiseRegisteredActiveEffects(const std::unordered_map<UInt8, UInt8>& loadordertable, const UInt32*buf, const UInt32 length){
#ifdef _DEBUG
		std::cout << std::dec << "ActiveEffect buffer length: " << static_cast<UInt32>(length);
		std::cout << "\nLoaded ActiveEffect buffer: " << std::hex;
#endif
		for (UInt32 i = 0; i < length; ++i){
#ifdef _DEBUG
			std::cout << buf[i] << "-" << buf[i + 1] << "-" << buf[i + 2] << "-";
#endif
			UInt32 id = buf[i++];
			TESForm* form = GetFormByOldID(loadordertable, buf[i++]);
			if (form && form->formType == kFormType_Character){
				ActiveEffect* effect = reinterpret_cast<Actor*>(form)->magicTarget.GetActiveEffects()
					->GetNthItem(buf[i]);
				if (effect)
					activeEffects.emplace(id, effect);
			}
		}
#ifdef _DEBUG
		std::cout << std::endl;
#endif
	}

	void MyRegisterFuncs(VMClassRegistry* registry){
		registry->RegisterFunction(
			new NativeFunction0 <ActiveEffect, UInt32>("GetActorValueIndex", "ActiveMagicEffect", GetActorValueIndex, registry));
		//registry->RegisterFunction(
		//	new NativeFunction0 <ActiveEffect, bool>("DispelEX", "ActiveMagicEffect", DispelEX, registry));
		registry->RegisterFunction(
			new NativeFunction1 <ActiveEffect, void, float>("SetMagnitude", "ActiveMagicEffect", SetMagnitude, registry));
		registry->RegisterFunction(
			new NativeFunction0 <ActiveEffect, UInt32>("RequestEffectID", "CustomMagEffect", RequestEffectID, registry));
		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, void, UInt32>("InvalidateEffect", "CustomMagEffect", InvalidateEffect, registry));
		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, ActiveEffect*, UInt32>("GetEffectByID", "CustomMagEffect", GetEffectByID, registry));
	}
}
