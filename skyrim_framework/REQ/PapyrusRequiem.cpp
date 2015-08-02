#include "skyrim_framework\REQ\PapyrusRequiem.h"
#include <algorithm>
#include "skyrim_framework\REQ\REQ_const.h"
#include "skyrim_framework\common_const.h"
#include "skse\GameRTTI.h"
#include "skse\GameReferences.h"
#include "skse\PapyrusNativeFunctions.h"
#include "skyrim_framework\MySkseExtension\MyPapyrusObjectReference.h"
#include "skyrim_framework\MySkseExtension\MyPapyrusActor.h"
#include "skyrim_framework\MySkseExtension\MyPapyrusForm.h"
#include "skyrim_framework\MySkseExtension\MyGameApi.h"

namespace Requiem{
	namespace papyrusRequiem
	{
		std::unordered_map<UInt8, TESObjectARMO*> equippedArmour; //only a singleton is needed for the player

		template<typename result, typename functor, typename...A>
		result RefAliasFunc(functor func, BGSRefAlias* thisRef, A... args){
			TESForm* refForm = PapyrusGetReference(0, 0, thisRef);
			if (refForm){
				Actor* ref = reinterpret_cast<Actor*>(refForm);
				return func(ref, args...);
			}
#ifdef _DEBUG
			std::cout << "RefAlias Cast failture" << std::endl;
#endif
			return static_cast<result>(0);
		}

		void GetArmorSkillGain(TESObjectARMO* armor, UInt8&HeavyArmourCount, UInt8&LightArmourCount, float&gainHeavy, float&gainLight){
			for (auto gainMask = armorGainMask.begin(); gainMask != armorGainMask.end(); ++gainMask) {
				if ((gainMask->first & armor->bipedObject.data.parts) != 0)
					switch (armor->bipedObject.data.weightClass){
					case BGSBipedObjectForm::kWeight_Light:
						gainLight += gainMask->second;
						LightArmourCount += 1;
						break;
					case BGSBipedObjectForm::kWeight_Heavy:
						gainHeavy += gainMask->second;
						HeavyArmourCount += 1;
						break;
					case BGSBipedObjectForm::kWeight_None:
						gainLight += gainMask->second / 2;
						break;
				}
			}
		}

		UInt8 GetActualArmourCount(TESObjectARMO* armo, UInt32 armourClass){
			UInt8 result = 0;
			for (auto gainMask = armorGainMask.begin(); gainMask != armorGainMask.end(); ++gainMask)
				if ((gainMask->first & armo->bipedObject.data.parts) != 0)
					if (armo->bipedObject.data.weightClass == armourClass)
						result++;
			return result;
		}

		struct ArmorCountGetter : papyrusActor::WornArmorVisiter<UInt32>{
			UInt32 armourClass;
			ArmorCountGetter(Actor* ref, UInt32 armourClass) : WornArmorVisiter(ref), armourClass(armourClass) {}
			bool conditionFunc(TESObjectARMO* armo){
				result += GetActualArmourCount(armo, armourClass);
				return true;
			}
		};

		UInt32 GetArmourCount(StaticFunctionTag* thisObj, Actor* thisActor, UInt32 armourClass){ return ArmorCountGetter(thisActor, armourClass)(); }

		UInt32 GetActualArmourCount(StaticFunctionTag* thisObj, TESObjectARMO* armo, UInt32 armourClass){
			return GetActualArmourCount(armo, armourClass);
		}

