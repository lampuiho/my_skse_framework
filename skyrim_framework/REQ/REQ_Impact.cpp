#include "skse\PapyrusArgs.h"
#include "skse\GameReferences.h"
#include "skse\GameObjects.h"
#include "skse\PapyrusNativeFunctions.h"
#include "skyrim_framework\MySkseExtension\MyPapyrusForm.h"
#include "skyrim_framework\FormInitialiser.h"
#include "skyrim_framework\REQ\REQ_const.h"
#include "skse\GameRTTI.h"
#include "skse\Utilities.h"
#include "skyrim_framework\common_const.h"
#include "skyrim_framework\REQ\REQ_Impact.h"
#include <iterator>
#include <thread>
#ifdef _DEBUG
#include <bitset>
#endif

namespace Requiem{
	namespace eventRequiem{
		REQ_ImpactHandler g_REQ_ImpactHandler;
		bool REQ_ImpactHandler::RegisterBlock(Actor* actor){
			std::lock_guard<std::mutex> lock(simpleLock);
			if (std::find(safeActors.begin(), safeActors.end(), actor) != safeActors.end()){
#ifdef _DEBUG
				std::cout << std::hex << actor->formID
					<< " was hit by a spell and caused multiple OnHit trigger, exiting processing." << std::endl;
#endif
				return false;
			}
#ifdef _DEBUG
			std::cout << std::hex << (UInt32)actor << " was hit by a spell. Registering.\n";
#endif
			safeActors.push_back(actor);
			std::thread removalCallBack([&](Actor* actor){
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				std::lock_guard<std::mutex> lock(simpleLock);
				auto it = std::find(safeActors.begin(), safeActors.end(), actor);
				if (it != safeActors.end()){
#ifdef _DEBUG
					std::cout << std::hex << actor->formID << " being removed from vector: " << &safeActors << std::endl;
#endif
					safeActors.erase(it);
				}
#ifdef _DEBUG
				else
					std::cout << std::hex << actor->formID << " not found on block list despite registered!" << std::endl;
#endif
			}, actor);
			removalCallBack.detach();
			return true;
		}

