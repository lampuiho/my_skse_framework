#include "Duel.h"
#include "skse\GameRTTI.h"
#include "skse\Utilities.h"
#include "skyrim_framework\common_const.h"
#include "skyrim_framework\MySkseExtension\MyPapyrusForm.h"
#include <iterator>
#include <memory>

#ifdef _DEBUG
#include <iostream>
#include <iomanip>
#endif

namespace Duel{
	Duel_ImpactHandler g_Duel_ImpactHandler;

	template <typename T>
	void __fastcall ExtendAI(T& t){
		if (t.vicHPerc > 0.0 && t.vicHPerc < 0.5){
			float vicMorale = static_cast<float>(t.akAgHPerc - t.vicHPerc);
#ifdef _DEBUG
			std::cout << "---Processing extended ai---\nAttacker health: " << t.akAgHPerc << ".\n"
				<< "Target health: " << t.vicHPerc << ".\n";
#endif
			if (vicMorale > 0){
				UInt8 FleeSpellMag = DuelFleeSpell20;
				if (vicMorale <= 0.1)
					FleeSpellMag = DuelFleeSpell;
				else if (vicMorale <= 0.25)
					FleeSpellMag = DuelFleeSpell10;
				else if (vicMorale <= 0.4)
					FleeSpellMag = DuelFleeSpell15;

				SpellItem* fleeSpell = reinterpret_cast<SpellItem*>(Duel::FormContainer::Forms[Duel::FormContainer::DuelFlee]);
				fleeSpell->effectItemList[0]->magnitude = fleePow[FleeSpellMag];
				fleeSpell->effectItemList[0]->duration = fleeDuration[FleeSpellMag];
#ifdef _DEBUG
				std::cout << "Casting Stagger Spell: " << std::hex << fleeSpell->formID <<
					std::dec << "\nMagnitude: " << fleeSpell->effectItemList[0]->magnitude <<
					"\nDuration: " << fleeSpell->effectItemList[0]->duration << std::endl;
				//CastSpell((SpellItem*)(Forms[testSpell]), t.akAggressor, t.akVictim, errorCode, (*g_skyrimVM)->GetClassRegistry(), 0);
#endif
				UInt32 errorCode;
				CastSpell(fleeSpell, t.akAggressor, t.akVictim, errorCode, (*g_skyrimVM)->GetClassRegistry(), 0);
				//PapyrusCastSpell((*g_skyrimVM)->GetClassRegistry(), 0, (SpellItem*)(Forms[FleeSpell]), t.akAggressor, t.akVictim);
			}
		}
	}

	Duel_Impact::Duel_Impact(Actor* i_akAggressor, Actor* i_akVictim)
		: akAggressor(i_akAggressor), akVictim(i_akVictim){}
	void Duel_Impact::Apply(){
		if (Initialise()){
			Impact();
		}
	}
	bool Duel_Impact::Initialise(){
		if ((vicHPerc = CALL_MEMBER_FN_EX(akVictim, GetActorValuePercentage)(kAVHealth)) > 0.0){
			vicSPerc = CALL_MEMBER_FN_EX(akVictim, GetActorValuePercentage)(kAVStamina);
			vicAr = akVictim->actorValueOwner.GetCurrent(kAVDamageResist);
			return true;
		}
#ifdef _DEBUG
		else
			std::cout << "Target health less than or equal to 0, exit.\n";
#endif
		return false;
	}

