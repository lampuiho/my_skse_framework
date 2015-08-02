#pragma once
#include "Utilities.h"
#include "skse\GameEvents.h"
#include "skse\GameReferences.h"
#include "skse\GameHandlers.h"
#include "skse\Hooks_Handlers.h"

class VMClassRegistry;
class SpellItem;

enum AV{
	kAVAggression,
	kAVConfidence,
	kAVEnergy,
	kAVMorality,
	kAVMood,
	kAVAssistance,
	kAVOneHanded,
	kAVTwoHanded,
	kAVMarksman,
	kAVBlock,
	kAVSmithing,
	kAVHeavyArmor,
	kAVLightArmor,
	kAVPickpocket,
	kAVLockpicking,
	kAVSneak,
	kAVAlchemy,
	kAVSpeechcraft,
	kAVAlteration,
	kAVConjuration,
	kAVDestruction,
	kAVIllusion,
	kAVRestoration,
	kAVEnchanting,
	kAVHealth,
	kAVMagicka,
	kAVStamina,
	kAVHealRate,
	kAVMagickaRate,
	kAVStaminaRate,
	kAVSpeedMult,
	kAVInventoryWeight,
	kAVCarryWeight,
	kAVCritChance,
	kAVMeleeDamage,
	kAVUnarmedDamage,
	kAVMass,
	kAVVoicePoints,
	kAVVoiceRate,
	kAVDamageResist,
	kAVPoisonResist,
	kAVFireResist,
	kAVElectricResist,
	kAVFrostResist,
	kAVMagicResist,
	kAVDiseaseResist,
	kAVPerceptionCondition,
	kAVEnduranceCondition,
	kAVLeftAttackCondition,
	kAVRightAttackCondition,
	kAVLeftMobilityCondition,
	kAVRightMobilityCondition,
	kAVBrainCondition,
	kAVParalysis,
	kAVInvisibility,
	kAVNightEye,
	kAVDetectLifeRange,
	kAVWaterBreathing,
	kAVWaterWalking,
	kAVIgnoreCrippledLimbs,
	kAVFame,
	kAVInfamy,
	kAVJumpingBonus,
	kAVWardPower,
	kAVRightItemCharge,
	kAVArmorPerks,
	kAVShieldPerks,
	kAVWardDeflection,
	kAVVariable01,
	kAVVariable02,
	kAVVariable03,
	kAVVariable04,
	kAVVariable05,
	kAVVariable06,
	kAVVariable07,
	kAVVariable08,
	kAVVariable09,
	kAVVariable10,
	kAVBowSpeedBonus,
	kAVFavorActive,
	kAVFavorsPerDay,
	kAVFavorsPerDayTimer,
	kAVLeftItemCharge,
	kAVAbsorbChance,
	kAVBlindness,
	kAVWeaponSpeedMult,
	kAVShoutRecoveryMult,
	kAVBowStaggerBonus,
	kAVTelekinesis,
	kAVFavorPointsBonus,
	kAVLastBribedIntimidated,
	kAVLastFlattered,
	kAVMovementNoiseMult,
	kAVBypassVendorStolenCheck,
	kAVBypassVendorKeywordCheck,
	kAVWaitingForPlayer,
	kAVOneHandedMod,
	kAVTwoHandedMod,
	kAVMarksmanMod,
	kAVBlockMod,
	kAVSmithingMod,
	kAVHeavyArmorMod,
	kAVLightArmorMod,
	kAVPickPocketMod,
	kAVLockpickingMod,
	kAVSneakMod,
	kAVAlchemyMod,
	kAVSpeechcraftMod,
	kAVAlterationMod,
	kAVConjurationMod,
	kAVDestructionMod,
	kAVIllusionMod,
	kAVRestorationMod,
	kAVEnchantingMod,
	kAVOneHandedSkillAdvance,
	kAVTwoHandedSkillAdvance,
	kAVMarksmanSkillAdvance,
	kAVBlockSkillAdvance,
	kAVSmithingSkillAdvance,
	kAVHeavyArmorSkillAdvance,
	kAVLightArmorSkillAdvance,
	kAVPickPocketSkillAdvance,
	kAVLockpickingSkillAdvance,
	kAVSneakSkillAdvance,
	kAVAlchemySkillAdvance,
	kAVSpeechcraftSkillAdvance,
	kAVAlterationSkillAdvance,
	kAVConjurationSkillAdvance,
	kAVDestructionSkillAdvance,
	kAVIllusionSkillAdvance,
	kAVRestorationSkillAdvance,
	kAVEnchantingSkillAdvance,
	kAVLeftWeaponSpeedMult,
	kAVDragonSouls,
	kAVCombatHealthRegenMult,
	kAVOneHandedPowerMod,
	kAVTwoHandedPowerMod,
	kAVMarksmanPowerMod,
	kAVBlockPowerMod,
	kAVSmithingPowerMod,
	kAVHeavyArmorPowerMod,
	kAVLightArmorPowerMod,
	kAVPickPocketPowerMod,
	kAVLockpickingPowerMod,
	kAVSneakPowerMod,
	kAVAlchemyPowerMod,
	kAVSpeechcraftPowerMod,
	kAVAlterationPowerMod,
	kAVConjurationPowerMod,
	kAVDestructionPowerMod,
	kAVIllusionPowerMod,
	kAVRestorationPowerMod,
	kAVEnchantingPowerMod,
	kAVDragonRend,
	kAVAttackDamageMult,
	kAVHealRateMult,
	kAVMagickaRateMult,
	kAVStaminaRateMult,
	kAVWerewolfPerks,
	kAVVampirePerks,
	kAVGrabActorOffset,
	kAVGrabbed,
	kAVDEPRECATED05,
	kAVReflectDamage
};

