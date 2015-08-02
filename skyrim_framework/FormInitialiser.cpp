#include "skyrim_framework\FormInitialiser.h"
#include "skyrim_framework\MySkseExtension\Utilities.h"
#include "skyrim_framework\MySkseExtension\MyGameApi.h"
#include "skse\GameData.h"
#include "skse\SafeWrite.h"
#include <cstdint>
#include "skyrim_framework\common_const.h"
#ifdef RequiemBuild
#include "skse\PluginAPI.h"
#include "skyrim_framework\MySkseExtension\MyPapyrusAE.h"
#include "skyrim_framework\MySkseExtension\MyPapyrusActor.h"
#include "skyrim_framework\REQ\PapyrusRequiem.h"
#include "skyrim_framework\REQ\REQ_const.h"
#include "skyrim_framework\REQ\REQ_Init.h"
#endif
#ifdef DuelBuild
#include "Duel.h"
#endif
#ifdef LevelersBuild
#include "skyrim_framework\MySkseExtension\MyPapyrusForm.h"
#include "LevelersTower\LevelersTower.h"
#endif
//#include "DiseasePoisonImmunity.h"
//Dual Sheath Redux
//DualSheathReduxApplySpell 0xff00182B
#ifdef _DEBUG
#include <iostream>
#include <iomanip>
#endif
/*
SpellItem* DualSheathReduxApplySpell;

class EffectSupplier:public BSTEventSink<
>{
	EventResult ReceiveEvent(CellAttachDetachEvent * evn, EventDispatcher<CellAttachDetachEvent> * dispatcher){
		if (evn->bIsAttach == 1){
			if (evn->akActionRef && evn->akActionRef->baseForm->formType == kFormType_NPC){
				UInt32 error;
				CastSpell(DualSheathReduxApplySpell, evn->akActionRef, evn->akActionRef, error, (*g_skyrimVM)->GetClassRegistry(), 0);
			}
		}
		return kEvent_Continue;
	}
};

EffectSupplier g_EffectSupplier;*/

template<typename PluginFormContainer>
bool initialise_forms(){
	UInt32 j = 0;
	for (UInt32 i = 0; i < PluginFormContainer::FormArrayCount; ++i){
		SInt32 ModIndex = DataHandler::GetSingleton()->GetModIndex(PluginFormContainer::modNames[i]);
		if (ModIndex >= 0){
			UInt32 FormModIndex = ModIndex;
			FormModIndex = (FormModIndex << 24) | 0x00FFFFFF;
#ifdef _DEBUG
			std::cout << "Begin loading forms for: " << PluginFormContainer::modNames[i] << " to: " << std::hex << &(PluginFormContainer::Forms.begin()) << '\n';
#endif
			while (j < PluginFormContainer::FormArrayEndingIndex[i]){
#ifdef _DEBUG
				UInt32 BackUp = FormModIndex & PluginFormContainer::Forms[j];
				std::cout << "Loading Form: " << std::hex << BackUp << std::endl;
#endif
				PluginFormContainer::Forms[j] = (UInt32)LookupFormByID(FormModIndex & PluginFormContainer::Forms[j]);
				if (!PluginFormContainer::Forms[j++])
				{
#ifdef _DEBUG
					std::cout << "Form loading fail for formID: " << std::hex << BackUp << " from mod: " << PluginFormContainer::modNames[i] << std::endl;
#endif
					return false;
				}
			}
		}
		else{
#ifdef _DEBUG
			std::cout << PluginFormContainer::modNames[i] << " not loaded.\nSkipping event registration." << std::endl;
#endif
			return false;
		}
	}
	return true;
}

