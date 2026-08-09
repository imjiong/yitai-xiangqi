#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/wx.h>
#include <wx/aui/aui.h>
#include "BoardPanel.h"

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

    wxAuiManager m_auiManager;
    BoardPanel* m_boardPanel;

    wxDECLARE_EVENT_TABLE();
};

#endif // MAINFRAME_H
