#pragma once

// �ȉ��� ifdef �u���b�N�� DLL ����̃G�N�X�|�[�g��e�Ղɂ���}�N�����쐬���邽�߂� 
// ��ʓI�ȕ��@�ł��B���� DLL ���̂��ׂẴt�@�C���́A�R�}���h ���C���Œ�`���ꂽ WTSEUTILITY_EXPORTS
// �V���{���ŃR���p�C������܂��B���̃V���{���́A���� DLL ���g���v���W�F�N�g�Œ�`���邱�Ƃ͂ł��܂���B
// �\�[�X�t�@�C�������̃t�@�C�����܂�ł��鑼�̃v���W�F�N�g�́A 
// WTSEUTILITY_API �֐��� DLL ����C���|�[�g���ꂽ�ƌ��Ȃ��̂ɑ΂��A���� DLL �́A���̃}�N���Œ�`���ꂽ
// �V���{�����G�N�X�|�[�g���ꂽ�ƌ��Ȃ��܂��B
#ifdef WTSEUTILITY_EXPORTS
#define WTSEUTILITY_API __declspec(dllexport)
#else
#define WTSEUTILITY_API __declspec(dllimport)
#endif

// ���̃N���X�� WTSEUtility.dll ����G�N�X�|�[�g����܂����B
class WTSEUTILITY_API CWTSEUtility {
public:
	CWTSEUtility(void);
	// TODO: ���\�b�h�������ɒǉ����Ă��������B
};

extern WTSEUTILITY_API int nWTSEUtility;

WTSEUTILITY_API int fnWTSEUtility(void);
