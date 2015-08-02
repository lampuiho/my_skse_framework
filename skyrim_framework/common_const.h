#include <array>

static const UInt32 kSerializationDataVersion = 1;
namespace Common{
	struct FormContainer{
		enum Forms{ MagConcPerk, BlockPerk, SpellBlockPerk, Remnant0, Remnant1, Remnant2, ShieldWall80, WeapMaterialWood, WeapMaterialIron, WeapMaterialSteel, WeapMaterialDwarven, WeapMaterialElven, WeapMaterialOrcish, WeapMaterialGlass, WeapMaterialEbony, WeapMaterialDaedric, ArmorNightingale, ArmorDarkBrotherhood, DBFullSet, TGFullSet, Juggernaut00, Juggernaut20, WeapTypeWarhammer, PlayerIsVampire, FormCount };
		static std::array<UInt32, FormCount> Forms;

		static const UInt32 FormArrayCount = 1;
		static const char* modNames[FormArrayCount];
		static const UInt32 FormArrayEndingIndex[FormArrayCount];
	};
}
