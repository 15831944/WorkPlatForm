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
// BCGPRibbonInfo.cpp: implementation of the CBCGPRibbonInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BCGPRibbonInfo.h"
#include "BCGPTagManager.h"
#include "BCGPCalculator.h"
#include "BCGPMath.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifndef BCGP_EXCLUDE_RIBBON

static const DWORD c_dwVersion = MAKELONG (1, 0);

static LPCTSTR s_szTag_Text                   = _T("TEXT");
static LPCTSTR s_szTag_ToolTip                = _T("TOOLTIP");
static LPCTSTR s_szTag_Description            = _T("DESCRIPTION");
static LPCTSTR s_szTag_Keys                   = _T("KEYS");
static LPCTSTR s_szTag_MenuKeys               = _T("KEYS_MENU");
static LPCTSTR s_szTag_PaletteTop             = _T("PALETTE_TOP");
static LPCTSTR s_szTag_AlwaysLarge            = _T("ALWAYS_LARGE");
static LPCTSTR s_szTag_AlwaysShowDescription  = _T("ALWAYS_DESCRIPTION");
static LPCTSTR s_szTag_QATType                = _T("QAT_TYPE");

static LPCTSTR s_szTag_Index                  = _T("INDEX");
static LPCTSTR s_szTag_IndexSmall             = _T("INDEX_SMALL");
static LPCTSTR s_szTag_IndexLarge             = _T("INDEX_LARGE");
static LPCTSTR s_szTag_DefaultCommand         = _T("DEFAULT_COMMAND");
static LPCTSTR s_szTag_Link                   = _T("LINK");
static LPCTSTR s_szTag_Width                  = _T("WIDTH");
static LPCTSTR s_szTag_Height                 = _T("HEIGHT");
static LPCTSTR s_szTag_WidthFloaty            = _T("WIDTH_FLOATY");
static LPCTSTR s_szTag_SpinButtons            = _T("SPIN_BUTTONS");
static LPCTSTR s_szTag_Min                    = _T("MIN");
static LPCTSTR s_szTag_Max                    = _T("MAX");
static LPCTSTR s_szTag_SearchMode             = _T("SEARCH_MODE");
static LPCTSTR s_szTag_SearchPrompt           = _T("SEARCH_PROMPT");
static LPCTSTR s_szTag_CalculatorMode         = _T("CALCULATOR_MODE");
static LPCTSTR s_szTag_CalculatorCmdExt       = _T("CALCULATOR_CMD_EXT");

static LPCTSTR s_szTag_EditBox                = _T("EDIT_BOX");
static LPCTSTR s_szTag_DropDownList           = _T("DROPDOWN_LIST");
static LPCTSTR s_szTag_ResizeDropDownList     = _T("DROPDOWN_LIST_RESIZE");
static LPCTSTR s_szTag_AutoComplete           = _T("AUTO_COMPLETE");

static LPCTSTR s_szTag_FontType               = _T("FONT_TYPE");
static LPCTSTR s_szTag_CharSet                = _T("CHAR_SET");
static LPCTSTR s_szTag_PitchAndFamily         = _T("PITCH_AND_FAMILY");

static LPCTSTR s_szTag_ButtonMode             = _T("BUTTON_MODE");
static LPCTSTR s_szTag_MenuResize             = _T("MENU_RESIZE");
static LPCTSTR s_szTag_MenuResizeVertical     = _T("MENU_RESIZE_VERTICAL");
static LPCTSTR s_szTag_IconsInRow             = _T("ICONS_IN_ROW");
static LPCTSTR s_szTag_IconWidth              = _T("ICON_WIDTH");
static LPCTSTR s_szTag_SizeIcon               = _T("SIZE_ICON");

static LPCTSTR s_szTag_Color                  = _T("COLOR");
static LPCTSTR s_szTag_SizeBox                = _T("SIZE_BOX");
static LPCTSTR s_szTag_SimpleButtonLook       = _T("SIMPLE_LOOK");
static LPCTSTR s_szTag_AutomaticColorBtn      = _T("AUTOMATIC_BTN");
static LPCTSTR s_szTag_OtherColorBtn          = _T("OTHER_BTN");
static LPCTSTR s_szTag_Border                 = _T("BORDER");

static LPCTSTR s_szTag_Style                  = _T("STYLE");
static LPCTSTR s_szTag_Pos                    = _T("POS");
static LPCTSTR s_szTag_ZoomButtons            = _T("ZOOM_BUTTONS");

static LPCTSTR s_szTag_Horiz                  = _T("HORIZ");

static LPCTSTR s_szTag_AlmostLargeText        = _T("ALMOST_LARGE_TEXT");
static LPCTSTR s_szTag_Static                 = _T("STATIC");
static LPCTSTR s_szTag_TextAlign              = _T("TEXT_ALIGN");

static LPCTSTR s_szTag_QATTop                 = _T("QAT_TOP");
static LPCTSTR s_szTag_JustifyColumns         = _T("JUSTIFY_COLUMNS");
static LPCTSTR s_szTag_CenterColumnVert       = _T("CENTER_COLUMN_VERT");

static LPCTSTR s_szTag_Enable                 = _T("ENABLE");
static LPCTSTR s_szTag_EnableToolTips         = _T("ENABLE_TOOLTIPS");
static LPCTSTR s_szTag_EnableToolTipsDescr    = _T("ENABLE_TOOLTIPS_DESCRIPTION");
static LPCTSTR s_szTag_EnableKeys             = _T("ENABLE_KEYS");
static LPCTSTR s_szTag_EnablePrintPreview     = _T("ENABLE_PRINTPREVIEW");
static LPCTSTR s_szTag_DrawUsingFont          = _T("ENABLE_DRAWUSINGFONT");
static LPCTSTR s_szTag_EnableBackstageMode    = _T("ENABLE_BACKSTAGEMODE");

static LPCTSTR s_szTag_Label                  = _T("LABEL");
static LPCTSTR s_szTag_Visible                = _T("VISIBLE");
static LPCTSTR s_szTag_Infinite               = _T("INFINITE");

static LPCTSTR s_szTag_RecentFileList         = _T("RECENT_FILE_LIST");
static LPCTSTR s_szTag_Search                 = _T("SEARCH_COMMAND");

static LPCTSTR s_szTag_MenuMode               = _T("MENU_MODE");

static LPCTSTR s_szTag_Element                = _T("ELEMENT");
static LPCTSTR s_szTag_Elements               = _T("ELEMENTS");
static LPCTSTR s_szTag_ElementsExtended       = _T("ELEMENTS_EXTENDED");
static LPCTSTR s_szTag_Item                   = _T("ITEM");
static LPCTSTR s_szTag_Items                  = _T("ITEMS");
static LPCTSTR s_szTag_Panel                  = _T("PANEL");
static LPCTSTR s_szTag_Panels                 = _T("PANELS");
static LPCTSTR s_szTag_Category               = _T("CATEGORY");
static LPCTSTR s_szTag_Categories             = _T("CATEGORIES");
static LPCTSTR s_szTag_Context                = _T("CONTEXT");
static LPCTSTR s_szTag_Contexts               = _T("CONTEXTS");
static LPCTSTR s_szTag_Group                  = _T("GROUP");
static LPCTSTR s_szTag_Groups                 = _T("GROUPS");
static LPCTSTR s_szTag_RibbonBar              = _T("RIBBON_BAR");
static LPCTSTR s_szTag_StatusBar              = _T("STATUS_BAR");
static LPCTSTR s_szTag_Button_Main            = _T("BUTTON_MAIN");
static LPCTSTR s_szTag_QAT_Elements           = _T("QAT_ELEMENTS");
static LPCTSTR s_szTag_Tab_Elements           = _T("TAB_ELEMENTS");
static LPCTSTR s_szTag_Button_Launch          = _T("BUTTON_LAUNCH");
static LPCTSTR s_szTag_CategoryMain           = _T("CATEGORY_MAIN");
static LPCTSTR s_szTag_CategoryBackstage      = _T("CATEGORY_BACKSTAGE");
static LPCTSTR s_szTag_CollapseOrder          = _T("COLLAPSE_ORDER");

static LPCTSTR s_szTag_Body                   = _T("BCGP_RIBBON");
static LPCTSTR s_szTag_Sizes                  = _T("SIZES");

static LPCTSTR s_szTag_Image                  = _T("IMAGE");
static LPCTSTR s_szTag_Image_Scenic           = _T("IMAGE_SCENIC");
//static LPCTSTR s_szTag_Image_Hot              = _T("IMAGE_HOT");
//static LPCTSTR s_szTag_Image_Disabled         = _T("IMAGE_DISABLED");
static LPCTSTR s_szTag_Image_Small            = _T("IMAGE_SMALL");
static LPCTSTR s_szTag_Image_Large            = _T("IMAGE_LARGE");
static LPCTSTR s_szTag_Image_SBGroup          = _T("IMAGE_SBGROUP");


LPCTSTR CBCGPRibbonInfo::s_szButton            = _T("Button");
LPCTSTR CBCGPRibbonInfo::s_szButton_Check      = _T("Button_Check");
LPCTSTR CBCGPRibbonInfo::s_szButton_Radio      = _T("Button_Radio");
LPCTSTR CBCGPRibbonInfo::s_szButton_Color      = _T("Button_Color");
LPCTSTR CBCGPRibbonInfo::s_szButton_Undo       = _T("Button_Undo");
LPCTSTR CBCGPRibbonInfo::s_szButton_Palette    = _T("Button_Palette");
LPCTSTR CBCGPRibbonInfo::s_szButton_Hyperlink  = _T("Button_Hyperlink");
LPCTSTR CBCGPRibbonInfo::s_szButton_Main       = _T("Button_Main");
LPCTSTR CBCGPRibbonInfo::s_szButton_MainPanel  = _T("Button_Main_Panel");
LPCTSTR CBCGPRibbonInfo::s_szButton_Command    = _T("Button_Command");
LPCTSTR CBCGPRibbonInfo::s_szButton_Launch     = _T("Button_Launch");
LPCTSTR CBCGPRibbonInfo::s_szLabel             = _T("Label");
LPCTSTR CBCGPRibbonInfo::s_szEdit              = _T("Edit");
LPCTSTR CBCGPRibbonInfo::s_szComboBox          = _T("ComboBox");
LPCTSTR CBCGPRibbonInfo::s_szComboBox_Font     = _T("ComboBox_Font");
LPCTSTR CBCGPRibbonInfo::s_szSlider            = _T("Slider");
LPCTSTR CBCGPRibbonInfo::s_szProgress          = _T("Progress");
LPCTSTR CBCGPRibbonInfo::s_szSeparator         = _T("Separator");
LPCTSTR CBCGPRibbonInfo::s_szGroup             = _T("Group");
LPCTSTR CBCGPRibbonInfo::s_szStatusPane        = _T("StatusPane");
LPCTSTR CBCGPRibbonInfo::s_szPanel             = _T("Panel");
LPCTSTR CBCGPRibbonInfo::s_szCategory          = _T("Category");
LPCTSTR CBCGPRibbonInfo::s_szContext           = _T("Context");
LPCTSTR CBCGPRibbonInfo::s_szCategoryMain      = _T("Category_Main");
LPCTSTR CBCGPRibbonInfo::s_szCategoryBackstage = _T("Category_Backstage");
LPCTSTR CBCGPRibbonInfo::s_szQAT               = _T("QAT");
LPCTSTR CBCGPRibbonInfo::s_szRibbonBar         = _T("RibbonBar");
LPCTSTR CBCGPRibbonInfo::s_szStatusBar         = _T("StatusBar");

CBCGPRibbonInfo::XElement::XElement(const CString& strElementName)
	: CBCGPBaseInfo::XBase (strElementName)
	, m_bIsOnPaletteTop    (FALSE)
	, m_bIsAlwaysLarge     (FALSE)
{
}

CBCGPRibbonInfo::XElement::~XElement()
{
}

