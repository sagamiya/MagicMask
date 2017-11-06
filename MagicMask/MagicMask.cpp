// MagicMask.cpp : DLL 用の初期化処理の定義を行います。
//

#include "stdafx.h"
#include "MagicMask.h"
#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	メモ!
//
//		この DLL が MFC DLL に対して動的にリンクされる場合、
//		MFC 内で呼び出されるこの DLL からエクスポートされた
//		どの関数も関数の最初に追加される AFX_MANAGE_STATE 
//		マクロを含んでいなければなりません。
//
//		例:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 通常関数の本体はこの位置にあります
//		}
//
//		このマクロが各関数に含まれていること、MFC 内の
//		どの呼び出しより優先することは非常に重要です。
//		これは関数内の最初のステートメントでなければな
//		らないことを意味します、コンストラクタが MFC 
//		DLL 内への呼び出しを行う可能性があるので、オブ
//		ジェクト変数の宣言よりも前でなければなりません。
//
//		詳細については MFC テクニカル ノート 33 および
//		58 を参照してください。
//

/////////////////////////////////////////////////////////////////////////////
// CMagicMaskApp

BEGIN_MESSAGE_MAP(CMagicMaskApp, CWinApp)
	//{{AFX_MSG_MAP(CMagicMaskApp)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMagicMaskApp の構築

CMagicMaskApp::CMagicMaskApp()
{
	// TODO: この位置に構築用のコードを追加してください。
	// ここに InitInstance の中の重要な初期化処理をすべて記述してください。
}

/////////////////////////////////////////////////////////////////////////////
// 唯一の CMagicMaskApp オブジェクト

CMagicMaskApp theApp;

/* 以降がプラグインのSPIの実装です */
#include <string.h>
#include "mpplugin.h"
#include "resource.h"
#include "xdw_api.h"
#include "xdwapian.h"
#include "MaskRect.h"


void print_error(int code)
{
	switch (code) {
	case XDW_E_NOT_INSTALLED:
		AfxMessageBox("DocuWorksがインストールされていません。");
		break;
	case XDW_E_FILE_NOT_FOUND:
		AfxMessageBox("指定されたファイルが見つかりません。");
		break;
	case XDW_E_FILE_EXISTS:
		AfxMessageBox("指定されたファイルはすでに存在します。");
		break;
	case XDW_E_ACCESSDENIED:
	case XDW_E_INVALID_NAME:
	case XDW_E_BAD_NETPATH:
		AfxMessageBox("指定されたファイルを開くことができません。");
		break;
	case XDW_E_BAD_FORMAT:
		AfxMessageBox("指定されたファイルは正しいフォーマットではありません。");
		break;
	case XDW_E_INVALID_ACCESS:
		AfxMessageBox("指定された操作をする権利がありません。");
		break;
	default:
		AfxMessageBox("エラーが発生しました。");
		break;
	}
}

CString Color2String(int v) {
	CString s;
	switch (v) {
	case XDW_COLOR_BLACK: s = "黒";
		break;
	case XDW_COLOR_WHITE: s = "白";
		break;
	case XDW_COLOR_BLUE: s = "青";
		break;
	case XDW_COLOR_RED: s = "赤";
		break;
	case XDW_COLOR_YELLOW: s = "黄";
		break;
	case XDW_COLOR_LIME: s = "緑";
		break;
	case XDW_COLOR_AQUA: s= "水色";
		break;
	case XDW_COLOR_FUCHIA : s = "紫";
		break;
	}
	return s;
}

