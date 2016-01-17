//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a part of the BCGControlBar Library
// Copyright (C) 1998-2011 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//*******************************************************************************
//
// BCGPWnd.cpp : implementation file
//

#include "stdafx.h"
#include "oleacc.h"
#include "comdef.h"

#if _MSC_VER < 1300
#include "WinAble.h"
#endif

#include "BCGPWnd.h"
#include "bcgglobals.h"
#include "BCGPDlgImpl.h"
#include "BCGPDialog.h"
#ifndef _BCGSUITE_
#include "BCGPPropertyPage.h"
#endif

#pragma comment(lib,"Oleacc.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBCGPWnd

IMPLEMENT_DYNCREATE(CBCGPWnd, CWnd)

CBCGPWnd::CBCGPWnd()
{
	EnableAutomation();

#if _MSC_VER >= 1300
		EnableActiveAccessibility ();
#endif
	
	m_bVisualManagerStyle	= FALSE;
	m_bOnGlass				= FALSE;
	m_pStdObject			= NULL;
}

CBCGPWnd::~CBCGPWnd()
{
#if _MSC_VER < 1300
	if (m_pStdObject != NULL)
	{
		//force disconnect accessibility clients
		::CoDisconnectObject ((IAccessible*)m_pStdObject, NULL);
		m_pStdObject = NULL;
	}
#else
	if (m_pProxy != NULL)
	{
		//force disconnect accessibility clients
		::CoDisconnectObject ((IAccessible*)m_pProxy, NULL);
	}
#endif
}

BEGIN_MESSAGE_MAP(CBCGPWnd, CWnd)
	//{{AFX_MSG_MAP(CBCGPWnd)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_GETOBJECT, OnGetObject)
	ON_REGISTERED_MESSAGE(BCGM_ONSETCONTROLVMMODE, OnBCGSetControlVMMode)
	ON_REGISTERED_MESSAGE(BCGM_ONSETCONTROLAERO, OnBCGSetControlAero)
END_MESSAGE_MAP()

#if _MSC_VER < 1300
BEGIN_INTERFACE_MAP(CBCGPWnd, CWnd)
    INTERFACE_PART(CBCGPWnd, IID_IAccessible, Accessible)
END_INTERFACE_MAP()

IMPLEMENT_OLECREATE(CBCGPWnd, "BCGPOleAcc.BCGPPopupMenuAcc2", 0xd4082021, 0x53de, 0x4f13, 0x83, 0x34, 0x7d, 0x5d, 0x70, 0xd5, 0xe, 0xc1)
#endif

/////////////////////////////////////////////////////////////////////////////
// CBCGPWnd message handlers

LRESULT CBCGPWnd::OnGetObject(WPARAM wParam, LPARAM lParam)
{
	if (globalData.IsAccessibilitySupport () &&
		IsAccessibilityCompatible ())
	{

#if _MSC_VER >= 1300
		return CWnd::OnGetObject (wParam, lParam);
#else
        if ((DWORD)lParam == OBJID_CLIENT)
		{
			LPUNKNOWN pUnknown = GetInterface(&IID_IAccessible);
			if (!pUnknown)
			{
				return E_FAIL;
			}

			m_pStdObject = (IAccessible*)pUnknown;

			return LresultFromObject(IID_IAccessible, wParam, pUnknown);
		}
#endif
	}

	return (LRESULT)0L;
}	