CBCGPRibbonInfo::XElementSeparator::XElementSeparator()
	: CBCGPRibbonInfo::XElement (CBCGPRibbonInfo::s_szSeparator)
	, m_bIsHoriz                (FALSE)
{
}

CBCGPRibbonInfo::XElementSeparator::~XElementSeparator()
{
}

CBCGPRibbonInfo::XElementGroup::XElementGroup()
	: CBCGPRibbonInfo::XElement (CBCGPRibbonInfo::s_szGroup)
{
}

CBCGPRibbonInfo::XElementGroup::~XElementGroup()
{
	int i;
	for (i = 0; i < m_arButtons.GetSize (); i++)
	{
		if (m_arButtons[i] != NULL)
		{
			delete m_arButtons[i];
		}
	}
}

CBCGPRibbonInfo::XElementButton::XElementButton(const CString& strElementName)
	: CBCGPRibbonInfo::XElement (strElementName)
	, m_nSmallImageIndex        (-1)
	, m_nLargeImageIndex        (-1)
	, m_bIsDefaultCommand       (TRUE)
	, m_QATType                 (CBCGPRibbonButton::BCGPRibbonButton_Show_Default)
{
}

CBCGPRibbonInfo::XElementButton::XElementButton()
	: CBCGPRibbonInfo::XElement (CBCGPRibbonInfo::s_szButton)
	, m_nSmallImageIndex        (-1)
	, m_nLargeImageIndex        (-1)
	, m_bIsDefaultCommand       (TRUE)
	, m_bIsAlwaysShowDescription(FALSE)
	, m_QATType                 (CBCGPRibbonButton::BCGPRibbonButton_Show_Default)
{
}

CBCGPRibbonInfo::XElementButton::~XElementButton()
{
	int i;
	for (i = 0; i < m_arSubItems.GetSize (); i++)
	{
		if (m_arSubItems[i] != NULL)
		{
			delete m_arSubItems[i];
		}
	}
}

CBCGPRibbonInfo::XElementLabel::XElementLabel()
	: CBCGPRibbonInfo::XElementButton (CBCGPRibbonInfo::s_szLabel)
{
}

CBCGPRibbonInfo::XElementLabel::~XElementLabel()
{
}

CBCGPRibbonInfo::XElementButtonCheck::XElementButtonCheck()
	: CBCGPRibbonInfo::XElementButton (CBCGPRibbonInfo::s_szButton_Check)
{
}

CBCGPRibbonInfo::XElementButtonCheck::~XElementButtonCheck()
{
}

CBCGPRibbonInfo::XElementButtonRadio::XElementButtonRadio()
	: XElementButton (CBCGPRibbonInfo::s_szButton_Radio)
{
}

CBCGPRibbonInfo::XElementButtonRadio::~XElementButtonRadio()
{
}

CBCGPRibbonInfo::XElementButtonHyperlink::XElementButtonHyperlink()
	: CBCGPRibbonInfo::XElementButton (CBCGPRibbonInfo::s_szButton_Hyperlink)
{
}

CBCGPRibbonInfo::XElementButtonHyperlink::~XElementButtonHyperlink()
{
}

CBCGPRibbonInfo::XElementEdit::XElementEdit(const CString& strElementName)
	: CBCGPRibbonInfo::XElementButton (strElementName)
	, m_nWidth                        (0)
	, m_nWidthFloaty                  (0)
	, m_nTextAlign                    (ES_LEFT)
	, m_bHasSpinButtons               (FALSE)
	, m_nMin                          (INT_MAX)
	, m_nMax                          (INT_MAX)
	, m_bSearchMode                   (FALSE)
{
}

CBCGPRibbonInfo::XElementEdit::XElementEdit()
	: CBCGPRibbonInfo::XElementButton (CBCGPRibbonInfo::s_szEdit)
	, m_nWidth                        (0)
	, m_nWidthFloaty                  (0)
	, m_nTextAlign                    (ES_LEFT)
	, m_bHasSpinButtons               (FALSE)
	, m_nMin                          (INT_MAX)
	, m_nMax                          (INT_MAX)
	, m_bSearchMode                   (FALSE)
{
}

CBCGPRibbonInfo::XElementEdit::~XElementEdit()
{
}

CBCGPRibbonInfo::XElementComboBox::XElementComboBox(const CString& strElementName)
	: CBCGPRibbonInfo::XElementEdit (strElementName)
	, m_bHasEditBox                 (FALSE)
	, m_bHasDropDownList            (TRUE)
	, m_bResizeDropDownList         (TRUE)
	, m_bCalculatorMode             (FALSE)
	, m_bAutoComplete               (FALSE)
{
	m_nWidth = 108;
}

CBCGPRibbonInfo::XElementComboBox::XElementComboBox()
	: CBCGPRibbonInfo::XElementEdit (CBCGPRibbonInfo::s_szComboBox)
	, m_bHasEditBox                 (FALSE)
	, m_bHasDropDownList            (TRUE)
	, m_bResizeDropDownList         (TRUE)
	, m_bCalculatorMode             (FALSE)
	, m_bAutoComplete               (FALSE)
{
	m_nWidth = 108;
}

CBCGPRibbonInfo::XElementComboBox::~XElementComboBox()
{
}

CBCGPRibbonInfo::XElementFontComboBox::XElementFontComboBox()
	: CBCGPRibbonInfo::XElementComboBox (CBCGPRibbonInfo::s_szComboBox_Font)
	, m_nFontType                       (DEVICE_FONTTYPE | RASTER_FONTTYPE | TRUETYPE_FONTTYPE)
	, m_nCharSet                        (DEFAULT_CHARSET)
	, m_nPitchAndFamily                 (DEFAULT_PITCH)
{
	m_bHasEditBox = TRUE;
}

CBCGPRibbonInfo::XElementFontComboBox::~XElementFontComboBox()
{
}

CBCGPRibbonInfo::XElementButtonPalette::XPaletteGroup::XPaletteGroup()
	: m_nItems (0)
{
}

CBCGPRibbonInfo::XElementButtonPalette::XPaletteGroup::~XPaletteGroup()
{
}

CBCGPRibbonInfo::XElementButtonPalette::XElementButtonPalette(const CString& strElementName)
	: CBCGPRibbonInfo::XElementButton (strElementName)
	, m_bIsButtonMode                 (TRUE)
	, m_bEnableMenuResize             (FALSE)
	, m_bMenuResizeVertical           (FALSE)
	, m_nIconsInRow                   (-1)
	, m_sizeIcon                      (0, 0)
{
}

CBCGPRibbonInfo::XElementButtonPalette::XElementButtonPalette()
	: CBCGPRibbonInfo::XElementButton (CBCGPRibbonInfo::s_szButton_Palette)
	, m_bIsButtonMode                 (TRUE)
	, m_bEnableMenuResize             (FALSE)
	, m_bMenuResizeVertical           (FALSE)
	, m_nIconsInRow                   (-1)
	, m_sizeIcon                      (0, 0)
{
}

CBCGPRibbonInfo::XElementButtonPalette::~XElementButtonPalette()
{
	int i = 0;
	for (i = 0; i < m_arGroups.GetSize (); i++)
	{
		if (m_arGroups[i] != NULL)
		{
			delete m_arGroups[i];
		}
	}
}

CBCGPRibbonInfo::XElementButtonColor::XElementButtonColor()
	: CBCGPRibbonInfo::XElementButtonPalette (CBCGPRibbonInfo::s_szButton_Color)
	, m_clrColor                             (RGB (0, 0, 0))
	, m_bSimpleButtonLook                    (FALSE)
	, m_clrAutomaticBtnColor                 (RGB (0, 0, 0))
	, m_bAutomaticBtnOnTop                   (TRUE)
	, m_bAutomaticBtnBorder                  (FALSE)
{
	m_sizeIcon = CSize (22, 22);
	m_nIconsInRow   = 5;
}

CBCGPRibbonInfo::XElementButtonColor::~XElementButtonColor()
{
}

CBCGPRibbonInfo::XElementButtonUndo::XElementButtonUndo()
	: CBCGPRibbonInfo::XElementButtonPalette (CBCGPRibbonInfo::s_szButton_Undo)
{
}

CBCGPRibbonInfo::XElementButtonUndo::~XElementButtonUndo()
{
}

CBCGPRibbonInfo::XElementButtonLaunch::XElementButtonLaunch()
	: CBCGPRibbonInfo::XElementButton (CBCGPRibbonInfo::s_szButton_Launch)
{
}

CBCGPRibbonInfo::XElementButtonLaunch::~XElementButtonLaunch()
{
}

CBCGPRibbonInfo::XElementButtonMain::XElementButtonMain()
	: CBCGPRibbonInfo::XElementButton (CBCGPRibbonInfo::s_szButton_Main)
{
}

CBCGPRibbonInfo::XElementButtonMain::~XElementButtonMain()
{
}

CBCGPRibbonInfo::XElementButtonMainPanel::XElementButtonMainPanel()
	: CBCGPRibbonInfo::XElementButton (CBCGPRibbonInfo::s_szButton_MainPanel)
{
}

CBCGPRibbonInfo::XElementButtonMainPanel::~XElementButtonMainPanel()
{
}

CBCGPRibbonInfo::XElementButtonCommand::XElementButtonCommand()
	: CBCGPRibbonInfo::XElementButton (CBCGPRibbonInfo::s_szButton_Command)
	, m_bIsMenu                       (FALSE)
{
}

CBCGPRibbonInfo::XElementButtonCommand::~XElementButtonCommand()
{
}

CBCGPRibbonInfo::XElementSlider::XElementSlider()
	: CBCGPRibbonInfo::XElement (CBCGPRibbonInfo::s_szSlider)
	, m_dwStyle                 (0)
	, m_nWidth                  (100)
	, m_nMin                    (0)
	, m_nMax                    (100)
	, m_nPos                    (0)
	, m_bZoomButtons            (FALSE)
{
}

CBCGPRibbonInfo::XElementSlider::~XElementSlider()
{
}

CBCGPRibbonInfo::XElementProgressBar::XElementProgressBar()
	: CBCGPRibbonInfo::XElement (CBCGPRibbonInfo::s_szProgress)
	, m_nWidth                  (100)
	, m_nHeight                 (22)
	, m_nMin                    (0)
	, m_nMax                    (100)
	, m_nPos                    (0)
	, m_bInfinite               (FALSE)
{
}

CBCGPRibbonInfo::XElementProgressBar::~XElementProgressBar()
{
}

CBCGPRibbonInfo::XElementStatusPane::XElementStatusPane()
	: CBCGPRibbonInfo::XElementButton (CBCGPRibbonInfo::s_szStatusPane)
	, m_bIsStatic                     (TRUE)
	, m_nTextAlign                    (TA_LEFT)
{
}

CBCGPRibbonInfo::XElementStatusPane::~XElementStatusPane()
{
}

CBCGPRibbonInfo::XPanel::XPanel()
	: CBCGPBaseInfo::XBase (CBCGPRibbonInfo::s_szPanel)
	, m_nImageIndex        (-1)
	, m_bJustifyColumns    (FALSE)
	, m_bCenterColumnVert  (FALSE)
{
}

CBCGPRibbonInfo::XPanel::~XPanel()
{
	int i;
	for (i = 0; i < m_arElements.GetSize (); i++)
	{
		if (m_arElements[i] != NULL)
		{
			delete m_arElements[i];
		}
	}
}

CBCGPRibbonInfo::XCategory::XCategory()
	: CBCGPBaseInfo::XBase (CBCGPRibbonInfo::s_szCategory)
{
}

CBCGPRibbonInfo::XCategory::~XCategory()
{
	int i;
	for (i = 0; i < m_arPanels.GetSize (); i++)
	{
		if (m_arPanels[i] != NULL)
		{
			delete m_arPanels[i];
		}
	}

	for (i = 0; i < m_arElements.GetSize (); i++)
	{
		if (m_arElements[i] != NULL)
		{
			delete m_arElements[i];
		}
	}
}