	Duel_ArrowImpact::Duel_ArrowImpact(Actor* akAggressor, Actor* akVictim) : Duel_Impact(akAggressor, akVictim){}
	void Duel_ArrowImpact::Impact(){
		float impactResistance = vicAr;
		impactResistance = pow(min(impactResistance, 537), 0.6223392);
		impactResistance += (vicSPerc + vicHPerc) * 25;
		float impactStrength = RandomFloat(0.0, 100.0);
		if (impactStrength >= impactResistance){
			float staggerMag = impactStrength - impactResistance;
			//float stamina = akVictim->actorValueOwner.GetCurrent(26);
			//stamina = min(stamina, staggerMag);
			akVictim->actorValueOwner.Unk_06(2, 26, -staggerMag);
#ifdef _DEBUG
			std::cout << "---Processing arrow staggering---\nstaggerMag:" << staggerMag << ".\n";
#endif
			if (staggerMag > 20){
				UInt8 staggerType = DuelStaggerSwordSpell;
				if (staggerMag > 50) staggerType = DuelStaggerGreatSwordSpell;
				else if (staggerMag > 40) staggerType = DuelStaggerMaceSpell;
				else if (staggerMag > 30) staggerType = DuelStaggerWarAxeSpell;

				SpellItem* staggerSpell = (SpellItem*)(Duel::FormContainer::Forms[Duel::FormContainer::DuelStagger]);
				staggerSpell->effectItemList[0]->magnitude = Duel::staggerPow[staggerType];
#ifdef _DEBUG
				std::cout << "Casting Stagger Spell: " << std::hex << staggerSpell->formID <<
					std::dec << " Magnitude: " << staggerSpell->effectItemList[0]->magnitude << std::endl;
#endif
				UInt32 errorCode;
				CastSpell(staggerSpell, akAggressor, akVictim, errorCode, (*g_skyrimVM)->GetClassRegistry(), 0);
				//PapyrusCastSpell((*g_skyrimVM)->GetClassRegistry(), 0, (SpellItem*)(Forms[staggerType]), akAggressor, akVictim);
			}
		}
	}

	Duel_ArrowImpactAI::Duel_ArrowImpactAI(Actor* akAggressor, Actor* akVictim) : Duel_ArrowImpact(akAggressor, akVictim){}
	bool Duel_ArrowImpactAI::Initialise(){
		if (Duel_Impact::Initialise() && (akAgHPerc = CALL_MEMBER_FN_EX(akAggressor, GetActorValuePercentage)(kAVHealth)) > 0.0)
			return true;
		return false;
	}
	void Duel_ArrowImpactAI::Impact(){
		Duel_ArrowImpact::Impact();
		ExtendAI(*this);
	}

