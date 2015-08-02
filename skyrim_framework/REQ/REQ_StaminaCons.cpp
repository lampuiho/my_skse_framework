#include "skyrim_framework\REQ\REQ_StaminaCons.h"
#include "skse\GameObjects.h"
#include "skse\GameReferences.h"
#include "skyrim_framework\MySkseExtension\MyGameApi.h"
#include "skyrim_framework\REQ\REQ_const.h"

namespace Requiem{
	namespace eventRequiem{
/*		static const UInt32 timeslice = 10;

		BowDrawCost::BowDrawCost(std::unordered_map<Actor*, BowDrawCost*>::iterator it, float cost) : it(it),
			bExit(false), bIsDraw(true), cost(cost)
#ifdef _DEBUG
			, count(1)
#endif
		{}
		void BowDrawCost::operator()(){
			while (bIsDraw){
				bool dead = it->first->Unk_99(1);
#ifdef _DEBUG
				if (--count == 0)
				{
					count = 10;
					std::cout << "Damaging actor: " << std::hex << it->first->formID << " stamina: " << cost
						<< " Death state: " << dead << std::endl;
				}
#endif
				if (dead)
					break;
				else
					it->first->actorValueOwner.Unk_06(2, kAVStamina, cost);

				std::this_thread::sleep_for(std::chrono::milliseconds(1000 / timeslice));
			}
		}

		struct BowDrawThread{
			REQ_AttackConsHandler* attackHandler;
			BowDrawCost* cost;
			BowDrawThread(REQ_AttackConsHandler* attackHandler, BowDrawCost* cost) :attackHandler(attackHandler)
				, cost(cost){}
			void operator()(){
				cost->operator()();
#ifdef _DEBUG
				auto formID = cost->it->first->formID;
				std::cout << "ThreadID exiting: " << std::hex << std::this_thread::get_id();
#endif
				if (cost->bExit){

					std::lock_guard<std::mutex> lock(attackHandler->simpleLock);
					auto temp = attackHandler->active.erase(cost->it);
#ifdef _DEBUG
					if (temp != attackHandler->active.end() || attackHandler->active.size() == 0){
						std::cout << std::hex << "\nBow stamina cost unregistered for actor: "
							<< formID;
					}
					std::cout << "\nBowDrawCost deleted: " << cost << std::endl;
#endif
					delete(cost);
				}
#ifdef _DEBUG
				else
					std::cout << "\nBow stamina cost not unregistered for actor: " << formID << std::endl;
#endif
			}
		};

		void REQ_AttackConsHandler::RegisterBowAttack(Actor* actor, float cost){
#ifdef _DEBUG
			std::cout << "Registering actor: " << std::hex << actor->formID << " Damage: " << cost << std::endl;
#endif
			std::lock_guard<std::mutex> lock(simpleLock);
			auto ret = active.emplace(actor, reinterpret_cast<BowDrawCost*>(NULL));
			if (ret.second){
				auto it = ret.first;
				BowDrawCost *bowDrawCost = new BowDrawCost(it, cost);
#ifdef _DEBUG
				std::cout << std::hex << "\nBowDrawCost created: " << bowDrawCost << std::endl;
#endif
				it->second = bowDrawCost;
				std::thread newBowDrawCost(BowDrawThread(&g_REQ_AttackConsHandler, bowDrawCost));
#ifdef _DEBUG
				std::cout << std::hex << "Thread created: " << newBowDrawCost.get_id() << std::endl;
#endif
				newBowDrawCost.detach();
			}
#ifdef _DEBUG
			else
				std::cout << "Actor doing continuous bow attack" << std::endl;
#endif
		}
		*/
		EventResult	REQ_AttackConsHandler::ReceiveEvent(SKSEActionEvent* evn, EventDispatcher<SKSEActionEvent>* dispatcher){
				float staminaMod;
				float mass;
				float skill;
				switch (evn->type){
				case SKSEActionEvent::kType_WeaponSwing:
#ifdef _DEBUG
					std::cout << std::hex
						<< "---Proccessing Attack Stamina Consumption on Actor: " << evn->actor->formID
						<< "---\n";
#endif
					mass = evn->actor->actorValueOwner.GetCurrent(kAVMass);
					staminaMod = -8.0;
					if (evn->sourceForm && reinterpret_cast<TESObjectWEAP*>(evn->sourceForm)->type() > 5){
						skill = evn->actor->actorValueOwner.GetCurrent(kAVTwoHanded);
						staminaMod += 0.02 * skill;
#ifdef _DEBUG
						std::cout << "Twohanded Skill: " << skill << std::endl;
#endif
					}
					else{
						skill = evn->actor->actorValueOwner.GetCurrent(kAVOneHanded);
						staminaMod += 0.02 * skill;
#ifdef _DEBUG
						std::cout << "Onehanded Skill: " << skill << std::endl;
#endif
					}
					evn->actor->actorValueOwner.Unk_06(2, kAVStamina, staminaMod * mass);
					break;
				case ActionEvent::kType_ArrowAttach:
#ifdef _DEBUG
					std::cout << std::hex
						<< "---Proccessing Attack Stamina Consumption on Actor: " << evn->actor->formID
						<< "---\n";
#endif
					skill = evn->actor->actorValueOwner.GetCurrent(kAVMarksman);
					staminaMod = 4 - 0.02 * skill;
#ifdef _DEBUG
					std::cout << "Archery Skill: " << skill << "Stamina cost: " << staminaMod << std::endl;
#endif
					SpellItem* costSpell = reinterpret_cast<SpellItem*>(FormContainer::
						Forms[FormContainer::REQ_FatigueBow]);
					costSpell->effectItemList[0]->magnitude = staminaMod;
					CALL_MEMBER_FN_EX(evn->actor, AddSpell)(costSpell);
					break;
					/*staminaMod /= timeslice;
					mass = evn->actor->actorValueOwner.GetCurrent(kAVMass);
					if (reinterpret_cast<TESObjectWEAP*>(evn->sourceForm)->type() == 9)
						mass /= 4;
					RegisterBowAttack(evn->actor, staminaMod);
					break;
				case ActionEvent::kType_ArrowDetach:
#ifdef _DEBUG
					std::cout << "Arrow Detaching for Actor: " << std::hex << evn->actor->formID;
#endif
					std::lock_guard<std::mutex> lock(simpleLock);
					auto it = active.find(evn->actor);
					if (it != active.end()){
#ifdef _DEBUG
						std::cout << "Stopping bow stamina consumption at ThreadID: " << std::this_thread::get_id() << std::endl;
#endif
						it->second->bExit = true;
						it->second->bIsDraw = false;
					}
					else
#ifdef _DEBUG
						std::cout << "Actor not found on registration map." << std::endl;
#endif
					break;

				case SKSEActionEvent::kType_Jump){
					staminaMod = -10.0 * evn->actor->actorValueOwner.GetCurrent(kAVMass);
					evn->actor->actorValueOwner.Unk_06(2, kAVStamina, staminaMod);
					}*/
			}
			return kEvent_Continue;
		}

/*		EventResult	REQ_MenuHandler::ReceiveEvent(MenuOpenCloseEvent* evn, EventDispatcher<MenuOpenCloseEvent>* dispatcher){
			if (evn->opening)
				for (auto p : g_REQ_AttackConsHandler.active){
					p.second->bIsDraw = false;
				}
			else{
				for (auto p : g_REQ_AttackConsHandler.active){
					if (p.second->bIsDraw == false){
						p.second->bIsDraw = true;
						std::thread newBowDrawCost(BowDrawThread(&g_REQ_AttackConsHandler, p.second));
#ifdef _DEBUG
						std::cout << std::hex << "Thread created: " << newBowDrawCost.get_id() << std::endl;
#endif
						newBowDrawCost.detach();
					}
				}
			}
			return kEvent_Continue;
		}
		REQ_MenuHandler g_REQ_MenuHandler;*/
		REQ_AttackConsHandler g_REQ_AttackConsHandler;

		void __fastcall JumpHook(Actor* actor){
			if (!IsOnMount(0, 0, actor)){
				float staminaMod = -10.0 * actor->actorValueOwner.GetCurrent(kAVMass);
				actor->actorValueOwner.Unk_06(2, kAVStamina, staminaMod);
			}
			JumpOriginal(actor);
		}
	}
}
