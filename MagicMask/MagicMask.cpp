// MagicMask.cpp : DLL �p�̏����������̒�`���s���܂��B
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
//	����!
//
//		���� DLL �� MFC DLL �ɑ΂��ē��I�Ƀ����N�����ꍇ�A
//		MFC ���ŌĂяo����邱�� DLL ����G�N�X�|�[�g���ꂽ
//		�ǂ̊֐����֐��̍ŏ��ɒǉ������ AFX_MANAGE_STATE 
//		�}�N�����܂�ł��Ȃ���΂Ȃ�܂���B
//
//		��:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �ʏ�֐��̖{�̂͂��̈ʒu�ɂ���܂�
//		}
//
//		���̃}�N�����e�֐��Ɋ܂܂�Ă��邱�ƁAMFC ����
//		�ǂ̌Ăяo�����D�悷�邱�Ƃ͔��ɏd�v�ł��B
//		����͊֐����̍ŏ��̃X�e�[�g�����g�łȂ���΂�
//		��Ȃ����Ƃ��Ӗ����܂��A�R���X�g���N�^�� MFC 
//		DLL ���ւ̌Ăяo�����s���\��������̂ŁA�I�u
//		�W�F�N�g�ϐ��̐錾�����O�łȂ���΂Ȃ�܂���B
//
//		�ڍׂɂ��Ă� MFC �e�N�j�J�� �m�[�g 33 �����
//		58 ���Q�Ƃ��Ă��������B
//

/////////////////////////////////////////////////////////////////////////////
// CMagicMaskApp

BEGIN_MESSAGE_MAP(CMagicMaskApp, CWinApp)
	//{{AFX_MSG_MAP(CMagicMaskApp)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ}�b�s���O�p�̃}�N����ǉ��܂��͍폜���܂��B
		//        ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������B
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMagicMaskApp �̍\�z

CMagicMaskApp::CMagicMaskApp()
{
	// TODO: ���̈ʒu�ɍ\�z�p�̃R�[�h��ǉ����Ă��������B
	// ������ InitInstance �̒��̏d�v�ȏ��������������ׂċL�q���Ă��������B
}

/////////////////////////////////////////////////////////////////////////////
// �B��� CMagicMaskApp �I�u�W�F�N�g

CMagicMaskApp theApp;

/* �ȍ~���v���O�C����SPI�̎����ł� */
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
		AfxMessageBox("DocuWorks���C���X�g�[������Ă��܂���B");
		break;
	case XDW_E_FILE_NOT_FOUND:
		AfxMessageBox("�w�肳�ꂽ�t�@�C����������܂���B");
		break;
	case XDW_E_FILE_EXISTS:
		AfxMessageBox("�w�肳�ꂽ�t�@�C���͂��łɑ��݂��܂��B");
		break;
	case XDW_E_ACCESSDENIED:
	case XDW_E_INVALID_NAME:
	case XDW_E_BAD_NETPATH:
		AfxMessageBox("�w�肳�ꂽ�t�@�C�����J�����Ƃ��ł��܂���B");
		break;
	case XDW_E_BAD_FORMAT:
		AfxMessageBox("�w�肳�ꂽ�t�@�C���͐������t�H�[�}�b�g�ł͂���܂���B");
		break;
	case XDW_E_INVALID_ACCESS:
		AfxMessageBox("�w�肳�ꂽ��������錠��������܂���B");
		break;
	default:
		AfxMessageBox("�G���[���������܂����B");
		break;
	}
}

