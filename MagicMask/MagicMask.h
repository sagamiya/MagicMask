// MagicMask.h : MAGICMASK アプリケーションのメイン ヘッダー ファイルです。
//

#if !defined(AFX_MAGICMASK_H__A1B0F264_B380_4BAE_8D30_2C9A84FDFDCF__INCLUDED_)
#define AFX_MAGICMASK_H__A1B0F264_B380_4BAE_8D30_2C9A84FDFDCF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// メイン シンボル

/////////////////////////////////////////////////////////////////////////////
// CMagicMaskApp
// このクラスの動作の定義に関しては MagicMask.cpp ファイルを参照してください。
//

class CMagicMaskApp : public CWinApp
{
public:
	CMagicMaskApp();

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CMagicMaskApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMagicMaskApp)
		// メモ -  ClassWizard はこの位置にメンバ関数を追加または削除します。
		//         この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_MAGICMASK_H__A1B0F264_B380_4BAE_8D30_2C9A84FDFDCF__INCLUDED_)
