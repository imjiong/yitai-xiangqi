#include "MainFrame.h"
#include <wx/dcclient.h>
#include <wx/gdicmn.h>
#include <wx/filedlg.h>

static const wxString APP_TITLE = wxT("中国象棋 UCI 客户端");
static const int STATUSBAR_PANES = 5;

enum
{
    ID_FLIP_VIEW = wxID_HIGHEST + 1,
    ID_RESET_BOARD,
    ID_GO_BACK,
    ID_GO_FORWARD,
    ID_GO_START,
    ID_GO_END,
    ID_PLAY
};

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(ID_FLIP_VIEW, MainFrame::OnFlipView)
    EVT_MENU(ID_RESET_BOARD, MainFrame::OnResetBoard)
    EVT_MENU(ID_GO_BACK, MainFrame::OnGoBack)
    EVT_MENU(ID_GO_FORWARD, MainFrame::OnGoForward)
    EVT_MENU(ID_GO_START, MainFrame::OnGoToStart)
    EVT_MENU(ID_GO_END, MainFrame::OnGoToEnd)
    EVT_MENU(ID_PLAY, MainFrame::OnPlay)
    EVT_MENU(wxID_OPEN, MainFrame::OnOpen)
    EVT_MENU(wxID_SAVE, MainFrame::OnSave)
    EVT_MENU(wxID_SAVEAS, MainFrame::OnSaveAs)
    EVT_MENU(wxID_NEW, MainFrame::OnNew)
    EVT_TIMER(wxID_ANY, MainFrame::OnPlayTimer)
wxEND_EVENT_TABLE()

