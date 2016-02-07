#include <time.h>
#include <exception>
#include "skse\GameData.h"
#include "skse\PluginAPI.h"
#include "skse\skse_version.h"
#include "skyrim_framework\MySkseExtension\MyGameApi.h"
#include "skyrim_framework\common_const.h"
#include "skyrim_framework\FormInitialiser.h"
#include "skyrim_framework\REQ\REQ_Init.h"

#if defined(RequiemBuild) && defined(DuelBuild)
static const char* pluginName = "MyPlugin";
#else
#if defined(RequiemBuild)
static const char* pluginName = "RequiemPlugin";
#else
#if defined(DuelBuild)
static const char* pluginName = "DuelPlugin";
#else
static const char* pluginName = "LevelersTowerPlugin";
#endif
#endif
#endif
SKSEPapyrusInterface* g_papyrus = NULL;
PluginHandle g_pluginHandle = kPluginHandle_Invalid;
#ifdef RequiemBuild
SKSESerializationInterface	* g_serialization = NULL;
#endif

#ifdef AlchemySorter
#include "alchemysorter.h"
SKSEScaleformInterface		* g_scaleform = NULL;
#endif
extern "C"
{
bool SKSEPlugin_Query(const SKSEInterface * skse, PluginInfo * info)
{
	_MESSAGE(pluginName);

	// populate info structure
	info->infoVersion =	PluginInfo::kInfoVersion;
	info->name = pluginName;
	info->version =		1;

	// store plugin handle so we can identify ourselves later
	g_pluginHandle = skse->GetPluginHandle();

	if(skse->isEditor)
	{
		_MESSAGE("loaded in editor, marking as incompatible");
		return false;
	}
	else if(skse->runtimeVersion != RUNTIME_VERSION_1_9_32_0)
	{
		_MESSAGE("unsupported runtime version %08X", skse->runtimeVersion);
		return false;
	}
	
	g_papyrus = (SKSEPapyrusInterface *)skse->QueryInterface(kInterface_Papyrus);
	if (!g_papyrus)
	{
		_MESSAGE("couldn't get papyrus interface");
		return false;
	}

	if (g_papyrus->interfaceVersion < SKSEPapyrusInterface::kInterfaceVersion)
	{
		_MESSAGE("papyrus interface too old (%d expected %d)", g_papyrus->interfaceVersion, SKSEPapyrusInterface::kInterfaceVersion);
		return false;
	}
	/*
#ifdef RequiemBuild
	g_serialization = (SKSESerializationInterface *)skse->QueryInterface(kInterface_Serialization);
	if (!g_serialization)
	{
		_MESSAGE("couldn't get serialization interface");

		return false;
	}

	if (g_serialization->version < SKSESerializationInterface::kVersion)
	{
		_MESSAGE("serialization interface too old (%d expected %d)", g_serialization->version, SKSESerializationInterface::kVersion);

		return false;
	}
#endif
*/
#ifdef AlchemySorter
	// get the scaleform interface and query its version
	g_scaleform = (SKSEScaleformInterface *)skse->QueryInterface(kInterface_Scaleform);
	if (!g_scaleform)
	{
		_MESSAGE("couldn't get scaleform interface");

		return false;
	}

	if (g_scaleform->interfaceVersion < SKSEScaleformInterface::kInterfaceVersion)
	{
		_MESSAGE("scaleform interface too old (%d expected %d)", g_scaleform->interfaceVersion, SKSEScaleformInterface::kInterfaceVersion);

		return false;
	}
#endif

	return true;
}

bool SKSEPlugin_Load(const SKSEInterface * skse)
{
#if defined(_DEBUG) || defined(EventDebug)
	OpenConsole();
	_MESSAGE("%s loaded", pluginName);
#endif

	srand(time(NULL));
	g_papyrus->Register(RegisterPapyrus);

		//g_loadEventDispatcher->AddEventSink(&g_FormInitialiser);
//#ifdef RequiemBuild
		//g_serialization->SetUniqueID(g_pluginHandle, 'REQ_');
		//g_serialization->SetRevertCallback(g_pluginHandle, Serialization_Revert);
		//g_serialization->SetSaveCallback(g_pluginHandle, Serialization_Save);
		//g_serialization->SetLoadCallback(g_pluginHandle, Serialization_Load);
//#endif

#ifdef AlchemySorter
	g_scaleform->Register("AlchemySorter", RegisterScaleform);
#endif
	return true;
}

};
