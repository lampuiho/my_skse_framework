#include "alchemysorter.h"
#include <unordered_map>
#include <sstream>
#include "skse\GameData.h"
#include "skse\GameObjects.h"
#include "skse\GlobalLocks.h"
#include "skse\ScaleformCallbacks.h"
#include "skse\ScaleformExtendedData.h"
#include "skse\ScaleformMovie.h"
#include "skyrim_framework\MySkseExtension\Utilities.h"

class SKSEScaleform_GetPotionList : public GFxFunctionHandler{
public:
	virtual void Invoke(Args* args){
		ASSERT(args->numArgs >= 3);
		ASSERT(args->args->GetType() == GFxValue::kType_Array);
		ASSERT(args->args[1].GetType() == GFxValue::kType_Array);
		ASSERT(args->args[2].GetType() == GFxValue::kType_Array);


	}
};

class SKSEScaleform_GetEffectList : public GFxFunctionHandler
{
public:
	virtual void Invoke(Args* args){
		ASSERT(args->numArgs >= 2);
		ASSERT(args->args->GetType() == GFxValue::kType_Array);
		ASSERT(args->args[1].GetType() == GFxValue::kType_Array);

		std::unordered_map<UInt32, const char*> effectNameTable;
		effectNameTable.reserve(128);
		UInt32 ingredCount = args->args->GetArraySize();
		for (UInt32 i = 0; i < ingredCount; ++i){
			GFxValue *e, *member; e = 0; member = 0;
			args->args->GetElement(i, e);
			e->GetMember("numItemEffects", member);
			UInt8 effectCount = member->GetNumber();
			member->GetMember("effectIDs", e);
			member->GetMember("effects", member);
			for (UInt32 j = 0; j < effectCount; ++j){
				GFxValue *effect, *effectP;
				effect = 0;
				e->GetElement(j, effect);
				UInt32 effectSetting = effect->GetNumber();
				effectNameTable[effectSetting] = reinterpret_cast<EffectSetting*>(effectSetting)->fullName.name.data;
			}
		}

		for (auto&p : effectNameTable){
			std::stringstream stream;
			stream << std::hex << p.first;
			RegisterNumber(&args->args[1], "id", p.first);
			RegisterString(&args->args[1], args->movie, "name", p.second);
		}
	}
};

class SKSEScaleform_GetIngredList : public GFxFunctionHandler
{
public:
	virtual void Invoke(Args* args)
	{
		if (!g_loadGameLock.TryEnter())
			return;

		ASSERT(args->numArgs >= 1);
		ASSERT(args->args->GetType() == GFxValue::kType_Array);
		tArray<IngredientItem*> ingredients = DataHandler::GetSingleton()->ingredients;

		GFxMovieView *movieView = args->movie;
		GFxValue *gfxIngredients = args->args;
		for (UInt32 i = 0; i < ingredients.count; ++i){
			GFxValue obj;
			movieView->CreateObject(&obj);
			RegisterString(&obj, movieView, "name", ingredients[i]->fullName.name.data);
			RegisterNumber(&obj, "id", (UInt32)ingredients[i]);
			RegisterNumber(&obj, "numItemEffects", ingredients[i]->effectItemList.count);
			GFxValue effects;
			GFxValue effectIDs;
			movieView->CreateArray(&effects);
			movieView->CreateArray(&effectIDs);
			for (UInt32 j = 0; j < ingredients[i]->effectItemList.count; ++j){
				GFxValue id;
				id.SetNumber((UInt32)ingredients[i]->effectItemList[j]->mgef);
				effectIDs.PushBack(&id);
				GFxValue effect;
				movieView->CreateObject(&effect);
				RegisterNumber(&effect, format("mag", j).c_str(), ingredients[i]->effectItemList[j]->magnitude);
				RegisterNumber(&effect, format("dur", j).c_str(), ingredients[i]->effectItemList[j]->duration);
				effects.PushBack(&effect);
			}
			obj.SetMember("effects", &effects);
			obj.SetMember("effectIDs", &effectIDs);
			gfxIngredients->PushBack(&obj);
		}

		g_loadGameLock.Leave();
	}
};

bool RegisterScaleform(GFxMovieView * view, GFxValue * root)
{
	RegisterFunction<SKSEScaleform_GetEffectList>(root, view, "GetEffectList");
	RegisterFunction<SKSEScaleform_GetIngredList>(root, view, "GetIngredList");

	return true;
}