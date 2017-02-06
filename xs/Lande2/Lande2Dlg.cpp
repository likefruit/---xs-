// Lande2Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "Lande2.h"
#include "Lande2Dlg.h"
#include <afxtempl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <ole2.h>
#import "c:\\Program Files\\Common Files\\Microsoft Shared\\OFFICE12\MSO.DLL" 
#import "c:\\Program Files\\Common Files\\Microsoft Shared\\vba\\VBA6\\VBE6EXT.OLB" 
//#import "c:\Program Files\Microsoft Office\Office10\MSWORD.olb" rename("ExitWindows","ExitWindowsEx")
#import "c:\\Program Files\\Microsoft Office\\Office12\\EXCEL.EXE" rename("DialogBox", "DialogBoxXL") rename("RGB", "RBGXL") \
rename("DocumentProperties", "DocumentPropertiesXL") exclude("IFont","IPicture")

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLande2Dlg dialog

CLande2Dlg::CLande2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLande2Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLande2Dlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLande2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLande2Dlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLande2Dlg, CDialog)
	//{{AFX_MSG_MAP(CLande2Dlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLande2Dlg message handlers

BOOL CLande2Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLande2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLande2Dlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLande2Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/*

{
    //
    // Property data
    //

    __declspec(property(get=Get_Default,put=Put_Default))
    _variant_t _Default[][];
    __declspec(property(get=GetRange))
    RangePtr _Range[][];
    __declspec(property(get=GetMergeArea))
    RangePtr MergeArea;
    __declspec(property(get=GetQueryTable))
    _QueryTablePtr QueryTable;
    __declspec(property(get=GetIndentLevel,put=PutIndentLevel))
    _variant_t IndentLevel;
    __declspec(property(get=GetValidation))
    ValidationPtr Validation;
    __declspec(property(get=GetValue,put=PutValue))
    _variant_t Value[];
    __declspec(property(get=GetValue2,put=PutValue2))
    _variant_t Value2;
    __declspec(property(get=GetPhonetic))
    PhoneticPtr Phonetic;
    __declspec(property(get=GetCharacters))
    CharactersPtr Characters[][];
    __declspec(property(get=GetMergeCells,put=PutMergeCells))
    _variant_t MergeCells;
    __declspec(property(get=GetShrinkToFit,put=PutShrinkToFit))
    _variant_t ShrinkToFit;
    __declspec(property(get=GetFormatConditions))
    FormatConditionsPtr FormatConditions;
    __declspec(property(get=GetHyperlinks))
    HyperlinksPtr Hyperlinks;
    __declspec(property(get=GetPhonetics))
    PhoneticsPtr Phonetics;
    __declspec(property(get=GetID,put=PutID))
    _bstr_t ID;
    __declspec(property(get=GetPivotCell))
    PivotCellPtr PivotCell;
    __declspec(property(get=GetErrors))
    ErrorsPtr Errors;
    __declspec(property(get=GetSmartTags))
    SmartTagsPtr SmartTags;
    __declspec(property(get=GetAddress))
    _bstr_t Address[][][][][];
    __declspec(property(get=GetBorders))
    BordersPtr Borders;
    __declspec(property(get=GetCells))
    RangePtr Cells;
    __declspec(property(get=GetAllowEdit))
    VARIANT_BOOL AllowEdit;
    __declspec(property(get=GetColumn))
    long Column;
    __declspec(property(get=GetColumns))
    RangePtr Columns;
    __declspec(property(get=GetColumnWidth,put=PutColumnWidth))
    _variant_t ColumnWidth;
    __declspec(property(get=GetCurrentRegion))
    RangePtr CurrentRegion;
    __declspec(property(get=GetAddressLocal))
    _bstr_t AddressLocal[][][][][];
    __declspec(property(get=GetEntireColumn))
    RangePtr EntireColumn;
    __declspec(property(get=GetEntireRow))
    RangePtr EntireRow;
    __declspec(property(get=GetOffset))
    RangePtr Offset[][];
    __declspec(property(get=GetPageBreak,put=PutPageBreak))
    long PageBreak;
    __declspec(property(get=GetResize))
    RangePtr Resize[][];
    __declspec(property(get=GetRow))
    long Row;
    __declspec(property(get=GetRows))
    RangePtr Rows;
    __declspec(property(get=GetStyle,put=PutStyle))
    _variant_t Style;
    __declspec(property(get=GetFormula,put=PutFormula))
    _variant_t Formula;
    __declspec(property(get=GetFormulaHidden,put=PutFormulaHidden))
    _variant_t FormulaHidden;
    __declspec(property(get=GetFormulaLocal,put=PutFormulaLocal))
    _variant_t FormulaLocal;
    __declspec(property(get=GetFormulaR1C1,put=PutFormulaR1C1))
    _variant_t FormulaR1C1;
    __declspec(property(get=GetFormulaR1C1Local,put=PutFormulaR1C1Local))
    _variant_t FormulaR1C1Local;
    __declspec(property(get=GetHasArray))
    _variant_t HasArray;
    __declspec(property(get=GetHasFormula))
    _variant_t HasFormula;
    __declspec(property(get=GetHidden,put=PutHidden))
    _variant_t Hidden;
    __declspec(property(get=GetLocked,put=PutLocked))
    _variant_t Locked;
    __declspec(property(get=GetDisplayFormat))
    DisplayFormatPtr DisplayFormat;
    __declspec(property(get=GetAddIndent,put=PutAddIndent))
    _variant_t AddIndent;
    __declspec(property(get=GetOutlineLevel,put=PutOutlineLevel))
    _variant_t OutlineLevel;
    __declspec(property(get=GetRowHeight,put=PutRowHeight))
    _variant_t RowHeight;
    __declspec(property(get=GetSummary))
    _variant_t Summary;
    __declspec(property(get=GetUseStandardHeight,put=PutUseStandardHeight))
    _variant_t UseStandardHeight;
    __declspec(property(get=GetUseStandardWidth,put=PutUseStandardWidth))
    _variant_t UseStandardWidth;
    __declspec(property(get=GetListObject))
    ListObjectPtr ListObject;
    __declspec(property(get=GetXPath))
    XPathPtr XPath;
    __declspec(property(get=GetSparklineGroups))
    SparklineGroupsPtr SparklineGroups;
    __declspec(property(get=GetWrapText,put=PutWrapText))
    _variant_t WrapText;
    __declspec(property(get=GetLocationInTable))
    enum XlLocationInTable LocationInTable;
    __declspec(property(get=GetEnd))
    RangePtr End[];
    __declspec(property(get=GetCurrentArray))
    RangePtr CurrentArray;
    __declspec(property(get=GetNext))
    RangePtr Next;
    __declspec(property(get=GetPrevious))
    RangePtr Previous;
    __declspec(property(get=GetPrefixCharacter))
    _variant_t PrefixCharacter;
    __declspec(property(get=GetNumberFormatLocal,put=PutNumberFormatLocal))
    _variant_t NumberFormatLocal;
    __declspec(property(get=GetName,put=PutName))
    _variant_t Name;
    __declspec(property(get=GetServerActions))
    ActionsPtr ServerActions;
    __declspec(property(get=GetComment))
    CommentPtr Comment;
    __declspec(property(get=GetCount))
    long Count;
    __declspec(property(get=GetPivotTable))
    PivotTablePtr PivotTable;
    __declspec(property(get=GetCountLarge))
    _variant_t CountLarge;
    __declspec(property(get=GetSoundNote))
    SoundNotePtr SoundNote;
    __declspec(property(get=GetHeight))
    _variant_t Height;
    __declspec(property(get=GetTop))
    _variant_t Top;
    __declspec(property(get=GetLeft))
    _variant_t Left;
    __declspec(property(get=GetWidth))
    _variant_t Width;
    __declspec(property(get=GetInterior))
    InteriorPtr Interior;
    __declspec(property(get=GetOrientation,put=PutOrientation))
    _variant_t Orientation;
    __declspec(property(get=GetHorizontalAlignment,put=PutHorizontalAlignment))
    _variant_t HorizontalAlignment;
    __declspec(property(get=GetVerticalAlignment,put=PutVerticalAlignment))
    _variant_t VerticalAlignment;
    __declspec(property(get=GetText))
    _variant_t Text;
    __declspec(property(get=GetPivotField))
    PivotFieldPtr PivotField;
    __declspec(property(get=GetMDX))
    _bstr_t MDX;
    __declspec(property(get=GetPivotItem))
    PivotItemPtr PivotItem;
    __declspec(property(get=GetDependents))
    RangePtr Dependents;
    __declspec(property(get=GetApplication))
    _ApplicationPtr Application;
    __declspec(property(get=GetCreator))
    enum XlCreator Creator;
    __declspec(property(get=GetParent))
    IDispatchPtr Parent;
    __declspec(property(get=GetWorksheet))
    _WorksheetPtr Worksheet;
    __declspec(property(get=GetPrecedents))
    RangePtr Precedents;
    __declspec(property(get=GetDirectDependents))
    RangePtr DirectDependents;
    __declspec(property(get=GetDirectPrecedents))
    RangePtr DirectPrecedents;
    __declspec(property(get=GetFont))
    FontPtr Font;
    __declspec(property(get=GetItem,put=PutItem))
    _variant_t Item[][];
    __declspec(property(get=GetAreas))
    AreasPtr Areas;
    __declspec(property(get=GetReadingOrder,put=PutReadingOrder))
    long ReadingOrder;
    __declspec(property(get=GetShowDetail,put=PutShowDetail))
    _variant_t ShowDetail;
    __declspec(property(get=GetFormulaArray,put=PutFormulaArray))
    _variant_t FormulaArray;
    __declspec(property(get=GetFormulaLabel,put=PutFormulaLabel))
    enum XlFormulaLabel FormulaLabel;
    __declspec(property(get=GetNumberFormat,put=PutNumberFormat))
    _variant_t NumberFormat;
    __declspec(property(get=Get_NewEnum))
    IUnknownPtr _NewEnum;
    __declspec(property(get=GetListHeaderRows))
    long ListHeaderRows;

    //
    // Wrapper methods for error-handling
    //

    // Methods:
    _ApplicationPtr GetApplication ( );
    enum XlCreator GetCreator ( );
    IDispatchPtr GetParent ( );
    _variant_t Activate ( );
    _variant_t GetAddIndent ( );
    void PutAddIndent (
        const _variant_t & _arg1 );
    _bstr_t GetAddress (
        const _variant_t & RowAbsolute,
        const _variant_t & ColumnAbsolute,
        enum XlReferenceStyle ReferenceStyle,
        const _variant_t & External = vtMissing,
        const _variant_t & RelativeTo = vtMissing );
    _bstr_t GetAddressLocal (
        const _variant_t & RowAbsolute,
        const _variant_t & ColumnAbsolute,
        enum XlReferenceStyle ReferenceStyle,
        const _variant_t & External = vtMissing,
        const _variant_t & RelativeTo = vtMissing );
    _variant_t AdvancedFilter (
        enum XlFilterAction Action,
        const _variant_t & CriteriaRange = vtMissing,
        const _variant_t & CopyToRange = vtMissing,
        const _variant_t & Unique = vtMissing );
    _variant_t ApplyNames (
        const _variant_t & Names,
        const _variant_t & IgnoreRelativeAbsolute,
        const _variant_t & UseRowColumnNames,
        const _variant_t & OmitColumn,
        const _variant_t & OmitRow,
        enum XlApplyNamesOrder Order,
        const _variant_t & AppendLast = vtMissing );
    _variant_t ApplyOutlineStyles ( );
    AreasPtr GetAreas ( );
    _bstr_t AutoComplete (
        _bstr_t String );
    _variant_t AutoFill (
        struct Range * Destination,
        enum XlAutoFillType Type );
    _variant_t AutoFilter (
        const _variant_t & Field,
        const _variant_t & Criteria1,
        enum XlAutoFilterOperator Operator,
        const _variant_t & Criteria2 = vtMissing,
        const _variant_t & VisibleDropDown = vtMissing );
    _variant_t AutoFit ( );
    _variant_t AutoFormat (
        enum XlRangeAutoFormat Format,
        const _variant_t & Number = vtMissing,
        const _variant_t & Font = vtMissing,
        const _variant_t & Alignment = vtMissing,
        const _variant_t & Border = vtMissing,
        const _variant_t & Pattern = vtMissing,
        const _variant_t & Width = vtMissing );
    _variant_t AutoOutline ( );
    _variant_t _BorderAround (
        const _variant_t & LineStyle,
        enum XlBorderWeight Weight,
        enum XlColorIndex ColorIndex,
        const _variant_t & Color = vtMissing );
    BordersPtr GetBorders ( );
    _variant_t Calculate ( );
    RangePtr GetCells ( );
    CharactersPtr GetCharacters (
        const _variant_t & Start = vtMissing,
        const _variant_t & Length = vtMissing );
    _variant_t CheckSpelling (
        const _variant_t & CustomDictionary = vtMissing,
        const _variant_t & IgnoreUppercase = vtMissing,
        const _variant_t & AlwaysSuggest = vtMissing,
        const _variant_t & SpellLang = vtMissing );
    _variant_t Clear ( );
    _variant_t ClearContents ( );
    _variant_t ClearFormats ( );
    _variant_t ClearNotes ( );
    _variant_t ClearOutline ( );
    long GetColumn ( );
    RangePtr ColumnDifferences (
        const _variant_t & Comparison );
    RangePtr GetColumns ( );
    _variant_t GetColumnWidth ( );
    void PutColumnWidth (
        const _variant_t & _arg1 );
    _variant_t Consolidate (
        const _variant_t & Sources = vtMissing,
        const _variant_t & Function = vtMissing,
        const _variant_t & TopRow = vtMissing,
        const _variant_t & LeftColumn = vtMissing,
        const _variant_t & CreateLinks = vtMissing );
    _variant_t Copy (
        const _variant_t & Destination = vtMissing );
    long CopyFromRecordset (
        IUnknown * Data,
        const _variant_t & MaxRows = vtMissing,
        const _variant_t & MaxColumns = vtMissing );
    _variant_t CopyPicture (
        enum XlPictureAppearance Appearance,
        enum XlCopyPictureFormat Format );
    long GetCount ( );
    _variant_t CreateNames (
        const _variant_t & Top = vtMissing,
        const _variant_t & Left = vtMissing,
        const _variant_t & Bottom = vtMissing,
        const _variant_t & Right = vtMissing );
    _variant_t CreatePublisher (
        const _variant_t & Edition,
        enum XlPictureAppearance Appearance,
        const _variant_t & ContainsPICT = vtMissing,
        const _variant_t & ContainsBIFF = vtMissing,
        const _variant_t & ContainsRTF = vtMissing,
        const _variant_t & ContainsVALU = vtMissing );
    RangePtr GetCurrentArray ( );
    RangePtr GetCurrentRegion ( );
    _variant_t Cut (
        const _variant_t & Destination = vtMissing );
    _variant_t DataSeries (
        const _variant_t & Rowcol,
        enum XlDataSeriesType Type,
        enum XlDataSeriesDate Date,
        const _variant_t & Step = vtMissing,
        const _variant_t & Stop = vtMissing,
        const _variant_t & Trend = vtMissing );
    _variant_t Get_Default (
        const _variant_t & RowIndex = vtMissing,
        const _variant_t & ColumnIndex = vtMissing );
    void Put_Default (
        const _variant_t & RowIndex,
        const _variant_t & ColumnIndex = vtMissing,
        const _variant_t & _arg3 = vtMissing );
    _variant_t Delete (
        const _variant_t & Shift = vtMissing );
    RangePtr GetDependents ( );
    _variant_t DialogBoxXL ( );
    RangePtr GetDirectDependents ( );
    RangePtr GetDirectPrecedents ( );
    _variant_t EditionOptions (
        enum XlEditionType Type,
        enum XlEditionOptionsOption Option,
        const _variant_t & Name,
        const _variant_t & Reference,
        enum XlPictureAppearance Appearance,
        enum XlPictureAppearance ChartSize,
        const _variant_t & Format = vtMissing );
    RangePtr GetEnd (
        enum XlDirection Direction );
    RangePtr GetEntireColumn ( );
    RangePtr GetEntireRow ( );
    _variant_t FillDown ( );
    _variant_t FillLeft ( );
    _variant_t FillRight ( );
    _variant_t FillUp ( );
    RangePtr Find (
        const _variant_t & What,
        const _variant_t & After,
        const _variant_t & LookIn,
        const _variant_t & LookAt,
        const _variant_t & SearchOrder,
        enum XlSearchDirection SearchDirection,
        const _variant_t & MatchCase = vtMissing,
        const _variant_t & MatchByte = vtMissing,
        const _variant_t & SearchFormat = vtMissing );
    RangePtr FindNext (
        const _variant_t & After = vtMissing );
    RangePtr FindPrevious (
        const _variant_t & After = vtMissing );
    FontPtr GetFont ( );
    _variant_t GetFormula ( );
    void PutFormula (
        const _variant_t & _arg1 );
    _variant_t GetFormulaArray ( );
    void PutFormulaArray (
        const _variant_t & _arg1 );
    enum XlFormulaLabel GetFormulaLabel ( );
    void PutFormulaLabel (
        enum XlFormulaLabel _arg1 );
    _variant_t GetFormulaHidden ( );
    void PutFormulaHidden (
        const _variant_t & _arg1 );
    _variant_t GetFormulaLocal ( );
    void PutFormulaLocal (
        const _variant_t & _arg1 );
    _variant_t GetFormulaR1C1 ( );
    void PutFormulaR1C1 (
        const _variant_t & _arg1 );
    _variant_t GetFormulaR1C1Local ( );
    void PutFormulaR1C1Local (
        const _variant_t & _arg1 );
    _variant_t FunctionWizard ( );
    VARIANT_BOOL GoalSeek (
        const _variant_t & Goal,
        struct Range * ChangingCell );
    _variant_t Group (
        const _variant_t & Start = vtMissing,
        const _variant_t & End = vtMissing,
        const _variant_t & By = vtMissing,
        const _variant_t & Periods = vtMissing );
    _variant_t GetHasArray ( );
    _variant_t GetHasFormula ( );
    _variant_t GetHeight ( );
    _variant_t GetHidden ( );
    void PutHidden (
        const _variant_t & _arg1 );
    _variant_t GetHorizontalAlignment ( );
    void PutHorizontalAlignment (
        const _variant_t & _arg1 );
    _variant_t GetIndentLevel ( );
    void PutIndentLevel (
        const _variant_t & _arg1 );
    HRESULT InsertIndent (
        long InsertAmount );
    _variant_t Insert (
        const _variant_t & Shift = vtMissing,
        const _variant_t & CopyOrigin = vtMissing );
    InteriorPtr GetInterior ( );
    _variant_t GetItem (
        const _variant_t & RowIndex,
        const _variant_t & ColumnIndex = vtMissing );
    void PutItem (
        const _variant_t & RowIndex,
        const _variant_t & ColumnIndex,
        const _variant_t & _arg3 = vtMissing );
    _variant_t Justify ( );
    _variant_t GetLeft ( );
    long GetListHeaderRows ( );
    _variant_t ListNames ( );
    enum XlLocationInTable GetLocationInTable ( );
    _variant_t GetLocked ( );
    void PutLocked (
        const _variant_t & _arg1 );
    HRESULT Merge (
        const _variant_t & Across = vtMissing );
    HRESULT UnMerge ( );
    RangePtr GetMergeArea ( );
    _variant_t GetMergeCells ( );
    void PutMergeCells (
        const _variant_t & _arg1 );
    _variant_t GetName ( );
    void PutName (
        const _variant_t & _arg1 );
    _variant_t NavigateArrow (
        const _variant_t & TowardPrecedent = vtMissing,
        const _variant_t & ArrowNumber = vtMissing,
        const _variant_t & LinkNumber = vtMissing );
    IUnknownPtr Get_NewEnum ( );
    RangePtr GetNext ( );
    _bstr_t NoteText (
        const _variant_t & Text = vtMissing,
        const _variant_t & Start = vtMissing,
        const _variant_t & Length = vtMissing );
    _variant_t GetNumberFormat ( );
    void PutNumberFormat (
        const _variant_t & _arg1 );
    _variant_t GetNumberFormatLocal ( );
    void PutNumberFormatLocal (
        const _variant_t & _arg1 );
    RangePtr GetOffset (
        const _variant_t & RowOffset = vtMissing,
        const _variant_t & ColumnOffset = vtMissing );
    _variant_t GetOrientation ( );
    void PutOrientation (
        const _variant_t & _arg1 );
    _variant_t GetOutlineLevel ( );
    void PutOutlineLevel (
        const _variant_t & _arg1 );
    long GetPageBreak ( );
    void PutPageBreak (
        long _arg1 );
    _variant_t Parse (
        const _variant_t & ParseLine = vtMissing,
        const _variant_t & Destination = vtMissing );
    _variant_t _PasteSpecial (
        enum XlPasteType Paste,
        enum XlPasteSpecialOperation Operation,
        const _variant_t & SkipBlanks = vtMissing,
        const _variant_t & Transpose = vtMissing );
    PivotFieldPtr GetPivotField ( );
    PivotItemPtr GetPivotItem ( );
    PivotTablePtr GetPivotTable ( );
    RangePtr GetPrecedents ( );
    _variant_t GetPrefixCharacter ( );
    RangePtr GetPrevious ( );
    _variant_t __PrintOut (
        const _variant_t & From = vtMissing,
        const _variant_t & To = vtMissing,
        const _variant_t & Copies = vtMissing,
        const _variant_t & Preview = vtMissing,
        const _variant_t & ActivePrinter = vtMissing,
        const _variant_t & PrintToFile = vtMissing,
        const _variant_t & Collate = vtMissing );
    _variant_t PrintPreview (
        const _variant_t & EnableChanges = vtMissing );
    _QueryTablePtr GetQueryTable ( );
    RangePtr GetRange (
        const _variant_t & Cell1,
        const _variant_t & Cell2 = vtMissing );
    _variant_t RemoveSubtotal ( );
    VARIANT_BOOL Replace (
        const _variant_t & What,
        const _variant_t & Replacement,
        const _variant_t & LookAt = vtMissing,
        const _variant_t & SearchOrder = vtMissing,
        const _variant_t & MatchCase = vtMissing,
        const _variant_t & MatchByte = vtMissing,
        const _variant_t & SearchFormat = vtMissing,
        const _variant_t & ReplaceFormat = vtMissing );
    RangePtr GetResize (
        const _variant_t & RowSize = vtMissing,
        const _variant_t & ColumnSize = vtMissing );
    long GetRow ( );
    RangePtr RowDifferences (
        const _variant_t & Comparison );
    _variant_t GetRowHeight ( );
    void PutRowHeight (
        const _variant_t & _arg1 );
    RangePtr GetRows ( );
    _variant_t Run (
        const _variant_t & Arg1 = vtMissing,
        const _variant_t & Arg2 = vtMissing,
        const _variant_t & Arg3 = vtMissing,
        const _variant_t & Arg4 = vtMissing,
        const _variant_t & Arg5 = vtMissing,
        const _variant_t & Arg6 = vtMissing,
        const _variant_t & Arg7 = vtMissing,
        const _variant_t & Arg8 = vtMissing,
        const _variant_t & Arg9 = vtMissing,
        const _variant_t & Arg10 = vtMissing,
        const _variant_t & Arg11 = vtMissing,
        const _variant_t & Arg12 = vtMissing,
        const _variant_t & Arg13 = vtMissing,
        const _variant_t & Arg14 = vtMissing,
        const _variant_t & Arg15 = vtMissing,
        const _variant_t & Arg16 = vtMissing,
        const _variant_t & Arg17 = vtMissing,
        const _variant_t & Arg18 = vtMissing,
        const _variant_t & Arg19 = vtMissing,
        const _variant_t & Arg20 = vtMissing,
        const _variant_t & Arg21 = vtMissing,
        const _variant_t & Arg22 = vtMissing,
        const _variant_t & Arg23 = vtMissing,
        const _variant_t & Arg24 = vtMissing,
        const _variant_t & Arg25 = vtMissing,
        const _variant_t & Arg26 = vtMissing,
        const _variant_t & Arg27 = vtMissing,
        const _variant_t & Arg28 = vtMissing,
        const _variant_t & Arg29 = vtMissing,
        const _variant_t & Arg30 = vtMissing );
    _variant_t Select ( );
    _variant_t Show ( );
    _variant_t ShowDependents (
        const _variant_t & Remove = vtMissing );
    _variant_t GetShowDetail ( );
    void PutShowDetail (
        const _variant_t & _arg1 );
    _variant_t ShowErrors ( );
    _variant_t ShowPrecedents (
        const _variant_t & Remove = vtMissing );
    _variant_t GetShrinkToFit ( );
    void PutShrinkToFit (
        const _variant_t & _arg1 );
    _variant_t Sort (
        const _variant_t & Key1,
        enum XlSortOrder Order1,
        const _variant_t & Key2,
        const _variant_t & Type,
        enum XlSortOrder Order2,
        const _variant_t & Key3,
        enum XlSortOrder Order3,
        enum XlYesNoGuess Header,
        const _variant_t & OrderCustom,
        const _variant_t & MatchCase,
        enum XlSortOrientation Orientation,
        enum XlSortMethod SortMethod,
        enum XlSortDataOption DataOption1,
        enum XlSortDataOption DataOption2,
        enum XlSortDataOption DataOption3 );
    _variant_t SortSpecial (
        enum XlSortMethod SortMethod,
        const _variant_t & Key1,
        enum XlSortOrder Order1,
        const _variant_t & Type,
        const _variant_t & Key2,
        enum XlSortOrder Order2,
        const _variant_t & Key3,
        enum XlSortOrder Order3,
        enum XlYesNoGuess Header,
        const _variant_t & OrderCustom,
        const _variant_t & MatchCase,
        enum XlSortOrientation Orientation,
        enum XlSortDataOption DataOption1,
        enum XlSortDataOption DataOption2,
        enum XlSortDataOption DataOption3 );
    SoundNotePtr GetSoundNote ( );
    RangePtr SpecialCells (
        enum XlCellType Type,
        const _variant_t & Value = vtMissing );
    _variant_t GetStyle ( );
    void PutStyle (
        const _variant_t & _arg1 );
    _variant_t SubscribeTo (
        _bstr_t Edition,
        enum XlSubscribeToFormat Format );
    _variant_t Subtotal (
        long GroupBy,
        enum XlConsolidationFunction Function,
        const _variant_t & TotalList,
        const _variant_t & Replace,
        const _variant_t & PageBreaks,
        enum XlSummaryRow SummaryBelowData );
    _variant_t GetSummary ( );
    _variant_t Table (
        const _variant_t & RowInput = vtMissing,
        const _variant_t & ColumnInput = vtMissing );
    _variant_t GetText ( );
    _variant_t TextToColumns (
        const _variant_t & Destination,
        enum XlTextParsingType DataType,
        enum XlTextQualifier TextQualifier,
        const _variant_t & ConsecutiveDelimiter = vtMissing,
        const _variant_t & Tab = vtMissing,
        const _variant_t & Semicolon = vtMissing,
        const _variant_t & Comma = vtMissing,
        const _variant_t & Space = vtMissing,
        const _variant_t & Other = vtMissing,
        const _variant_t & OtherChar = vtMissing,
        const _variant_t & FieldInfo = vtMissing,
        const _variant_t & DecimalSeparator = vtMissing,
        const _variant_t & ThousandsSeparator = vtMissing,
        const _variant_t & TrailingMinusNumbers = vtMissing );
    _variant_t GetTop ( );
    _variant_t Ungroup ( );
    _variant_t GetUseStandardHeight ( );
    void PutUseStandardHeight (
        const _variant_t & _arg1 );
    _variant_t GetUseStandardWidth ( );
    void PutUseStandardWidth (
        const _variant_t & _arg1 );
    ValidationPtr GetValidation ( );
    _variant_t GetValue (
        const _variant_t & RangeValueDataType = vtMissing );
    void PutValue (
        const _variant_t & RangeValueDataType,
        const _variant_t & _arg2 = vtMissing );
    _variant_t GetValue2 ( );
    void PutValue2 (
        const _variant_t & _arg1 );
    _variant_t GetVerticalAlignment ( );
    void PutVerticalAlignment (
        const _variant_t & _arg1 );
    _variant_t GetWidth ( );
    _WorksheetPtr GetWorksheet ( );
    _variant_t GetWrapText ( );
    void PutWrapText (
        const _variant_t & _arg1 );
    CommentPtr AddComment (
        const _variant_t & Text = vtMissing );
    CommentPtr GetComment ( );
    HRESULT ClearComments ( );
    PhoneticPtr GetPhonetic ( );
    FormatConditionsPtr GetFormatConditions ( );
    long GetReadingOrder ( );
    void PutReadingOrder (
        long _arg1 );
    HyperlinksPtr GetHyperlinks ( );
    PhoneticsPtr GetPhonetics ( );
    HRESULT SetPhonetic ( );
    _bstr_t GetID ( );
    void PutID (
        _bstr_t _arg1 );
    _variant_t _PrintOut (
        const _variant_t & From = vtMissing,
        const _variant_t & To = vtMissing,
        const _variant_t & Copies = vtMissing,
        const _variant_t & Preview = vtMissing,
        const _variant_t & ActivePrinter = vtMissing,
        const _variant_t & PrintToFile = vtMissing,
        const _variant_t & Collate = vtMissing,
        const _variant_t & PrToFileName = vtMissing );
    PivotCellPtr GetPivotCell ( );
    HRESULT Dirty ( );
    ErrorsPtr GetErrors ( );
    SmartTagsPtr GetSmartTags ( );
    HRESULT Speak (
        const _variant_t & SpeakDirection = vtMissing,
        const _variant_t & SpeakFormulas = vtMissing );
    _variant_t PasteSpecial (
        enum XlPasteType Paste,
        enum XlPasteSpecialOperation Operation,
        const _variant_t & SkipBlanks = vtMissing,
        const _variant_t & Transpose = vtMissing );
    VARIANT_BOOL GetAllowEdit ( );
    ListObjectPtr GetListObject ( );
    XPathPtr GetXPath ( );
    ActionsPtr GetServerActions ( );
    HRESULT RemoveDuplicates (
        const _variant_t & Columns,
        enum XlYesNoGuess Header );
    _variant_t PrintOut (
        const _variant_t & From = vtMissing,
        const _variant_t & To = vtMissing,
        const _variant_t & Copies = vtMissing,
        const _variant_t & Preview = vtMissing,
        const _variant_t & ActivePrinter = vtMissing,
        const _variant_t & PrintToFile = vtMissing,
        const _variant_t & Collate = vtMissing,
        const _variant_t & PrToFileName = vtMissing );
    _bstr_t GetMDX ( );
    HRESULT ExportAsFixedFormat (
        enum XlFixedFormatType Type,
        const _variant_t & Filename = vtMissing,
        const _variant_t & Quality = vtMissing,
        const _variant_t & IncludeDocProperties = vtMissing,
        const _variant_t & IgnorePrintAreas = vtMissing,
        const _variant_t & From = vtMissing,
        const _variant_t & To = vtMissing,
        const _variant_t & OpenAfterPublish = vtMissing,
        const _variant_t & FixedFormatExtClassPtr = vtMissing );
    _variant_t GetCountLarge ( );
    _variant_t CalculateRowMajorOrder ( );
    SparklineGroupsPtr GetSparklineGroups ( );
    HRESULT ClearHyperlinks ( );
    DisplayFormatPtr GetDisplayFormat ( );
    _variant_t BorderAround (
        const _variant_t & LineStyle,
        enum XlBorderWeight Weight,
        enum XlColorIndex ColorIndex,
        const _variant_t & Color = vtMissing,
        const _variant_t & ThemeColor = vtMissing );
    HRESULT AllocateChanges ( );
    HRESULT DiscardChanges ( );
};

*/

