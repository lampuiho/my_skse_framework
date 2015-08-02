#include <mutex>
#include "skse\GameEvents.h"
#include "skyrim_framework\MySkseExtension\MyGameApi.h"

class TESForm;
class Actor;
struct TESHitEvent;

namespace Requiem{
	namespace eventRequiem{
		class REQ_Impact{
			SInt8 GetAttackType(TESForm*akSource, bool bHasProjectile);
			float StaggerChance();
			void StaggerVictim();
			void DmgVictimMagicka();
			void DmgVictimStamina();
			void ResVictimStamina();
			void DisarmVictim();
		protected:
			Actor* akVictim; Actor* caster; float staggerStrength; SInt8 attackType; bool power, bash, block;
			void DestroyBow(Actor* who);
			virtual void BlockRes();
			virtual void GetStaggerStrength();
		public:
			bool verified;
			REQ_Impact(TESHitEvent* evn);
			void operator()();
		};

		class REQ_ImpactActorAgg : public REQ_Impact {
			void GetStaggerStrength();
			void BlockRes();
		public:
			REQ_ImpactActorAgg(TESHitEvent* evn);
		};

		class REQ_ImpactHandler : public BSTEventSink<TESHitEvent>{
		public:
			std::mutex simpleLock;
			std::vector<Actor*> safeActors;
			bool RegisterBlock(Actor* actor);
			virtual	EventResult ReceiveEvent(TESHitEvent* evn, EventDispatcher<TESHitEvent>* dispatcher);
		};

		extern REQ_ImpactHandler g_REQ_ImpactHandler;
	}
}