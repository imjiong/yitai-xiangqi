#ifndef MOVERECORDPANEL_H
#define MOVERECORDPANEL_H

#include <wx/wx.h>
#include <wx/listctrl.h>
#include "../core/Game.h"

class MoveRecordPanel : public wxPanel
{
public:
    MoveRecordPanel(wxWindow* parent);

    void SetGame(Game* game);

    void RefreshList();
    void GoToMove(int index);
    int GetSelectedIndex() const;

private:
    void OnItemSelected(wxListEvent& event);
    void OnItemActivated(wxListEvent& event);

    wxListCtrl* m_listCtrl;
    Game* m_game;

    wxDECLARE_EVENT_TABLE();
};

#endif // MOVERECORDPANEL_H