char szFilters[] = "Excel Files(*.xlsx)|*.xlsx|Excel Files(*.xls)|*.xls|All Files (*.*)|*.*||";

void AddLog(CString strPath)
{
	CFile file;
	CString strLogPath;
	CString toWrite;
	CTime t = CTime::GetCurrentTime();
	toWrite = t.Format("%Y-%m-%d, %H:%M:%S ");
	toWrite += strPath;
	toWrite += "\r\n";

	GetModuleFileName(GetModuleHandle(NULL), strLogPath.GetBufferSetLength(MAX_PATH), MAX_PATH);
	strLogPath.ReleaseBuffer();

//	AfxMessageBox(strLogPath);
	strLogPath.Replace(".exe", ".txt");
	
	
	if(file.Open(strLogPath, CFile::modeWrite|CFile::modeCreate|CFile::modeNoTruncate))
	{
	file.SeekToEnd();
	file.Write(toWrite.GetBuffer(0), toWrite.GetLength());


	file.Close();
	}
	else
	{
		AfxMessageBox("write log error");
	}
}



_variant_t vadd(_variant_t& a1, _variant_t& a2)
{
	double f1 = a1;
	double f2 = a2;
	return (_variant_t)(double)(f1+f2);
}
_variant_t operator+(_variant_t& a1, _variant_t& a2)
{
	return vadd(a1, a2);
}
class DI
{
public:
	_variant_t _1vessel;
	_variant_t _2voyage;
	_variant_t _3pol_date;
	_variant_t _4trade;
	_variant_t _5boxcif20;
	_variant_t _6boxcif40;
	_variant_t _7boxfob20;
	_variant_t _8boxfob40;
	_variant_t _9OF;

public:
	DI()
	{
		
	}
	
