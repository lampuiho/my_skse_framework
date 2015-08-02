/*#include <vector>
#include "skyrim_framework\MySkseExtension\MyGameApi.h"
#include "skse\GameEvents.h"
class Actor; class EffectSetting;

namespace mslVT{
	extern const char* modName;
	extern std::vector<UInt32> BaseForms;
	extern std::vector<UInt32> Forms;
	class DiseasePoisonImmunity : public BSTEventSink<TESMagicEffectApplyEvent>{
		bool RemoveDisease(Actor* ref, EffectSetting* me);
		bool RemoveDiseasePoison(Actor* ref, EffectSetting* me);
	public:
		EventResult ReceiveEvent(TESMagicEffectApplyEvent * evn, EventDispatcher<TESMagicEffectApplyEvent> * dispatcher);
	};
	extern DiseasePoisonImmunity g_DiseasePoisonImmunity;
	}*/