		struct ArmorFullSet : public papyrusActor::WornArmorVisiter<float>{
			UInt32 tg, db;
			ArmorFullSet(Actor* ref): WornArmorVisiter(ref), tg(0), db(0){}
			virtual bool conditionFunc(TESObjectARMO* armor){
				if (armor->keyword.numKeywords > 0){
#ifdef _DEBUG
					__try
					{
#endif
						if (papyrusForm::HasKeywordFormID(armor, Common::FormContainer::Forms[Common::FormContainer::ArmorDarkBrotherhood]))
							db += 1;
						else if (papyrusForm::HasKeywordFormID(armor, Common::FormContainer::Forms[Common::FormContainer::ArmorNightingale]))
							tg += 1;
#ifdef _DEBUG
					}
					__except (EXCEPTION_EXECUTE_HANDLER)
					{
						std::cout << "Exception when checking keywords for armour: " << armor->fullName.name.data
							<< " ID: " << std::hex << armor->formID << std::endl;
					}
#endif
				}
				return true;
			}
			void fullSetBonus(){
				if (db >= 4)
					CALL_MEMBER_FN_EX(ref, AddSpell)(reinterpret_cast<SpellItem*>(Common::FormContainer::Forms[Common::FormContainer::DBFullSet]));
				else
				{
					PapyrusRemoveSpell(0, 0, ref, reinterpret_cast<SpellItem*>(Common::FormContainer::Forms[Common::FormContainer::DBFullSet]));
					if (tg >= 4)
						CALL_MEMBER_FN_EX(ref, AddSpell)(reinterpret_cast<SpellItem*>(Common::FormContainer::Forms[Common::FormContainer::TGFullSet]));
					else
						PapyrusRemoveSpell(0, 0, ref, reinterpret_cast<SpellItem*>(Common::FormContainer::Forms[Common::FormContainer::TGFullSet]));
				}
			}
		};

		struct ArmorMass : public ArmorFullSet{
			ArmorMass(Actor* ref) : ArmorFullSet(ref){}
			virtual bool conditionFunc(TESObjectARMO* armor){;
				result += armor->weight.weight;
				return ArmorFullSet::conditionFunc(armor);
			}
		};

		struct ArmorSkillGain : public ArmorMass{
			UInt8 HeavyArmourCount; UInt8 LightArmourCount;
			float gainHeavy, gainLight;
			ArmorSkillGain(Actor* ref) : ArmorMass(ref), HeavyArmourCount(0), LightArmourCount(0), gainHeavy(0), gainLight(0){
				equippedArmour.clear();
				equippedArmour.reserve(32);
			}
			bool conditionFunc(TESObjectARMO* armor){
				equippedArmour.emplace(log2(armor->bipedObject.data.parts), armor);
				GetArmorSkillGain(armor, HeavyArmourCount, LightArmourCount, gainHeavy, gainLight);
				return ArmorMass::conditionFunc(armor);
			}
		};

		inline void RestoreShouts(Actor* thisActor, VMArray<float> mods){
			float shoutmod = 0;
			for (UInt32 i = attributeModSize; i < attributeModSize + shoutModSize; ++i){
				float mod;	mods.Get(&mod, i); shoutmod += mod; mod = 0; mods.Set(&mod, i);
			}
			thisActor->actorValueOwner.Unk_06(0, kAVShoutRecoveryMult, shoutmod);
		}

		inline void EvaluateShouts(Actor* thisActor, VMArray<float> mods){
			RestoreShouts(thisActor, mods);
			float shoutmod = 0;
			for (UInt32 i = 0; i < 3; ++i){
				UInt32 miscStat;
				if (QueryStat(&BSFixedString(shoutstats[i]), miscStat)){
#ifdef _DEBUG
					std::cout << "Player's misc stat: " << shoutstats[i] << " is " << miscStat << std::endl;
#endif
					if (static_cast<float>(miscStat) > shoutthreshold[i]){
						float mod = shoutcoef[i] * sqrt(static_cast<float>(miscStat)-shoutthreshold[i]);
						mods.Set(&mod, i + attributeModSize);
						shoutmod += mod;
					}
				}
			}
			thisActor->actorValueOwner.Unk_06(0, kAVShoutRecoveryMult, -shoutmod);
		}

		float GetWeaponWeight(Actor* thisActor){
			float mass = 0;
#ifdef _DEBUG
			std::cout << "Calculating weapon mass from " << std::hex << thisActor->formID << ", " << static_cast<TESNPC*>(thisActor->baseForm)->fullName.name.data << std::endl;
#endif
			if (thisActor->processManager){
				TESForm* eq = thisActor->processManager->equippedObject[0];
				if (eq && eq->IsWeapon()){
					TESObjectWEAP* weap = reinterpret_cast<TESObjectWEAP*>(eq);
					if (weap){
						mass += weap->weight.weight;
						if (weap->type() == 5 || weap->type() == 6)
							return mass;
					}
				}
				eq = thisActor->processManager->equippedObject[1];
				if (eq && eq->IsWeapon()){
					TESObjectWEAP* weap = reinterpret_cast<TESObjectWEAP*>(eq);
					if (weap)
						mass += weap->weight.weight;
				}
			}
			return mass;
		}

