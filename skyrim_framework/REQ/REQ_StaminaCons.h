#pragma once
//#include <mutex>
//#include <thread>
//#include <unordered_map>
#include "skse\PapyrusEvents.h"

class Actor;

namespace Requiem{
	namespace eventRequiem{
		/*struct BowDrawCost{
			std::unordered_map<Actor*, BowDrawCost*>::iterator it;
			bool bIsDraw; bool bExit; float cost;
#ifdef _DEBUG
			UInt32 count;
#endif
			BowDrawCost(std::unordered_map<Actor*, BowDrawCost*>::iterator it, float cost);
			void operator()();
		};
		class REQ_MenuHandler : public MenuEventHandler{
		public:
			virtual	EventResult	ReceiveEvent(MenuOpenCloseEvent* evn, EventDispatcher<MenuOpenCloseEvent>* dispatcher);
		};
		extern REQ_MenuHandler g_REQ_MenuHandler;
		*/
		class REQ_AttackConsHandler : public ActionEventHandler{
			/*	void RegisterBowAttack(Actor* actor, float cost);
			std::mutex simpleLock;
			std::unordered_map<Actor*, BowDrawCost*> active;*/
		public:
			virtual	EventResult	ReceiveEvent(SKSEActionEvent* evn, EventDispatcher<SKSEActionEvent>* dispatcher);
		};
		extern REQ_AttackConsHandler g_REQ_AttackConsHandler;
		

		void __fastcall JumpHook(Actor* actor);
	}
}

