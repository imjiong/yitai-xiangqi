#include "MainFrame.h"
#include <wx/dcclient.h>
#include <wx/gdicmn.h>

static const wxString APP_TITLE = wxT("中国象棋 UCI 客户端");
static const int STATUSBAR_PANES = 5;

enum
{
    ID_FLIP_VIEW = wxID_HIGHEST + 1,
    ID_RESET_BOARD
};

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(ID_FLIP_VIEW, MainFrame::OnFlipView)
    EVT_MENU(ID_RESET_BOARD, MainFrame::OnResetBoard)
wxEND_EVENT_TABLE()

static wxBitmap CreatePlaceholderBitmap(const wxString& label, const wxSize& size)
{
    wxBitmap bmp(size);
    wxMemoryDC dc;
    dc.SelectObject(bmp);

    dc.SetBackground(wxBrush(wxColour(60, 60, 60)));
    dc.Clear();

    dc.SetTextForeground(wxColour(255, 255, 255));
    dc.SetFont(wxFont(40, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    dc.DrawLabel(label, wxRect(size), wxALIGN_CENTER);

    dc.SelectObject(wxNullBitmap);
    return bmp;
}

MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, APP_TITLE,
              wxDefaultPosition, wxSize(1200, 800),
              wxDEFAULT_FRAME_STYLE | wxCLIP_CHILDREN),
      m_boardPanel(nullptr)
{
    SetMinSize(wxSize(800, 600));

    CreateMenuBar();
    SetupToolBar();
    SetupStatusBar();

    m_boardPanel = new BoardPanel(this);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_boardPanel, 1, wxEXPAND | wxALL, 5);
    SetSizer(sizer);

    m_auiManager.SetManagedWindow(this);

    SetBackgroundColour(wxColour(240, 240, 240));

    Centre(wxBOTH);
}

MainFrame::~MainFrame()
{
    m_auiManager.UnInit();
}

void MainFrame::CreateMenuBar()
{
    wxMenuBar* menuBar = new wxMenuBar();

    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(wxID_NEW, wxT("新建棋谱\tCtrl+N"));
    fileMenu->Append(wxID_OPEN, wxT("打开棋谱\tCtrl+O"));
    fileMenu->Append(wxID_SAVE, wxT("保存棋谱\tCtrl+S"));
    fileMenu->Append(wxID_SAVEAS, wxT("另存为...\tCtrl+Shift+S"));
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, wxT("退出\tAlt+F4"));
    menuBar->Append(fileMenu, wxT("文件(&F)"));

    wxMenu* editMenu = new wxMenu();
    editMenu->Append(wxID_ANY, wxT("后退一招\tLeft"));
    editMenu->Append(wxID_ANY, wxT("前进一招\tRight"));
    editMenu->Append(wxID_ANY, wxT("后退至开局\tHome"));
    editMenu->Append(wxID_ANY, wxT("前进至中局\tEnd"));
    editMenu->AppendSeparator();
    editMenu->Append(wxID_ANY, wxT("悔棋\tCtrl+Z"));
    editMenu->Append(wxID_ANY, wxT("编辑局面"));
    editMenu->AppendSeparator();
    editMenu->Append(wxID_ANY, wxT("复制局面\tCtrl+C"));
    editMenu->Append(wxID_ANY, wxT("粘贴局面\tCtrl+V"));
    editMenu->AppendSeparator();
    editMenu->Append(ID_FLIP_VIEW, wxT("翻转棋盘\tCtrl+F"));
    editMenu->Append(ID_RESET_BOARD, wxT("重置棋盘"));
    menuBar->Append(editMenu, wxT("编辑(&E)"));

    wxMenu* dbMenu = new wxMenu();
    dbMenu->Append(wxID_ANY, wxT("导入棋库"));
    dbMenu->Append(wxID_ANY, wxT("导出棋库"));
    dbMenu->AppendSeparator();
    dbMenu->Append(wxID_ANY, wxT("数据库设置..."));
    menuBar->Append(dbMenu, wxT("数据库(&D)"));

    wxMenu* engineMenu = new wxMenu();
    engineMenu->Append(wxID_ANY, wxT("引擎执红"));
    engineMenu->Append(wxID_ANY, wxT("引擎执黑"));
    engineMenu->Append(wxID_ANY, wxT("立即出招"));
    engineMenu->Append(wxID_ANY, wxT("强制变招"));
    engineMenu->AppendSeparator();
    engineMenu->Append(wxID_ANY, wxT("深度分析"));
    engineMenu->AppendSeparator();
    engineMenu->Append(wxID_ANY, wxT("引擎设置..."));
    menuBar->Append(engineMenu, wxT("引擎程序(&G)"));

    wxMenu* externalMenu = new wxMenu();
    externalMenu->Append(wxID_ANY, wxT("开始连线"));
    externalMenu->Append(wxID_ANY, wxT("外部对局设置..."));
    menuBar->Append(externalMenu, wxT("外部对局(&X)"));

    wxMenu* layoutMenu = new wxMenu();
    layoutMenu->Append(wxID_ANY, wxT("恢复默认布局"));
    menuBar->Append(layoutMenu, wxT("窗口布局(&W)"));

    wxMenu* aboutMenu = new wxMenu();
    aboutMenu->Append(wxID_ABOUT, wxT("关于"));
    menuBar->Append(aboutMenu, wxT("关于(&A)"));

    SetMenuBar(menuBar);
}