		template <typename T>
		float GetArmorStuff(Actor* ref, T& stuff){
			//static_assert(static_cast<ArmorMass*>((T*)0), "Must be class inherited from ArmorMass");
			float mass = stuff();
			stuff.fullSetBonus();
			mass += GetWeaponWeight(ref);
			mass /= 100;
			return mass;
		}

		float GetArmorMass(StaticFunctionTag* dum, Actor* ref){
			if (ref){
				ArmorMass armorMass(ref);
				return GetArmorStuff(ref, armorMass);
			}
			return 0;
		}
		
		bool IsPreviouslyEquipped(StaticFunctionTag* thisObj, TESObjectARMO* armo){
			for (auto const& p : Requiem::papyrusRequiem::equippedArmour){
				if (p.second == armo)
					return true;
			}
			return false;
		}

		SInt32 GetAmmoWeight(StaticFunctionTag* thisObj, TESForm* form){
			if (form){
				if (form->IsAmmo()){
					if (form->formID == exceptionArrowID)
						return 4;
					if (papyrusForm::HasKeywordAny(form, ammoWeightKeywords[0]))
						return 0;
					else if (papyrusForm::HasKeywordAny(form, ammoWeightKeywords[1]))
						return 2;
					else if (papyrusForm::HasKeywordAny(form, ammoWeightKeywords[2]))
						return 3;
					else
						return 1;
				}
			}
			return 4;
		}

		void FillArmorSlots(TESObjectARMO* armo, bool isEquip){
			TESObjectARMO* noneVar = 0;
			UInt32 slot = 0;
			UInt32 slotMask = armo->bipedObject.data.parts;
			std::unordered_map<UInt8, TESObjectARMO*>::iterator armoSlot;
			do{
				if ((slotMask & 1) == 1){
					armoSlot = equippedArmour.find(slot);
					if (armoSlot != equippedArmour.end())
						equippedArmour.erase(armoSlot);
				}
				++slot;
			} while (slotMask >>= 1);
			if (isEquip)
				equippedArmour.emplace(slot - 1, armo);
		}

		void ArmorPenalty(Actor* ref, bool heavyArmour, bool lightArmour){
			bool runExhaustion = reinterpret_cast<TESGlobal*>(Requiem::FormContainer::Forms[Requiem::FormContainer::
				REQ_Atmosphere_NoRunningExhaustion])->unk20 == 0 && reinterpret_cast<TESGlobal*>(Common::FormContainer::
				Forms[Common::FormContainer::PlayerIsVampire])->unk20 == 0;

			if (heavyArmour){
				if(reinterpret_cast<TESGlobal*>(Requiem::FormContainer::Forms[Requiem::FormContainer::
					REQ_Skills_NoHeavyArmorExhaustion])->unk20 == 0)
				if (!CALL_MEMBER_FN(ref, HasPerk)(reinterpret_cast<BGSPerk*>(Common::FormContainer::Forms[Common::FormContainer::Juggernaut00])))
					CALL_MEMBER_FN_EX(ref, AddSpell)(reinterpret_cast<SpellItem*>(reinterpret_cast<BGSListForm*>(
					Requiem::FormContainer::Forms[Requiem::FormContainer::req_massabilities])->forms[1]));
				
				if (!CALL_MEMBER_FN(ref, HasPerk)(reinterpret_cast<BGSPerk*>(Common::FormContainer::Forms[Common::FormContainer::Juggernaut20])))
					CALL_MEMBER_FN_EX(ref, AddSpell)(reinterpret_cast<SpellItem*>(reinterpret_cast<BGSListForm*>(
					Requiem::FormContainer::Forms[Requiem::FormContainer::req_massabilities])->forms[2]));

				if (runExhaustion)
					CALL_MEMBER_FN_EX(ref, AddSpell)(reinterpret_cast<SpellItem*>(reinterpret_cast<BGSListForm*>(
					Requiem::FormContainer::Forms[Requiem::FormContainer::req_massabilities])->forms[4]));
			}
			if (runExhaustion){
				CALL_MEMBER_FN_EX(ref, AddSpell)(reinterpret_cast<SpellItem*>(reinterpret_cast<BGSListForm*>(
					Requiem::FormContainer::Forms[Requiem::FormContainer::req_massabilities])->forms[0]));
				if (lightArmour){
					CALL_MEMBER_FN_EX(ref, AddSpell)(reinterpret_cast<SpellItem*>(reinterpret_cast<BGSListForm*>(
						Requiem::FormContainer::Forms[Requiem::FormContainer::req_massabilities])->forms[3]));
				}
			}
		}