CBCGPRibbonInfo::XContext::XContext()
	: CBCGPBaseInfo::XBase (CBCGPRibbonInfo::s_szContext)
	, m_Color              (BCGPCategoryColor_None)
{
}

CBCGPRibbonInfo::XContext::~XContext()
{
	int i;
	for (i = 0; i < m_arCategories.GetSize (); i++)
	{
		if (m_arCategories[i] != NULL)
		{
			delete m_arCategories[i];
		}
	}
}

CBCGPRibbonInfo::XCategoryMain::XCategoryMain()
	: CBCGPBaseInfo::XBase (CBCGPRibbonInfo::s_szCategoryMain)
	, m_bRecentListEnable  (FALSE)
	, m_nRecentListWidth   (300)
	, m_bSearchEnable      (FALSE)
	, m_nSearchWidth       (0)
{
}

CBCGPRibbonInfo::XCategoryMain::~XCategoryMain()
{
	int i = 0;
	for (i = 0; i < m_arElements.GetSize (); i++)
	{
		if (m_arElements[i] != NULL)
		{
			delete m_arElements[i];
		}
	}
}

CBCGPRibbonInfo::XCategoryBackstage::XCategoryBackstage()
	: CBCGPBaseInfo::XBase (CBCGPRibbonInfo::s_szCategoryBackstage)
{
}

CBCGPRibbonInfo::XCategoryBackstage::~XCategoryBackstage()
{
	int i = 0;
	for (i = 0; i < m_arElements.GetSize (); i++)
	{
		if (m_arElements[i] != NULL)
		{
			delete m_arElements[i];
		}
	}
}

CBCGPRibbonInfo::XQAT::XQATItem::XQATItem()
	: m_bVisible (TRUE)
{
}

CBCGPRibbonInfo::XQAT::XQATItem::~XQATItem()
{
}

CBCGPRibbonInfo::XQAT::XQAT()
	: CBCGPBaseInfo::XBase (CBCGPRibbonInfo::s_szQAT)
	, m_bOnTop             (TRUE)
{
}

CBCGPRibbonInfo::XQAT::~XQAT()
{
	m_arItems.RemoveAll ();
}

CBCGPRibbonInfo::XRibbonBar::XRibbonBar()
	: CBCGPBaseInfo::XBase (CBCGPRibbonInfo::s_szRibbonBar)
	, m_bToolTip           (TRUE)
	, m_bToolTipDescr      (TRUE)
	, m_bKeyTips           (TRUE)
	, m_bPrintPreview      (TRUE)
	, m_bDrawUsingFont     (FALSE)
	, m_btnMain            (NULL)
	, m_MainCategory       (NULL)
	, m_bBackstageMode     (FALSE)
	, m_BackstageCategory  (NULL)
{
}

CBCGPRibbonInfo::XRibbonBar::~XRibbonBar()
{
	if (m_btnMain != NULL)
	{
		delete m_btnMain;
	}

	if (m_MainCategory != NULL)
	{
		delete m_MainCategory;
	}

	if (m_BackstageCategory != NULL)
	{
		delete m_BackstageCategory;
	}

	int i;
	for (i = 0; i < m_arCategories.GetSize (); i++)
	{
		if (m_arCategories[i] != NULL)
		{
			delete m_arCategories[i];
		}
	}

	for (i = 0; i < m_arContexts.GetSize (); i++)
	{
		if (m_arContexts[i] != NULL)
		{
			delete m_arContexts[i];
		}
	}
}

CBCGPRibbonInfo::XStatusBar::XStatusElements::XStatusElements()
{
}

CBCGPRibbonInfo::XStatusBar::XStatusElements::~XStatusElements()
{
	int i;
	for (i = 0; i < m_arElements.GetSize (); i++)
	{
		if (m_arElements[i] != NULL)
		{
			delete m_arElements[i];
		}
	}
}

CBCGPRibbonInfo::XStatusBar::XStatusBar()
	: CBCGPBaseInfo::XBase (CBCGPRibbonInfo::s_szStatusBar)
{
}

CBCGPRibbonInfo::XStatusBar::~XStatusBar()
{
}


CBCGPBaseInfo::XBase* CBCGPRibbonInfo::CreateBaseFromName (const CString& name)
{
	CBCGPBaseInfo::XBase* base = NULL;

	if (name.Compare (CBCGPRibbonInfo::s_szPanel) == 0)
	{
		base = new CBCGPRibbonInfo::XPanel;
	}
	else if (name.Compare (CBCGPRibbonInfo::s_szCategoryMain) == 0)
	{
		base = new CBCGPRibbonInfo::XCategoryMain;
	}
	else if (name.Compare (CBCGPRibbonInfo::s_szCategoryBackstage) == 0)
	{
		base = new CBCGPRibbonInfo::XCategoryBackstage;
	}
	else if (name.Compare (CBCGPRibbonInfo::s_szCategory) == 0)
	{
		base = new CBCGPRibbonInfo::XCategory;
	}
	else if (name.Compare (CBCGPRibbonInfo::s_szRibbonBar) == 0)
	{
		base = new CBCGPRibbonInfo::XRibbonBar;
	}
	else if (name.Compare (CBCGPRibbonInfo::s_szStatusBar) == 0)
	{
		base = new CBCGPRibbonInfo::XStatusBar;
	}
	else if (name.Compare (CBCGPRibbonInfo::s_szQAT) == 0)
	{
		base = new CBCGPRibbonInfo::XQAT;
	}
	else if (name.Compare (CBCGPRibbonInfo::s_szContext) == 0)
	{
		base = new CBCGPRibbonInfo::XContext;
	}
	else
	{
		base = CBCGPRibbonInfo::XElement::CreateFromName (name);
	}

	return base;
}

CBCGPBaseInfo::XBase* CBCGPRibbonInfo::CreateBaseFromTag (const CString& tag)
{
	return CBCGPBaseInfo::CreateBaseFromTag (tag, &CBCGPRibbonInfo::CreateBaseFromName);
}

CBCGPRibbonInfo::XElement* CBCGPRibbonInfo::XElement::CreateFromName (const CString& name)
{
	CBCGPRibbonInfo::XElement* element = NULL;

	if (name.Compare (CBCGPRibbonInfo::s_szButton) == 0)
	{
		element = new CBCGPRibbonInfo::XElementButton;
	}
	else if (name.Compare (CBCGPRibbonInfo::s_szButton_Check) == 0)
	{
		element = new CBCGPRibbonInfo::XElementButtonCheck;
	}
	else if (name.Compare (CBCGPRibbonInfo::s_szButton_Radio) == 0)
	{
		element = new CBCGPRibbonInfo::XElementButtonRadio;
	}
	else if (name.Compare (CBCGPRibbonInfo::s_szButton_Color) == 0)
	{
		element = new CBCGPRibbonInfo::XElementButtonColor;
	}
	else if (name.Compare (CBCGPRibbonInfo::s_szButton_Undo) == 0)
	{
		element = new CBCGPRibbonInfo::XElementButtonUndo;
	}
	else if (name.Compare (CBCGPRibbonInfo::s_szButton_Palette) == 0 ||
			name.Compare (_T("Button_Gallery")) == 0)
	{
		element = new CBCGPRibbonInfo::XElementButtonPalette;
	}
	else if (name.Compare (CBCGPRibbonInfo::s_szButton_Hyperlink) == 0 ||
			name.Compare (_T("Button_LinkCtrl")) == 0)
	{
		element = new CBCGPRibbonInfo::XElementButtonHyperlink;
	}
	else if (name.Compare (CBCGPRibbonInfo::s_szButton_Main) == 0)
	{
		element = new CBCGPRibbonInfo::XElementButtonMain;
	}
	else if (name.Compare (CBCGPRibbonInfo::s_szButton_MainPanel) == 0)
	{
		element = new CBCGPRibbonInfo::XElementButtonMainPanel;
	}
	else if (name.Compare (CBCGPRibbonInfo::s_szButton_Command) == 0)
	{
		element = new CBCGPRibbonInfo::XElementButtonCommand;
	}
	else if (name.Compare (CBCGPRibbonInfo::s_szButton_Launch) == 0)
	{
		element = new CBCGPRibbonInfo::XElementButtonLaunch;
	}
	else if (name.Compare (CBCGPRibbonInfo::s_szLabel) == 0)
	{
		element = new CBCGPRibbonInfo::XElementLabel;
	}
	else if (name.Compare (CBCGPRibbonInfo::s_szEdit) == 0)
	{
		element = new CBCGPRibbonInfo::XElementEdit;
	}
	else if (name.Compare (CBCGPRibbonInfo::s_szComboBox) == 0)
	{
		element = new CBCGPRibbonInfo::XElementComboBox;
	}
	else if (name.Compare (CBCGPRibbonInfo::s_szComboBox_Font) == 0)
	{
		element = new CBCGPRibbonInfo::XElementFontComboBox;
	}
	else if (name.Compare (CBCGPRibbonInfo::s_szSlider) == 0)
	{
		element = new CBCGPRibbonInfo::XElementSlider;
	}
	else if (name.Compare (CBCGPRibbonInfo::s_szProgress) == 0)
	{
		element = new CBCGPRibbonInfo::XElementProgressBar;
	}
	else if (name.Compare (CBCGPRibbonInfo::s_szSeparator) == 0)
	{
		element = new CBCGPRibbonInfo::XElementSeparator;
	}
	else if (name.Compare (CBCGPRibbonInfo::s_szGroup) == 0)
	{
		element = new CBCGPRibbonInfo::XElementGroup;
	}
	else if (name.Compare (CBCGPRibbonInfo::s_szStatusPane) == 0)
	{
		element = new CBCGPRibbonInfo::XElementStatusPane;
	}

	return element;
}

BOOL CBCGPRibbonInfo::XElement::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	CString strID;
	if (tm.ExcludeTag (s_szTag_ID, strID))
	{
		m_ID.FromTag (strID);
	}

	tm.ReadString (s_szTag_Text, m_strText);
	CBCGPTagManager::Entity_FromTag (m_strText);
	tm.ReadString (s_szTag_ToolTip, m_strToolTip);
	CBCGPTagManager::Entity_FromTag (m_strToolTip);
	tm.ReadString (s_szTag_Description, m_strDescription);
	CBCGPTagManager::Entity_FromTag (m_strDescription);
	tm.ReadString (s_szTag_Keys, m_strKeys);
	CBCGPTagManager::Entity_FromTag (m_strKeys);
	tm.ReadString (s_szTag_MenuKeys, m_strMenuKeys);
	CBCGPTagManager::Entity_FromTag (m_strMenuKeys);
	tm.ReadBool (s_szTag_PaletteTop, m_bIsOnPaletteTop);
	tm.ReadBool (s_szTag_AlwaysLarge, m_bIsAlwaysLarge);

	return TRUE;
}

void CBCGPRibbonInfo::XElement::ToTag (CString& strTag) const
{
	XBase::ToTag (strTag);

	CString strID;
	m_ID.ToTag (strID);
	CBCGPTagManager::WriteItem (strTag, s_szTag_ID, strID);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteString (s_szTag_Text, CBCGPTagManager::Entity_ToTag (m_strText)));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteString (s_szTag_ToolTip, CBCGPTagManager::Entity_ToTag (m_strToolTip)));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteString (s_szTag_Description, CBCGPTagManager::Entity_ToTag (m_strDescription)));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteString (s_szTag_Keys, CBCGPTagManager::Entity_ToTag (m_strKeys)));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteString (s_szTag_MenuKeys, CBCGPTagManager::Entity_ToTag (m_strMenuKeys)));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_PaletteTop, m_bIsOnPaletteTop, FALSE));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_AlwaysLarge, m_bIsAlwaysLarge, FALSE));
}

BOOL CBCGPRibbonInfo::XElementSeparator::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	tm.ReadBool (s_szTag_Horiz, m_bIsHoriz);

	return TRUE;
}

void CBCGPRibbonInfo::XElementSeparator::ToTag (CString& strTag) const
{
	XBase::ToTag (strTag);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_Horiz, m_bIsHoriz, FALSE));
}