	Duel_MeleeImpact::Duel_MeleeImpact(Actor* akAggressor, Actor* akVictim, TESObjectWEAP* i_akSource, UInt8 i_akSourceType, bool i_abPowerAttack)
		: Duel_Impact(akAggressor, akVictim), akSourceType(i_akSourceType), akSource(i_akSource), abPowerAttack(i_abPowerAttack), chanceMultiplier(0.5){}
	Duel_MeleeImpact::Duel_MeleeImpact(Actor* akAggressor, Actor* akVictim, TESObjectWEAP* i_akSource, UInt8 i_akSourceType, bool i_abPowerAttack, float chanceM)
		: Duel_Impact(akAggressor, akVictim), akSource(i_akSource), akSourceType(i_akSourceType), abPowerAttack(i_abPowerAttack), chanceMultiplier(chanceM){}
	bool Duel_MeleeImpact::Initialise(){
		if (Duel_Impact::Initialise())
			if (vicHPerc > 0.01 &&
				(vicLevel = CALL_MEMBER_FN(akVictim, GetLevel)()) &&
				(akAgLevel = CALL_MEMBER_FN(akAggressor, GetLevel)()) &&
				(akAgHPerc = CALL_MEMBER_FN_EX(akAggressor, GetActorValuePercentage)(kAVHealth)) > 0.0)
				return true;

#ifdef _DEBUG
		std::cout << "Exiting Duel_MeleeImpact initialisation, logging variables:\nvicHPerc: " 
			<< vicHPerc << ".\nvicLevel: " << vicLevel << ".\nakAgLevel: " << akAgLevel << ".\nakAgHPerc: " << akAgHPerc;
#endif
		return false;
	}
	void Duel_MeleeImpact::Impact(){		
		if (CalChance() >= RandomFloat(0.0, 100.0)){
			UInt8 staggerType;
			if (akSourceType == 6 && papyrusForm::HasKeywordFormID(akSource, Common::FormContainer::Forms[Common::FormContainer::WeapTypeWarhammer]))
				staggerType = DuelStaggerWarHammerSpell;
			else
				staggerType = akSourceType;

			SpellItem* staggerSpell = (SpellItem*)(Duel::FormContainer::Forms[Duel::FormContainer::DuelStagger]);
			staggerSpell->effectItemList[0]->magnitude = Duel::staggerPow[staggerType];
#ifdef _DEBUG
			std::cout << "Casting Stagger Spell: " << std::hex << staggerSpell->formID <<
				std::dec << " Magnitude: " << staggerSpell->effectItemList[0]->magnitude << std::endl;
#endif
			UInt32 errorCode;
			CastSpell(staggerSpell, akAggressor, akVictim, errorCode, (*g_skyrimVM)->GetClassRegistry(), 0);
			//PapyrusCastSpell((*g_skyrimVM)->GetClassRegistry(), 0, (SpellItem*)(Forms[staggerType]), akAggressor, akVictim);
			hardcoreDmg();
		}
	}
	float Duel_MeleeImpact::CalChance(){
		float aggStrength = pow(1 + akSourceType, 0.58) * (1 + akAgHPerc);
		float impactResist = pow(vicAr, 0.5394309759140015) * vicSPerc * 1.8;
		float levelDif = pow(akAgLevel - vicLevel, 0.2);
		if (akAgLevel < vicLevel)
			levelDif = -levelDif;
		float chance = (aggStrength + levelDif) * 25 - impactResist;
		if (abPowerAttack)
			chance += 100;
		chance *= chanceMultiplier;
#ifdef _DEBUG
		std::cout << "---Processing hardcore hit chance---"
			<< "Aggressor adjusted strength: " << aggStrength << ".\n"
			<< "Impact resistance: " << impactResist << ".\n"
			<< "Level Difference: " << levelDif << ".\n"
			<< "Is power attack: " << abPowerAttack << ".\n"
			<< "Hardcore hit chance multiplier: " << chanceMultiplier << ".\n" 
			<< "Hardcore hit chance(uncapped): " << chance << ".\n";
#endif
		return max(min(static_cast<float>(chance), 100), 1);
	}
	float Duel_MeleeImpact::CalAr(){
		float impactResist = vicHPerc*100.0;
		impactResist += vicAr;
		impactResist += RandomFloat(impactResist * 0.15, impactResist * 1.5);
		impactResist /= 4;
#ifdef _DEBUG
		std::cout << "ArmorDefense: " << impactResist << ".\n";
#endif
		return impactResist;
	}
	void Duel_MeleeImpact::hardcoreDmg(){
		float bleedingChances = RandomFloat(0.0, 2.0) + akSourceType * 0.1;
#ifdef _DEBUG
		std::cout << "---Processing hardcore damage---\nBleeding Chance: " << bleedingChances << ".\n"
			<< "Current Health: " << vicHPerc << ".\n";
#endif
		if (bleedingChances > 1.25){
			bleedingChances = RandomFloat(0.0, vicHPerc * 100.0) * abPowerAttack ? 0.3 : 0.25;
			akVictim->actorValueOwner.Unk_06(2, 24, -bleedingChances);
			vicHPerc = CALL_MEMBER_FN_EX(akVictim, GetActorValuePercentage)(24);
#ifdef _DEBUG
			std::cout << "> 1.25, bleeding: " << bleedingChances << " hp.\n"
				<< "Health after bleeding: " << vicHPerc << ".\n";;
#endif
		}
		bleedingChances = CalAr() / akSourceType;
		bleedingChances *= 0.2;
#ifdef _DEBUG
		std::cout << "Adjusted stamina resist: " << bleedingChances << ".\n"
			<< "Current Stamina: " << vicSPerc << ".\n";;
#endif
		bleedingChances = max(1.0, bleedingChances);
		bleedingChances = RandomFloat(0.0, 200.0) / bleedingChances;
		akVictim->actorValueOwner.Unk_06(2, 26, -bleedingChances);
		vicSPerc = CALL_MEMBER_FN_EX(akVictim, GetActorValuePercentage)(26);
#ifdef _DEBUG
		std::cout << "bleeding: " << bleedingChances << " stamina.\n"
			<< "Stamina after bleeding: " << vicSPerc << ".\n";;;
#endif
		if (vicSPerc < 0.1){
			SpellItem* staggerSpell = (SpellItem*)(Duel::FormContainer::Forms[Duel::FormContainer::DuelStagger]);
			staggerSpell->effectItemList[0]->magnitude = Duel::staggerPow[DuelStaggerWarHammerSpell];
			UInt32 errorCode;
			CastSpell(staggerSpell, akAggressor, akVictim, errorCode, (*g_skyrimVM)->GetClassRegistry(), 0);
#ifdef _DEBUG
			std::cout << "Stamina < 10%, staggering";
#endif
		}
	}

