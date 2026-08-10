#include "MoveRecordPanel.h"

wxBEGIN_EVENT_TABLE(MoveRecordPanel, wxPanel)
    EVT_LIST_ITEM_SELECTED(wxID_ANY, MoveRecordPanel::OnItemSelected)
    EVT_LIST_ITEM_ACTIVATED(wxID_ANY, MoveRecordPanel::OnItemActivated)
wxEND_EVENT_TABLE()

MoveRecordPanel::MoveRecordPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY),
      m_game(nullptr)
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    m_listCtrl = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                 wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES | wxLC_VRULES);

    m_listCtrl->AppendColumn(wxT("No."), wxLIST_FORMAT_CENTER, 40);
    m_listCtrl->AppendColumn(wxT("着法"), wxLIST_FORMAT_CENTER, 100);
    m_listCtrl->AppendColumn(wxT("注释"), wxLIST_FORMAT_LEFT, 200);

    sizer->Add(m_listCtrl, 1, wxEXPAND | wxALL, 2);
    SetSizer(sizer);
}

void MoveRecordPanel::SetGame(Game* game)
{
    m_game = game;
    RefreshList();
}

void MoveRecordPanel::RefreshList()
{
    m_listCtrl->DeleteAllItems();

    if (!m_game)
        return;

    int count = m_game->GetMoveCount();

    for (int i = 0; i < count; i++)
    {
        const MoveRecord* record = m_game->GetMove(i);
        if (!record)
            continue;

        bool isRed = (i % 2 == 0);
        int moveNum = (i / 2) + 1;

        long itemIndex;
        if (isRed)
        {
            itemIndex = m_listCtrl->InsertItem(i, wxString::Format(wxT("%d"), moveNum));
        }
        else
        {
            itemIndex = m_listCtrl->InsertItem(i, wxT(""));
        }

        m_listCtrl->SetItem(itemIndex, 1, wxString::FromUTF8(record->notation));

        if (!record->comment.empty())
        {
            m_listCtrl->SetItem(itemIndex, 2, wxString::FromUTF8(record->comment));
        }

        if (i == m_game->GetCurrentIndex())
        {
            m_listCtrl->SetItemBackgroundColour(itemIndex, wxColour(180, 220, 255));
        }
    }

    int currentIndex = m_game->GetCurrentIndex();
    if (currentIndex >= 0 && currentIndex < m_listCtrl->GetItemCount())
    {
        m_listCtrl->EnsureVisible(currentIndex);
    }
}

void MoveRecordPanel::GoToMove(int index)
{
    if (!m_game)
        return;

    m_game->SetCurrentIndex(index);
    RefreshList();
}

int MoveRecordPanel::GetSelectedIndex() const
{
    return m_listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
}

void MoveRecordPanel::OnItemSelected(wxListEvent& event)
{
    // Selection handled on activation (double click)
}

void MoveRecordPanel::OnItemActivated(wxListEvent& event)
{
    int itemIndex = event.GetIndex();
    if (itemIndex < 0 || !m_game)
        return;

    m_game->SetCurrentIndex(itemIndex);
    RefreshList();
}