		void REQ_Impact::GetStaggerStrength(){
			staggerStrength /= (akVictim->actorValueOwner.GetCurrent(kAVStamina) + akVictim->actorValueOwner.GetCurrent(kAVHealth)) / 200;
			staggerStrength /= pow(akVictim->actorValueOwner.GetCurrent(kAVMass),2);
		}
		float REQ_Impact::StaggerChance(){
			float ar = akVictim->actorValueOwner.GetCurrent(kAVDamageResist) + 100.0;
			float threshold = ArmorThresholds[attackType];
			if (bash)
				threshold *= 1.5;
			float chance = pow((threshold / ar), 2.0) * staggerStrength;
			if (block){
				float skill = akVictim->actorValueOwner.GetCurrent(kAVBlock) + 50.0;
				if (akVictim->processManager){
					TESForm* leftHandItem = akVictim->processManager->equippedObject[ActorProcessManager::kEquippedHand_Left];
					if (leftHandItem && leftHandItem->IsArmor())
						skill *= 2.0;
					chance *= 50.0 / skill;
				}
			}
#ifdef _DEBUG
			std::cout << "Stagger Strength: " << staggerStrength << " Stagger Chance: " << chance << std::endl;
#endif
			return chance;
		}
		void REQ_Impact::StaggerVictim(){
			SpellItem* staggerSpell = reinterpret_cast<SpellItem*>(Requiem::FormContainer::Forms[Requiem::FormContainer::StaggerSpell]);
			staggerSpell->effectItemList[0]->magnitude = eventRequiem::staggerPow[attackType] * staggerStrength;
#ifdef _DEBUG
			std::cout << "Casting Stagger Spell: " << std::hex << staggerSpell->formID <<
				std::dec << " Magnitude: " << staggerSpell->effectItemList[0]->magnitude << std::endl;
#endif
			UInt32 errorCode;
			CastSpell(staggerSpell, caster, akVictim, errorCode, (*g_skyrimVM)->GetClassRegistry(), 0);
			//PapyrusCastSpell((*g_skyrimVM)->GetClassRegistry(), 0, (SpellItem*)Forms[type], akVictim, akVictim);
		}
		void REQ_Impact::DmgVictimMagicka(){
			if (!CALL_MEMBER_FN_EX(&akVictim->magicTarget, HasMagicEffectKeyword)((BGSKeyword*)Requiem::FormContainer::Forms[Requiem::FormContainer::NegateMagDmgKeyword], 0)
				&& !CALL_MEMBER_FN(akVictim, HasPerk)((BGSPerk*)Common::FormContainer::Forms[Common::FormContainer::MagConcPerk])){
#ifdef _DEBUG
				std::cout << "Current victim's magicka: " << akVictim->actorValueOwner.GetCurrent(kAVMagicka) << ".\n";
#endif
				akVictim->actorValueOwner.Unk_06(2, kAVMagicka, -MagickaDamage[attackType]);
#ifdef _DEBUG
				std::cout << "After Damage: " << akVictim->actorValueOwner.GetCurrent(kAVMagicka) << ".\n";
#endif
			}
		}
		void REQ_Impact::DmgVictimStamina(){
			akVictim->actorValueOwner.Unk_06(2, kAVStamina, -MagickaDamage[attackType]);
		}
		void REQ_Impact::ResVictimStamina(){
			switch (attackType){
			case 5:
				if (CALL_MEMBER_FN(akVictim, HasPerk)((BGSPerk*)Common::FormContainer::Forms[Common::FormContainer::SpellBlockPerk]))
					akVictim->actorValueOwner.Unk_06(2, kAVStamina, 12.0);
				break;
			case 6:
				if (CALL_MEMBER_FN(akVictim, HasPerk)((BGSPerk*)Common::FormContainer::Forms[Common::FormContainer::SpellBlockPerk]))
					akVictim->actorValueOwner.Unk_06(2, kAVStamina, 0.5);
				break;
			default:
				if (CALL_MEMBER_FN(akVictim, HasPerk)((BGSPerk*)Common::FormContainer::Forms[Common::FormContainer::BlockPerk]))
					akVictim->actorValueOwner.Unk_06(2, kAVStamina, 12.0);
				break;
			}
		}
		void REQ_Impact::DisarmVictim(){
			if (akVictim->actorValueOwner.GetCurrent(kAVStamina) < 25){
				UInt32 errorCode;
				CastSpell((SpellItem*)Requiem::FormContainer::Forms[Requiem::FormContainer::StaggerLowStamina],
					caster, akVictim, errorCode, (*g_skyrimVM)->GetClassRegistry(), 0);
				CastSpell((SpellItem*)Requiem::FormContainer::Forms[Requiem::FormContainer::DisarmSpell + (attackType == 6 ? 1 : 0)],
					caster, akVictim, errorCode, (*g_skyrimVM)->GetClassRegistry(), 0);
				//PapyrusCastSpell((*g_skyrimVM)->GetClassRegistry(), 0,
				//(SpellItem*)Forms[DisarmSpell + (attackType == 6 ? 1 : 0)], akVictim, akVictim);
			}
		}
		void REQ_Impact::BlockRes(){
#ifdef _DEBUG
			std::cout << "Processing Blocking.";
#endif
			ResVictimStamina();
		}
		void REQ_Impact::DestroyBow(Actor* who){
			if (who->processManager){
				TESForm* rightHandItem = who->processManager->equippedObject[ActorProcessManager::kEquippedHand_Right];
				if (rightHandItem && rightHandItem->IsWeapon() &&
					papyrusForm::HasKeywordFormID(rightHandItem, Requiem::FormContainer::Forms[Requiem::FormContainer::BreakableBow])){
#ifdef _DEBUG
					std::cout << std::hex << "Destroying bow. Weap: " << rightHandItem->formID << std::endl;
#endif
					UInt8 itemType = reinterpret_cast<TESObjectWEAP*>(rightHandItem)->type();
					if (itemType == 7){
						UInt32 errorCode;
						CALL_MEMBER_FN_OFFSET(who, RemoveItem)(errorCode, rightHandItem, 1, 0, 0, 0, 0, 0);
						//showmessage
						AddItem(who, (TESForm*)Common::FormContainer::Forms[Common::FormContainer::Remnant0], 2, true, (*g_skyrimVM)->GetClassRegistry(), 0);
						AddItem(who, (TESForm*)Common::FormContainer::Forms[Common::FormContainer::Remnant1], 1, true, (*g_skyrimVM)->GetClassRegistry(), 0);
					}
					else if (itemType == 9){
						UInt32 errorCode;
						CALL_MEMBER_FN_OFFSET(who, RemoveItem)(errorCode, rightHandItem, 1, 0, 0, 0, 0, 0);
						AddItem(who, (TESForm*)Common::FormContainer::Forms[Common::FormContainer::Remnant0], 2, true, (*g_skyrimVM)->GetClassRegistry(), 0);
						AddItem(who, (TESForm*)Common::FormContainer::Forms[Common::FormContainer::Remnant2], 1, true, (*g_skyrimVM)->GetClassRegistry(), 0);
					}
				}
			}
		}
		SInt8 REQ_Impact::GetAttackType(TESForm*akSource, bool bHasProjectile){
			if (akSource){
#ifdef _DEBUG
				std::cout << "Attack source FormID: " << std::hex << akSource->formID << " formType: " << static_cast<UInt32>(akSource->formType) << std::endl;
#endif
				if (bash && akSource->formType == kFormType_Armor){
					staggerStrength = reinterpret_cast<TESObjectARMO*>(akSource)->weight.weight / 10;
					return OneHNormal;
				}
				else if (akSource->formType == kFormType_Weapon){
					SInt8 result = OneHNormal;
					TESObjectWEAP* weapon = reinterpret_cast<TESObjectWEAP*>(akSource);
					SInt8 type = weapon->type();
					if (type > 6){
						staggerStrength = weapon->weight.weight / 50;
						return Ranged;
					}
					else {
						if (type > 0){
							staggerStrength = weapon->weight.weight / 10;
							if (type > 4)
								return TwoHNormal;
						}
						staggerStrength = 0.1;
						return result;
					}
				}
				else if (akSource->formType == kFormType_Enchantment){
					EnchantmentItem* staff = reinterpret_cast<EnchantmentItem*>(akSource);
					if (staff->hostile > 0)
					switch (staff->data.unk08){//castType
					case 1:
						return Spell;
					case 2:
						return ConcSpell;
					}
					return -1;
				}
				else if (akSource->formType == kFormType_Spell){
					SpellItem* spell = reinterpret_cast<SpellItem*>(akSource);
					if (spell->hostile > 0)
						switch (spell->data.castType){
						case 1:
							return Spell;
						case 2:
							return ConcSpell;
					}
					return -1;
				}
			}
			staggerStrength = 1;
			return 0;
		}
		REQ_Impact::REQ_Impact(TESHitEvent* evn) : verified(false){
			if ((evn->target->formType == kFormType_Character)
				&& (akVictim = reinterpret_cast<Actor*>(evn->target)))
				if (evn->sourceFormID > 0 && LookupFormByID(evn->sourceFormID)){
					bash = (evn->flags & 4) != 0; block = (evn->flags & 8) != 0;
					if ((attackType = GetAttackType(LookupFormByID(evn->sourceFormID), evn->sourceFormID > 0)) > -1){
						caster = akVictim;
						if (power = ((evn->flags & 3) != 0)) attackType += 2;
						verified = true;
					}
				}
#ifdef _DEBUG
			std::cout << "Attack type: " << static_cast<SInt32>(attackType) << std::endl;
#endif
		}
		void REQ_Impact::operator()(){
			if (verified){
				if (attackType == Spell)
					if (!g_REQ_ImpactHandler.RegisterBlock(akVictim))
						return;
				if (block)BlockRes();
				if (!CALL_MEMBER_FN_EX(&akVictim->magicTarget, HasMagicEffectKeyword)((BGSKeyword*)Requiem::FormContainer::Forms[Requiem::FormContainer::NegateImpactKeyword], 0))
				{
					DmgVictimMagicka();
					if (!block)
						DmgVictimStamina();
					if (attackType < 5){
						GetStaggerStrength();
						if (staggerStrength > 0 && StaggerChance() > RandomFloat(0.0, 1.0))
							StaggerVictim();
					}
					DisarmVictim();
				}

				if (attackType < 4 && !bash && ((TESGlobal*)Requiem::FormContainer::Forms[Requiem::FormContainer::DisableBowBreak])->unk20 == 0)
					DestroyBow(akVictim);
			}
#ifdef _DEBUG
			else
				std::cout << "Not Verified. Exiting Processing.\n";
#endif
		}