void MainFrame::SetupToolBar()
{
    wxToolBar* toolBar = wxFrame::CreateToolBar(wxTB_HORIZONTAL | wxTB_TEXT | wxTB_NODIVIDER, wxID_ANY);

    wxSize toolSize = FromDIP(wxSize(48, 48));
    toolBar->SetToolBitmapSize(toolSize);

    toolBar->AddTool(ID_RESET_BOARD, wxT("重置"), CreatePlaceholderBitmap(wxT("重"), toolSize), wxT("重置棋盘"), wxITEM_NORMAL);
    toolBar->AddTool(ID_FLIP_VIEW, wxT("翻转"), CreatePlaceholderBitmap(wxT("翻"), toolSize), wxT("翻转棋盘"), wxITEM_NORMAL);
    toolBar->AddSeparator();
    toolBar->AddTool(wxID_ANY, wxT("后退"), CreatePlaceholderBitmap(wxT("←"), toolSize), wxT("后退一招"), wxITEM_NORMAL);
    toolBar->AddTool(wxID_ANY, wxT("前进"), CreatePlaceholderBitmap(wxT("→"), toolSize), wxT("前进一招"), wxITEM_NORMAL);
    toolBar->AddSeparator();
    toolBar->AddTool(wxID_ANY, wxT("悔棋"), CreatePlaceholderBitmap(wxT("悔"), toolSize), wxT("悔棋"), wxITEM_NORMAL);
    toolBar->AddSeparator();
    toolBar->AddTool(wxID_ANY, wxT("引擎"), CreatePlaceholderBitmap(wxT("引"), toolSize), wxT("引擎设置"), wxITEM_NORMAL);

    toolBar->Realize();
}

void MainFrame::SetupStatusBar()
{
    wxFrame::CreateStatusBar(STATUSBAR_PANES);
    int widths[] = {-2, -1, -1, -1, -1};
    SetStatusWidths(STATUSBAR_PANES, widths);

    SetStatusText(wxT("就绪"), 0);
    SetStatusText(wxT("深度: 0"), 1);
    SetStatusText(wxT("分数: 0"), 2);
    SetStatusText(wxT("时间: 0s"), 3);
    SetStatusText(wxT("NPS: 0"), 4);
}

void MainFrame::OnFlipView(wxCommandEvent& event)
{
    if (m_boardPanel)
        m_boardPanel->FlipView();
}

void MainFrame::OnResetBoard(wxCommandEvent& event)
{
    if (m_boardPanel)
        m_boardPanel->ResetBoard();
}