BOOL CBCGPRibbonInfo::XElementGroup::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	CString strElements;
	if (tm.ExcludeTag (s_szTag_Elements, strElements))
	{
		CBCGPTagManager tmElements (strElements);

		CString strElement;
		while (tmElements.ExcludeTag (s_szTag_Element, strElement))
		{
			XElement* pElement = (XElement*)CBCGPRibbonInfo::CreateBaseFromTag (strElement);
			if (pElement != NULL)
			{
				m_arButtons.Add (pElement);
			}
		}
	}

    CString strImage;
    if (tm.ExcludeTag(s_szTag_Image, strImage))
    {
        m_Images.FromTag(strImage);
    }

	return TRUE;
}

void CBCGPRibbonInfo::XElementGroup::ToTag (CString& strTag) const
{
	XBase::ToTag (strTag);

	CString strImage;
	m_Images.ToTag (strImage);
	CBCGPTagManager::WriteItem (strTag, s_szTag_Image, strImage);
	
	CString strElements;

	int i = 0;
	for (i = 0; i < m_arButtons.GetSize (); i++)
	{
		CString strElement;
		m_arButtons[i]->ToTag (strElement);
		CBCGPTagManager::WriteItem (strElements, s_szTag_Element, strElement);
	}

	CBCGPTagManager::WriteItem (strTag, s_szTag_Elements, strElements);
}

BOOL CBCGPRibbonInfo::XElementButton::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	CString strElements;
	if (tm.ExcludeTag (s_szTag_Elements, strElements))
	{
		CBCGPTagManager tmElements (strElements);

		CString strElement;
		while (tmElements.ExcludeTag (s_szTag_Element, strElement))
		{
			XElement* pElement = (XElement*)CBCGPRibbonInfo::CreateBaseFromTag (strElement);
			if (pElement != NULL)
			{
				m_arSubItems.Add (pElement);
			}
		}
	}

	tm.ReadInt (s_szTag_IndexSmall, m_nSmallImageIndex);
	tm.ReadInt (s_szTag_IndexLarge, m_nLargeImageIndex);
	tm.ReadBool (s_szTag_DefaultCommand, m_bIsDefaultCommand);
	tm.ReadBool (s_szTag_AlwaysShowDescription, m_bIsAlwaysShowDescription);

	int QATType = (int)CBCGPRibbonButton::BCGPRibbonButton_Show_Default;
	tm.ReadInt (s_szTag_QATType, QATType);
	m_QATType = (CBCGPRibbonButton::RibbonButtonOnQAT)bcg_clamp (QATType, (int)CBCGPRibbonButton::BCGPRibbonButton_Show_Default, (int)CBCGPRibbonButton::BCGPRibbonButton_Show_As_RadioButton);

	return XElement::FromTag (tm.GetBuffer ());
}

void CBCGPRibbonInfo::XElementButton::ToTag (CString& strTag) const
{
	XElement::ToTag (strTag);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_IndexSmall, m_nSmallImageIndex, -1));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_IndexLarge, m_nLargeImageIndex, -1));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_DefaultCommand, m_bIsDefaultCommand, TRUE));

	if (GetElementName ().Compare (s_szButton) == 0)
	{
		CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_AlwaysShowDescription, m_bIsAlwaysShowDescription, FALSE));
	}

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_QATType, (int)m_QATType, (int)CBCGPRibbonButton::BCGPRibbonButton_Show_Default));

	CString strElements;

	int i = 0;
	for (i = 0; i < m_arSubItems.GetSize (); i++)
	{
		CString strElement;
		m_arSubItems[i]->ToTag (strElement);
		CBCGPTagManager::WriteItem (strElements, s_szTag_Element, strElement);
	}

	CBCGPTagManager::WriteItem (strTag, s_szTag_Elements, strElements);
}

BOOL CBCGPRibbonInfo::XElementLabel::FromTag (const CString& strTag)
{
	return XElementButton::FromTag (strTag);
}

void CBCGPRibbonInfo::XElementLabel::ToTag (CString& strTag) const
{
	XElementButton::ToTag (strTag);
}

BOOL CBCGPRibbonInfo::XElementButtonCheck::FromTag (const CString& strTag)
{
	return XElementButton::FromTag (strTag);
}

void CBCGPRibbonInfo::XElementButtonCheck::ToTag (CString& strTag) const
{
	XElementButton::ToTag (strTag);
}

BOOL CBCGPRibbonInfo::XElementButtonRadio::FromTag (const CString& strTag)
{
	return XElementButton::FromTag (strTag);
}

void CBCGPRibbonInfo::XElementButtonRadio::ToTag (CString& strTag) const
{
	XElementButton::ToTag (strTag);
}

BOOL CBCGPRibbonInfo::XElementButtonHyperlink::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	tm.ReadString (s_szTag_Link, m_strLink);

	return XElementButton::FromTag (tm.GetBuffer ());
}

void CBCGPRibbonInfo::XElementButtonHyperlink::ToTag (CString& strTag) const
{
	XElementButton::ToTag (strTag);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteString (s_szTag_Link, m_strLink));
}

BOOL CBCGPRibbonInfo::XElementEdit::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	tm.ReadInt (s_szTag_Width, m_nWidth);
	tm.ReadInt (s_szTag_WidthFloaty, m_nWidthFloaty);
	tm.ReadBool (s_szTag_SearchMode, m_bSearchMode);
	tm.ReadInt (s_szTag_TextAlign, m_nTextAlign);

	if (m_bSearchMode)
	{
		tm.ReadString (s_szTag_SearchPrompt, m_strSearchPrompt);
		CBCGPTagManager::Entity_FromTag (m_strSearchPrompt);
	}
	else
	{
		tm.ReadBool (s_szTag_SpinButtons, m_bHasSpinButtons);
		if (m_bHasSpinButtons)
		{
			tm.ReadInt (s_szTag_Min, m_nMin);
			tm.ReadInt (s_szTag_Max, m_nMax);
		}
	}

	if (!XElementButton::FromTag (tm.GetBuffer ()))
	{
		return FALSE;
	}

	CString strID;
	tm.ExcludeTag (s_szTag_ID, strID);

	tm.ReadString(s_szTag_Value, m_strValue);
	CBCGPTagManager::Entity_FromTag (m_strValue);

	return TRUE;
}

void CBCGPRibbonInfo::XElementEdit::ToTag (CString& strTag) const
{
	XElementButton::ToTag (strTag);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_Width, m_nWidth, 0));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_WidthFloaty, m_nWidthFloaty, 0));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_TextAlign, m_nTextAlign, ES_LEFT));

	if (m_bSearchMode)
	{
		CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_SearchMode, m_bSearchMode, FALSE));
		CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteString (s_szTag_SearchPrompt, CBCGPTagManager::Entity_ToTag (m_strSearchPrompt)));
	}
	else
	{
		if (m_bHasSpinButtons)
		{
			CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_SpinButtons, m_bHasSpinButtons, FALSE));
			CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_Min, m_nMin, INT_MAX));
			CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_Max, m_nMax, INT_MAX));
		}
	}

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteString(s_szTag_Value, CBCGPTagManager::Entity_ToTag (m_strValue)));
}

BOOL CBCGPRibbonInfo::XElementComboBox::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	tm.ReadBool (s_szTag_CalculatorMode, m_bCalculatorMode);
	if (m_bCalculatorMode)
	{
		CString strExt;
		if (tm.ReadString (s_szTag_CalculatorCmdExt, strExt))
		{
			if (!strExt.IsEmpty ())
			{
				CBCGPTagManager::Entity_FromTag (strExt);

				CStringArray sa;
				CBCGPTagManager::ParseString(strExt, _T(";"), sa, TRUE, FALSE);
				if (sa.GetSize () == 0)
				{
					sa.Add (strExt);
				}

				for (int i = 0; i < sa.GetSize (); i++)
				{
					CBCGPCalculator::CalculatorCommands nCommand = 
						CBCGPCalculator::NameToCommand (sa[i]);
					if (nCommand != CBCGPCalculator::idCommandNone)
					{
						m_lstCalculatorExt.AddTail (nCommand);
					}
				}
			}
		}
	}
	else
	{
		CString strItems;
		if (tm.ExcludeTag (s_szTag_Items, strItems))
		{
			CBCGPTagManager tmItem (strItems);

			CString strItem;
			while (tmItem.ExcludeTag (s_szTag_Item, strItem))
			{
				CBCGPTagManager::Entity_FromTag (strItem);
				m_arItems.Add (strItem);
			}
		}

		tm.ReadBool (s_szTag_EditBox, m_bHasEditBox);
		tm.ReadBool (s_szTag_DropDownList, m_bHasDropDownList);
		tm.ReadBool (s_szTag_ResizeDropDownList, m_bResizeDropDownList);
		tm.ReadBool (s_szTag_AutoComplete, m_bAutoComplete);
	}

	return XElementEdit::FromTag (tm.GetBuffer ());
}

void CBCGPRibbonInfo::XElementComboBox::ToTag (CString& strTag) const
{
	XElementEdit::ToTag (strTag);

	if (m_bCalculatorMode)
	{
		CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_CalculatorMode, m_bCalculatorMode, FALSE));

		CString strExt;
		POSITION pos = m_lstCalculatorExt.GetHeadPosition ();
		while (pos != NULL)
		{
			if (!strExt.IsEmpty ())
			{
				strExt += _T(";");
			}

			strExt += CBCGPCalculator::CommandToName ((CBCGPCalculator::CalculatorCommands)m_lstCalculatorExt.GetNext (pos));
		}

		CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteString (s_szTag_CalculatorCmdExt, CBCGPTagManager::Entity_ToTag (strExt)));
	}
	else
	{
		CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_EditBox, m_bHasEditBox, FALSE));
		CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_DropDownList, m_bHasDropDownList, TRUE));
		CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_ResizeDropDownList, m_bResizeDropDownList, TRUE));
		CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_AutoComplete, m_bAutoComplete, FALSE));

		CString strItems;

		int i = 0;
		for (i = 0; i < m_arItems.GetSize (); i++)
		{
			CBCGPTagManager::WriteTag (strItems, CBCGPTagManager::WriteString (s_szTag_Item, CBCGPTagManager::Entity_ToTag (m_arItems[i]), m_arItems[i] + _T("_")));
		}

		CBCGPTagManager::WriteItem (strTag, s_szTag_Items, strItems);
	}
}

BOOL CBCGPRibbonInfo::XElementFontComboBox::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	tm.ReadInt (s_szTag_FontType, m_nFontType);

	int nValue = m_nCharSet;
	tm.ReadInt (s_szTag_CharSet, nValue);
	m_nCharSet = (BYTE)nValue;

	nValue = m_nPitchAndFamily;
	tm.ReadInt (s_szTag_PitchAndFamily, nValue);
	m_nPitchAndFamily = (BYTE)nValue;

	return XElementComboBox::FromTag (tm.GetBuffer ());
}

void CBCGPRibbonInfo::XElementFontComboBox::ToTag (CString& strTag) const
{
	XElementComboBox::ToTag (strTag);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_FontType, m_nFontType, DEVICE_FONTTYPE | RASTER_FONTTYPE | TRUETYPE_FONTTYPE));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_CharSet, m_nCharSet, DEFAULT_CHARSET));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_PitchAndFamily, m_nPitchAndFamily, DEFAULT_PITCH));
}

BOOL CBCGPRibbonInfo::XElementButtonPalette::XPaletteGroup::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	tm.ReadString (s_szTag_Name, m_strName);
	CBCGPTagManager::Entity_FromTag (m_strName);

	tm.ReadInt (s_szTag_Items, m_nItems);
/*
	CString strToolTip;
	while (tm.ExcludeTag (s_szTag_ToolTip, strToolTip))
	{
		CBCGPTagManager::Entity_FromTag (strToolTip);
		m_arToolTips.Add (strToolTip);
	}
*/
	return TRUE;
}