	bool operator == (DI& r)
	{
		return _1vessel == r._1vessel 
			&& _2voyage == r._2voyage
			&& _4trade == r._4trade;
	}

	void operator += (DI& r)
	{
		_5boxcif20 = vadd(_5boxcif20, r._5boxcif20);
		_6boxcif40 = vadd(_6boxcif40, r._6boxcif40);
		_7boxfob20 = vadd(_7boxfob20, r._7boxfob20);
		_8boxfob40 = vadd(_8boxfob40, r._8boxfob40);
		_9OF = vadd(_9OF, r._9OF);
	}

	BOOL operator < (DI& r)
	{
		return strcmp((char*)(_bstr_t)_2voyage, (char*)(_bstr_t)r._2voyage) < 0;
	}
};

CTypedPtrArray<CPtrArray, DI*> aDI;

void paixu()
{
	DI* tmp;
	for(int i=0; i<aDI.GetSize(); i++)
	{
		for(int j=0; j<aDI.GetSize(); j++)
		{
			if(i != j)
			{
				if(*aDI[j] < *aDI[i]){}
				else
				{
					tmp = aDI[j];
					aDI[j] = aDI[i];
					aDI[i] = tmp;
				}
			}
		}
	}
}

void HandleFile(CString strPath)
{
	AddLog(strPath);

	Excel::_ApplicationPtr pApp;
	Excel::WorkbooksPtr pWbs;
	Excel::_WorkbookPtr pWb;
	Excel::SheetsPtr pwss;
	Excel::RangePtr range;

	Excel::_WorksheetPtr shSource;
	Excel::_WorksheetPtr shDest;

	int i=2;
	CString pos;

	_variant_t value;

	VARIANT var;
	var.intVal = 1;
	var.vt = VT_INT;
	
	_variant_t varPath(strPath);

	

//	CLSID clsid;
//	CLSIDFromProgID(L"Excel.Application", &clsid);
	pApp.CreateInstance("Excel.Application");
	pApp->PutVisible(0, VARIANT_TRUE);


	pWbs = pApp->GetWorkbooks();
	pWb = pWbs->Add(varPath);
	
	//pWb->GetSheets()->get_Item(var, &disp);
	pwss = pWb->GetSheets();

	
	shSource = pwss->GetItem(var);
	shDest = pwss->Add();
	Excel::InteriorPtr     pInterior;
	

	/* 设置目标表格的边框，去掉所有虚线 */
	Excel::BordersPtr Borders;
	Excel::FontPtr font;
	range = shDest->GetCells();
//	Borders = range->GetBorders();
//	Borders->PutLineStyle((long)Excel::xlDash);
	font = range->GetFont();
	font->PutName(_T("Arial"));
	font->PutSize(long(10));

	range = shDest->GetRange(_T("A1"), _T("I2"));
	range->PutHorizontalAlignment((long)Excel::xlCenter);
	font = range->GetFont();
	font->PutBold(long(1));
	//range->PutFont(_T("Arial"));
	
	// 开始整理目标excel的表头
	range = shDest->GetRange(_T("A1"), _T("A2"));
	range->Merge(vtMissing);
	range->PutValue2(_T("Vessel Name"));

	Borders = range->GetBorders();
	Borders->PutLineStyle((long)Excel::xlContinuous);
	


	range = shDest->GetRange(_T("B1"), _T("B2"));
	range->Merge(vtMissing);
	range->PutValue2(_T("Voyage Reference"));

	Borders = range->GetBorders();
	Borders->PutLineStyle((long)Excel::xlContinuous);



	range = shDest->GetRange(_T("C1"), _T("C2"));
	range->Merge(vtMissing);
	range->PutValue2(_T("POL DepDate"));
	Borders = range->GetBorders();
	Borders->PutLineStyle((long)Excel::xlContinuous);


	range = shDest->GetRange(_T("D1"), _T("D2"));
	range->Merge(vtMissing);
	range->PutValue2(_T("TRADE"));
	Borders = range->GetBorders();
	Borders->PutLineStyle((long)Excel::xlContinuous);


	range = shDest->GetRange(_T("E1"), _T("F1"));
	range->Merge(vtMissing);
	range->PutValue2(_T("BOX(CIF)"));
	Borders = range->GetBorders();
	Borders->PutLineStyle((long)Excel::xlContinuous);

	pInterior =  range->GetInterior();
	pInterior->PutColor(long(Excel::rgbYellow));


	range = shDest->GetRange(_T("E2"));
//	range->Merge(vtMissing);
	range->PutValue2(_T("20'"));
	Borders = range->GetBorders();
	Borders->PutLineStyle((long)Excel::xlContinuous);
	pInterior =  range->GetInterior();
	pInterior->PutColor(long(Excel::rgbYellow));


	range = shDest->GetRange(_T("F2"));
//	range->Merge(vtMissing);
	range->PutValue2(_T("40'"));
	Borders = range->GetBorders();
	Borders->PutLineStyle((long)Excel::xlContinuous);
	pInterior =  range->GetInterior();
	pInterior->PutColor(long(Excel::rgbYellow));
	
	range = shDest->GetRange(_T("G2"));
//	range->Merge(vtMissing);
	range->PutValue2(_T("20'"));
	Borders = range->GetBorders();
	Borders->PutLineStyle((long)Excel::xlContinuous);
	pInterior =  range->GetInterior();
	pInterior->PutColor(long(Excel::rgbYellow));
	

	range = shDest->GetRange(_T("H2"));
//	range->Merge(vtMissing);
	range->PutValue2(_T("40'"));
	Borders = range->GetBorders();
	Borders->PutLineStyle((long)Excel::xlContinuous);
	pInterior =  range->GetInterior();
	pInterior->PutColor(long(Excel::rgbYellow));

	range = shDest->GetRange(_T("G1"), _T("H1"));
	range->Merge(vtMissing);
	range->PutValue2(_T("BOX(FOB)"));
	Borders = range->GetBorders();
	Borders->PutLineStyle((long)Excel::xlContinuous);
	pInterior =  range->GetInterior();
	pInterior->PutColor(long(Excel::rgbYellow));

	range = shDest->GetRange(_T("I1"), _T("I2"));
	range->Merge(vtMissing);
	range->PutValue2(_T("O/F (In USD$)"));
	Borders = range->GetBorders();
	Borders->PutLineStyle((long)Excel::xlContinuous);
	pInterior =  range->GetInterior();
	pInterior->PutColor(long(Excel::rgbYellow));


	_variant_t c;
	_variant_t d;
	_variant_t e;
	_variant_t j;
	_variant_t l;
	_variant_t m;
	_variant_t n;
	_variant_t o;
	_variant_t p;
	_variant_t q;
	_variant_t u;
	DI* index = NULL;
	while(1)
	{
		// _variant_t c;
		pos.Format("C%d", i);
		range = shSource->GetRange(pos.GetBuffer(0));
		c = range->GetValue();

		if(CString((char*)(_bstr_t)c).GetLength() == 0)
			break;

		// _variant_t d;
		pos.Format("D%d", i);
		range = shSource->GetRange(pos.GetBuffer(0));
		d = range->GetValue();

		// _variant_t e;
		pos.Format("E%d", i);
		range = shSource->GetRange(pos.GetBuffer(0));
		e = range->GetValue();

		// _variant_t j;
		pos.Format("J%d", i);
		range = shSource->GetRange(pos.GetBuffer(0));
		j = range->GetValue();

		// _variant_t l;
		pos.Format("L%d", i);
		range = shSource->GetRange(pos.GetBuffer(0));
		l = range->GetValue();

		// _variant_t m;
		pos.Format("M%d", i);
		range = shSource->GetRange(pos.GetBuffer(0));
		m = range->GetValue();

		// _variant_t n;
		pos.Format("N%d", i);
		range = shSource->GetRange(pos.GetBuffer(0));
		n = range->GetValue();

		//_variant_t o; 
		pos.Format("O%d", i);
		range = shSource->GetRange(pos.GetBuffer(0));
		o = range->GetValue();

		// _variant_t p;
		pos.Format("P%d", i);
		range = shSource->GetRange(pos.GetBuffer(0));
		p = range->GetValue();

		// _variant_t q;
		pos.Format("Q%d", i);
		range = shSource->GetRange(pos.GetBuffer(0));
		q = range->GetValue();

		// _variant_t u;
		pos.Format("U%d", i);
		range = shSource->GetRange(pos.GetBuffer(0));
		u = range->GetValue();

		DI* newIndex = new DI();
		newIndex->_1vessel = c;
		newIndex->_2voyage = d;
		newIndex->_3pol_date = e;
		newIndex->_4trade = j;
		if(strcmp((char*)(_bstr_t)l, "CIF") == 0)
		{
			newIndex->_5boxcif20 = m;
			newIndex->_6boxcif40 = n+o+p+q;
			newIndex->_7boxfob20 = (double)0.0;
			newIndex->_8boxfob40 = (double)0.0;
		}
		else if(strcmp((char*)(_bstr_t)l, "FOB") == 0)
		{
			newIndex->_5boxcif20 = (double)0.0;
			newIndex->_6boxcif40 = (double)0.0;
			newIndex->_7boxfob20 = m;
			newIndex->_8boxfob40 = n+o+p+q;
		}
		else
		{
			CString strMsg;
			strMsg.Format("Hi,兰德，发现数据第%d行的CIF_FOB既不是CIF也不是FOB，注意要全部大写啊！", i);
			AfxMessageBox(strMsg);
		}
		newIndex->_9OF = u;

		
		for(int i1=0; i1<aDI.GetSize(); i1++)
		{
			index = aDI.GetAt(i1);
			if(*index == *newIndex)
			{
				*index += *newIndex;
				delete newIndex;
				newIndex = NULL;
				break;
			}
		}

		if(i1 == aDI.GetSize())
		{
			if(newIndex == NULL)
			{
				AfxMessageBox("有错误，请立即致电13926584273");	
			}
			else
			{
				aDI.Add(newIndex);
			}
		}

		i++;
	}

	/* 对Array进行排序 */
	paixu();

	i = 3;

	DI* lastDI = NULL;
	int firstIndex = i;

	while(1)
	{
		if(aDI.GetSize() != 0)
		{
			index = aDI[0];
			aDI.RemoveAt(0);
		}
		else
		{
			index = NULL;
		}

		if(lastDI == NULL)
		{
			lastDI = index;
		}
		else
		{
			if(index != NULL && lastDI->_2voyage == index->_2voyage)
			{
				delete lastDI;
				lastDI = index;
			}
			else
			{
				/* 插入一行 */
				pos.Format("A%d", i);
				range = shDest->GetRange(pos.GetBuffer(0));
				range->PutValue2(lastDI->_1vessel);
				Borders = range->GetBorders();
				Borders->PutLineStyle((long)Excel::xlContinuous);
				font = range->GetFont();
				font->PutBold(long(1));

				range = range->GetEntireColumn();
				range->AutoFit();

				pos.Format("B%d", i);
				range = shDest->GetRange(pos.GetBuffer(0));
				range->PutValue2(lastDI->_2voyage);
				Borders = range->GetBorders();
				Borders->PutLineStyle((long)Excel::xlContinuous);
				font = range->GetFont();
				font->PutBold(long(1));

				range = range->GetEntireColumn();
				range->AutoFit();

				pos.Format("C%d", i);
				range = shDest->GetRange(pos.GetBuffer(0));
				range->PutValue2(lastDI->_3pol_date);
				range->PutNumberFormat(_T("YYYY-MM-DD"));
				Borders = range->GetBorders();
				Borders->PutLineStyle((long)Excel::xlContinuous);
				font = range->GetFont();
				font->PutBold(long(1));

				range = range->GetEntireColumn();
				range->AutoFit();

				pos.Format("D%d", i);
				range = shDest->GetRange(pos.GetBuffer(0));
				range->PutValue2(_T("SUB TTL"));
				Borders = range->GetBorders();
				Borders->PutLineStyle((long)Excel::xlContinuous);
				font = range->GetFont();
				font->PutBold(long(1));
				font->PutItalic(long(1));

				range = range->GetEntireColumn();
				range->AutoFit();

				pos.Format("E%d", i);
				range = shDest->GetRange(pos.GetBuffer(0));
				pos.Format("=SUM(E%d:E%d)", firstIndex, i-1);
				range->PutValue2(pos.GetBuffer(0));
				Borders = range->GetBorders();
				Borders->PutLineStyle((long)Excel::xlContinuous);
				font = range->GetFont();
				font->PutBold(long(1));
				font->PutItalic(long(1));

				range = range->GetEntireColumn();
				range->AutoFit();

				pos.Format("F%d", i);
				range = shDest->GetRange(pos.GetBuffer(0));
				pos.Format("=SUM(F%d:F%d)", firstIndex, i-1);
				range->PutValue2(pos.GetBuffer(0));
				Borders = range->GetBorders();
				Borders->PutLineStyle((long)Excel::xlContinuous);
				font = range->GetFont();
				font->PutBold(long(1));
				font->PutItalic(long(1));

				range = range->GetEntireColumn();
				range->AutoFit();

				pos.Format("G%d", i);
				range = shDest->GetRange(pos.GetBuffer(0));
				pos.Format("=SUM(G%d:G%d)", firstIndex, i-1);
				range->PutValue2(pos.GetBuffer(0));
				Borders = range->GetBorders();
				Borders->PutLineStyle((long)Excel::xlContinuous);
				font = range->GetFont();
				font->PutBold(long(1));
				font->PutItalic(long(1));

				range = range->GetEntireColumn();
				range->AutoFit();

				pos.Format("H%d", i);
				range = shDest->GetRange(pos.GetBuffer(0));
				pos.Format("=SUM(H%d:H%d)", firstIndex, i-1);
				range->PutValue2(pos.GetBuffer(0));
				Borders = range->GetBorders();
				Borders->PutLineStyle((long)Excel::xlContinuous);
				font = range->GetFont();
				font->PutBold(long(1));
				font->PutItalic(long(1));

				range = range->GetEntireColumn();
				range->AutoFit();

				pos.Format("I%d", i);
				range = shDest->GetRange(pos.GetBuffer(0));
				pos.Format("=SUM(I%d:I%d)", firstIndex, i-1);
				range->PutValue2(pos.GetBuffer(0));
				Borders = range->GetBorders();
				Borders->PutLineStyle((long)Excel::xlContinuous);
				font = range->GetFont();
				font->PutBold(long(1));
				font->PutItalic(long(1));

				range = range->GetEntireColumn();
				range->AutoFit();

				delete lastDI;
				lastDI = index;
	
				i++;
				firstIndex = i;
				
			}
		}
		
		if(index == NULL) break;

		pos.Format("A%d", i);
		range = shDest->GetRange(pos.GetBuffer(0));
		range->PutValue2(index->_1vessel);
		Borders = range->GetBorders();
		Borders->PutLineStyle((long)Excel::xlContinuous);
		range = range->GetEntireColumn();
		range->AutoFit();

		pos.Format("B%d", i);
		range = shDest->GetRange(pos.GetBuffer(0));
		range->PutValue2(index->_2voyage);
		Borders = range->GetBorders();
		Borders->PutLineStyle((long)Excel::xlContinuous);
		range = range->GetEntireColumn();
		range->AutoFit();
		
		pos.Format("C%d", i);
		range = shDest->GetRange(pos.GetBuffer(0));
		range->PutValue2(index->_3pol_date);
		Borders = range->GetBorders();
		Borders->PutLineStyle((long)Excel::xlContinuous);
		range->PutNumberFormat(_T("YYYY-MM-DD"));
		range = range->GetEntireColumn();
		range->AutoFit();

		pos.Format("D%d", i);
		range = shDest->GetRange(pos.GetBuffer(0));
		range->PutValue2(index->_4trade);
		Borders = range->GetBorders();
		Borders->PutLineStyle((long)Excel::xlContinuous);
		range = range->GetEntireColumn();
		range->AutoFit();

		pos.Format("E%d", i);
		range = shDest->GetRange(pos.GetBuffer(0));
		range->PutValue2(index->_5boxcif20);
		Borders = range->GetBorders();
		Borders->PutLineStyle((long)Excel::xlContinuous);
		font = range->GetFont();
		font->PutColor((long)Excel::rgbRed);
		range = range->GetEntireColumn();
		range->AutoFit();

		pos.Format("F%d", i);
		range = shDest->GetRange(pos.GetBuffer(0));
		range->PutValue2(index->_6boxcif40);
		Borders = range->GetBorders();
		Borders->PutLineStyle((long)Excel::xlContinuous);
		font = range->GetFont();
		font->PutColor((long)Excel::rgbRed);
		range = range->GetEntireColumn();
		range->AutoFit();

		pos.Format("G%d", i);
		range = shDest->GetRange(pos.GetBuffer(0));
		range->PutValue2(index->_7boxfob20);
		Borders = range->GetBorders();
		Borders->PutLineStyle((long)Excel::xlContinuous);
		font = range->GetFont();
		font->PutColor((long)Excel::rgbRed);
		range = range->GetEntireColumn();
		range->AutoFit();

		pos.Format("H%d", i);
		range = shDest->GetRange(pos.GetBuffer(0));
		range->PutValue2(index->_8boxfob40);
		Borders = range->GetBorders();
		Borders->PutLineStyle((long)Excel::xlContinuous);
		font = range->GetFont();
		font->PutColor((long)Excel::rgbRed);
		range = range->GetEntireColumn();
		range->AutoFit();

		pos.Format("I%d", i);
		range = shDest->GetRange(pos.GetBuffer(0));
		range->PutValue2(index->_9OF);
		Borders = range->GetBorders();
		Borders->PutLineStyle((long)Excel::xlContinuous);
		font = range->GetFont();
		font->PutColor((long)Excel::rgbRed);
		range = range->GetEntireColumn();
		range->AutoFit();

//		delete index;
		
		i++;
//		if(aDI.GetSize() == 0)
//			break;
	}


//	AfxMessageBox((char*)(_bstr_t)(range->GetText()));
//	AfxMessageBox("aaa");

//	range = shDest->GetCells();
//	range->AutoFit();
//	pApp->PutDisplayAlerts(0, FALSE);
	
//	pWb->SaveAs(strPath.GetBuffer(0), vtMissing, 
//		vtMissing, vtMissing,
//		vtMissing, vtMissing,
//		Excel::xlNoChange);
	
//	pApp->PutDisplayAlerts(0, TRUE);

//	pWbs->Close();
//	pApp->Quit();

//	pApp->Release();
//	pApp.Release();
//	if(!oExcel.CreateDispatch("Excel.Application"))
//	{
//		AfxMessageBox("Couldn't start Excel and get Application object.");
//		return;
//	}

}