		void UpdateHeavyArmorPenalty(BGSRefAlias* thisRef, TESObjectARMO* armo, VMArray<float> skillgains, bool isEquip){
			RefAliasFunc<void>([](Actor* ref, TESObjectARMO* armo, VMArray<float> skillgains, bool isEquip){
				FillArmorSlots(armo, isEquip);

				ArmorFullSet fullSet(ref);
				for (auto&p : equippedArmour)
					fullSet.conditionFunc(p.second);
				fullSet.fullSetBonus();

				UInt8 HeavyArmorCount = false; UInt8 LightArmorCount = false; float heavygain = 0, lightgain = 0;
				GetArmorSkillGain(armo, HeavyArmorCount, LightArmorCount, heavygain, lightgain);
				if (heavygain != 0){
					float oldheavygain;
					skillgains.Get(&oldheavygain, 0);
					if (isEquip) { heavygain += oldheavygain; }
					else { heavygain = oldheavygain - heavygain; }
					skillgains.Set(&heavygain, 0);
				}
				if (lightgain != 0){
					float oldlightgain;
					skillgains.Get(&oldlightgain, 1);
					if (isEquip) { lightgain += oldlightgain; }
					else { lightgain = oldlightgain - lightgain; }
					skillgains.Set(&lightgain, 1);
				}

				if (isEquip){
					ArmorPenalty(ref, HeavyArmorCount > 0, LightArmorCount > 0);
				}}, thisRef, armo, skillgains, isEquip);
		}
		
		float GetArmorSkillGain(BGSRefAlias* thisRef, VMArray<float> array){
			return RefAliasFunc<float>([](Actor* ref, VMArray<float> m_array)->float{
				ArmorSkillGain skillGain(ref);
				float mass = GetArmorStuff(ref, skillGain);
				ArmorPenalty(ref, skillGain.HeavyArmourCount > 0, skillGain.LightArmourCount > 0);
				m_array.Set(&skillGain.gainHeavy, 0);
				m_array.Set(&skillGain.gainLight, 1);
				return mass;
			}, thisRef, array);

		}

		void RestoreAttributes(BGSRefAlias* thisRef, VMArray<float> mods){
			RefAliasFunc<void>([](Actor* ref, VMArray<float> m_mods){
				for (UInt32 i = 0; i < attributeModSize; ++i){
					float mod;	m_mods.Get(&mod, i);
					ref->actorValueOwner.Unk_06(0, attributeToMod[i], -mod);
					mod = 0; m_mods.Set(&mod, i);
				}
				RestoreShouts(ref, m_mods);
			}, thisRef, mods);
		}

		void EvaluateAttributes(BGSRefAlias* thisRef, VMArray<float> mods){
			RefAliasFunc<void>([](Actor* ref, VMArray<float> m_mods){
				float weights[attributeModSize];
				for (UInt32 i = 0; i < attributeModSize; ++i){
					m_mods.Get(&weights[i], i);
					ref->actorValueOwner.Unk_06(0, attributeToMod[i], -weights[i]);
				}

				std::vector<float> mainAtt = { ref->actorValueOwner.GetBase(kAVHealth),
					ref->actorValueOwner.GetBase(kAVMagicka),
					ref->actorValueOwner.GetBase(kAVStamina) };

				//to be vectorized...
				for (UInt32 i = 0; i < attributeModSize; ++i){
					weights[i] = 0;
					for (UInt32 j = 0; j < 3; ++j)
						weights[i] += attributeWeight[i][j] * mainAtt[j];
					weights[i] = weights[i] > threshold[i] ? attributePreF[i] * sqrt(weights[i] - threshold[i]) : 0;
				}

				weights[8] = weights[8] == 0 ? 0 : 100 / (1 + weights[7]) + weights[8] / (1 + weights[7]) - 100;
				weights[9] = weights[9] == 0 ? 0 : 100 / (1 + weights[7]) + weights[9] / (1 + weights[7]) - 100;
				for (UInt32 i = 0; i < attributeModSize; ++i){
					m_mods.Set(&weights[i], i);
					ref->actorValueOwner.Unk_06(0, attributeToMod[i], weights[i]);
				}
				EvaluateShouts(ref, m_mods);
			}, thisRef, mods);
		}