void CBCGPRibbonInfo::XElementButtonPalette::XPaletteGroup::ToTag (CString& strTag) const
{
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteString (s_szTag_Name, CBCGPTagManager::Entity_ToTag (m_strName)));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_Items, m_nItems, 0));
/*
	if (m_arToolTips.GetSize () > 0)
	{
		int i = 0;
		for (i = 0; i < m_arToolTips.GetSize (); i++)
		{
			CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteString (s_szTag_ToolTip, CBCGPTagManager::Entity_ToTag (m_arToolTips[i]), m_arToolTips[i] + _T("_")));
		}
	}
*/
}

BOOL CBCGPRibbonInfo::XElementButtonPalette::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	CString strGroups;
	if (tm.ExcludeTag (s_szTag_Groups, strGroups))
	{
		CBCGPTagManager tmGroups (strGroups);

		CString strGroup;
		while (tmGroups.ExcludeTag (s_szTag_Group, strGroup))
		{
			XPaletteGroup* pGroup = new XPaletteGroup;
			if (pGroup->FromTag (strGroup))
			{
				m_arGroups.Add (pGroup);
			}
			else
			{
				delete pGroup;
			}
		}
	}

	tm.ReadBool (s_szTag_ButtonMode, m_bIsButtonMode);
	tm.ReadBool (s_szTag_MenuResize, m_bEnableMenuResize);
	tm.ReadBool (s_szTag_MenuResizeVertical, m_bMenuResizeVertical);
	tm.ReadInt (s_szTag_IconsInRow, m_nIconsInRow);
	if (!tm.ReadSize (s_szTag_SizeBox, m_sizeIcon))
	{
		tm.ReadSize (s_szTag_SizeIcon, m_sizeIcon);
	}

	CString strImage;
	if (tm.ExcludeTag(s_szTag_Image, strImage))
	{
		m_Images.FromTag(strImage);
		m_Images.m_Image.SetImageSize (m_sizeIcon);
	}

	return XElementButton::FromTag (tm.GetBuffer ());
}

void CBCGPRibbonInfo::XElementButtonPalette::ToTag (CString& strTag) const
{
	XElementButton::ToTag (strTag);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_ButtonMode, m_bIsButtonMode, TRUE));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_MenuResize, m_bEnableMenuResize, FALSE));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_MenuResizeVertical, m_bMenuResizeVertical, FALSE));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_IconsInRow, m_nIconsInRow, -1));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteSize (s_szTag_SizeIcon, m_sizeIcon, CSize (0, 0)));

	if (m_arGroups.GetSize () > 0)
	{
		CString strGroups;

		int i = 0;
		for (i = 0; i < m_arGroups.GetSize (); i++)
		{
			CString strGroup;
			m_arGroups[i]->ToTag (strGroup);
			CBCGPTagManager::WriteItem (strGroups, s_szTag_Group, strGroup);
		}

		CBCGPTagManager::WriteItem (strTag, s_szTag_Groups, strGroups);
	}

	if (!m_Images.IsEmpty ())
	{
		CString strImage;
		m_Images.ToTag (strImage);
		CBCGPTagManager::WriteItem (strTag, s_szTag_Image, strImage);
	}
}

BOOL CBCGPRibbonInfo::XElementButtonColor::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	CString strTagBtn;
	if (tm.ExcludeTag (s_szTag_AutomaticColorBtn, strTagBtn))
	{
		CBCGPTagManager tmBtn (strTagBtn);

		tmBtn.CBCGPTagManager::ReadString (s_szTag_Label, m_strAutomaticBtnLabel);
		CBCGPTagManager::Entity_FromTag (m_strAutomaticBtnLabel);
		tmBtn.CBCGPTagManager::ReadString (s_szTag_ToolTip, m_strAutomaticBtnToolTip);
		CBCGPTagManager::Entity_FromTag (m_strAutomaticBtnToolTip);
		tmBtn.CBCGPTagManager::ReadColor (s_szTag_Color, m_clrAutomaticBtnColor);
		tmBtn.CBCGPTagManager::ReadBool (s_szTag_PaletteTop, m_bAutomaticBtnOnTop);
		tmBtn.CBCGPTagManager::ReadBool (s_szTag_Border, m_bAutomaticBtnBorder);
	}

	strTagBtn.Empty ();
	if (tm.ExcludeTag (s_szTag_OtherColorBtn, strTagBtn))
	{
		CBCGPTagManager tmBtn (strTagBtn);

		tmBtn.CBCGPTagManager::ReadString (s_szTag_Label, m_strOtherBtnLabel);
		CBCGPTagManager::Entity_FromTag (m_strOtherBtnLabel);
		tmBtn.CBCGPTagManager::ReadString (s_szTag_ToolTip, m_strOtherBtnToolTip);
		CBCGPTagManager::Entity_FromTag (m_strOtherBtnToolTip);
	}

	tm.ReadColor (s_szTag_Color, m_clrColor);
	tm.ReadBool (s_szTag_SimpleButtonLook, m_bSimpleButtonLook);

	return XElementButtonPalette::FromTag (tm.GetBuffer ());
}

void CBCGPRibbonInfo::XElementButtonColor::ToTag (CString& strTag) const
{
	XElementButtonPalette::ToTag (strTag);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteColor (s_szTag_Color, m_clrColor, RGB (0, 0, 0)));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_SimpleButtonLook, m_bSimpleButtonLook, FALSE));

	CString strTagBtn;
	CBCGPTagManager::WriteTag (strTagBtn, CBCGPTagManager::WriteString (s_szTag_Label, CBCGPTagManager::Entity_ToTag(m_strAutomaticBtnLabel)));
	CBCGPTagManager::WriteTag (strTagBtn, CBCGPTagManager::WriteString (s_szTag_ToolTip, CBCGPTagManager::Entity_ToTag(m_strAutomaticBtnToolTip)));
	CBCGPTagManager::WriteTag (strTagBtn, CBCGPTagManager::WriteColor (s_szTag_Color, m_clrAutomaticBtnColor, RGB (0, 0, 0)));
	CBCGPTagManager::WriteTag (strTagBtn, CBCGPTagManager::WriteBool (s_szTag_PaletteTop, m_bAutomaticBtnOnTop, TRUE));
	CBCGPTagManager::WriteTag (strTagBtn, CBCGPTagManager::WriteBool (s_szTag_Border, m_bAutomaticBtnBorder, FALSE));

	CBCGPTagManager::WriteItem (strTag, s_szTag_AutomaticColorBtn, strTagBtn);

	strTagBtn.Empty ();
	CBCGPTagManager::WriteTag (strTagBtn, CBCGPTagManager::WriteString (s_szTag_Label, CBCGPTagManager::Entity_ToTag(m_strOtherBtnLabel)));
	CBCGPTagManager::WriteTag (strTagBtn, CBCGPTagManager::WriteString (s_szTag_ToolTip, CBCGPTagManager::Entity_ToTag(m_strOtherBtnToolTip)));

	CBCGPTagManager::WriteItem (strTag, s_szTag_OtherColorBtn, strTagBtn);
}

BOOL CBCGPRibbonInfo::XElementButtonUndo::FromTag (const CString& strTag)
{
	return XElementButtonPalette::FromTag (strTag);
}

void CBCGPRibbonInfo::XElementButtonUndo::ToTag (CString& strTag) const
{
	XElementButtonPalette::ToTag (strTag);
}

BOOL CBCGPRibbonInfo::XElementButtonLaunch::FromTag (const CString& strTag)
{
	return XElementButton::FromTag (strTag);
}

void CBCGPRibbonInfo::XElementButtonLaunch::ToTag (CString& strTag) const
{
	if (m_ID.m_Value != 0)
	{
		XElementButton::ToTag (strTag);
	}
}

BOOL CBCGPRibbonInfo::XElementButtonMain::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

    CString strImage;
    if (tm.ExcludeTag(s_szTag_Image, strImage))
    {
        m_Image.FromTag(strImage);
    }

    CString strImageScenic;
    if (tm.ExcludeTag(s_szTag_Image_Scenic, strImageScenic))
    {
        m_ImageScenic.FromTag(strImageScenic);
    }

	return XElementButton::FromTag (tm.GetBuffer ());
}

void CBCGPRibbonInfo::XElementButtonMain::ToTag (CString& strTag) const
{
	XElementButton::ToTag (strTag);

	CString strImage;
	m_Image.ToTag (strImage);
	CBCGPTagManager::WriteItem (strTag, s_szTag_Image, strImage);

	CString strImageScenic;
	m_ImageScenic.ToTag (strImageScenic);
	CBCGPTagManager::WriteItem (strTag, s_szTag_Image_Scenic, strImageScenic);
}

BOOL CBCGPRibbonInfo::XElementButtonMainPanel::FromTag (const CString& strTag)
{
	return XElementButton::FromTag (strTag);
}

void CBCGPRibbonInfo::XElementButtonMainPanel::ToTag (CString& strTag) const
{
	XElementButton::ToTag (strTag);
}

BOOL CBCGPRibbonInfo::XElementButtonCommand::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	tm.ReadBool (s_szTag_MenuMode, m_bIsMenu);

	return XElementButton::FromTag (strTag);
}

void CBCGPRibbonInfo::XElementButtonCommand::ToTag (CString& strTag) const
{
	XElementButton::ToTag (strTag);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_MenuMode, m_bIsMenu, FALSE));
}

BOOL CBCGPRibbonInfo::XElementSlider::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	int nValue = (int)m_dwStyle;
	tm.ReadInt (s_szTag_Style, nValue);
	m_dwStyle = (DWORD)nValue;

	tm.ReadInt (s_szTag_Width, m_nWidth);
	tm.ReadInt (s_szTag_Min, m_nMin);
	tm.ReadInt (s_szTag_Max, m_nMax);
	tm.ReadInt (s_szTag_Pos, m_nPos);
	m_nPos = min (max (m_nMin, m_nPos), m_nMax);
	tm.ReadBool (s_szTag_ZoomButtons, m_bZoomButtons);

	return XElement::FromTag (tm.GetBuffer ());
}

void CBCGPRibbonInfo::XElementSlider::ToTag (CString& strTag) const
{
	XElement::ToTag (strTag);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteUInt (s_szTag_Style, m_dwStyle, 0));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_Width, m_nWidth, 100));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_Min, m_nMin, 0));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_Max, m_nMax, 100));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_Pos, m_nPos, m_nMin));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_ZoomButtons, m_bZoomButtons, FALSE));
}

BOOL CBCGPRibbonInfo::XElementProgressBar::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	tm.ReadInt (s_szTag_Width, m_nWidth);
	tm.ReadInt (s_szTag_Height, m_nHeight);
	tm.ReadInt (s_szTag_Min, m_nMin);
	tm.ReadInt (s_szTag_Max, m_nMax);
	tm.ReadInt (s_szTag_Pos, m_nPos);
	m_nPos = min (max (m_nMin, m_nPos), m_nMax);
	tm.ReadBool (s_szTag_Infinite, m_bInfinite);

	return XElement::FromTag (tm.GetBuffer ());
}

void CBCGPRibbonInfo::XElementProgressBar::ToTag (CString& strTag) const
{
	XElement::ToTag (strTag);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_Width, m_nWidth, 100));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_Height, m_nHeight, 22));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_Min, m_nMin, 0));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_Max, m_nMax, 100));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_Pos, m_nPos, m_nMin));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_Infinite, m_bInfinite, FALSE));
}

BOOL CBCGPRibbonInfo::XElementStatusPane::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	tm.ReadString (s_szTag_AlmostLargeText, m_strAlmostLargeText);
	CBCGPTagManager::Entity_FromTag (m_strAlmostLargeText);
	tm.ReadBool (s_szTag_Static, m_bIsStatic);
	tm.ReadInt (s_szTag_TextAlign, m_nTextAlign);

	return XElementButton::FromTag (tm.GetBuffer ());
}