void RegisterEventInternal(){
	SafeWrite32(0xc4edb6, 0xfffff646);
	if (initialise_forms<Common::FormContainer>())
#ifdef RequiemBuild
		if (initialise_forms<Requiem::FormContainer>())
		Requiem::RegisterPluginEvents();
	//if (initialise_forms(&mslVT::BaseForms) && initialise_forms(&mslVT::Forms, mslVT::modName))
		//g_MagicEffectApplyEventDispatcher->AddEventSink(&mslVT::g_DiseasePoisonImmunity);
	/*SInt8 ModIndex = DataHandler::GetSingleton()->GetModIndex("Dual Sheath Redux.esp");
	if (ModIndex > 0){
		UInt32 formIndex = static_cast<UInt32>(ModIndex);
		formIndex = (formIndex << 24) | 0x0000182B;
		DualSheathReduxApplySpell = (SpellItem*)LookupFormByID(formIndex);
		if (DualSheathReduxApplySpell)
			g_CellAttachDetachEventDispatcher->AddEventSink(&g_EffectSupplier);
	}*/
#endif
#ifdef DuelBuild
	if (initialise_forms<Duel::FormContainer>())
		g_hitEventDispatcher->AddEventSink(&Duel::g_Duel_ImpactHandler);
#endif
}

void _fastcall RegisterEvent(UInt32 unk, UInt32 edx, UInt32 unk04){
	//g_loadEventDispatcher->RemoveEventSink(&g_FormInitialiser);
	RegisterEventInternal();
	OnInit(unk, edx, unk04);
}

UInt32 SerialiseLoadOrderTable(std::unordered_map<UInt8, char*> loadordertable, char*& buf){
	auto loadedModList = DataHandler::GetSingleton()->modList.modInfoList;
	UInt32 length = 0;
	for (auto& p : loadordertable) {
		p.second = loadedModList.GetNthItem(p.first)->name;
		UInt32 len = strlen(p.second);
		length += len + 2; //for ID & terminating character
	}
	if (length > 0){
		buf = new char[length];
		UInt32 i = 0;
		for (auto const& p : loadordertable) {
			buf[i++] = p.first;
			UInt32 len = strlen(p.second) + 1;
			strcpy_s(&buf[i], len, p.second);
			i += len;
		}
#ifdef _DEBUG
		_MESSAGE("Buffer: %s", buf);
#endif
	}
#ifdef _DEBUG
	_MESSAGE("Done serialising load order table.");
#endif
	return length;
}

void Serialization_Save(SKSESerializationInterface * intfc){
	_MESSAGE("Saving Requiem Data");
	std::unordered_map<UInt8, char*> loadordertable;
	loadordertable.reserve(64);
	{ UInt32* buf;
	UInt32	length = Requiem::SerialiseArmour(loadordertable, buf);
	if (length > 0){
		if (intfc->OpenRecord('ARMO', kSerializationDataVersion)){
#ifdef _DEBUG
			_MESSAGE("Written: %d bytes", length);
#endif
			intfc->WriteRecordData(buf, length);
		}
		delete(buf);
	}}{
		UInt32* buf;
		UInt32	length = papyrusActiveMagicEffect::SerialiseRegisteredActiveEffects(loadordertable, buf);
		if (length > 0){
			if (intfc->OpenRecord('PAME', kSerializationDataVersion))
			{
#ifdef _DEBUG
				_MESSAGE("Written: %d bytes", length);
#endif
				intfc->WriteRecordData(buf, length);
			}
			delete(buf);
		}}{
			char* buf;
			UInt32	length = SerialiseLoadOrderTable(loadordertable, buf);
			if (length > 0){
				if (intfc->OpenRecord('LOAD', kSerializationDataVersion)){
#ifdef _DEBUG
					_MESSAGE("Written: %d bytes", length);
#endif
					intfc->WriteRecordData(buf, length);
				}
				delete(buf);
			}}
}

