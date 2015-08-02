/*#include "DiseasePoisonImmunity.h"
#include <thread>
#include "skyrim_framework\REQ\REQ_const.h"
#include "skse\GameSettings.h"
#include "skse\GameObjects.h"
#include "skse\GameReferences.h"
#include "skse\PapyrusVM.h"

namespace mslVT{
	const char* modName = "mslVampiricThirst.esp";
	enum BaseForms{ PlayerIsWerewolf, Vampire };
	std::vector<UInt32> BaseForms{ 0xed06c, 0xa82bb };
	enum Forms{ mslVTCureDisease };
	std::vector<UInt32> Forms{ 0xff008efa };

	struct RemovePoison{
		EffectSetting* me;
		RemovePoison(EffectSetting* i_me) : me(i_me){}
		bool Accept(ActiveEffect* ae){
			if (ae->effect->mgef == me){
				Dispel(ae, 0);
				return true;
			}
			return false;
		}
	};

	struct RemoveDisease : RemovePoison{
		RemoveDisease(EffectSetting* i_me) : RemovePoison(i_me){}
		bool Accept(ActiveEffect* ae){
			if (ae->effect->mgef == me){
				if (ae->item->formType == kFormType_Spell){
					if (reinterpret_cast<SpellItem*>(ae->item)->data.type == 0x01){
						PapyrusRemoveSpell(0, 0, reinterpret_cast<Actor*>(ae->reference), 
							reinterpret_cast<SpellItem*>(ae->item));
						//Dispel(ae, 0);
					}
				}
				return true;
			}
			return false;
		}
	};

	bool DiseasePoisonImmunity::RemoveDisease(Actor* ref, EffectSetting* me){
		if (me->properties.castType == EffectSetting::Properties::kCastingType_ConstantEffect
			&& ((me->properties.flags & (EffectSetting::Properties::kEffectType_NoArea
			| EffectSetting::Properties::kEffectType_NoDuration) | EffectSetting::Properties::kEffectType_Detrimental)) != 0){
			mslVT::RemoveDisease removeDisease(me);
			return ref->magicTarget.GetActiveEffects()->Find(removeDisease) != 0;
		}
		return true;
	}
	bool DiseasePoisonImmunity::RemoveDiseasePoison(Actor* ref, EffectSetting* me){
		if (me->properties.resistance == kAVPoisonResist){
			mslVT::RemovePoison removePoison(me);
			return ref->magicTarget.GetActiveEffects()->Find(removePoison) != 0;
		}
		else
			return RemoveDisease(ref, me);
	}
	EventResult DiseasePoisonImmunity::ReceiveEvent(TESMagicEffectApplyEvent * evn, EventDispatcher<TESMagicEffectApplyEvent> * dispatcher){
	if (evn->target){
		EffectSetting* me = reinterpret_cast<EffectSetting*>(LookupFormByID(evn->magicEffectID));
		if (evn->target == *g_thePlayer)
			if (reinterpret_cast<TESGlobal*>(BaseForms[BaseForms::PlayerIsVampire])->unk20 != 0){
				return RemoveDiseasePoison(*g_thePlayer, me) ? kEvent_Continue : kEvent_Abort;
			}
			else if (reinterpret_cast<TESGlobal*>(BaseForms[BaseForms::PlayerIsWerewolf])->unk20 != 0){
				return RemoveDisease(*g_thePlayer, me) ? kEvent_Continue : kEvent_Abort;
			}
		else if (evn->target->baseForm->formType == kFormType_NPC){
			Actor* ref = reinterpret_cast<Actor*>(evn->target);
			if (ref->race->keyword.HasKeyword(BaseForms[BaseForms::Vampire]) ||
				reinterpret_cast<TESNPC*>(evn->target->baseForm)->keyword.HasKeyword(BaseForms[BaseForms::Vampire]))
				return RemoveDiseasePoison(ref, me) ? kEvent_Continue : kEvent_Abort;
		}
	}
	return kEvent_Continue;
}
	DiseasePoisonImmunity g_DiseasePoisonImmunity;
}*/