bool DoIt (const char *path1) {
	int api_result = 0;
	CString	s;

	XDW_DOCUMENT_HANDLE h1 = NULL;
	XDW_OPEN_MODE_EX mode = {
		sizeof(XDW_OPEN_MODE_EX), XDW_OPEN_UPDATE, XDW_AUTH_NODIALOGUE };
	api_result = XDW_OpenDocumentHandle(path1, &h1, (XDW_OPEN_MODE*)&mode);
	if (api_result < 0) {
		print_error(api_result);
		return false;
	}
	// XDW_GetDocumentInformationを用いて総ページ数を得る
	XDW_DOCUMENT_INFO info = { sizeof(XDW_DOCUMENT_INFO), 0, 0, 0 };
	XDW_GetDocumentInformation(h1, &info);
	int last_page = info.nPages;
	XDW_PAGE_INFO	pageInfo;
	pageInfo.nSize = sizeof XDW_PAGE_INFO;

	for (int page=1; page<= last_page; page++) {
		bool maskFound = false;
		int j;
		bool isFirstItemInPage = true;
		api_result = XDW_GetPageInformation( h1, page, &pageInfo);
		if (api_result < 0 ) {
			print_error(api_result);
			goto error_exit;
		}
		Mask *mask = new Mask(pageInfo.nWidth, pageInfo.nHeight);
		for ( j=1; j<= pageInfo.nAnnotations; j++) {
			XDW_ANNOTATION_INFO annotationInfo;
			annotationInfo.nSize = sizeof annotationInfo;
			api_result = XDW_GetAnnotationInformation(h1, page, NULL, j, &annotationInfo, NULL);
			if (api_result < 0 ) {
				print_error(api_result);
				goto error_exit;
			}
			if (annotationInfo.nAnnotationType == XDW_AID_RECTANGLE ) {
				bool hasVisibleBorder = false;
				bool isFilled = false;
				int value;
				
				api_result = XDW_GetAnnotationAttribute(annotationInfo.handle, XDW_ATN_BorderStyle, (char *)(&value), 4, NULL);
				if (api_result < 0) {
					goto error_exit;
				}
				hasVisibleBorder = (value==1);
				api_result = XDW_GetAnnotationAttribute(annotationInfo.handle, XDW_ATN_FillStyle, (char *)(&value), 4, NULL);
				if (api_result < 0) {
					goto error_exit;
				}
				isFilled = (value==1);
				if (hasVisibleBorder && !isFilled) {
					maskFound = true;
					mask->openWindow(annotationInfo.nHorPos,annotationInfo.nVerPos,annotationInfo.nWidth, annotationInfo.nHeight);
				}
			}
		}
		for ( j=pageInfo.nAnnotations; j>=1; j--) {
			XDW_ANNOTATION_INFO annotationInfo;
			annotationInfo.nSize = sizeof annotationInfo;
			api_result = XDW_GetAnnotationInformation(h1, page, NULL, j, &annotationInfo, NULL);
			if (api_result < 0 ) {
				print_error(api_result);
				goto error_exit;
			}
			if (annotationInfo.nAnnotationType == XDW_AID_RECTANGLE ) {
				bool hasVisibleBorder = false;
				bool isFilled = false;
				int value;
				
				api_result = XDW_GetAnnotationAttribute(annotationInfo.handle, XDW_ATN_BorderStyle, (char *)(&value), 4, NULL);
				if (api_result < 0) {
					goto error_exit;
				}
				hasVisibleBorder = (value==1);
				api_result = XDW_GetAnnotationAttribute(annotationInfo.handle, XDW_ATN_FillStyle, (char *)(&value), 4, NULL);
				if (api_result < 0) {
					goto error_exit;
				}
				isFilled = (value==1);
				if (hasVisibleBorder && !isFilled) {
					api_result = XDW_RemoveAnnotation(h1, annotationInfo.handle,NULL);
					if (api_result < 0) {
						goto error_exit;
					}
				}
			}
		}
		if (maskFound) { // 切取窓の矩形がないと、ページの全面がマスクされてしまうので不都合。
			bool hasHole = false;
			mask->shakeout(300);
			MaskRect *rect;
			for (rect = mask->root->next; rect != NULL; rect = rect->next) {
				XDW_AA_RECT_INITIAL_DATA rect_data;
				rect_data.common.nSize = sizeof XDW_AA_RECT_INITIAL_DATA;
				rect_data.common.nAnnotationType = XDW_AID_RECTANGLE;
				rect_data.common.nReserved1 = 0;
				rect_data.common.nReserved2 = 0;
				rect_data.nWidth = rect->w;
				rect_data.nHeight = rect->h;
				XDW_ANNOTATION_HANDLE nah;
				api_result = XDW_AddAnnotation(h1, XDW_AID_RECTANGLE , page, rect->x, rect->y, &rect_data.common, &nah,  NULL);
				if (api_result < 0) { // エラー処理
					if (api_result==XDW_E_INVALIDARG) {
						// おそらく細すぎるか薄すぎるアノテーション.
						hasHole = true;
						// 中止ではなく、そこを無視して、つまりマスクできないまま処理する。
					} else {
						goto error_exit;
					}
				} else { // 正常処理
					int borderStyle = 0;	
					api_result = XDW_SetAnnotationAttribute(h1, nah, XDW_ATN_BorderStyle, XDW_ATYPE_INT, (char *)(&borderStyle), 0, NULL);
					if (api_result < 0) {
						goto error_exit;
					}
				}
			}
			if ( hasHole ) {
				CString msg;
				msg.Format("%dページにマスクできない部分がありました。", page);
				msg = msg + "("+path1+")";
				AfxMessageBox(msg);
			}
		}
		delete mask;
	}
	XDW_SaveDocument(h1, NULL);
	XDW_CloseDocumentHandle(h1, NULL);
	return true;
error_exit:
	XDW_CloseDocumentHandle(h1, NULL);
	return false;
}



PLUGGEDIN_EXPORT( long ) PLG_ExecuteCommand(PLUGGEDIN_STRUCT *ps)
{
    /* このコマンド実行の途中、引数として渡されるDWファイルは開放されています。
       読み書き可能でアクセスできます。
    */
    /* MFCを使用する場合、AFX_MANAGE_STATE( AfxGetStaticModuleState() );を
       必ずコールしてください。これにより、このDLLのリソースが使われるようになります。
    */
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if (strcmp(ps->ps_pszFunction, "マスキング"))
		return 0;
	int	i;

	if ( ps->ps_nFiles > 0 ) {
		for (i=0; i<ps->ps_nFiles; i++) {
			if (! DoIt(ps->ps_pszFiles[i])) {
				AfxMessageBox("エラーのため中止します。");
				goto error_exit;
			}
		}
		AfxMessageBox("マスキング処理を終了しました。");
error_exit:;
	} else {
		AfxMessageBox("対象のXDW文書を選択してから実行してください。");
	}
	/*
    for (i=0;i< ps->ps_nFiles; i++){
        strcat(buf," ");
        _splitpath(ps->ps_pszFiles[i],tmp1,tmp1,path,tmp1);
        strcat(buf, path);
    }
    AfxMessageBox(buf);
    */

    return 1;
}