void CBCGPRibbonInfo::XElementStatusPane::ToTag (CString& strTag) const
{
	XElementButton::ToTag (strTag);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteString (s_szTag_AlmostLargeText, CBCGPTagManager::Entity_ToTag (m_strAlmostLargeText)));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_Static, m_bIsStatic, TRUE));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_TextAlign, m_nTextAlign, TA_LEFT));
}

BOOL CBCGPRibbonInfo::XPanel::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	CString strElements;
	if (tm.ExcludeTag (s_szTag_Elements, strElements))
	{
		CBCGPTagManager tmElements (strElements);

		CString strElement;
		while (tmElements.ExcludeTag (s_szTag_Element, strElement))
		{
			XElement* pElement = (XElement*)CBCGPRibbonInfo::CreateBaseFromTag (strElement);
			if (pElement != NULL)
			{
				m_arElements.Add (pElement);
			}
		}
	}

	CString strButton;
	if (tm.ExcludeTag (s_szTag_Button_Launch, strButton))
	{
		m_btnLaunch.FromTag (strButton);
	}

	tm.ReadBool (s_szTag_JustifyColumns, m_bJustifyColumns);
	tm.ReadBool (s_szTag_CenterColumnVert, m_bCenterColumnVert);
	tm.ReadInt (s_szTag_Index, m_nImageIndex);

	tm.ReadString (s_szTag_Name, m_strName);
	CBCGPTagManager::Entity_FromTag (m_strName);
	tm.ReadString (s_szTag_Keys, m_strKeys);
	CBCGPTagManager::Entity_FromTag (m_strKeys);

	return TRUE;
}

void CBCGPRibbonInfo::XPanel::ToTag (CString& strTag) const
{
	XBase::ToTag (strTag);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteString (s_szTag_Name, CBCGPTagManager::Entity_ToTag (m_strName)));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteString (s_szTag_Keys, CBCGPTagManager::Entity_ToTag (m_strKeys)));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_Index, m_nImageIndex, -1));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_JustifyColumns, m_bJustifyColumns, FALSE));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_CenterColumnVert, m_bCenterColumnVert, FALSE));

	CString strButton;
	m_btnLaunch.ToTag (strButton);
	CBCGPTagManager::WriteItem (strTag, s_szTag_Button_Launch, strButton);

	CString strElements;

	int i = 0;
	for (i = 0; i < m_arElements.GetSize (); i++)
	{
		CString strElement;
		m_arElements[i]->ToTag (strElement);
		CBCGPTagManager::WriteItem (strElements, s_szTag_Element, strElement);
	}

	CBCGPTagManager::WriteItem (strTag, s_szTag_Elements, strElements);
}

BOOL CBCGPRibbonInfo::XCategory::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	CString strPanels;
	if (tm.ExcludeTag (s_szTag_Panels, strPanels))
	{
		CBCGPTagManager tmPanel (strPanels);

		CString strPanel;
		while (tmPanel.ExcludeTag (s_szTag_Panel, strPanel))
		{
			XPanel* pPanel = (XPanel*)CBCGPRibbonInfo::CreateBaseFromTag (strPanel);
			if (pPanel != NULL)
			{
				m_arPanels.Add (pPanel);
			}
		}
	}

	CString strElements;
	if (tm.ExcludeTag (s_szTag_Elements, strElements))
	{
		CBCGPTagManager tmElements (strElements);

		CString strElement;
		while (tmElements.ExcludeTag (s_szTag_Element, strElement))
		{
			XElement* pElement = (XElement*)CBCGPRibbonInfo::CreateBaseFromTag (strElement);
			if (pElement != NULL)
			{
				m_arElements.Add (pElement);
			}
		}
	}

	tm.ReadString (s_szTag_Name, m_strName);
	CBCGPTagManager::Entity_FromTag (m_strName);
	tm.ReadString (s_szTag_Keys, m_strKeys);
	CBCGPTagManager::Entity_FromTag (m_strKeys);

    CString strImage;
    if (tm.ExcludeTag(s_szTag_Image_Small, strImage))
    {
        m_SmallImages.FromTag(strImage);
    }
    if (tm.ExcludeTag(s_szTag_Image_Large, strImage))
    {
        m_LargeImages.FromTag(strImage);
    }

	CString strOrder;
	tm.ReadString (s_szTag_CollapseOrder, strOrder);
	if (!strOrder.IsEmpty ())
	{
		CStringArray sa;
		CBCGPTagManager::ParseString (strOrder, _T(";"), sa, TRUE, FALSE);

		for (int i = 0; i < sa.GetSize (); i++)
		{
			if (!sa[i].IsEmpty ())
			{
				m_arCollapseOrder.Add (_ttoi(sa[i]));
			}
		}
	}

	return TRUE;
}

void CBCGPRibbonInfo::XCategory::ToTag (CString& strTag) const
{
	XBase::ToTag (strTag);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteString (s_szTag_Name, CBCGPTagManager::Entity_ToTag (m_strName)));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteString (s_szTag_Keys, CBCGPTagManager::Entity_ToTag (m_strKeys)));

	CString strImage;
	m_SmallImages.ToTag (strImage);
	CBCGPTagManager::WriteItem (strTag, s_szTag_Image_Small, strImage);

	strImage.Empty ();
	m_LargeImages.ToTag (strImage);
	CBCGPTagManager::WriteItem (strTag, s_szTag_Image_Large, strImage);

	CString strPanels;

	int i = 0;
	for (i = 0; i < m_arPanels.GetSize (); i++)
	{
		CString strPanel;
		m_arPanels[i]->ToTag (strPanel);
		CBCGPTagManager::WriteItem (strPanels, s_szTag_Panel, strPanel);
	}

	CBCGPTagManager::WriteItem (strTag, s_szTag_Panels, strPanels);

	CString strElements;

	for (i = 0; i < m_arElements.GetSize (); i++)
	{
		CString strElement;
		m_arElements[i]->ToTag (strElement);
		CBCGPTagManager::WriteItem (strElements, s_szTag_Element, strElement);
	}

	CBCGPTagManager::WriteItem (strTag, s_szTag_Elements, strElements);

	int count = (int)m_arCollapseOrder.GetSize ();
	if (count > 0)
	{
		CString strOrder;
		for (i = 0; i < count; i++)
		{
			CString str;
			str.Format(_T("%d"), m_arCollapseOrder[i]);

			strOrder += str;
			if (i < (count - 1))
			{
				strOrder += _T(";");
			}
		}

		CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteString (s_szTag_CollapseOrder, strOrder));
	}
}

BOOL CBCGPRibbonInfo::XContext::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	CString strCategories;
	if (tm.ExcludeTag (s_szTag_Categories, strCategories))
	{
		CBCGPTagManager tmElements (strCategories);

		CString strCategory;
		while (tmElements.ExcludeTag (s_szTag_Category, strCategory))
		{
			XCategory* pCategory = (XCategory*)CBCGPRibbonInfo::CreateBaseFromTag (strCategory);
			if (pCategory != NULL)
			{
				m_arCategories.Add (pCategory);
			}
		}
	}

	CString strID;
	if (tm.ExcludeTag (s_szTag_ID, strID))
	{
		m_ID.FromTag (strID);
	}

	tm.ReadString (s_szTag_Text, m_strText);
	CBCGPTagManager::Entity_FromTag (m_strText);

	int color = (int)BCGPCategoryColor_None;
	tm.ReadInt (s_szTag_Color, color);
	m_Color = (BCGPRibbonCategoryColor)color;

	return TRUE;
}

void CBCGPRibbonInfo::XContext::ToTag (CString& strTag) const
{
	XBase::ToTag (strTag);

	CString strID;
	m_ID.ToTag (strID);
	CBCGPTagManager::WriteItem (strTag, s_szTag_ID, strID);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteString (s_szTag_Text, CBCGPTagManager::Entity_ToTag (m_strText)));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteInt (s_szTag_Color, (int)m_Color, (int)BCGPCategoryColor_None));

	CString strCategories;
	for (int i = 0; i < m_arCategories.GetSize (); i++)
	{
		CString strCategoryTag;
		m_arCategories[i]->ToTag (strCategoryTag);
		CBCGPTagManager::WriteItem (strCategories, s_szTag_Category, strCategoryTag);
	}
	CBCGPTagManager::WriteItem (strTag, s_szTag_Categories, strCategories);
}

BOOL CBCGPRibbonInfo::XCategoryMain::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	CString strElements;
	if (tm.ExcludeTag (s_szTag_Elements, strElements))
	{
		CBCGPTagManager tmElements (strElements);

		CString strElement;
		while (tmElements.ExcludeTag (s_szTag_Element, strElement))
		{
			XElement* pElement = (XElement*)CBCGPRibbonInfo::CreateBaseFromTag (strElement);
			if (pElement != NULL)
			{
				m_arElements.Add (pElement);
			}
		}
	}

	CString strRecent;
	if (tm.ExcludeTag (s_szTag_RecentFileList, strRecent))
	{
		CBCGPTagManager tmRecent (strRecent);

		tmRecent.CBCGPTagManager::ReadBool (s_szTag_Enable, m_bRecentListEnable);
		tmRecent.CBCGPTagManager::ReadString (s_szTag_Label, m_strRecentListLabel);
		CBCGPTagManager::Entity_FromTag (m_strRecentListLabel);
		tmRecent.CBCGPTagManager::ReadInt (s_szTag_Width, m_nRecentListWidth);
	}

	CString strSearch;
	if (tm.ExcludeTag (s_szTag_Search, strSearch))
	{
		CBCGPTagManager tmSearch (strSearch);

		tmSearch.CBCGPTagManager::ReadBool (s_szTag_Enable, m_bSearchEnable);
		tmSearch.CBCGPTagManager::ReadString (s_szTag_Label, m_strSearchLabel);
		CBCGPTagManager::Entity_FromTag (m_strSearchLabel);
		tm.ReadString (s_szTag_Keys, m_strSearchKeys);
		CBCGPTagManager::Entity_FromTag (m_strSearchKeys);
		tmSearch.CBCGPTagManager::ReadInt (s_szTag_Width, m_nSearchWidth);
	}

	tm.ReadString (s_szTag_Name, m_strName);
	CBCGPTagManager::Entity_FromTag (m_strName);

    CString strImage;
    if (tm.ExcludeTag(s_szTag_Image_Small, strImage))
    {
        m_SmallImages.FromTag(strImage);
    }
    if (tm.ExcludeTag(s_szTag_Image_Large, strImage))
    {
        m_LargeImages.FromTag(strImage);
    }

	return TRUE;
}

void CBCGPRibbonInfo::XCategoryMain::ToTag (CString& strTag) const
{
	if (m_arElements.GetSize () == 0 && !m_bRecentListEnable)
	{
		return;
	}

	XBase::ToTag (strTag);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteString (s_szTag_Name, CBCGPTagManager::Entity_ToTag (m_strName)));

	CString strImage;
	m_SmallImages.ToTag (strImage);
	CBCGPTagManager::WriteItem (strTag, s_szTag_Image_Small, strImage);

	strImage.Empty ();
	m_LargeImages.ToTag (strImage);
	CBCGPTagManager::WriteItem (strTag, s_szTag_Image_Large, strImage);

	CString strElements;

	int i = 0;
	for (i = 0; i < m_arElements.GetSize (); i++)
	{
		CString strElement;
		m_arElements[i]->ToTag (strElement);
		CBCGPTagManager::WriteItem (strElements, s_szTag_Element, strElement);
	}

	CBCGPTagManager::WriteItem (strTag, s_szTag_Elements, strElements);

	CString strRecent;
	CBCGPTagManager::WriteTag (strRecent, CBCGPTagManager::WriteBool (s_szTag_Enable, m_bRecentListEnable, FALSE));
	CBCGPTagManager::WriteTag (strRecent, CBCGPTagManager::WriteString (s_szTag_Label, CBCGPTagManager::Entity_ToTag (m_strRecentListLabel)));
	CBCGPTagManager::WriteTag (strRecent, CBCGPTagManager::WriteInt (s_szTag_Width, m_nRecentListWidth, 300));

	CBCGPTagManager::WriteItem (strTag, s_szTag_RecentFileList, strRecent);

	CString strSearch;
	CBCGPTagManager::WriteTag (strSearch, CBCGPTagManager::WriteBool (s_szTag_Enable, m_bSearchEnable, FALSE));
	CBCGPTagManager::WriteTag (strSearch, CBCGPTagManager::WriteString (s_szTag_Label, CBCGPTagManager::Entity_ToTag (m_strSearchLabel)));
	CBCGPTagManager::WriteTag (strSearch, CBCGPTagManager::WriteString (s_szTag_Keys, CBCGPTagManager::Entity_ToTag (m_strSearchKeys)));
	CBCGPTagManager::WriteTag (strSearch, CBCGPTagManager::WriteInt (s_szTag_Width, m_nSearchWidth, 0));

	CBCGPTagManager::WriteItem (strTag, s_szTag_Search, strSearch);
}