void SetHeader(CString value, Excel::_WorksheetPtr shDest, CString cell1, CString cell2=CString(), BOOL bk=TRUE)
{
	Excel::InteriorPtr pInterior;
	Excel::BordersPtr Borders;
	Excel::RangePtr range;
	if(cell2 == CString())
	{
		range = shDest->GetRange(cell1.GetBuffer(0));
	}
	else
	{
		range = shDest->GetRange(cell1.GetBuffer(0), cell2.GetBuffer(0));	/* E1 F1 合并*/
		range->Merge(vtMissing);
	}
	range->PutValue2(value.GetBuffer(0));			/* 写上字 */
	Borders = range->GetBorders();					/* 字体粗 */
	Borders->PutLineStyle((long)Excel::xlContinuous);	/* 设置线条 */
	if(bk)
	{
		pInterior =  range->GetInterior();				/* 内部倒上黄色背影 */
		pInterior->PutColor(long(Excel::rgbYellow));
	}
}

BOOL ReadSrc(Excel::_WorksheetPtr sh, CMapStringToPtr& src, int i)
{
	Excel::RangePtr range;
	CString pos;
	_variant_t* v;
	for(char j='A'; j<='Y'; j++)
	{
		v = new _variant_t();
		pos.Format("%c%d", j, i);
		range = sh->GetRange(pos.GetBuffer(0));
		*v = range->GetValue();

		if(CString((char*)(_bstr_t)(*v)).GetLength() == 0 && j == 'A')
			return FALSE;

		src[pos.Left(1)] = v;
	}
	return TRUE;
}