bool DeserialiseLoadOrderTable(std::unordered_map<UInt8, UInt8>& loadordertable, const char* buf, const UInt32 length){
	loadordertable.reserve(64);
	UInt32 i = 0;
	while (i < length){
		UInt8 oldModIndex = buf[i++];
		try {
			SInt32 NewModIndex = DataHandler::GetSingleton()-> GetModIndex(&buf[i]);
			if (NewModIndex > 0)
				loadordertable.emplace(oldModIndex, NewModIndex);
			else
				_MESSAGE("Unable to find mod %s.", &buf[i]);

			i += strlen(&buf[i]) + 1;
		}
		catch (std::exception e){
			_MESSAGE(e.what());
			return false;
		}
	}
	return true;
}

void Serialization_Revert(SKSESerializationInterface * intfc)
{
	_MESSAGE("Reverting Requiem Data");
	Requiem::ClearWornArmour();
	papyrusActiveMagicEffect::activeEffects.clear();
	papyrusActiveMagicEffect::activeEffects.reserve(3);
}

void Serialization_Load(SKSESerializationInterface * intfc)
{
	_MESSAGE("Loading Requiem Data");

	UInt32	type;
	UInt32	version;
	UInt32	length;
	bool	error = false;
	UInt32 armourLength = 0;
	UInt32*armourBuf;
	UInt32 aeLength = 0;
	UInt32*aeBuf;
	std::unordered_map<UInt8, UInt8> loadordertable;

	while (!error && intfc->GetNextRecordInfo(&type, &version, &length))
	{
		switch (type){
		case 'LOAD':{
			if (version == kSerializationDataVersion){
				if (length > 0)
				{
					char* buf = new char[length];
					intfc->ReadRecordData(buf, length);
					if (DeserialiseLoadOrderTable(loadordertable, buf, length))
						_MESSAGE("read load order table success");
					delete(buf);
				}
				else
					_MESSAGE("empty data?");
			}
			else
				error = true;
		}
			break;
		case 'ARMO':{
			if (version == kSerializationDataVersion){
				if (length > 0){
					armourBuf = new UInt32[length];
					intfc->ReadRecordData(armourBuf, length);
					armourLength = length / sizeof(*armourBuf);
					if (armourLength > 0) {
						Requiem::DeSerialiseArmour(loadordertable, armourBuf, armourLength);
						_MESSAGE("read armour success");
						delete(armourBuf);
					}
				}
				else
					_MESSAGE("empty data?");
			}
			else
				error = true;
		}
			break;

		case 'PAME':{
			if (version == kSerializationDataVersion){
				if (length > 0){
					aeBuf = new UInt32[length];
					intfc->ReadRecordData(aeBuf, length);
					aeLength = length / sizeof(*aeBuf);
					if (aeLength > 0) {
						papyrusActiveMagicEffect::DeserialiseRegisteredActiveEffects(loadordertable, aeBuf, aeLength);
						_MESSAGE("read AE success");
						delete(aeBuf);
					}
				}
				else
					_MESSAGE("empty data?");
			}
			else
				error = true;
		}
			break;

		default:
			_MESSAGE("unhandled type %08X", type);
			error = true;
			break;
		}
	}
}

bool RegisterPapyrus(VMClassRegistry* registry){
	SafeWrite32(0xc4edb6, reinterpret_cast<std::intptr_t>(RegisterEvent)-0xc4edba);
#ifdef RequiemBuild
	Requiem::papyrusRequiem::MyRegisterFuncs(registry);
	papyrusActor::MyRegisterFuncs(registry);
	papyrusActiveMagicEffect::MyRegisterFuncs(registry);
#endif
#ifdef LevelersBuild
	LevelersTower::MyRegisterFuncs(registry);
	papyrusForm::MyRegisterFuncs(registry);
#endif
	return true;
}
/*
EventResult FormInitialiser::ReceiveEvent(TESLoadGameEvent* evn, EventDispatcher<TESLoadGameEvent>* dispatcher){
	dispatcher->RemoveEventSink(this);
	RegisterEventInternal();
	return kEvent_Continue;
}

FormInitialiser g_FormInitialiser;*/