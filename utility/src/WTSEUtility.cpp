// WTSEUtility.cpp : DLL �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
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

// ����́A�G�N�X�|�[�g���ꂽ�ϐ��̗�ł��B
WTSEUTILITY_API int nWTSEUtility=0;

// ����́A�G�N�X�|�[�g���ꂽ�֐��̗�ł��B
WTSEUTILITY_API int fnWTSEUtility(void)
{
	return 42;
}

// ����́A�G�N�X�|�[�g���ꂽ�N���X�̃R���X�g���N�^�ł��B
// �N���X��`�Ɋւ��Ă� WTSEUtility.h ���Q�Ƃ��Ă��������B
CWTSEUtility::CWTSEUtility()
{
	return;
}

int WTSECommon::PORT_NO = -1;
