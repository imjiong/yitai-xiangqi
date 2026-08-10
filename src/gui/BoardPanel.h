#ifndef BOARDPANEL_H
#define BOARDPANEL_H

#include <wx/wx.h>
#include "../core/Board.h"
#include "../core/Game.h"

class BoardPanel : public wxPanel
{
public:
    BoardPanel(wxWindow* parent);
    ~BoardPanel();

    void ResetBoard();
    void FlipView();
    bool IsFlipped() const { return m_flipped; }

    void SetGame(Game* game);
    Game* GetGame() { return m_game; }

    void SyncWithGame();

private:
    void OnPaint(wxPaintEvent& event);
    void OnLeftDown(wxMouseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnIllegalTimer(wxTimerEvent& event);

    void CalculateLayout();
    void DrawBoard(wxDC& dc);
    void DrawPieces(wxDC& dc);
    void DrawPiece(wxDC& dc, int row, int col, const Piece& piece);
    void DrawSelection(wxDC& dc);
    void DrawMoveHints(wxDC& dc);
    void DrawIllegalText(wxDC& dc);

    wxPoint BoardToScreen(int row, int col) const;
    bool ScreenToBoard(int x, int y, int& row, int& col) const;

    Board m_board;
    Game* m_game;
    bool m_flipped;

    int m_selectedRow;
    int m_selectedCol;
    std::vector<ChessMove> m_legalMoves;

    bool m_showIllegal;
    wxTimer* m_illegalTimer;

    int m_cellSize;
    int m_marginX;
    int m_marginY;
    int m_pieceRadius;

    wxDECLARE_EVENT_TABLE();
};

#endif // BOARDPANEL_H