BOOL CBCGPRibbonInfo::XCategoryBackstage::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	CString strElements;
	if (tm.ExcludeTag (s_szTag_Elements, strElements))
	{
		CBCGPTagManager tmElements (strElements);

		CString strElement;
		while (tmElements.ExcludeTag (s_szTag_Element, strElement))
		{
			XElement* pElement = (XElement*)CBCGPRibbonInfo::CreateBaseFromTag (strElement);
			if (pElement != NULL)
			{
				m_arElements.Add (pElement);
			}
		}
	}

	tm.ReadString (s_szTag_Name, m_strName);
	CBCGPTagManager::Entity_FromTag (m_strName);

    CString strImage;
    if (tm.ExcludeTag(s_szTag_Image_Small, strImage))
    {
        m_SmallImages.FromTag(strImage);
    }

	return TRUE;
}

void CBCGPRibbonInfo::XCategoryBackstage::ToTag (CString& strTag) const
{
	if (m_arElements.GetSize () == 0)
	{
		return;
	}

	XBase::ToTag (strTag);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteString (s_szTag_Name, CBCGPTagManager::Entity_ToTag (m_strName)));

	CString strImage;
	m_SmallImages.ToTag (strImage);
	CBCGPTagManager::WriteItem (strTag, s_szTag_Image_Small, strImage);

	CString strElements;

	int i = 0;
	for (i = 0; i < m_arElements.GetSize (); i++)
	{
		CString strElement;
		m_arElements[i]->ToTag (strElement);
		CBCGPTagManager::WriteItem (strElements, s_szTag_Element, strElement);
	}

	CBCGPTagManager::WriteItem (strTag, s_szTag_Elements, strElements);
}

BOOL CBCGPRibbonInfo::XQAT::XQATItem::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	CString strID;
	if (tm.ExcludeTag (s_szTag_ID, strID))
	{
		if (m_ID.FromTag (strID))
		{
			tm.ReadBool (s_szTag_Visible, m_bVisible);
			return TRUE;
		}
	}

	return TRUE;
}

void CBCGPRibbonInfo::XQAT::XQATItem::ToTag (CString& strTag) const
{
	CString strID;
	m_ID.ToTag (strID);
	CBCGPTagManager::WriteItem (strTag, s_szTag_ID, strID);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_Visible, m_bVisible, TRUE));
}

BOOL CBCGPRibbonInfo::XQAT::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	CString strItem;
	while (tm.ExcludeTag (s_szTag_Item, strItem))
	{
		XQATItem item;
		if (item.FromTag (strItem))
		{
			m_arItems.Add (item);
		}
	}

	tm.ReadBool (s_szTag_QATTop, m_bOnTop);

	return TRUE;
}

void CBCGPRibbonInfo::XQAT::ToTag (CString& strTag) const
{
	XBase::ToTag (strTag);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_QATTop, m_bOnTop, TRUE));

	CString strItems;
	for (int i = 0; i < m_arItems.GetSize (); i++)
	{
		CString strItem;
		m_arItems[i].ToTag (strItem);

		CBCGPTagManager::WriteItem (strItems, s_szTag_Item, strItem);
	}

	CBCGPTagManager::WriteItem (strTag, s_szTag_Items, strItems);
}

BOOL CBCGPRibbonInfo::XRibbonBar::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	CString strButton;
	if (tm.ExcludeTag (s_szTag_Button_Main, strButton))
	{
		m_btnMain = new XElementButtonMain;
		m_btnMain->FromTag (strButton);
	}

	CString strMainCategory;
	if (tm.ExcludeTag (s_szTag_CategoryMain, strMainCategory))
	{
		m_MainCategory = new XCategoryMain;
		m_MainCategory->FromTag (strMainCategory);
	}

	CString strBackstageCategory;
	if (tm.ExcludeTag (s_szTag_CategoryBackstage, strBackstageCategory))
	{
		m_BackstageCategory = new XCategoryBackstage;
		m_BackstageCategory->FromTag (strBackstageCategory);
	}

	CString strQATElements;
	if (tm.ExcludeTag (s_szTag_QAT_Elements, strQATElements))
	{
		m_QAT.FromTag (strQATElements);
	}

	CString strTabElements;
	if (tm.ExcludeTag (s_szTag_Tab_Elements, strTabElements))
	{
		m_TabElements.FromTag (strTabElements);
	}

	CString strContexts;
	if (tm.ExcludeTag (s_szTag_Contexts, strContexts))
	{
		CBCGPTagManager tmElements (strContexts);

		CString strContext;
		while (tmElements.ExcludeTag (s_szTag_Context, strContext))
		{
			XContext* pContext = (XContext*)CBCGPRibbonInfo::CreateBaseFromTag (strContext);
			if (pContext != NULL)
			{
				m_arContexts.Add (pContext);
			}
		}
	}

	CString strCategories;
	if (tm.ExcludeTag (s_szTag_Categories, strCategories))
	{
		CBCGPTagManager tmElements (strCategories);

		CString strCategory;
		while (tmElements.ExcludeTag (s_szTag_Category, strCategory))
		{
			XCategory* pCategory = (XCategory*)CBCGPRibbonInfo::CreateBaseFromTag (strCategory);
			if (pCategory != NULL)
			{
				m_arCategories.Add (pCategory);
			}
		}
	}

	tm.ReadBool (s_szTag_EnableToolTips, m_bToolTip);
	tm.ReadBool (s_szTag_EnableToolTipsDescr, m_bToolTipDescr);
	tm.ReadBool (s_szTag_EnableKeys, m_bKeyTips);
	tm.ReadBool (s_szTag_EnablePrintPreview, m_bPrintPreview);
	tm.ReadBool (s_szTag_DrawUsingFont, m_bDrawUsingFont);
	tm.ReadBool (s_szTag_EnableBackstageMode, m_bBackstageMode);

    CString strImage;
    if (tm.ExcludeTag(s_szTag_Image, strImage))
    {
        m_Images.FromTag(strImage);
    }

	return TRUE;
}

void CBCGPRibbonInfo::XRibbonBar::ToTag (CString& strTag) const
{
	XBase::ToTag (strTag);

	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_EnableToolTips, m_bToolTip, TRUE));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_EnableToolTipsDescr, m_bToolTipDescr, TRUE));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_EnableKeys, m_bKeyTips, TRUE));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_EnablePrintPreview, m_bPrintPreview, TRUE));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_DrawUsingFont, m_bDrawUsingFont, FALSE));
	CBCGPTagManager::WriteTag (strTag, CBCGPTagManager::WriteBool (s_szTag_EnableBackstageMode, m_bBackstageMode, FALSE));

	CString strImage;
	m_Images.ToTag (strImage);
	CBCGPTagManager::WriteItem (strTag, s_szTag_Image, strImage);

	if (m_btnMain != NULL)
	{
		CString strButton;
		m_btnMain->ToTag (strButton);
		CBCGPTagManager::WriteItem (strTag, s_szTag_Button_Main, strButton);
	}

	if (m_MainCategory != NULL)
	{
		CString strMainCategory;
		m_MainCategory->ToTag (strMainCategory);
		CBCGPTagManager::WriteItem (strTag, s_szTag_CategoryMain, strMainCategory);
	}

	if (m_BackstageCategory != NULL)
	{
		CString strBackstageCategory;
		m_BackstageCategory->ToTag (strBackstageCategory);
		CBCGPTagManager::WriteItem (strTag, s_szTag_CategoryBackstage, strBackstageCategory);
	}

	int i = 0;

	CString strQATElements;
	m_QAT.ToTag (strQATElements);
	CBCGPTagManager::WriteItem (strTag, s_szTag_QAT_Elements, strQATElements);

	CString strTabElements;
	m_TabElements.ToTag (strTabElements);
	CBCGPTagManager::WriteItem (strTag, s_szTag_Tab_Elements, strTabElements);

	CString strCategories;
	for (i = 0; i < m_arCategories.GetSize (); i++)
	{
		CString strCategory;
		m_arCategories[i]->ToTag (strCategory);
		CBCGPTagManager::WriteItem (strCategories, s_szTag_Category, strCategory);
	}
	CBCGPTagManager::WriteItem (strTag, s_szTag_Categories, strCategories);

	CString strContexts;
	for (i = 0; i < m_arContexts.GetSize (); i++)
	{
		CString strContext;
		m_arContexts[i]->ToTag (strContext);
		CBCGPTagManager::WriteItem (strContexts, s_szTag_Context, strContext);
	}
	CBCGPTagManager::WriteItem (strTag, s_szTag_Contexts, strContexts);
}

BOOL CBCGPRibbonInfo::XStatusBar::XStatusElements::FromTag (const CString& strTag)
{
	CBCGPTagManager tmElements (strTag);

	CString strElement;
	while (tmElements.ExcludeTag (s_szTag_Element, strElement))
	{
		XElement* pElement = (XElement*)CBCGPRibbonInfo::CreateBaseFromTag (strElement);
		if (pElement != NULL)
		{
			m_arElements.Add (pElement);

			CBCGPTagManager tmElement (strElement);

			CString strLabel;
			tmElement.CBCGPTagManager::ReadString (s_szTag_Label, strLabel);
			CBCGPTagManager::Entity_FromTag (strLabel);
			m_arLabels.Add (strLabel);

			BOOL bVisible = TRUE;
			tmElement.CBCGPTagManager::ReadBool (s_szTag_Visible, bVisible);
			m_arVisible.Add (bVisible);
		}
	}

	return TRUE;
}

void CBCGPRibbonInfo::XStatusBar::XStatusElements::ToTag (CString& strTag) const
{
	int i = 0;
	for (i = 0; i < m_arElements.GetSize (); i++)
	{
		CString strElement;
		m_arElements[i]->ToTag (strElement);

		CBCGPTagManager::WriteTag (strElement, CBCGPTagManager::WriteString (s_szTag_Label, CBCGPTagManager::Entity_ToTag (m_arLabels[i])));
		CBCGPTagManager::WriteTag (strElement, CBCGPTagManager::WriteBool (s_szTag_Visible, m_arVisible[i], TRUE));

		CBCGPTagManager::WriteItem (strTag, s_szTag_Element, strElement);
	}
}

BOOL CBCGPRibbonInfo::XStatusBar::FromTag (const CString& strTag)
{
	CBCGPTagManager tm (strTag);

	CString strExElements;
	if (tm.ExcludeTag (s_szTag_ElementsExtended, strExElements))
	{
		m_ExElements.FromTag (strExElements);
	}

	CString strElements;
	if (tm.ExcludeTag (s_szTag_Elements, strElements))
	{
		m_Elements.FromTag (strElements);
	}

    CString strImage;
    if (tm.ExcludeTag(s_szTag_Image, strImage))
    {
        m_Images.FromTag(strImage);
    }

	return TRUE;
}