		void GetWeightToAdd(BGSRefAlias* thisRef, VMArray<UInt32> toAdd){
#define weightLength 4
			RefAliasFunc<void>([](Actor* ref, VMArray<UInt32> m_toAdd){
				if (ref && m_toAdd.Length() == weightLength){
					UInt32 weight[weightLength] = { 0, 0, 0, 0 };
					papyrusObjectReference::Container_for_each(ref, [&](TESForm* form, UInt32 count){
						if (form->IsAmmo()){
							UInt32 weighType;
							if (weighType = GetAmmoWeight(0, form) < weightLength)
								weight[weighType] += count;
						}
					});
					for (UInt8 i = 0; i < weightLength; ++i)
						m_toAdd.Set(&weight[i], i);
				}
#ifdef _DEBUG
				else
					std::cout << "toAdd length is not 4 or other error with toAdd or unable to get referenceAlias" << std::endl;
#endif
			}, thisRef, toAdd);
		}

		SInt32 GetRaceID(BGSRefAlias* thisRef){
			SInt32 result = RefAliasFunc<SInt32>([](Actor* ref)->SInt32{
				for (auto i = 0; i < raceSig.size(); ++i)
					if (papyrusForm::HasKeywordContainingString(ref->race, raceSig[i])) return i;
				return raceSig.size();
			}, thisRef);
			return result == 0 ? raceSig.size() : result;
		}

		bool MyRegisterFuncs(VMClassRegistry* registry){
			registry->RegisterFunction(
				new NativeFunction1 <StaticFunctionTag, float, Actor*>("GetArmorMass", "REQ_MassEffectField", papyrusRequiem::GetArmorMass, registry));
			registry->RegisterFunction(
				new NativeFunction1 <StaticFunctionTag, SInt32, TESForm*>("GetAmmoWeight", "REQ_ArrowWeight", papyrusRequiem::GetAmmoWeight, registry));
			registry->RegisterFunction(
				new NativeFunction1 <StaticFunctionTag, bool, TESObjectARMO*>("IsPreviouslyEquipped", "REQ_PassiveArmorTraining", papyrusRequiem::IsPreviouslyEquipped, registry));
			registry->RegisterFunction(
				new NativeFunction2 <StaticFunctionTag, UInt32, Actor*, UInt32>("GetArmourCount", "REQ_PassiveArmorTraining", papyrusRequiem::GetArmourCount, registry));
			registry->RegisterFunction(
				new NativeFunction2 <StaticFunctionTag, UInt32, TESObjectARMO*, UInt32>("GetActualArmourCount", "REQ_PassiveArmorTraining", papyrusRequiem::GetActualArmourCount, registry));
			registry->RegisterFunction(
				new NativeFunction3 <BGSRefAlias, void, TESObjectARMO*, VMArray<float>, bool>("UpdateHeavyArmorPenalty", "REQ_PassiveArmorTraining", papyrusRequiem::UpdateHeavyArmorPenalty, registry));
			registry->RegisterFunction(
				new NativeFunction1 <BGSRefAlias, void, VMArray<UInt32>>("GetWeightToAdd", "REQ_ArrowWeight", papyrusRequiem::GetWeightToAdd, registry));
			registry->RegisterFunction(
				new NativeFunction1 <BGSRefAlias, float, VMArray<float>>("GetArmorSkillGain", "REQ_PassiveArmorTraining", papyrusRequiem::GetArmorSkillGain, registry));
			registry->RegisterFunction(
				new NativeFunction0 <BGSRefAlias, SInt32>("GetRaceID", "REQ_StartingPerks", papyrusRequiem::GetRaceID, registry));
			registry->RegisterFunction(
				new NativeFunction1 <BGSRefAlias, void, VMArray<float>>("RestoreAttributes", "REQ_AttributeSystem", papyrusRequiem::RestoreAttributes, registry));
			registry->RegisterFunction(
				new NativeFunction1 <BGSRefAlias, void, VMArray<float>>("EvaluateAttributes", "REQ_AttributeSystem", papyrusRequiem::EvaluateAttributes, registry));
			return true;
		}
	}
}