CString Color2String(int v) {
	CString s;
	switch (v) {
	case XDW_COLOR_BLACK: s = "��";
		break;
	case XDW_COLOR_WHITE: s = "��";
		break;
	case XDW_COLOR_BLUE: s = "��";
		break;
	case XDW_COLOR_RED: s = "��";
		break;
	case XDW_COLOR_YELLOW: s = "��";
		break;
	case XDW_COLOR_LIME: s = "��";
		break;
	case XDW_COLOR_AQUA: s= "���F";
		break;
	case XDW_COLOR_FUCHIA : s = "��";
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
	// XDW_GetDocumentInformation��p���đ��y�[�W���𓾂�
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
		if (maskFound) { // �؎摋�̋�`���Ȃ��ƁA�y�[�W�̑S�ʂ��}�X�N����Ă��܂��̂ŕs�s���B
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
				if (api_result < 0) { // �G���[����
					if (api_result==XDW_E_INVALIDARG) {
						// �����炭�ׂ����邩��������A�m�e�[�V����.
						hasHole = true;
						// ���~�ł͂Ȃ��A�����𖳎����āA�܂�}�X�N�ł��Ȃ��܂܏�������B
					} else {
						goto error_exit;
					}
				} else { // ���폈��
					int borderStyle = 0;	
					api_result = XDW_SetAnnotationAttribute(h1, nah, XDW_ATN_BorderStyle, XDW_ATYPE_INT, (char *)(&borderStyle), 0, NULL);
					if (api_result < 0) {
						goto error_exit;
					}
				}
			}
			if ( hasHole ) {
				CString msg;
				msg.Format("%d�y�[�W�Ƀ}�X�N�ł��Ȃ�����������܂����B", page);
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
    /* ���̃R�}���h���s�̓r���A�����Ƃ��ēn�����DW�t�@�C���͊J������Ă��܂��B
       �ǂݏ����\�ŃA�N�Z�X�ł��܂��B
    */
    /* MFC���g�p����ꍇ�AAFX_MANAGE_STATE( AfxGetStaticModuleState() );��
       �K���R�[�����Ă��������B����ɂ��A����DLL�̃��\�[�X���g����悤�ɂȂ�܂��B
    */
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if (strcmp(ps->ps_pszFunction, "�}�X�L���O"))
		return 0;
	int	i;

	if ( ps->ps_nFiles > 0 ) {
		for (i=0; i<ps->ps_nFiles; i++) {
			if (! DoIt(ps->ps_pszFiles[i])) {
				AfxMessageBox("�G���[�̂��ߒ��~���܂��B");
				goto error_exit;
			}
		}
		AfxMessageBox("�}�X�L���O�������I�����܂����B");
error_exit:;
	} else {
		AfxMessageBox("�Ώۂ�XDW������I�����Ă�����s���Ă��������B");
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
    /* *pnRealSize�ɕK�v�ȃo�b�t�@�T�C�Y�����Ă������� */
    *pnRealSize=strlen("�}�X�L���O")+1;

    if (nBufSize<*pnRealSize)
        return 0;

    /* nBufSize���K�v�ȃo�b�t�@�T�C�Y�𒴂��Ă���ꍇ�̂݁ApszEntries�ɃR�s�[��
       �����������Ƃ����� return 1��Ԃ��Ă�������
    */

    /*
     0�^�[�~�l�[�g�̕�����̃��X�g��Ԃ��Ă��������BDWDesk�̃v���O�C��/�ݒ�̌��ɏo�܂�
    */
   // DoCopy Commands!
    strcpy(pszEntries, "�}�X�L���O");
	return 1;
}

PLUGGEDIN_EXPORT( long ) PLG_Executable(const char *pszFunction, long nSel)
{
    //�I���t�@�C���̐� nSel�ɉ����Ď��s�\����Ԃ��Ă�������
	if (nSel>=1) {
		return 1;
	} else {
		return 0;
	}
}
PLUGGEDIN_EXPORT( long ) PLG_RequireFiles(const char *pszFunction)
{
    // 0��Ԃ��ƁA�t�@�C���I����Ԃɂ�炸�����͓n����܂���
	return 1;
}

PLUGGEDIN_EXPORT( HICON ) PLG_GetCommandIcon(const char *pszFunction, long bNormal)
{
    /* ��ɑ傫��(32x32 or 24x24)�A�C�R���ł����܂��܂��� */

	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	HICON hIcon=NULL;
	unsigned resID=0;
	if (!strcmp(pszFunction, "�}�X�L���O"))
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
    /* �K�� 1��Ԃ��Ă�������. 0��Ԃ��ƃv���O�C�������[�h����܂���*/
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
    IsCloningCommand/CanSetProfile�� 0�ȊO��Ԃ��ƁA
    GetNewClone / SetProfile���R�[������܂�

*/

PLUGGEDIN_EXPORT( long ) PLG_IsParallel(const char *pszFunction)
{
    /* v2.0�ł͂���SPI�͖�������܂� */
	return 0;
}


PLUGGEDIN_EXPORT( long ) PLG_CanSetProfile(const char *pszFunction)
{
    /* SetProfile��L���ɂ���ꍇ�A������1��Ԃ��Ă������� */
	return 1;
}

PLUGGEDIN_EXPORT( long ) PLG_SetProfile(const char *pszFunction)
{
    /* ������GUI���o���Ď��g�̐ݒ���s���Ă������� */
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	CAboutDlg dlg;
	dlg.DoModal();
	return 1;
}


/* IsCloning Entry*/
PLUGGEDIN_EXPORT( long ) PLG_IsCloningCommand(const char *pszFunction)
{
    /* ����@�\�ŕ����̐ݒ��ۑ����ē��삳�������ꍇ�Ȃǂ́A����
       SPI�� 1 ��Ԃ��Ă�������
     */
    return 0;
}

/* GetNewClone Entry*/
PLUGGEDIN_EXPORT( long ) PLG_GetNewClone(char* pszNewClone, long bufsize)
{
    /* pszNewClone�Ƃ͈قȂ�����������쐬���ApszNewClone�����������ĕԂ��Ă������� */
    /* �Ⴆ�� "�v���O�C���@�\" -> "�v���O�C���@�\01","�v���O�C���@�\02"�Ȃ�
       �ȍ~�̃Z�b�V�����ł́A�N���[�����ꂽ�����񂪈���pszCommand�Ƃ��ēn����܂�
       �N���[�����Ƃɐݒ��ۑ����Ă��������B
       PLUGGEDIN_REGKEY\ �ȉ��̃��W�X�g���ɕۑ����ꂽ�f�[�^�́A
       Desk�̃v���O�C��/�ݒ�_�C�A���O�� OK ��������Ȃ�����A�ꊇ�L�����Z������܂�
     */  
    return 0;
}

/* ReleaseClone Entry*/
PLUGGEDIN_EXPORT( long ) PLG_ReleaseClone(const char* pszClone)
{
    /* pszClone�Ŏw�肳�ꂽ�R�}���h���g�p���Ă����̈���J�����Ă������� */
    return 0;
}