struct TESMagicEffectApplyEvent {
	TESObjectREFR* target; TESObjectREFR* caster; UInt32 spellFormID;
};
static EventDispatcher<TESMagicEffectApplyEvent>* g_magicEffectApplyEventDispatcher = (EventDispatcher<TESMagicEffectApplyEvent>*)0x012E5020;

/*
static const UInt32 JumpAnimEventHandler_vtbl = 0x10d57d4;
class JumpAnimEventHandler : public IHandlerFunctor<Actor, UInt32>
{
public:
	virtual ~JumpAnimEventHandler();
	virtual	UInt8	Process(Actor * form, UInt32 unk04);

	MEMBER_FN_PREFIX(JumpAnimEventHandler);
	DEFINE_MEMBER_FN(Process_Origin, UInt8, 0x781530, Actor * form, UInt32 unk04);
};
static const UInt32 JumpHandler_vtbl = 0x10d460c;
class JumpHandler
{
public:
	virtual ~JumpHandler();
	//void Process_Hook(float* unk, UInt32 unk04);

	MEMBER_FN_PREFIX(JumpHandler);
	DEFINE_MEMBER_FN(Process_Origin, void, 0x771800, float* unk, UInt32 unk04);
};

template <>
class BSTEventSink<CellAttachDetachEvent> {
public:
virtual ~BSTEventSink() {}
virtual	EventResult ReceiveEvent(CellAttachDetachEvent * evn, EventDispatcher<CellAttachDetachEvent> * dispatcher) = 0;
};
*/


template <>
class BSTEventSink<TESMagicEffectApplyEvent> {
public:
	virtual ~BSTEventSink() {}
	virtual	EventResult ReceiveEvent(TESMagicEffectApplyEvent * evn, EventDispatcher<TESMagicEffectApplyEvent> * dispatcher) = 0;
};


namespace ActionEvent{
	enum {
		kType_ArrowAttach = 11,
		kType_ArrowDetach = 12
	};
}


template<class T, UInt32 type, UInt32 slot>
UInt8 IHandlerFunctor<Actor, UInt32>::ProcessAction(Actor * actor, UInt32 unk04)
{
	TESForm * object = slot == SKSEActionEvent::kSlot_Voice ? actor->equippedShout : actor->GetEquippedObject(slot == SKSEActionEvent::kSlot_Left);
	SKSEActionEvent evn(type, actor, object, slot);
	g_actionEventDispatcher.SendEvent(&evn);
	return CALL_MEMBER_FN(static_cast<T*>(this), Process_Origin)(actor, unk04);
}

static const UInt32 ArrowAttachHandler_vtbl = 0x10d5648;
class ArrowAttachHandler : public IHandlerFunctor<Actor, UInt32>
{
public:
	virtual ~ArrowAttachHandler();
	virtual	UInt8	Process(Actor * form, UInt32 unk04);

	MEMBER_FN_PREFIX(ArrowAttachHandler);
	DEFINE_MEMBER_FN(Process_Origin, UInt8, 0x780BF0, Actor * form, UInt32 unk04);
};

