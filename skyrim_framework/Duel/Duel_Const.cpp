#include "Duel_Const.h"

namespace Duel{
	std::array<UInt32, FormContainer::FormCount> FormContainer::Forms = { 0xFF002038, 0xFF00B1DE };
	const char* FormContainer::modNames[FormArrayCount] { "Duel - Combat Realism.esp" };
	const UInt32 FormContainer::FormArrayEndingIndex[FormArrayCount] { FormCount };
	float staggerPow[]{0.05f, 0.1f, 0.15f, 0.2f, 0.3f, 0.4f};
	extern float fleePow[]{40,60,80,100};
	extern float fleeDuration[]{10,20,30,40};
}