		REQ_ImpactActorAgg::REQ_ImpactActorAgg(TESHitEvent* evn) : REQ_Impact(evn){
			caster = reinterpret_cast<Actor*>(evn->caster);

		}
		void REQ_ImpactActorAgg::BlockRes(){
			REQ_Impact::BlockRes();
			if (bash && CALL_MEMBER_FN(akVictim, HasPerk)(reinterpret_cast<BGSPerk*>(Common::FormContainer::Forms[Common::FormContainer::ShieldWall80])))
					DestroyBow(caster);
		}
		void REQ_ImpactActorAgg::GetStaggerStrength(){
			float massDiff = max(0.1, 1 + caster->actorValueOwner.GetCurrent(kAVMass) - akVictim->actorValueOwner.GetCurrent(kAVMass));
			float phyDiff = (max(min(
				(caster->actorValueOwner.GetCurrent(kAVStamina) - akVictim->actorValueOwner.GetCurrent(kAVStamina)
				+ caster->actorValueOwner.GetCurrent(kAVHealth) - akVictim->actorValueOwner.GetCurrent(kAVHealth)),-100),100)
				+100)*0.0095+0.1;
			staggerStrength *= massDiff * phyDiff;
		}

		EventResult REQ_ImpactHandler::ReceiveEvent(TESHitEvent* evn, EventDispatcher<TESHitEvent>* dispatcher){
#ifdef _DEBUG
			std::cout << "---Processing Req hit event---\n" << "Target: " << std::hex << (evn->target ? evn->target->formID : 0)
				<< " Type: " << (evn->target ? static_cast<UInt32>(evn->target->formType) : 0) << " Caster: " <<
				(evn->caster ? evn->caster->formID : 0) << " Type: " << (evn->caster ? static_cast<UInt32>(evn->caster->formType) : 0)
				<< " Source: " << evn->sourceFormID << " Projectile: " << evn->projectileFormID <<
				" Flags: " << std::bitset<8>(evn->flags) << std::endl;
#endif
			if (evn->caster && evn->caster->formType == kFormType_Character){
				REQ_ImpactActorAgg impact(evn);
				impact();
			}
			else{
				REQ_Impact impact(evn);
				impact();
			}

#ifdef _DEBUG
			std::cout << "---Ending REQ_Impact Processing---" << std::endl;
#endif
			return kEvent_Continue;
		}
	}
}