static const UInt32 ArrowDetachHandler_vtbl = 0x10d5654;
class ArrowDetachHandler : public IHandlerFunctor<Actor, UInt32>
{
public:
	virtual ~ArrowDetachHandler();
	virtual	UInt8	Process(Actor * form, UInt32 unk04);

	MEMBER_FN_PREFIX(ArrowDetachHandler);
	DEFINE_MEMBER_FN(Process_Origin, UInt8, 0x780C90, Actor * form, UInt32 unk04);
};

typedef void(_fastcall *_OnInit)(UInt32, UInt32, UInt32);
static const _OnInit OnInit = (_OnInit)0xc4e400;
typedef void(__thiscall *_JumpOriginal)(Actor* actor);
static const _JumpOriginal JumpOriginal = (_JumpOriginal)0x6b5960;
typedef bool(*_IsOnMount)(VMClassRegistry*, UInt32, Actor*);
static const _IsOnMount IsOnMount = (_IsOnMount)0x8dcb40;
typedef void(*_Dispel)(ActiveEffect*, bool);
static const _Dispel Dispel = (_Dispel)0x657160;
typedef bool(*_QueryStat)(BSFixedString*, UInt32&);
static const _QueryStat QueryStat = (_QueryStat)0x487f40;
//typedef bool(*_PapyrusAddSpell)(VMClassRegistry*, UInt32, Actor*, SpellItem*, bool);
//static const _PapyrusAddSpell PapyrusAddSpell = (_PapyrusAddSpell)0x8db300;
typedef bool(*_PapyrusRemoveSpell)(VMClassRegistry*, UInt32, Actor*, SpellItem*);
static const _PapyrusRemoveSpell PapyrusRemoveSpell = (_PapyrusRemoveSpell)0x8e0db0;
static const _PapyrusRemoveSpell PapyrusDispelSpell = (_PapyrusRemoveSpell)0x8e0e00;
typedef TESObjectREFR*(*_PapyrusGetReference)(VMClassRegistry*, UInt32, BGSRefAlias*);
static const _PapyrusGetReference PapyrusGetReference = (_PapyrusGetReference)0x8ec3c0;
typedef void(*_PapyrusCastSpell)(VMClassRegistry*, UInt32, SpellItem*, TESObjectREFR*, TESObjectREFR*);
static const _PapyrusCastSpell PapyrusCastSpell = (_PapyrusCastSpell)0x8f9d90;
typedef void(*_CastSpell)(SpellItem*, TESObjectREFR*, TESObjectREFR*, UInt32&, VMClassRegistry*, UInt32);
static const _CastSpell CastSpell = (_CastSpell)0x8f9a80;
typedef void(*_AddItem)(TESObjectREFR*, TESForm*, UInt32, bool, VMClassRegistry*, UInt32);
static const _AddItem AddItem = (_AddItem)0x90c180;
typedef BGSKeywordForm*(*_GetKeywordForm)(TESForm*);
static const _GetKeywordForm GetKeywordForm = (_GetKeywordForm)0x44B2D0;
typedef UInt8(*_GetHandItemType)(TESForm*);
static const _GetHandItemType GetHandItemType = (_GetHandItemType)0x44AB30;
typedef void(*_EvaluatePackage)(Actor*, bool, float);
static const _EvaluatePackage EvaluatePackage = (_EvaluatePackage)0x6BE790;
typedef Actor*(*_GetTargetActor)(void*);
static const _GetTargetActor GetTargetActor = (_GetTargetActor)0x662870;

DEFINE_MEMBER_FN_LONG(BGSListForm, GetAt, TESForm*, 0x4FAD60, UInt32);
DEFINE_MEMBER_FN_LONG(MagicTarget, HasMagicEffectKeyword, bool, 0x6635B0, BGSKeyword*, UInt32);
DEFINE_MEMBER_FN_LONG(Actor, GetActorValuePercentage, float, 0x6A8240, UInt8);
DEFINE_MEMBER_FN_LONG(Actor, AddSpell, bool, 0x6ec120, SpellItem*);
DEFINE_MEMBER_FN_OFFSET(TESObjectREFR, RemoveItem, UInt32*, 0x158, UInt32& error, TESForm* akBaseItem, UInt32 aiCount, UInt32 hasTarget, void* xMem, TESObjectREFR* targetCont, UInt32 unk8, UInt32 unk9);
