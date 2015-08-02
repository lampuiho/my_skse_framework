#include "skyrim_framework\REQ\REQ_const.h"
#include "skyrim_framework\common_const.h"
#include "skyrim_framework\MySkseExtension\MyGameApi.h"

namespace Requiem{
	std::array<UInt32, FormContainer::DLC1FormCount> FormContainer::Forms{ { 0xFF32C8B2, 0xFF1944DE, 0xFF31DA4E, 0xFF482639, 0xFF72659B, 0xFF0835CB, 0xFF2E232E, 0xff757de9, 0xFF03433f, 0xFF36AD6B, 0xFF36AD6D, 0xff073741, 0xFF019822 } };

	const char* FormContainer::modNames[FormArrayCount] { "Requiem.esp", "Dawnguard.esm" };
	const UInt32 FormContainer::FormArrayEndingIndex[FormArrayCount] { FormCount, DLC1FormCount };

	namespace papyrusRequiem{
		const UInt8 attributeToMod[attributeModSize]{ kAVMagicResist, kAVMagickaRateMult, kAVDiseaseResist, kAVPoisonResist, kAVStaminaRateMult, kAVSpeedMult, kAVCarryWeight, kAVAttackDamageMult, kAVOneHandedMod, kAVTwoHandedMod, kAVUnarmedDamage };
		const float attributePreF[attributeModSize]{ 1, 8, 4, 4, 8, 5, 4, 0.04f, 4, 4, 3 };
		const float threshold[attributeModSize]{ 150, 100, 100, 140, 100, 125, 110, 150, 150, 150, 125 };
		const float attributeWeight[attributeModSize][3]{ { 0, 1, 0 }, { 0, 1, 0 }, { 0.4f, 0, 0.6f },
		{ 0.6f, 0, 0.4f }, { 0, 0, 1 }, { 0.2f, 0, 0.8f }, { 0.8f, 0, 0.2f }, { 0.2f, 0, 0.8f }, { 0.5f, 0, 0.5f }, 
		{ 0.8f, 0, 0.2f }, { 1, 0, 0 } };
		const float shoutthreshold[shoutModSize]{50, 5, 0};
		extern const float shoutcoef[shoutModSize]{0.005f, 0.02f, 0.05f};
		const char* shoutstats[shoutModSize]{"Times Shouted", "Words Of Power Learned", "Words Of Power Unlocked"};
		const std::array<std::vector<UInt32*>, 3> ammoWeightKeywords{ {
			{ &Common::FormContainer::Forms[Common::FormContainer::Forms::WeapMaterialWood]
			, &Common::FormContainer::Forms[Common::FormContainer::Forms::WeapMaterialElven]
			, &Common::FormContainer::Forms[Common::FormContainer::Forms::WeapMaterialGlass] }

			, { &Common::FormContainer::Forms[Common::FormContainer::Forms::WeapMaterialDwarven]
			, &Common::FormContainer::Forms[Common::FormContainer::Forms::WeapMaterialOrcish] }

			, { &Common::FormContainer::Forms[Common::FormContainer::Forms::WeapMaterialEbony]
			, &FormContainer::Forms[FormContainer::DLC1Forms::DLC1WeapMaterialDragonbone]
			, &Common::FormContainer::Forms[Common::FormContainer::Forms::WeapMaterialDaedric] } } };

		const UInt32 exceptionArrowID = 0x10b0a7;
		const std::array<std::pair<UInt16, float>, 6> armorGainMask{ { { 3, 0.025 }, { 4, 0.35 }, { 0x18, 0.05 }, { 0x180, 0.05 },
		{ 0x200, 0.1 }, { 0x1800, 0.025 } } };
		const std::array<char*, 10> raceSig{ { "Argonian", "Breton", "DarkElf", "HighElf", "Imperial", "Khajiit", "Nord", "Orc", "Redguard", "WoodElf" } };
	}
	namespace eventRequiem{
		const float staggerPow[AttackTypes]{0.2f, 0.4f, 0.5f, 1.0f, 0.1f, 0.0f, 0.0f};
		const float MagickaDamage[AttackTypes] { 10.0f, 20.0f, 20.0f, 35.0f, 10.0f, 15.0f, 0.5f };
		const float ArmorThresholds[AttackTypes] { 300.0f, 400.0f, 400.0f, 600.0f, 500.0f, 0.0f, 0.0f };
	}
}