	Duel_MeleeImpactAI::Duel_MeleeImpactAI(Actor* akAggressor, Actor* akVictim, TESObjectWEAP* akSource, UInt8 akSourceType, bool abPowerAttack)
		: Duel_MeleeImpact(akAggressor, akVictim, akSource, akSourceType, abPowerAttack, 0.2){}
	void Duel_MeleeImpactAI::Impact(){
		Duel_MeleeImpact::Impact();
		ExtendAI(*this);
	}

	EventResult Duel_ImpactHandler::ReceiveEvent(TESHitEvent* evn, EventDispatcher<TESHitEvent>* dispatcher){
#ifdef _DEBUG
		std::cout << "---Processing new OnHitEvent---\n";
#endif
		bool notBlocked = (evn->flags & 8) == 0;
		if (notBlocked){
			std::unique_ptr<Duel_Impact> impact;
			Actor* akAggressor = DYNAMIC_CAST(evn->caster, TESObjectREFR, Actor);
			if (akAggressor){
				Actor* akVictim = DYNAMIC_CAST(evn->target, TESObjectREFR, Actor);
				if (akVictim){
					if (evn->sourceFormID > 0)
					{
						TESObjectWEAP* akSource = DYNAMIC_CAST(LookupFormByID((UInt32)evn->sourceFormID), TESForm, TESObjectWEAP);
						if (akSource){
							if (evn->projectileFormID > 0)
							{
#ifdef _DEBUG
								std::cout << "Attack source is projectile, processing using arrow algorithm.\n";
#endif
								if (akVictim == *g_thePlayer)
									impact.reset(new Duel_ArrowImpact(akAggressor, akVictim));
								else
									impact.reset(new Duel_ArrowImpactAI(akAggressor, akVictim));
							}
							else{
								UInt8 akSourceType = akSource->type();
								if (akSourceType != 0 && akSourceType != 2){
									if (akSourceType == 7 || akSourceType == 9){
										akSourceType = 1;
									}
									bool powerAttack = (evn->flags & 3) != 0;
									if (akVictim == *g_thePlayer)
										impact.reset(new Duel_MeleeImpact(akAggressor, akVictim, akSource, akSourceType, powerAttack));
									else
										impact.reset(new Duel_MeleeImpactAI(akAggressor, akVictim, akSource, akSourceType, powerAttack));
								}
#ifdef _DEBUG
								else
									std::cout << "Attack source is fist or dagger, exit.\n";
#endif
							}
							if (impact)
								impact->Apply();
						}
#ifdef _DEBUG
						else
							std::cout << "Attack source is not weapon, exit.\n";
#endif
					}
				}
#ifdef _DEBUG
				else
					std::cout << "Target is null or not an actor, exit.\n";
#endif
			}
#ifdef _DEBUG
			else
				std::cout << "Attacker is null or not an actor, exit.\n";
#endif
		}
#ifdef _DEBUG
		else
			std::cout << "Hit is blocked by Target, exit.\n";

		std::cout << "Exiting Duel hit event." << std::endl;
#endif
		return kEvent_Continue;
	}
}