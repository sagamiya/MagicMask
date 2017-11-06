#if !defined(AFX_ABOUTDLG_H__75B30A33_8CCE_444F_903A_081DC7B26A8E__INCLUDED_)
#define AFX_ABOUTDLG_H__75B30A33_8CCE_444F_903A_081DC7B26A8E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AboutDlg.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
// コンストラクション
public:
	CAboutDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_DIALOG1 };
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CAboutDlg)
		// メモ: ClassWizard はこの位置にメンバ関数を追加します。
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_ABOUTDLG_H__75B30A33_8CCE_444F_903A_081DC7B26A8E__INCLUDED_)