void ClearMap(CMapStringToPtr& map)
{
	POSITION pos ;
	CString key;
	_variant_t* value;
	for( pos = map.GetStartPosition(); pos != NULL; )
	{
		map.GetNextAssoc( pos, key, (void*&)value );
		delete value;
	}

	
	map.RemoveAll();
}

CString GetDstKey(CMapStringToPtr& map)
{
	CString ret;
	_variant_t* value;
	value = (_variant_t*)map["B"];
	ret += (_bstr_t)(*value);

	value = (_variant_t*)map["H"];
	ret += (_bstr_t)(*value);

	value = (_variant_t*)map["C"];
	ret += (_bstr_t)(*value);

	value = (_variant_t*)map["A"];
	ret += (_bstr_t)(*value);
	

	return ret;
}

void insertByOrder(CTypedPtrArray<CPtrArray, CMapStringToPtr*>& array, CMapStringToPtr* ptr)
{
	int i;
	CString key = GetDstKey(*ptr);
	for(i=0; i<array.GetSize(); i++)
	{
		CString key1 = GetDstKey(*array[i]);
		if(key1 > key)
		{
			array.InsertAt(i, ptr);
			return;
		}
	}
	array.Add(ptr);
}

void HandleFile2(CString strPath)
{
	AddLog(strPath);

	Excel::_ApplicationPtr pApp;
	Excel::WorkbooksPtr pWbs;
	Excel::_WorkbookPtr pWb;
	Excel::SheetsPtr pwss;
	Excel::RangePtr range;

	Excel::_WorksheetPtr shSource;
	Excel::_WorksheetPtr shDest;

	CString pos;

	_variant_t value;

	VARIANT var;
	var.intVal = 1;
	var.vt = VT_INT;
	
	_variant_t varPath(strPath);

	pApp.CreateInstance("Excel.Application");
	pApp->PutVisible(0, VARIANT_TRUE);


	pWbs = pApp->GetWorkbooks();
	pWb = pWbs->Add(varPath);
	
	pwss = pWb->GetSheets();

	shSource = pwss->GetItem(var);
	shDest = pwss->Add();
	
	Excel::FontPtr font;
	range = shDest->GetCells();
	font = range->GetFont();
	font->PutName(_T("Arial"));
	font->PutSize(long(10));

	range = shDest->GetRange(_T("A1"), _T("O2"));
	range->PutHorizontalAlignment((long)Excel::xlCenter);
	font = range->GetFont();
	font->PutBold(long(1));

	SetHeader(_T("VESSEL_NAME"),	shDest, _T("A1"), _T("A2"), FALSE);	
	SetHeader(_T("VOYAGE"),			shDest, _T("B1"), _T("B2"), FALSE);
	SetHeader(_T("BOL_DATE"),		shDest, _T("C1"), _T("C2"), FALSE);
	SetHeader(_T("TRADE"),			shDest, _T("D1"), _T("D2"), FALSE);

	SetHeader(_T("Standard (BOX)"), shDest, _T("E1"), _T("F1"), TRUE);
	SetHeader(_T("20' TEU"),		shDest, _T("E2"), _T("E2"), TRUE);
	SetHeader(_T("40' TEU"),		shDest, _T("F2"), _T("F2"), TRUE);
	SetHeader(_T("40_HC"),			shDest, _T("G1"), _T("G1"), TRUE);
	SetHeader(_T("TEU"),			shDest, _T("G2"), _T("G2"), TRUE);
	SetHeader(_T("45_HC"),			shDest, _T("H1"), _T("H1"), TRUE);
	SetHeader(_T("TEU"),			shDest, _T("H2"), _T("H2"), TRUE);
	SetHeader(_T("Total TEU"),		shDest, _T("I1"), _T("I2"), TRUE);
	SetHeader(_T("BOX(CIF)"),		shDest, _T("J1"), _T("K1"), TRUE);
	SetHeader(_T("20'"),			shDest, _T("J2"), _T("J2"), TRUE);
	SetHeader(_T("40'"),			shDest, _T("K2"), _T("K2"), TRUE);
	SetHeader(_T("BOX(FOB)"),		shDest, _T("L1"), _T("M1"), TRUE);
	SetHeader(_T("20'"),			shDest, _T("L2"), _T("L2"), TRUE);
	SetHeader(_T("40'"),			shDest, _T("M2"), _T("M2"), TRUE);
	SetHeader(_T("FRT00"),			shDest, _T("N1"), _T("N2"), TRUE);
	SetHeader(_T("Total Box"),		shDest, _T("O1"), _T("O2"), TRUE);

	CMapStringToPtr mapSrc;
	CMapStringToPtr mapDst;

	int i=2;
	for(; ;i++)
	{
		if(!ReadSrc(shSource, mapSrc, i))
		{
			break;
		}

		CString key = GetDstKey(mapSrc);
		CMapStringToPtr* dst;
		if(!mapDst.Lookup(key, (void*&)dst))
		{
			dst = new CMapStringToPtr();
			mapDst[key] = dst;

			(*dst)["A"] = new _variant_t(*(_variant_t*)mapSrc["A"]);
			(*dst)["B"] = new _variant_t(*(_variant_t*)mapSrc["B"]);
			(*dst)["C"] = new _variant_t(*(_variant_t*)mapSrc["C"]);
			(*dst)["D"] = new _variant_t(*(_variant_t*)mapSrc["H"]);

			/* set other col */
			for(char col='E'; col <= 'O'; col++)
			{
				(*dst)[CString(col)] = new _variant_t(0.0);
			}
		}

		CString I, K; /* I, K is condition */
		double M, U, R, S, O, Q, W;
		


#define STRVAL(__map, __var) __var = (char*)(_bstr_t(*(_variant_t*)(__map)[#__var]))
#define DBLVAL(__map, __var) __var = (double)(*(_variant_t*)(__map)[#__var])
#define COLVAR(__map, __index) (*(_variant_t*)(__map)[__index])
		
		STRVAL(mapSrc, I);
		STRVAL(mapSrc, K);
		DBLVAL(mapSrc, M);
		DBLVAL(mapSrc, U);
		DBLVAL(mapSrc, R);
		DBLVAL(mapSrc, S);
		DBLVAL(mapSrc, O);
		DBLVAL(mapSrc, Q);
		DBLVAL(mapSrc, W);

#define CALCCOL(__dstCol, __srcCol) do{\
	double __dstCol; \
	DBLVAL(*dst, __dstCol);\
	__dstCol += __srcCol; \
	COLVAR(*dst, #__dstCol) = __dstCol;\
		}while(0)

		/* set other col */
		for(char col='E'; col <= 'O'; col++)
		{
			if(col == 'E')	/* sum of source M */
			{
#if 1
				CALCCOL(E, M);
#else
				double E;
				DBLVAL(*dst, E);
				E += M;
				COLVAR(*dst, "E") = E;
#endif
			}
			if(col == 'F')  /* sum of source U(Total TEU) and final minus DST G and H */
			{
				CALCCOL(F, U);
			}
			if(col == 'G')	/* sum of R, if I is ACSA1 ACSA3 SEAS PEX2 PEX3 PRE ANL */
			{
				if(I == "ACSA1" || I == "ACSA3" || I == "SEAS" || I == "PEX2" || I == "PEX3" || I == "PRE" || I == "ANL")
					CALCCOL(G, R);
			}
			if(col == 'H')	/* sum of S, if I is ACSA1 ACSA3 SEAS PEX2 PEX3 PRE ANL */
			{
				if(I == "ACSA1" || I == "ACSA3" || I == "SEAS" || I == "PEX2" || I == "PEX3" || I == "PRE" || I == "ANL")
					CALCCOL(H, S);
			}
			if(col == 'I')	/* sum of U */
			{
				CALCCOL(I, U);
			}
			if(col == 'J')	/* sum of M, if K is CIF */
			{
				if(K == "CIF")
					CALCCOL(J, M);
			}
			if(col == 'K')	/* sum of O,Q,S if K is CIF */
			{
				if(K == "CIF")
				{
				CALCCOL(K, O);
				CALCCOL(K, Q);
				CALCCOL(K, S);
				}
			}
			if(col == 'L')	/* sum of M, if K is FOB */
			{
				if(K == "FOB")
				CALCCOL(L, M);
			}
			if(col == 'M')	/* sum of O,Q,S if K is FOB */
			{
				if(K == "FOB")
				{
				CALCCOL(M, O);
				CALCCOL(M, Q);
				CALCCOL(M, S);
				}
			}
			if(col == 'N')	/* sum of W */
			{
				CALCCOL(N, W);
			}
			if(col == 'O')	/* final sum of DST J K L M */
			{}
		}

		ClearMap(mapSrc);
	}

	POSITION posDst;
	CMapStringToPtr* dstLine;
	CString key;
	CString cellPos;

	int row = 3;
	CMapStringToPtr mapTotal;
	char col;
	for(col = 'E'; col<='O'; col++)
	{
		mapTotal[CString(col)] = new _variant_t(0.0);
	}

	CTypedPtrArray<CPtrArray, CMapStringToPtr*> orderDst;
	for(posDst = mapDst.GetStartPosition(); posDst != NULL;)
	{
		mapDst.GetNextAssoc(posDst, key, (void*&)dstLine);
		insertByOrder(orderDst, dstLine);
	}
	mapDst.RemoveAll();

	for(;;)
	{
		//mapDst.GetNextAssoc(posDst, key, (void*&)dstLine);
		if(row -3 == orderDst.GetSize())
			break;
		dstLine = orderDst[row-3];
		
		double F, G, H, J, K, L, M, E;
		DBLVAL(*dstLine, F);
		DBLVAL(*dstLine, E);
		DBLVAL(*dstLine, G);
		DBLVAL(*dstLine, H);
		DBLVAL(*dstLine, J);
		DBLVAL(*dstLine, K);
		DBLVAL(*dstLine, L);
		DBLVAL(*dstLine, M);

		COLVAR(*dstLine, "F") = F-G-H-E;
		COLVAR(*dstLine, "O") = J+K+L+M;

		for(col = 'A'; col<='O'; col++)
		{
			cellPos.Format("%c%d", col, row);
			range = shDest->GetRange(cellPos.GetBuffer(0));
			range->PutValue2(COLVAR(*dstLine, CString(col)));
			if(mapTotal[CString(col)])  /* if total not null, mean need add */
			{
				double _value = (double)(*(_variant_t*)mapTotal[CString(col)]);
				_value += (double)(*(_variant_t*)(*dstLine)[CString(col)]);
				*(_variant_t*)mapTotal[CString(col)] = _value;
			}
			delete (*dstLine)[CString(col)];
			if(col == 'C')
				range->PutNumberFormat(_T("YYYY-MM-DD"));
		}

		dstLine->RemoveAll();
		delete dstLine;
		row++;
	}

	
	for(col = 'E'; col<='O'; col++)
	{
		cellPos.Format("%c%d", col, row);
		range = shDest->GetRange(cellPos.GetBuffer(0));
		range->PutValue2(COLVAR(mapTotal, CString(col)));
		delete mapTotal[CString(col)];
		font = range->GetFont();
		font->PutBold(long(1));
	}

	cellPos.Format("%c%d", 'D', row);
	range = shDest->GetRange(cellPos.GetBuffer(0));
	range->PutValue2(CString("SUB TTL").GetBuffer(0));
	font = range->GetFont();
	font->PutBold(long(1));
	font->PutItalic(long(1));

	mapTotal.RemoveAll();

	//	AfxMessageBox((char*)(_bstr_t)(range->GetText()));
//	AfxMessageBox("aaa");

//	range = shDest->GetCells();
//	range->AutoFit();
	pApp->PutDisplayAlerts(0, FALSE);
	
	pWb->SaveAs(strPath.GetBuffer(0), vtMissing, 
		vtMissing, vtMissing,
		vtMissing, vtMissing,
		Excel::xlNoChange);
	
	pApp->PutDisplayAlerts(0, TRUE);

	pWbs->Close();
	pApp->Quit();

//	pApp->Release();
//	pApp.Release();
	
}

void CLande2Dlg::OnOK() 
{
	// TODO: Add extra validation here
	int ret;
	POSITION pos;
	CString strPath;

	CFileDialog fd(TRUE, "xlsx", "*.xlsx", OFN_FILEMUSTEXIST|OFN_ALLOWMULTISELECT, szFilters, this);
	ret = fd.DoModal();

	/* 打开文件失败 */
	if(ret != IDOK)
		return;
	
	pos = fd.GetStartPosition();
	while(pos != NULL)
	{
		strPath = fd.GetNextPathName(pos);
	//	MessageBox(strPath);

		HandleFile2(strPath);
	}
	

	AfxMessageBox("已经完成");
	
}
