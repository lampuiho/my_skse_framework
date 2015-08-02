#include "skse\PapyrusArgs.h"
#include "skse\GameReferences.h"
#include "skse\GameObjects.h"
#include "skse\GameEvents.h"
#include "skse\PapyrusNativeFunctions.h"
#include "skyrim_framework\MySkseExtension\MyGameApi.h"
#include "skyrim_framework\FormInitialiser.h"
#include "Duel_Const.h"

namespace Duel{

	class Duel_ImpactHandler : public BSTEventSink<TESHitEvent>{
	public:
		virtual	EventResult ReceiveEvent(TESHitEvent* evn, EventDispatcher<TESHitEvent>* dispatcher);
	};
	class Duel_Impact{
		virtual void Impact() = 0;
	protected:
		virtual bool Initialise();
	public:
		float vicHPerc; float vicSPerc; float vicAr;
		Actor* akVictim; Actor* akAggressor;

		Duel_Impact(Actor* i_akAggressor, Actor* i_akVictim);
		void Apply();
	};

	template <typename T>
	void __fastcall ExtendAI(T& t);

	class Duel_ArrowImpact : public Duel_Impact{
	protected:
		virtual void Impact();
	public:
		Duel_ArrowImpact(Actor* i_akAggressor, Actor* i_akVictim);
	};
	class Duel_ArrowImpactAI : public Duel_ArrowImpact{
	protected:
		bool Initialise();
		void Impact();
	public:
		float akAgHPerc;
		Duel_ArrowImpactAI(Actor* i_akAggressor, Actor* i_akVictim);
	};

	class Duel_MeleeImpact : public Duel_Impact{
		TESObjectWEAP* akSource;  bool abPowerAttack;
		UInt16 vicLevel; const float chanceMultiplier;
		UInt16 akAgLevel; UInt8 akSourceType;

		float CalAr();
		float CalChance();
		bool Initialise();
		void hardcoreDmg();
	protected:
		void Impact();
		Duel_MeleeImpact(Actor* i_akAggressor, Actor* i_akVictim, TESObjectWEAP* i_akSource, UInt8 i_akSourceType, bool abPowerAttack, float chanceM);
	public:
		float akAgHPerc;
		Duel_MeleeImpact(Actor* i_akAggressor, Actor* i_akVictim, TESObjectWEAP* i_akSource, UInt8 i_akSourceType, bool abPowerAttack);
	};
	class Duel_MeleeImpactAI : public Duel_MeleeImpact{
		void Impact();
	public:
		Duel_MeleeImpactAI(Actor* i_akAggressor, Actor* i_akVictim, TESObjectWEAP* akSource, UInt8 i_akSourceType, bool abPowerAttack);
	};

	extern Duel_ImpactHandler g_Duel_ImpactHandler;
}