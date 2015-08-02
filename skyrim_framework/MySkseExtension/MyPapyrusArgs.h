#include "skse\PapyrusArgs.h"
#include "skse\GameForms.h"

template <> void UnpackValue<VMArray<TESForm*>>(VMArray<TESForm*> * dst, VMValue * src);
/*
template <typename T>
void UnpackValue(VMArray<T*> * dst, VMValue * src){
	UnpackArray(dst, src, GetTypeIDFromFormTypeID(T::kTypeID, (*g_skyrimVM)->GetClassRegistry()) | VMValue::kType_Identifier);
}*/
