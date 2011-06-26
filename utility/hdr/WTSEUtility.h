#pragma once

// 以下の ifdef ブロックは DLL からのエクスポートを容易にするマクロを作成するための 
// 一般的な方法です。この DLL 内のすべてのファイルは、コマンド ラインで定義された WTSEUTILITY_EXPORTS
// シンボルでコンパイルされます。このシンボルは、この DLL を使うプロジェクトで定義することはできません。
// ソースファイルがこのファイルを含んでいる他のプロジェクトは、 
// WTSEUTILITY_API 関数を DLL からインポートされたと見なすのに対し、この DLL は、このマクロで定義された
// シンボルをエクスポートされたと見なします。
#ifdef WTSEUTILITY_EXPORTS
#define WTSEUTILITY_API __declspec(dllexport)
#else
#define WTSEUTILITY_API __declspec(dllimport)
#endif

// このクラスは WTSEUtility.dll からエクスポートされました。
class WTSEUTILITY_API CWTSEUtility {
public:
	CWTSEUtility(void);
	// TODO: メソッドをここに追加してください。
};

extern WTSEUTILITY_API int nWTSEUtility;

WTSEUTILITY_API int fnWTSEUtility(void);