/* EnumerateCommands Entry */
PLUGGEDIN_EXPORT( long ) PLG_EnumerateCommands(char* pszEntries, long nBufSize, long* pnRealSize)
{
    // Check buffer size is large enough
    /* *pnRealSizeに必要なバッファサイズを入れてください */
    *pnRealSize=strlen("マスキング")+1;

    if (nBufSize<*pnRealSize)
        return 0;

    /* nBufSizeが必要なバッファサイズを超えている場合のみ、pszEntriesにコピーし
       成功したことを示す return 1を返してください
    */

    /*
     0ターミネートの文字列のリストを返してください。DWDeskのプラグイン/設定の候補に出ます
    */
   // DoCopy Commands!
    strcpy(pszEntries, "マスキング");
	return 1;
}

PLUGGEDIN_EXPORT( long ) PLG_Executable(const char *pszFunction, long nSel)
{
    //選択ファイルの数 nSelに応じて実行可能性を返してください
	if (nSel>=1) {
		return 1;
	} else {
		return 0;
	}
}
PLUGGEDIN_EXPORT( long ) PLG_RequireFiles(const char *pszFunction)
{
    // 0を返すと、ファイル選択状態によらず引数は渡されません
	return 1;
}

PLUGGEDIN_EXPORT( HICON ) PLG_GetCommandIcon(const char *pszFunction, long bNormal)
{
    /* 常に大きな(32x32 or 24x24)アイコンでもかまいません */

	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	HICON hIcon=NULL;
	unsigned resID=0;
	if (!strcmp(pszFunction, "マスキング"))
		resID= IDI_APPICON1;
	else return 0;

	HINSTANCE myInst = (HINSTANCE) AfxGetInstanceHandle();
	if (bNormal)
		hIcon=LoadIcon(myInst,MAKEINTRESOURCE(resID));
	else
		hIcon=(HICON)LoadImage(myInst,MAKEINTRESOURCE(resID),IMAGE_ICON,16,16,0);
	return hIcon;
}

/* Followin SPI s  are NO-op */

PLUGGEDIN_EXPORT( long ) PLG_Initialize(const char* cmd)
{
    /* 必ず 1を返してください. 0を返すとプラグインがロードされません*/
	return 1;
}

PLUGGEDIN_EXPORT( long ) PLG_Finalize(const char*  cmd)
{
	return 1;
}

PLUGGEDIN_EXPORT( long ) PLG_CanFinalize(const char*  cmd)
{
	return 1;
}

/*
    IsCloningCommand/CanSetProfileは 0以外を返すと、
    GetNewClone / SetProfileがコールされます

*/

PLUGGEDIN_EXPORT( long ) PLG_IsParallel(const char *pszFunction)
{
    /* v2.0ではこのSPIは無視されます */
	return 0;
}


PLUGGEDIN_EXPORT( long ) PLG_CanSetProfile(const char *pszFunction)
{
    /* SetProfileを有効にする場合、ここで1を返してください */
	return 1;
}

PLUGGEDIN_EXPORT( long ) PLG_SetProfile(const char *pszFunction)
{
    /* ここでGUIを出して自身の設定を行ってください */
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	CAboutDlg dlg;
	dlg.DoModal();
	return 1;
}


/* IsCloning Entry*/
PLUGGEDIN_EXPORT( long ) PLG_IsCloningCommand(const char *pszFunction)
{
    /* 同一機能で複数の設定を保存して動作させたい場合などは、この
       SPIで 1 を返してください
     */
    return 0;
}

/* GetNewClone Entry*/
PLUGGEDIN_EXPORT( long ) PLG_GetNewClone(char* pszNewClone, long bufsize)
{
    /* pszNewCloneとは異なった文字列を作成し、pszNewCloneを書き換えて返してください */
    /* 例えば "プラグイン機能" -> "プラグイン機能01","プラグイン機能02"など
       以降のセッションでは、クローンされた文字列が引数pszCommandとして渡されます
       クローンごとに設定を保存してください。
       PLUGGEDIN_REGKEY\ 以下のレジストリに保存されたデータは、
       Deskのプラグイン/設定ダイアログで OK が押されない限り、一括キャンセルされます
     */  
    return 0;
}

/* ReleaseClone Entry*/
PLUGGEDIN_EXPORT( long ) PLG_ReleaseClone(const char* pszClone)
{
    /* pszCloneで指定されたコマンドが使用していた領域を開放してください */
    return 0;
}
