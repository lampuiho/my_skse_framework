#pragma once
#include <unordered_map>
#include <vector>
#include <array>

namespace Requiem{
	struct FormContainer{
		enum RequiemForms{ StaggerSpell, DisarmSpell, DisarmConcSpell, NegateImpactKeyword, NegateMagDmgKeyword, DisableBowBreak, BreakableBow, req_massabilities, StaggerLowStamina, REQ_Atmosphere_NoRunningExhaustion, REQ_Skills_NoHeavyArmorExhaustion, REQ_FatigueBow, FormCount };
	enum DLC1Forms{ DLC1WeapMaterialDragonbone = FormCount, DLC1FormCount };
	static std::array<UInt32, DLC1FormCount> Forms;

	static const UInt32 FormArrayCount = 2;
	static const char* modNames[FormArrayCount];
	static const UInt32 FormArrayEndingIndex[FormArrayCount];
	};

	namespace papyrusRequiem{
#define attributeModSize 11
#define shoutModSize 3
		extern const UInt8 attributeToMod[attributeModSize];
		extern const float attributePreF[attributeModSize];
		extern const float threshold[attributeModSize];
		extern const float attributeWeight[attributeModSize][shoutModSize];
		extern const float shoutthreshold[shoutModSize];
		extern const float shoutcoef[shoutModSize];
		extern const char* shoutstats[shoutModSize];
		extern const std::array<std::pair<UInt16, float>, 6> armorGainMask;
		extern const std::array<std::vector<UInt32*>, 3> ammoWeightKeywords;
		extern const UInt32 exceptionArrowID;
		extern const std::array<char*, 10> raceSig;
	}
	namespace eventRequiem{
#define AttackTypes 7
		enum attackType{ OneHNormal = 0, TwoHNormal, OneHPower, TwoHPower, Ranged, Spell, ConcSpell };
		extern const float staggerPow[AttackTypes];
		extern const float MagickaDamage[AttackTypes];
		extern const float ArmorThresholds[AttackTypes];
	}
}