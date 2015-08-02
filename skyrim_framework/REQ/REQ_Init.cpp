#include "skyrim_framework\REQ\REQ_Init.h"
#include <cstdint>
#include "skyrim_framework\MySkseExtension\MyGameApi.h"
#include "skyrim_framework\MySkseExtension\Utilities.h"
#include "skse\GameEvents.h"
#include "skse\GameForms.h"
#include "skse\GameMenus.h"
#include "skse\GameObjects.h"
#include "skse\Hooks_Handlers.h"
#include "skse\PapyrusEvents.h"
#include "skse\PluginAPI.h"
#include "skse\SafeWrite.h"
#include "skse\Utilities.h"
#include "skyrim_framework\REQ\PapyrusRequiem.h"
#include "skyrim_framework\REQ\REQ_Impact.h"
#include "skyrim_framework\REQ\PapyrusRequiem.h"
#include "skyrim_framework\REQ\REQ_StaminaCons.h"
#include "skyrim_framework\common_const.h"
#include "skyrim_framework\SerialisationUtilities.h"

namespace Requiem{
	UInt32 SerialiseArmour(std::unordered_map<UInt8, char*>& loadordertable, UInt32*& buf){
		UInt32 i = Requiem::papyrusRequiem::equippedArmour.size();
		if (i > 0){
#ifdef _DEBUG
			_MESSAGE("No. of equipped armour: %d > 0. Begin serialising armour data.", i);
#endif
			i <<= 1;
			buf = new UInt32[i];
			i = 0;
			for (auto const& p : Requiem::papyrusRequiem::equippedArmour) {
				buf[i++] = static_cast<UInt32>(p.first); buf[i++] = p.second->formID;
				UInt8 loadOrder = p.second->formID >> 24;
				if (loadOrder > 0)
					loadordertable.emplace(loadOrder, reinterpret_cast<char*>(NULL));
			}
#ifdef _DEBUG
			printBuffer(buf, i);
#endif
		}
		return i * sizeof(*buf);
	}
	void DeSerialiseArmour(const std::unordered_map<UInt8, UInt8>& loadOrderTable, const UInt32* buf, const UInt8 length){
#ifdef _DEBUG
		if (length <= 32){
			std::cout << std::dec << "Loaded armour buffer length: " << static_cast<UInt32>(length);
			std::cout << "\nLoaded armour buffer: " << std::hex;
#endif
			for (UInt16 i = 0; i < length; ++i) {
#ifdef _DEBUG
				std::cout << buf[i] << "-" << buf[i + 1] << "-";
#endif
				UInt8 key = buf[i++];
				TESForm* temp = GetFormByOldID(loadOrderTable, buf[i]);
				if (temp && temp->IsArmor())
					Requiem::papyrusRequiem::equippedArmour.emplace(key, reinterpret_cast<TESObjectARMO*>(temp));
				else
					_MESSAGE("\nUnable to load armour: %x", buf[i]);
			}
#ifdef _DEBUG
			std::cout << std::endl;
		}
		else
			_MESSAGE("Error! Saved armour array has unexpected length %d.", length);
#endif
	}
	void ClearWornArmour(){
		papyrusRequiem::equippedArmour.clear();
		papyrusRequiem::equippedArmour.reserve(32);
	}
	void RegisterPluginEvents(){
		//SafeWrite32(JumpAnimEventHandler_vtbl + 0x04, GetFnAddr(&IHandlerFunctor<Actor, UInt32>::ProcessAction<JumpAnimEventHandler, SKSEActionEvent::kType_Jump, SKSEActionEvent::kSlot_Right>));
		//SafeWrite32(JumpHandler_vtbl + 0x10, GetFnAddr(&JumpHandler::Process_Hook));
		//modIndex = (DataHandler::GetSingleton()->GetModIndex(modName) << 24) | 0x00FFFFFF;
		SafeWrite32(ArrowAttachHandler_vtbl + 0x04, GetFnAddr(&IHandlerFunctor<Actor, UInt32>::ProcessAction<ArrowAttachHandler, ActionEvent::kType_ArrowAttach, SKSEActionEvent::kSlot_Right>));
		SafeWrite32(ArrowDetachHandler_vtbl + 0x04, GetFnAddr(&IHandlerFunctor<Actor, UInt32>::ProcessAction<ArrowDetachHandler, ActionEvent::kType_ArrowDetach, SKSEActionEvent::kSlot_Right>));
		SafeWrite32(0x771934, reinterpret_cast<std::intptr_t>(Requiem::eventRequiem::JumpHook) - 0x771938);
		g_hitEventDispatcher->AddEventSink(&Requiem::eventRequiem::g_REQ_ImpactHandler);
		g_actionEventDispatcher.AddEventSink(&Requiem::eventRequiem::g_REQ_AttackConsHandler);
		//MenuManager::GetSingleton()->MenuOpenCloseEventDispatcher()->AddEventSink(&Requiem::eventRequiem::g_REQ_MenuHandler);
	}
}