static wxBitmap CreatePlaceholderBitmap(const wxString& label, const wxSize& size)
{
    wxBitmap bmp(size);
    wxMemoryDC dc;
    dc.SelectObject(bmp);

    dc.SetBackground(wxBrush(wxColour(60, 60, 60)));
    dc.Clear();

    dc.SetTextForeground(wxColour(255, 255, 255));
    dc.SetFont(wxFont(7, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    dc.DrawLabel(label, wxRect(size), wxALIGN_CENTER);

    dc.SelectObject(wxNullBitmap);
    return bmp;
}

MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, APP_TITLE,
              wxDefaultPosition, wxSize(1200, 800),
              wxDEFAULT_FRAME_STYLE | wxCLIP_CHILDREN),
      m_boardPanel(nullptr),
      m_movePanel(nullptr),
      m_isPlaying(false)
{
    SetMinSize(wxSize(800, 600));

    CreateMenuBar();
    SetupToolBar();
    SetupStatusBar();

    m_auiManager.SetManagedWindow(this);

    m_boardPanel = new BoardPanel(this);
    m_movePanel = new MoveRecordPanel(this);
    m_movePanel->SetGame(&m_game);

    m_auiManager.AddPane(m_boardPanel, wxAuiPaneInfo()
        .Name(wxT("Board"))
        .Caption(wxT("棋盘"))
        .CenterPane()
        .PaneBorder(false)
        .CloseButton(false)
        .MinSize(400, 300));

    m_auiManager.AddPane(m_movePanel, wxAuiPaneInfo()
        .Name(wxT("MoveRecord"))
        .Caption(wxT("棋谱记录"))
        .Right()
        .Layer(1)
        .BestSize(280, 400)
        .MinSize(200, 200)
        .CloseButton(true)
        .Resizable(true)
        .Dockable(true)
        .Floatable(true));

    m_boardPanel->SetGame(&m_game);
    m_boardPanel->SetMoveCallback(&MainFrame::OnBoardMoveMade, this);

    m_auiManager.Update();

    SetBackgroundColour(wxColour(240, 240, 240));

    m_playTimer = new wxTimer(this, wxID_ANY);

    Centre(wxBOTH);
}

MainFrame::~MainFrame()
{
    if (m_playTimer->IsRunning())
        m_playTimer->Stop();
    delete m_playTimer;
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
    editMenu->Append(ID_GO_BACK, wxT("后退一招\tLeft"));
    editMenu->Append(ID_GO_FORWARD, wxT("前进一招\tRight"));
    editMenu->Append(ID_GO_START, wxT("后退至开局\tHome"));
    editMenu->Append(ID_GO_END, wxT("前进至末招\tEnd"));
    editMenu->AppendSeparator();
    m_playMenuItem = editMenu->Append(ID_PLAY, wxT("播放棋谱\tSpace"));
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

    wxSize toolSize = FromDIP(wxSize(24, 24));
    toolBar->SetToolBitmapSize(toolSize);

    toolBar->AddTool(ID_GO_START, wxT("开局"), CreatePlaceholderBitmap(wxT("|<<"), toolSize), wxT("后退至开局"), wxITEM_NORMAL);
    toolBar->AddTool(ID_GO_BACK, wxT("后退"), CreatePlaceholderBitmap(wxT("<"), toolSize), wxT("后退一招"), wxITEM_NORMAL);
    m_playTool = toolBar->AddTool(ID_PLAY, wxT("播放"), CreatePlaceholderBitmap(wxT(">"), toolSize), wxT("播放棋谱"), wxITEM_NORMAL);
    toolBar->AddTool(ID_GO_FORWARD, wxT("前进"), CreatePlaceholderBitmap(wxT(">"), toolSize), wxT("前进一招"), wxITEM_NORMAL);
    toolBar->AddTool(ID_GO_END, wxT("末招"), CreatePlaceholderBitmap(wxT(">>|"), toolSize), wxT("前进至末招"), wxITEM_NORMAL);
    toolBar->AddSeparator();
    toolBar->AddTool(ID_RESET_BOARD, wxT("重置"), CreatePlaceholderBitmap(wxT("重"), toolSize), wxT("重置棋盘"), wxITEM_NORMAL);
    toolBar->AddTool(ID_FLIP_VIEW, wxT("翻转"), CreatePlaceholderBitmap(wxT("翻"), toolSize), wxT("翻转棋盘"), wxITEM_NORMAL);
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
    m_game.NewGame();
    if (m_boardPanel)
        m_boardPanel->SyncWithGame();
    if (m_movePanel)
        m_movePanel->RefreshList();
}

void MainFrame::OnGoBack(wxCommandEvent& event)
{
    m_game.GoBack();
    if (m_boardPanel)
        m_boardPanel->SyncWithGame();
    if (m_movePanel)
        m_movePanel->RefreshList();
}

void MainFrame::OnGoForward(wxCommandEvent& event)
{
    m_game.GoForward();
    if (m_boardPanel)
        m_boardPanel->SyncWithGame();
    if (m_movePanel)
        m_movePanel->RefreshList();
}

void MainFrame::OnGoToStart(wxCommandEvent& event)
{
    m_game.GoToStart();
    if (m_boardPanel)
        m_boardPanel->SyncWithGame();
    if (m_movePanel)
        m_movePanel->RefreshList();
}

void MainFrame::OnGoToEnd(wxCommandEvent& event)
{
    m_game.GoToEnd();
    if (m_boardPanel)
        m_boardPanel->SyncWithGame();
    if (m_movePanel)
        m_movePanel->RefreshList();
}

void MainFrame::OnPlay(wxCommandEvent& event)
{
    TogglePlay();
}

void MainFrame::TogglePlay()
{
    if (m_isPlaying)
    {
        m_isPlaying = false;
        m_playTimer->Stop();
        if (m_playMenuItem)
            m_playMenuItem->SetItemLabel(wxT("播放棋谱\tSpace"));
        if (m_playTool)
            m_playTool->SetShortHelp(wxT("播放棋谱"));
        if (wxToolBar* tb = GetToolBar())
            tb->Realize();
    }
    else
    {
        if (m_game.GetMoveCount() == 0)
            return;

        if (m_game.GetCurrentIndex() >= m_game.GetMoveCount() - 1)
            m_game.GoToStart();

        m_isPlaying = true;
        if (m_playMenuItem)
            m_playMenuItem->SetItemLabel(wxT("暂停\tSpace"));
        if (m_playTool)
            m_playTool->SetShortHelp(wxT("暂停"));
        if (wxToolBar* tb = GetToolBar())
            tb->Realize();

        m_playTimer->Start(800);
    }
}

void MainFrame::OnPlayTimer(wxTimerEvent& event)
{
    if (!m_isPlaying)
        return;

    if (m_game.GetCurrentIndex() >= m_game.GetMoveCount() - 1)
    {
        TogglePlay();
        return;
    }

    m_game.GoForward();
    if (m_boardPanel)
        m_boardPanel->SyncWithGame();
    if (m_movePanel)
        m_movePanel->RefreshList();
}

void MainFrame::OnOpen(wxCommandEvent& event)
{
    wxFileDialog dlg(this, wxT("打开棋谱"), "", "",
                     wxT("PGN 文件 (*.pgn)|*.pgn|所有文件 (*.*)|*.*"),
                     wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (dlg.ShowModal() == wxID_OK)
    {
        wxString path = dlg.GetPath();
        if (m_game.LoadFromPGN(std::string(path.mb_str())))
        {
            if (m_boardPanel)
                m_boardPanel->SyncWithGame();
            if (m_movePanel)
                m_movePanel->RefreshList();
            SetStatusText(wxT("已打开: ") + path, 0);
        }
        else
        {
            wxMessageBox(wxT("无法打开文件: ") + path, wxT("错误"), wxOK | wxICON_ERROR);
        }
    }
}

void MainFrame::OnSave(wxCommandEvent& event)
{
    wxFileDialog dlg(this, wxT("保存棋谱"), "", "",
                     wxT("PGN 文件 (*.pgn)|*.pgn"),
                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (dlg.ShowModal() == wxID_OK)
    {
        wxString path = dlg.GetPath();
        if (path.Right(4).Lower() != wxT(".pgn"))
            path += wxT(".pgn");

        if (m_game.SaveToPGN(std::string(path.mb_str())))
        {
            SetStatusText(wxT("已保存: ") + path, 0);
        }
        else
        {
            wxMessageBox(wxT("无法保存文件: ") + path, wxT("错误"), wxOK | wxICON_ERROR);
        }
    }
}

void MainFrame::OnSaveAs(wxCommandEvent& event)
{
    OnSave(event);
}

void MainFrame::OnNew(wxCommandEvent& event)
{
    m_game.NewGame();
    if (m_boardPanel)
        m_boardPanel->SyncWithGame();
    if (m_movePanel)
        m_movePanel->RefreshList();
    SetStatusText(wxT("新建棋谱"), 0);
}

void MainFrame::OnBoardMoveMade(void* userData)
{
    MainFrame* frame = static_cast<MainFrame*>(userData);
    if (frame && frame->m_movePanel)
        frame->m_movePanel->RefreshList();
}