#if _MSC_VER < 1300
//*********************************************************************************
//IUnknown interface
STDMETHODIMP_(ULONG) CBCGPWnd::XAccessible::AddRef()
{
	METHOD_PROLOGUE(CBCGPWnd, Accessible)
    
	return pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CBCGPWnd::XAccessible::Release()
{
	METHOD_PROLOGUE(CBCGPWnd, Accessible)

    return pThis->ExternalRelease();
}

STDMETHODIMP CBCGPWnd::XAccessible::QueryInterface(REFIID iid, LPVOID far* ppvObj)     
{
    METHOD_PROLOGUE(CBCGPWnd, Accessible)

    return pThis->ExternalQueryInterface(&iid, ppvObj);
}

STDMETHODIMP CBCGPWnd::XAccessible::GetTypeInfoCount(
    UINT FAR* pctinfo)
{
   METHOD_PROLOGUE(CBCGPWnd, Accessible)

  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfoCount(pctinfo);
}
STDMETHODIMP CBCGPWnd::XAccessible::GetTypeInfo(
  UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo)
{
   METHOD_PROLOGUE(CBCGPWnd, Accessible)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetTypeInfo(itinfo, lcid, pptinfo);
}
STDMETHODIMP CBCGPWnd::XAccessible::GetIDsOfNames(
  REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
  LCID lcid, DISPID FAR* rgdispid) 
{
   METHOD_PROLOGUE(CBCGPWnd, Accessible)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->GetIDsOfNames(riid, rgszNames, cNames, 
    lcid, rgdispid);
}
STDMETHODIMP CBCGPWnd::XAccessible::Invoke(
  DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
  DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult,
  EXCEPINFO FAR* pexcepinfo, UINT FAR* puArgErr)
{
   METHOD_PROLOGUE(CBCGPWnd, Accessible)
  LPDISPATCH lpDispatch = pThis->GetIDispatch(FALSE);
  ASSERT(lpDispatch != NULL);
  return lpDispatch->Invoke(dispidMember, riid, lcid,
    wFlags, pdispparams, pvarResult,
    pexcepinfo, puArgErr);
}

//IAccessible
//*********************************************************************************************
STDMETHODIMP CBCGPWnd::XAccessible::get_accParent(THIS_ IDispatch * FAR* ppdispParent)
{
	METHOD_PROLOGUE(CBCGPWnd, Accessible)
	return pThis->get_accParent(ppdispParent);
}
//*********************************************************************************************
STDMETHODIMP CBCGPWnd::XAccessible::get_accChildCount(THIS_ long FAR* pChildCount)
{
	METHOD_PROLOGUE(CBCGPWnd, Accessible)
	return pThis->get_accChildCount(pChildCount);
}
//*********************************************************************************************
STDMETHODIMP CBCGPWnd::XAccessible::get_accChild(THIS_ VARIANT varChildIndex, IDispatch * FAR* ppdispChild)
{
	METHOD_PROLOGUE(CBCGPWnd, Accessible)
	return pThis->get_accChild(varChildIndex, ppdispChild);
}
STDMETHODIMP CBCGPWnd::XAccessible::get_accName(THIS_ VARIANT varChild, BSTR* pszName)
{
	METHOD_PROLOGUE(CBCGPWnd, Accessible)
	return pThis->get_accName(varChild, pszName);
}
STDMETHODIMP CBCGPWnd::XAccessible::get_accValue(THIS_ VARIANT varChild, BSTR* pszValue)
{
	METHOD_PROLOGUE(CBCGPWnd, Accessible)
	return pThis->get_accValue(varChild, pszValue);
}
STDMETHODIMP CBCGPWnd::XAccessible::get_accDescription(THIS_ VARIANT varChild, BSTR FAR* pszDescription)
{
	METHOD_PROLOGUE(CBCGPWnd, Accessible)
	return pThis->get_accDescription(varChild, pszDescription);
}

STDMETHODIMP CBCGPWnd::XAccessible::get_accRole(THIS_ VARIANT varChild, VARIANT *pvarRole)
{
	METHOD_PROLOGUE(CBCGPWnd, Accessible)
	return pThis->get_accRole(varChild, pvarRole);
}
STDMETHODIMP CBCGPWnd::XAccessible::get_accState(THIS_ VARIANT varChild, VARIANT *pvarState)
{
	METHOD_PROLOGUE(CBCGPWnd, Accessible)
	return pThis->get_accState(varChild, pvarState);
}
STDMETHODIMP CBCGPWnd::XAccessible::get_accHelp(THIS_ VARIANT varChild, BSTR* pszHelp)
{
	METHOD_PROLOGUE(CBCGPWnd, Accessible)
	return pThis->get_accHelp(varChild, pszHelp);
}
STDMETHODIMP CBCGPWnd::XAccessible::get_accHelpTopic(THIS_ BSTR* pszHelpFile, VARIANT varChild, long* pidTopic)
{
	METHOD_PROLOGUE(CBCGPWnd, Accessible)
	return pThis->get_accHelpTopic(pszHelpFile, varChild, pidTopic);
}
STDMETHODIMP CBCGPWnd::XAccessible::get_accKeyboardShortcut(THIS_ VARIANT varChild, BSTR* pszKeyboardShortcut)
{
	METHOD_PROLOGUE(CBCGPWnd, Accessible)
	return pThis->get_accKeyboardShortcut(varChild, pszKeyboardShortcut);
}
STDMETHODIMP CBCGPWnd::XAccessible::get_accFocus(THIS_ VARIANT FAR * pvarFocusChild)
{
	METHOD_PROLOGUE(CBCGPWnd, Accessible)
	return pThis->get_accFocus(pvarFocusChild);
}
STDMETHODIMP CBCGPWnd::XAccessible::get_accSelection(THIS_ VARIANT FAR * pvarSelectedChildren)
{
	METHOD_PROLOGUE(CBCGPWnd, Accessible)
	return pThis->get_accSelection(pvarSelectedChildren);
}
STDMETHODIMP CBCGPWnd::XAccessible::get_accDefaultAction(THIS_ VARIANT varChild, BSTR* pszDefaultAction)
{
	METHOD_PROLOGUE(CBCGPWnd, Accessible)
	return pThis->get_accDefaultAction(varChild, pszDefaultAction);
}

STDMETHODIMP CBCGPWnd::XAccessible::accSelect(THIS_ long flagsSelect, VARIANT varChild)
{
	METHOD_PROLOGUE(CBCGPWnd, Accessible)
	return pThis->accSelect(flagsSelect, varChild);
}
STDMETHODIMP CBCGPWnd::XAccessible::accLocation(THIS_ long* pxLeft, long* pyTop, long* pcxWidth, long* pcyHeight, VARIANT varChild)
{
	METHOD_PROLOGUE(CBCGPWnd, Accessible)
	return pThis->accLocation(pxLeft, pyTop, pcxWidth, pcyHeight, varChild);
}
STDMETHODIMP CBCGPWnd::XAccessible::accNavigate(THIS_ long navDir, VARIANT varStart, VARIANT * pvarEndUpAt)
{
	METHOD_PROLOGUE(CBCGPWnd, Accessible)
	return pThis->accNavigate(navDir, varStart, pvarEndUpAt);
}
STDMETHODIMP CBCGPWnd::XAccessible::accHitTest(THIS_ long xLeft, long yTop, VARIANT * pvarChildAtPoint)
{
	METHOD_PROLOGUE(CBCGPWnd, Accessible)
	return pThis->accHitTest(xLeft, yTop, pvarChildAtPoint);
}
STDMETHODIMP CBCGPWnd::XAccessible::accDoDefaultAction(THIS_ VARIANT varChild)
{
	METHOD_PROLOGUE(CBCGPWnd, Accessible)
	return pThis->accDoDefaultAction(varChild);
}
STDMETHODIMP CBCGPWnd::XAccessible::put_accName(THIS_ VARIANT varChild, BSTR szName)
{
	METHOD_PROLOGUE(CBCGPWnd, Accessible)
	return pThis->put_accName(varChild, szName);
}
STDMETHODIMP CBCGPWnd::XAccessible::put_accValue(THIS_ VARIANT varChild, BSTR pszValue)
{
	METHOD_PROLOGUE(CBCGPWnd, Accessible)
	return pThis->put_accValue(varChild, pszValue);
}
#endif

HRESULT CBCGPWnd::get_accParent(IDispatch **ppdispParent)
{
	HRESULT hr = E_INVALIDARG;

    if (ppdispParent)
    {
        CWnd* pWnd = GetParent();
        if (pWnd)
        {
            AccessibleObjectFromWindow(pWnd->GetSafeHwnd () , (DWORD) OBJID_CLIENT,
               IID_IAccessible, (void**) ppdispParent);

            hr  = (*ppdispParent) ? S_OK : S_FALSE;
        }
    }           

    return hr ;
}


HRESULT CBCGPWnd::get_accChildCount(long *pcountChildren)
{
	if( !pcountChildren )
    {
        return E_INVALIDARG;
    }

	*pcountChildren = 0; 
	return S_OK;
}


HRESULT CBCGPWnd::get_accChild(VARIANT varChild, IDispatch **ppdispChild)
{
	if( !(*ppdispChild) )
    {
        return E_INVALIDARG;
    }

	if (varChild.vt == VT_I4 && varChild.lVal == CHILDID_SELF)
	{
		return E_INVALIDARG;
	}

	if (m_pStdObject != NULL) 
	{
		*ppdispChild = m_pStdObject;
	}
	else
	{
		*ppdispChild = NULL;
	}

	return S_OK;
}

// Override in users code
HRESULT CBCGPWnd::get_accName(VARIANT varChild, BSTR *pszName)
{
	if (varChild.vt == VT_I4 && varChild.lVal == CHILDID_SELF)
	{
		CString strText;
		GetWindowText (strText);
		*pszName = strText.AllocSysString ();
		return S_OK;
	}

	if (varChild.vt == VT_I4 && varChild.lVal > 0)
	{
		OnSetAccData (varChild.lVal);
		if (m_AccData.m_strAccName.IsEmpty ())
		{
			return S_FALSE;
		}
		*pszName = m_AccData.m_strAccName.AllocSysString ();
	}

	return S_OK;
}

// Override in users code
// Default inplementation will get window text and return it.
HRESULT CBCGPWnd::get_accValue(VARIANT varChild, BSTR *pszValue)
{
	if (varChild.vt == VT_I4 && varChild.lVal > 0)
	{
		OnSetAccData (varChild.lVal);
		if (m_AccData.m_strAccValue.IsEmpty ())
		{
			return S_FALSE;
		}
		*pszValue = m_AccData.m_strAccValue.AllocSysString ();
	}
	else
	{
		return S_FALSE;
	}

	return S_OK;
}

// Override in users code
HRESULT CBCGPWnd::get_accDescription(VARIANT varChild, BSTR *pszDescription)
{
	if (varChild.vt == VT_I4 && varChild.lVal == CHILDID_SELF )
	{
		CString strText;
		GetWindowText (strText);
		*pszDescription = strText.AllocSysString ();
		return S_OK;
	}

	if (varChild.vt == VT_I4 && varChild.lVal > 0)
	{
		OnSetAccData (varChild.lVal);
		if (m_AccData.m_strDescription.IsEmpty ())
		{
			return S_FALSE;
		}
		*pszDescription = m_AccData.m_strDescription.AllocSysString ();
		return S_OK;
	}

	return S_FALSE;
}

// Investigate
HRESULT CBCGPWnd::get_accRole(VARIANT varChild, VARIANT *pvarRole)
{
	if (varChild.vt == VT_I4 && varChild.lVal == CHILDID_SELF)
	{
		pvarRole->vt = VT_I4;
		pvarRole->lVal = ROLE_SYSTEM_TOOLBAR;
		return S_OK;
	}
	if ( !pvarRole || (( varChild.vt != VT_I4 ) && ( varChild.lVal != CHILDID_SELF )) )
    {
        return E_INVALIDARG;
    }

	if (varChild.vt == VT_I4 && varChild.lVal > 0)
	{
		pvarRole->vt = VT_I4;
		OnSetAccData (varChild.lVal);
		pvarRole->lVal = m_AccData.m_nAccRole;
		return S_OK;
	}

	pvarRole->vt = VT_I4;
	pvarRole->lVal = ROLE_SYSTEM_PUSHBUTTON;
			
    return S_OK;
}

// Investigate
HRESULT CBCGPWnd::get_accState(VARIANT varChild, VARIANT *pvarState)
{
	if (varChild.vt == VT_I4 && varChild.lVal == CHILDID_SELF)
	{
		pvarState->vt = VT_I4;
	    pvarState->lVal = STATE_SYSTEM_DEFAULT;
		return S_OK;
	}

	if ( !pvarState || (( varChild.vt != VT_I4 ) && ( varChild.lVal != CHILDID_SELF )) )
    {
        return E_INVALIDARG;
    }

	if (varChild.vt == VT_I4 && varChild.lVal > 0)
	{
		OnSetAccData (varChild.lVal);
		pvarState->vt = VT_I4;
		pvarState->lVal = m_AccData.m_bAccState;
		return S_OK; 
	}

	return E_INVALIDARG;
}

// Override in User's code?
HRESULT CBCGPWnd::get_accHelp(VARIANT varChild, BSTR *pszHelp)
{
	if (varChild.vt == VT_I4 && varChild.lVal == CHILDID_SELF )
	{
		*pszHelp = SysAllocString( L"ControlPane");
		 return S_OK;
	}

	if( (( varChild.vt != VT_I4 ) && ( varChild.lVal != CHILDID_SELF )) || ( NULL == pszHelp ) )
    {
        return E_INVALIDARG;
    }

	OnSetAccData (varChild.lVal);
	if (m_AccData.m_strAccHelp.IsEmpty ())
	{
		return S_FALSE;
	}

	*pszHelp = m_AccData.m_strAccHelp.AllocSysString ();
    return S_OK;
}

// Override in user's code?
HRESULT CBCGPWnd::get_accHelpTopic(BSTR* /*pszHelpFile*/, VARIANT /*varChild*/, long* /*pidTopic*/)
{
	return S_FALSE;
}

// Override in user's code?
HRESULT CBCGPWnd::get_accKeyboardShortcut(VARIANT varChild, BSTR *pszKeyboardShortcut)
{
	if( (( varChild.vt != VT_I4 ) && ( varChild.lVal != CHILDID_SELF )) )
    {
        return E_INVALIDARG;
    }

	if (varChild.vt == VT_I4 && varChild.lVal == CHILDID_SELF)
	{
		*pszKeyboardShortcut = SysAllocString (L"");
		return S_OK;
	}

	if ( !pszKeyboardShortcut || (( varChild.vt != VT_I4 ) && ( varChild.lVal != CHILDID_SELF )) )
    {
        return E_INVALIDARG;
    }

	if (varChild.vt == VT_I4 && varChild.lVal > 0)
	{
		OnSetAccData (varChild.lVal);
		if (m_AccData.m_strAccKeys.IsEmpty ())
		{
			return S_FALSE;
		}

		*pszKeyboardShortcut = m_AccData.m_strAccKeys.AllocSysString ();
		return S_OK;
	}

	return S_FALSE;
}

// Delegate to standard implementation?
HRESULT CBCGPWnd::get_accFocus(VARIANT *pvarChild)
{
	if( !pvarChild )
    {
        return E_INVALIDARG;
    }

    return DISP_E_MEMBERNOTFOUND;
}

// Investigate
HRESULT CBCGPWnd::get_accSelection(VARIANT *pvarChildren)
{
	if (!pvarChildren)
	{
		return E_INVALIDARG;
	}

	return DISP_E_MEMBERNOTFOUND;
}

// Override in user's code
HRESULT CBCGPWnd::get_accDefaultAction(VARIANT varChild, BSTR *pszDefaultAction)
{
	if (varChild.vt == VT_I4 && varChild.lVal == CHILDID_SELF)
	{
		return S_FALSE;
	}

	if( (( varChild.vt != VT_I4 ) && ( varChild.lVal != CHILDID_SELF ))) 
    {
        return E_INVALIDARG;
    }

	OnSetAccData (varChild.lVal);
	if (m_AccData.m_strAccDefAction.IsEmpty ())
	{
		return S_FALSE;
	}

	*pszDefaultAction = m_AccData.m_strAccDefAction.AllocSysString ();			
	return S_OK;
}

// Investigate
HRESULT CBCGPWnd::accSelect(long /*flagsSelect*/, VARIANT /*varChild*/)
{
	
	return E_FAIL;
}

// Delegate?
HRESULT CBCGPWnd::accLocation(long *pxLeft, long *pyTop, long *pcxWidth, long *pcyHeight, VARIANT varChild)
{
    if( !pxLeft || !pyTop || !pcxWidth || !pcyHeight )
    {
        return E_INVALIDARG;
    }

	if (varChild.vt == VT_I4 && varChild.lVal == CHILDID_SELF)
	{
		CRect rc;
		GetWindowRect (rc);
		
		*pxLeft = rc.left;
		*pyTop = rc.top;
		*pcxWidth = rc.Width ();
		*pcyHeight = rc.Height ();

		return S_OK;
	}

	if (varChild.vt == VT_I4 && varChild.lVal > 0)
	{
		OnSetAccData (varChild.lVal);

		*pxLeft = m_AccData.m_rectAccLocation.left;
		*pyTop = m_AccData.m_rectAccLocation.top;
		*pcxWidth = m_AccData.m_rectAccLocation.Width ();
		*pcyHeight = m_AccData.m_rectAccLocation.Height ();
		return S_OK;
	}

	return S_OK;
}

// Delegate? May have to implement for COM children
HRESULT CBCGPWnd::accNavigate(long /*navDir*/, VARIANT /*varStart*/, VARIANT* /*pvarEndUpAt*/)
{
	return S_FALSE;
}

// Delegate?
HRESULT CBCGPWnd::accHitTest(long xLeft, long yTop, VARIANT *pvarChild)
{
	if( !pvarChild )
    {
        return E_INVALIDARG;
    }

	OnSetAccData ((LONG) MAKELPARAM ((WORD)xLeft, (WORD)yTop));

	if (m_AccData.m_nAccHit != 0)
	{
		pvarChild->vt = VT_I4;
		LPARAM lParam = MAKELPARAM ((WORD)xLeft, (WORD)yTop);
		pvarChild->lVal = (LONG)lParam;
	}
	else
	{
		 pvarChild->vt = VT_I4;
         pvarChild->lVal = CHILDID_SELF;
	}

	return S_OK;
}

// Override in user's code
HRESULT CBCGPWnd::accDoDefaultAction(VARIANT /*varChild*/)
{
	ASSERT(FALSE);
	return S_FALSE;
}

HRESULT CBCGPWnd::put_accName(VARIANT /*varChild*/, BSTR /*szName*/)
{
	ASSERT(FALSE);
	return S_FALSE;
}

HRESULT CBCGPWnd::put_accValue(VARIANT /*varChild*/, BSTR /*szValue*/)
{
	ASSERT(FALSE);
	return S_FALSE;
}

BOOL CBCGPWnd::OnSetAccData (long /*lVal*/)
{
	return TRUE;
}

//**************************************************************************
LRESULT CBCGPWnd::OnBCGSetControlVMMode (WPARAM wp, LPARAM)
{
	m_bVisualManagerStyle = (BOOL) wp;
	return 0;
}
//**************************************************************************
LRESULT CBCGPWnd::OnBCGSetControlAero (WPARAM wp, LPARAM)
{
	m_bOnGlass = (BOOL) wp;
	return 0;
}
//**************************************************************************
void CBCGPWnd::SetControlVisualMode (CWnd* pParentWnd)
{
	if (pParentWnd == NULL)
	{
		return;
	}

	ASSERT_VALID (pParentWnd);

	if (pParentWnd->GetSafeHwnd () == NULL)
	{
		ASSERT (FALSE);
		return;
	}

	CBCGPDialog* pDlg = DYNAMIC_DOWNCAST(CBCGPDialog, pParentWnd);
	if (pDlg != NULL)
	{
		ASSERT_VALID (pDlg);
		m_bVisualManagerStyle = pDlg->IsVisualManagerStyle ();
		return;
	}

#ifndef _BCGSUITE_
	CBCGPPropertyPage* pPage = DYNAMIC_DOWNCAST(CBCGPPropertyPage, pParentWnd);
	if (pPage != NULL)
	{
		ASSERT_VALID (pPage);
		m_bVisualManagerStyle = pPage->IsVisualManagerStyle ();
		return;
	}
#endif
}