// WTSEUtility.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "WTSEUtility.h"

#include "CommonDef.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

// これは、エクスポートされた変数の例です。
WTSEUTILITY_API int nWTSEUtility=0;

// これは、エクスポートされた関数の例です。
WTSEUTILITY_API int fnWTSEUtility(void)
{
	return 42;
}

// これは、エクスポートされたクラスのコンストラクタです。
// クラス定義に関しては WTSEUtility.h を参照してください。
CWTSEUtility::CWTSEUtility()
{
	return;
}

int WTSECommon::PORT_NO = -1;
