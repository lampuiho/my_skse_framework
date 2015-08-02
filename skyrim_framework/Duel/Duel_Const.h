#include <array>
namespace Duel{

	struct FormContainer{
		enum Forms{ DuelStagger, DuelFlee, FormCount };
		static std::array<UInt32, FormCount> Forms;
		static const UInt32 FormArrayCount = 1;
		static const char* modNames[FormArrayCount];
		static const UInt32 FormArrayEndingIndex[FormArrayCount];
	};

	enum staggerPow { DuelStaggerSwordSpell, DuelStaggerWarAxeSpell, DuelStaggerMaceSpell, DuelStaggerGreatSwordSpell, DuelStaggerBattleAxeSpell, DuelStaggerWarHammerSpell };
	enum fleePow { DuelFleeSpell, DuelFleeSpell10, DuelFleeSpell15, DuelFleeSpell20 };
	extern float staggerPow[];
	extern float fleePow[];
	extern float fleeDuration[];
}