void CBCGPRibbonInfo::XStatusBar::ToTag (CString& strTag) const
{
	XBase::ToTag (strTag);

	CString strImage;
	m_Images.ToTag (strImage);
	CBCGPTagManager::WriteItem (strTag, s_szTag_Image, strImage);

	CString strElements;
	m_Elements.ToTag (strElements);
	CBCGPTagManager::WriteItem (strTag, s_szTag_Elements, strElements);

	CString strExElements;
	m_ExElements.ToTag (strExElements);
	CBCGPTagManager::WriteItem (strTag, s_szTag_ElementsExtended, strExElements);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBCGPRibbonInfo::CBCGPRibbonInfo()
	: CBCGPBaseInfo(c_dwVersion, e_UseRibbon | e_UseStatus)
{
	m_sizeImage[e_ImagesSmall]   = CSize(16, 16);
	m_sizeImage[e_ImagesLarge]   = CSize(32, 32);
	m_sizeImage[e_ImagesSBGroup] = CSize(14, 14);
	
	m_RibbonBar.m_Images.m_Image.SetImageSize (m_sizeImage [e_ImagesSmall]);
	m_StatusBar.m_Images.m_Image.SetImageSize (m_sizeImage [e_ImagesSmall]);
}

CBCGPRibbonInfo::~CBCGPRibbonInfo()
{
}

BOOL CBCGPRibbonInfo::FromTag (const CString& strTag)
{
	DWORD dwFlags = GetFlags ();

	if ((dwFlags & (e_UseRibbon | e_UseStatus)) == 0)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	CString strXML;
	{
		CBCGPTagManager tmXML (strTag);

		if (!tmXML.ExcludeTag (s_szTag_Body, strXML))
		{
			if (!tmXML.ExcludeTag (_T("AFX_RIBBON"), strXML))
			{
				return FALSE;
			}
		}
	}

	CBCGPTagManager tm (strXML);

	CString strHeader;
	if (tm.ExcludeTag (CBCGPBaseInfo::s_szTag_Header, strHeader))
	{
		CBCGPTagManager tmHeader (strHeader);

		int nValue = (int)GetVersion ();
		tmHeader.CBCGPTagManager::ReadInt (CBCGPBaseInfo::s_szTag_Version, nValue);
		SetVersion ((DWORD)nValue);

		CString strSizes;
		if (tmHeader.ExcludeTag (s_szTag_Sizes, strSizes))
		{
			CBCGPTagManager tmSizes (strSizes);

			tmSizes.CBCGPTagManager::ReadSize (s_szTag_Image_Small, m_sizeImage[e_ImagesSmall]);
			tmSizes.CBCGPTagManager::ReadSize (s_szTag_Image_Large, m_sizeImage[e_ImagesLarge]);
			tmSizes.CBCGPTagManager::ReadSize (s_szTag_Image_SBGroup, m_sizeImage[e_ImagesSBGroup]);
		}
	}
	else
	{
		return FALSE;
	}

	BOOL bRibbon = FALSE;
	if ((dwFlags & e_UseRibbon) == e_UseRibbon)
	{
		CString strRibbonBar;
		if (tm.ExcludeTag (s_szTag_RibbonBar, strRibbonBar))
		{
			bRibbon = m_RibbonBar.FromTag (strRibbonBar);
		}
	}

	BOOL bStatus = FALSE;
	if ((dwFlags & e_UseStatus) == e_UseStatus)
	{
		CString strStatusBar;
		if (tm.ExcludeTag (s_szTag_StatusBar, strStatusBar))
		{
			bStatus = m_StatusBar.FromTag (strStatusBar);
		}
	}

	if (bRibbon)
	{
		m_RibbonBar.m_Images.m_Image.SetImageSize (m_sizeImage [e_ImagesSmall]);

		if (m_RibbonBar.m_MainCategory != NULL)
		{
			m_RibbonBar.m_MainCategory->m_SmallImages.m_Image.SetImageSize (m_sizeImage [e_ImagesSmall]);
			m_RibbonBar.m_MainCategory->m_LargeImages.m_Image.SetImageSize (m_sizeImage [e_ImagesLarge]);
		}

		if (m_RibbonBar.m_BackstageCategory != NULL)
		{
			m_RibbonBar.m_BackstageCategory->m_SmallImages.m_Image.SetImageSize (m_sizeImage [e_ImagesSmall]);
		}

		XArrayCategory arCategories;
		arCategories.Append (m_RibbonBar.m_arCategories);

		int i = 0;
		for (i = 0; i < m_RibbonBar.m_arContexts.GetSize (); i++)
		{
			arCategories.Append (m_RibbonBar.m_arContexts [i]->m_arCategories);
		}

		for (i = 0; i < arCategories.GetSize (); i++)
		{
			XCategory* pCategory = arCategories[i];

			pCategory->m_SmallImages.m_Image.SetImageSize (m_sizeImage [e_ImagesSmall]);
			pCategory->m_LargeImages.m_Image.SetImageSize (m_sizeImage [e_ImagesLarge]);

			for (int j = 0; j < pCategory->m_arPanels.GetSize (); j++)
			{
				XPanel* pPanel = pCategory->m_arPanels[j];

				for (int k = 0; k < pPanel->m_arElements.GetSize (); k++)
				{
					XElement* pElement = pPanel->m_arElements[k];

					if (pElement->GetElementName ().Compare (s_szGroup) == 0)
					{
						((XElementGroup*)pElement)->m_Images.m_Image.SetImageSize (m_sizeImage [e_ImagesSmall]);
					}
				}
			}
		}
	}

	if (bStatus)
	{
		m_StatusBar.m_Images.m_Image.SetImageSize (m_sizeImage [e_ImagesSmall]);

		int i = 0;
		for (i = 0; i < m_StatusBar.m_Elements.m_arElements.GetSize (); i++)
		{
			if (m_StatusBar.m_Elements.m_arElements[i]->GetElementName ().Compare (s_szGroup) == 0)
			{
				((XElementGroup*)m_StatusBar.m_Elements.m_arElements[i])->m_Images.m_Image.SetImageSize (m_sizeImage [e_ImagesSBGroup]);
			}
		}
		for (i = 0; i < m_StatusBar.m_ExElements.m_arElements.GetSize (); i++)
		{
			if (m_StatusBar.m_ExElements.m_arElements[i]->GetElementName ().Compare (s_szGroup) == 0)
			{
				((XElementGroup*)m_StatusBar.m_ExElements.m_arElements[i])->m_Images.m_Image.SetImageSize (m_sizeImage [e_ImagesSBGroup]);
			}
		}
	}

	return TRUE;
}

void CBCGPRibbonInfo::ToTag (CString& strTag) const
{
	DWORD dwFlags = GetFlags ();

	if ((dwFlags & (e_UseRibbon | e_UseStatus)) == 0)
	{
		ASSERT (FALSE);
		return;
	}

	CString strData;

	CString strSizes;
	CBCGPTagManager::WriteTag (strSizes, CBCGPTagManager::WriteSize (s_szTag_Image_Small, m_sizeImage[e_ImagesSmall], CSize (16, 16)));
	CBCGPTagManager::WriteTag (strSizes, CBCGPTagManager::WriteSize (s_szTag_Image_Large, m_sizeImage[e_ImagesLarge], CSize (32, 32)));
	CBCGPTagManager::WriteTag (strSizes, CBCGPTagManager::WriteSize (s_szTag_Image_SBGroup, m_sizeImage[e_ImagesSBGroup], CSize (14, 14)));

	CString strHeader;
	CBCGPTagManager::WriteTag (strHeader, CBCGPTagManager::WriteUInt (CBCGPBaseInfo::s_szTag_Version, GetVersion (), 0));
	CBCGPTagManager::WriteItem (strHeader, s_szTag_Sizes, strSizes);

	CBCGPTagManager::WriteItem (strData, CBCGPBaseInfo::s_szTag_Header, strHeader);

	if ((dwFlags & e_UseRibbon) == e_UseRibbon)
	{
		CString strRibbonBar;
		m_RibbonBar.ToTag (strRibbonBar);
		CBCGPTagManager::WriteItem (strData, s_szTag_RibbonBar, strRibbonBar);
	}

	if ((dwFlags & e_UseStatus) == e_UseStatus)
	{
		CString strStatusBar;
		m_StatusBar.ToTag (strStatusBar);
		CBCGPTagManager::WriteItem (strData, s_szTag_StatusBar, strStatusBar);
	}

	CBCGPTagManager::WriteItem (strTag, s_szTag_Body, strData);
}

void CBCGPRibbonInfo::AddElementImages (XElement& info, XArrayImages& images)
{
	CString strName (info.GetElementName ());
	BOOL bPalette = FALSE;

	if (strName.Compare (s_szButton_Palette) == 0)
	{
		images.Add(&((XElementButtonPalette&)info).m_Images);
		bPalette = TRUE;
	}
	else if (info.GetElementName ().Compare (s_szGroup) == 0)
	{
		XElementGroup& infoGroup = (XElementGroup&)info;
		images.Add(&(infoGroup.m_Images));

		for (int i = 0; i < infoGroup.m_arButtons.GetSize (); i++)
		{
			AddElementImages (*(infoGroup.m_arButtons[i]), images);
		}
	}

	if (strName.Compare (s_szButton) == 0 || strName.Compare (s_szButton_Color) == 0 || bPalette)
	{
		XElementButton& infoBtn = (XElementButton&)info;

		for (int i = 0; i < infoBtn.m_arSubItems.GetSize(); i++)
		{
			AddElementImages (*((XElement*)(infoBtn.m_arSubItems[i])), images);
		}
	}
}

void CBCGPRibbonInfo::GetArrayImages (XArrayImages& images)
{
	images.RemoveAll ();

	images.Add (&m_RibbonBar.m_Images);

	if (m_RibbonBar.m_btnMain != NULL)
	{
		images.Add (&m_RibbonBar.m_btnMain->m_Image);
		images.Add (&m_RibbonBar.m_btnMain->m_ImageScenic);
	}

	if (m_RibbonBar.m_MainCategory != NULL)
	{
		images.Add (&m_RibbonBar.m_MainCategory->m_SmallImages);
		images.Add (&m_RibbonBar.m_MainCategory->m_LargeImages);
	}

	if (m_RibbonBar.m_BackstageCategory != NULL)
	{
		images.Add (&m_RibbonBar.m_BackstageCategory->m_SmallImages);
	}

	int i = 0;
	int j = 0;
	int k = 0;

	for (i = 0; i < m_RibbonBar.m_TabElements.m_arButtons.GetSize (); i++)
	{
		AddElementImages (*(m_RibbonBar.m_TabElements.m_arButtons[i]), images);
	}

	XArrayCategory arCategories;
	arCategories.Append(m_RibbonBar.m_arCategories);

	for (i = 0; i < m_RibbonBar.m_arContexts.GetSize(); i++)
	{
		arCategories.Append(m_RibbonBar.m_arContexts[i]->m_arCategories);
	}
	
	for (i = 0; i < arCategories.GetSize(); i++)
	{
		XCategory* pCategory = arCategories[i];

		images.Add(&pCategory->m_SmallImages);
		images.Add(&pCategory->m_LargeImages);

		for (j = 0; j < pCategory->m_arPanels.GetSize(); j++)
		{
			XPanel* pPanel = pCategory->m_arPanels[j];

			for (k = 0; k < pPanel->m_arElements.GetSize(); k++)
			{
				AddElementImages (*(pPanel->m_arElements[k]), images);
			}
		}
	}

	images.Add (&m_StatusBar.m_Images);

	for (i = 0; i < m_StatusBar.m_Elements.m_arElements.GetSize (); i++)
	{
		if (m_StatusBar.m_Elements.m_arElements[i]->GetElementName ().Compare (s_szGroup) == 0)
		{
			images.Add (&((XElementGroup*)m_StatusBar.m_Elements.m_arElements[i])->m_Images);
		}
	}
	for (i = 0; i < m_StatusBar.m_ExElements.m_arElements.GetSize (); i++)
	{
		if (m_StatusBar.m_ExElements.m_arElements[i]->GetElementName ().Compare (s_szGroup) == 0)
		{
			images.Add (&((XElementGroup*)m_StatusBar.m_ExElements.m_arElements[i])->m_Images);
		}
	}
}

#endif // BCGP_EXCLUDE_RIBBON
