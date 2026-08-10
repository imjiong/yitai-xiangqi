#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/wx.h>
#include <wx/aui/aui.h>
#include "BoardPanel.h"
#include "MoveRecordPanel.h"
#include "../core/Game.h"

class MainFrame : public wxFrame
{
public:
    MainFrame();
    ~MainFrame();

private:
    void CreateMenuBar();
    void SetupToolBar();
    void SetupStatusBar();

    void OnFlipView(wxCommandEvent& event);
    void OnResetBoard(wxCommandEvent& event);
    void OnGoBack(wxCommandEvent& event);
    void OnGoForward(wxCommandEvent& event);
    void OnGoToStart(wxCommandEvent& event);
    void OnGoToEnd(wxCommandEvent& event);
    void OnPlay(wxCommandEvent& event);
    void TogglePlay();
    void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);
    void OnNew(wxCommandEvent& event);

    void OnPlayTimer(wxTimerEvent& event);

    wxAuiManager m_auiManager;
    BoardPanel* m_boardPanel;
    MoveRecordPanel* m_movePanel;
    Game m_game;

    wxMenuItem* m_playMenuItem;
    wxToolBarToolBase* m_playTool;
    wxTimer* m_playTimer;
    bool m_isPlaying;

    wxDECLARE_EVENT_TABLE();
};

#endif // MAINFRAME_H
