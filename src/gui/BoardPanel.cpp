#include "BoardPanel.h"
#include <wx/dcclient.h>
#include <wx/dcbuffer.h>
#include <wx/gdicmn.h>
#include <cmath>

wxBEGIN_EVENT_TABLE(BoardPanel, wxPanel)
    EVT_PAINT(BoardPanel::OnPaint)
    EVT_LEFT_DOWN(BoardPanel::OnLeftDown)
    EVT_SIZE(BoardPanel::OnSize)
    EVT_TIMER(wxID_ANY, BoardPanel::OnIllegalTimer)
wxEND_EVENT_TABLE()

BoardPanel::BoardPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
              wxFULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN),
      m_flipped(false),
      m_selectedRow(-1), m_selectedCol(-1),
      m_showIllegal(false),
      m_cellSize(56), m_marginX(20), m_marginY(20), m_pieceRadius(24)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    m_illegalTimer = new wxTimer(this, wxID_ANY);
}

BoardPanel::~BoardPanel()
{
    if (m_illegalTimer->IsRunning())
        m_illegalTimer->Stop();
    delete m_illegalTimer;
}

void BoardPanel::OnSize(wxSizeEvent& event)
{
    CalculateLayout();
    event.Skip();
}

void BoardPanel::CalculateLayout()
{
    wxSize sz = GetClientSize();
    if (sz.GetWidth() <= 0 || sz.GetHeight() <= 0) return;

    int cellW = sz.GetWidth() / (Board::COLS + 1);
    int cellH = sz.GetHeight() / (Board::ROWS + 1);
    m_cellSize = wxMin(cellW, cellH);
    if (m_cellSize < 20) m_cellSize = 20;

    int boardW = m_cellSize * (Board::COLS - 1);
    int boardH = m_cellSize * (Board::ROWS - 1);
    m_marginX = (sz.GetWidth() - boardW) / 2;
    m_marginY = (sz.GetHeight() - boardH) / 2;

    m_pieceRadius = m_cellSize / 2 - 4;
    if (m_pieceRadius < 8) m_pieceRadius = 8;

    Refresh();
}

void BoardPanel::ResetBoard()
{
    m_board.Reset();
    m_selectedRow = -1;
    m_selectedCol = -1;
    m_legalMoves.clear();
    m_showIllegal = false;
    Refresh();
}

void BoardPanel::FlipView()
{
    m_flipped = !m_flipped;
    Refresh();
}

wxPoint BoardPanel::BoardToScreen(int row, int col) const
{
    if (m_flipped)
    {
        row = Board::ROWS - 1 - row;
        col = Board::COLS - 1 - col;
    }

    int x = m_marginX + col * m_cellSize;
    int y = m_marginY + row * m_cellSize;
    return wxPoint(x, y);
}

bool BoardPanel::ScreenToBoard(int x, int y, int& row, int& col) const
{
    col = (int)round((double)(x - m_marginX) / m_cellSize);
    row = (int)round((double)(y - m_marginY) / m_cellSize);

    if (col < 0 || col >= Board::COLS || row < 0 || row >= Board::ROWS)
        return false;

    if (m_flipped)
    {
        row = Board::ROWS - 1 - row;
        col = Board::COLS - 1 - col;
    }

    return true;
}

void BoardPanel::OnPaint(wxPaintEvent& event)
{
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    if (m_cellSize <= 0)
        CalculateLayout();

    DrawBoard(dc);
    DrawMoveHints(dc);
    DrawPieces(dc);
    DrawSelection(dc);

    if (m_showIllegal)
        DrawIllegalText(dc);
}

void BoardPanel::DrawBoard(wxDC& dc)
{
    dc.SetBackground(wxBrush(wxColour(232, 200, 150)));
    dc.Clear();

    wxPen thinPen(wxColour(100, 70, 30), 1);
    wxPen thickPen(wxColour(80, 50, 20), 2);

    dc.SetPen(thickPen);
    int left = m_marginX;
    int right = m_marginX + (Board::COLS - 1) * m_cellSize;
    int top = m_marginY;
    int bottom = m_marginY + (Board::ROWS - 1) * m_cellSize;

    dc.DrawRectangle(left, top, right - left, bottom - top);

    dc.SetPen(thinPen);

    for (int r = 0; r < Board::ROWS; r++)
    {
        int y = m_marginY + r * m_cellSize;
        dc.DrawLine(left, y, right, y);
    }

    for (int c = 0; c < Board::COLS; c++)
    {
        int x = m_marginX + c * m_cellSize;
        if (c == 0 || c == Board::COLS - 1)
            dc.DrawLine(x, top, x, bottom);
        else
        {
            dc.DrawLine(x, top, x, m_marginY + 4 * m_cellSize);
            dc.DrawLine(x, m_marginY + 5 * m_cellSize, x, bottom);
        }
    }

    dc.SetPen(thickPen);
    wxPoint p1 = BoardToScreen(0, 3);
    wxPoint p2 = BoardToScreen(2, 5);
    wxPoint p3 = BoardToScreen(2, 3);
    wxPoint p4 = BoardToScreen(0, 5);
    dc.DrawLine(p1.x, p1.y, p2.x, p2.y);
    dc.DrawLine(p3.x, p3.y, p4.x, p4.y);

    p1 = BoardToScreen(7, 3);
    p2 = BoardToScreen(9, 5);
    p3 = BoardToScreen(9, 3);
    p4 = BoardToScreen(7, 5);
    dc.DrawLine(p1.x, p1.y, p2.x, p2.y);
    dc.DrawLine(p3.x, p3.y, p4.x, p4.y);

    wxPen riverPen(wxColour(150, 100, 50), 1, wxPENSTYLE_DOT);
    dc.SetPen(riverPen);
    int riverY = m_marginY + 4 * m_cellSize + m_cellSize / 2;
    dc.DrawLine(left + m_cellSize, riverY, right - m_cellSize, riverY);

    int fontSize = m_cellSize / 3;
    if (fontSize < 10) fontSize = 10;
    dc.SetFont(wxFont(fontSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_BOLD));
    dc.SetTextForeground(wxColour(120, 70, 30));

    int textY = riverY - fontSize / 2;
    int leftX = m_marginX + m_cellSize;
    int rightX = m_marginX + (Board::COLS - 2) * m_cellSize;

    // if (m_flipped)
    // {
    //     dc.DrawText(wxT("楚 河"), rightX, textY);
    //     dc.DrawText(wxT("漢 界"), leftX - fontSize * 4, textY);
    // }
    // else
    // {
        dc.DrawText(wxT("楚 河"), leftX, textY);
        dc.DrawText(wxT("漢 界"), rightX - fontSize * 4, textY);
    // }
}

void BoardPanel::DrawPieces(wxDC& dc)
{
    for (int r = 0; r < Board::ROWS; r++)
    {
        for (int c = 0; c < Board::COLS; c++)
        {
            Piece piece = m_board.GetPiece(r, c);
            if (!piece.IsEmpty())
                DrawPiece(dc, r, c, piece);
        }
    }
}

void BoardPanel::DrawPiece(wxDC& dc, int row, int col, const Piece& piece)
{
    wxPoint pos = BoardToScreen(row, col);

    wxColour bgColor = (piece.color == RED) ? wxColour(220, 180, 140) : wxColour(60, 60, 60);
    wxColour borderColor = (piece.color == RED) ? wxColour(180, 60, 60) : wxColour(20, 20, 20);
    wxColour textColor = (piece.color == RED) ? wxColour(200, 30, 30) : wxColour(230, 230, 230);

    wxPen borderPen(borderColor, 2);
    dc.SetPen(borderPen);
    dc.SetBrush(wxBrush(bgColor));
    dc.DrawCircle(pos.x, pos.y, m_pieceRadius);

    dc.SetPen(wxPen(borderColor, 1));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawCircle(pos.x, pos.y, m_pieceRadius - 3);

    const wchar_t* name = nullptr;
    if (piece.color == RED)
    {
        switch (piece.type)
        {
            case KING: name = wxT("帥"); break;
            case ADVISOR: name = wxT("仕"); break;
            case ELEPHANT: name = wxT("相"); break;
            case HORSE: name = wxT("馬"); break;
            case CHARIOT: name = wxT("車"); break;
            case CANNON: name = wxT("炮"); break;
            case PAWN: name = wxT("兵"); break;
            default: break;
        }
    }
    else
    {
        switch (piece.type)
        {
            case KING: name = wxT("將"); break;
            case ADVISOR: name = wxT("士"); break;
            case ELEPHANT: name = wxT("象"); break;
            case HORSE: name = wxT("馬"); break;
            case CHARIOT: name = wxT("車"); break;
            case CANNON: name = wxT("砲"); break;
            case PAWN: name = wxT("卒"); break;
            default: break;
        }
    }

    if (name)
    {
        int fontSize = m_cellSize * 4 / 10;
        if (fontSize < 10) fontSize = 10;
        dc.SetFont(wxFont(fontSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
        wxSize textSize = dc.GetTextExtent(name);
        dc.SetTextForeground(textColor);
        dc.DrawText(name, pos.x - textSize.GetWidth() / 2, pos.y - textSize.GetHeight() / 2);
    }
}

void BoardPanel::DrawSelection(wxDC& dc)
{
    if (m_selectedRow < 0 || m_selectedCol < 0)
        return;

    wxPoint pos = BoardToScreen(m_selectedRow, m_selectedCol);

    dc.SetPen(wxPen(wxColour(0, 180, 0), 3));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawCircle(pos.x, pos.y, m_pieceRadius + 2);
}

void BoardPanel::DrawMoveHints(wxDC& dc)
{
    if (m_selectedRow < 0 || m_selectedCol < 0)
        return;

    dc.SetPen(wxPen(wxColour(0, 150, 0), 2, wxPENSTYLE_SHORT_DASH));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    for (const auto& move : m_legalMoves)
    {
        wxPoint pos = BoardToScreen(move.toRow, move.toCol);
        int radius = m_pieceRadius - 6;
        if (radius < 4) radius = 4;
        dc.DrawCircle(pos.x, pos.y, radius);
    }
}

void BoardPanel::DrawIllegalText(wxDC& dc)
{
    int fontSize = m_cellSize * 5 / 10;
    if (fontSize < 14) fontSize = 14;
    dc.SetFont(wxFont(fontSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    dc.SetTextForeground(wxColour(255, 0, 0));

    wxString text = wxT("非法招数");
    wxSize textSize = dc.GetTextExtent(text);

    int x = m_marginX + ((Board::COLS - 1) * m_cellSize) / 2 - textSize.GetWidth() / 2;
    int y = m_marginY + 4 * m_cellSize + m_cellSize / 2 + m_cellSize / 3;

    dc.DrawText(text, x, y);
}

void BoardPanel::OnLeftDown(wxMouseEvent& event)
{
    int row, col;
    if (!ScreenToBoard(event.GetX(), event.GetY(), row, col))
    {
        m_selectedRow = -1;
        m_selectedCol = -1;
        m_legalMoves.clear();
        m_showIllegal = false;
        Refresh();
        return;
    }

    Piece targetPiece = m_board.GetPiece(row, col);

    if (m_selectedRow >= 0 && m_selectedCol >= 0)
    {
        ChessMove move(m_selectedRow, m_selectedCol, row, col);

        bool isLegal = false;
        for (const auto& lm : m_legalMoves)
        {
            if (lm == move)
            {
                isLegal = true;
                break;
            }
        }

        if (isLegal)
        {
            m_board.MakeMove(move);
            m_selectedRow = -1;
            m_selectedCol = -1;
            m_legalMoves.clear();
            m_showIllegal = false;
            Refresh();
            return;
        }

        if (!targetPiece.IsEmpty() && targetPiece.color == m_board.GetCurrentPlayer())
        {
            m_selectedRow = row;
            m_selectedCol = col;
            m_legalMoves.clear();

            std::vector<ChessMove> allLegal = m_board.GenerateLegalMoves();
            for (const auto& lm : allLegal)
            {
                if (lm.fromRow == row && lm.fromCol == col)
                    m_legalMoves.push_back(lm);
            }

            m_showIllegal = false;
            Refresh();
            return;
        }

        m_selectedRow = -1;
        m_selectedCol = -1;
        m_legalMoves.clear();
        m_showIllegal = true;
        m_illegalTimer->Stop();
        m_illegalTimer->Start(1500, wxTIMER_ONE_SHOT);
        Refresh();
        return;
    }

    if (!targetPiece.IsEmpty() && targetPiece.color == m_board.GetCurrentPlayer())
    {
        m_selectedRow = row;
        m_selectedCol = col;
        m_legalMoves.clear();

        std::vector<ChessMove> allLegal = m_board.GenerateLegalMoves();
        for (const auto& lm : allLegal)
        {
            if (lm.fromRow == row && lm.fromCol == col)
                m_legalMoves.push_back(lm);
        }

        m_showIllegal = false;
        Refresh();
        return;
    }
}

void BoardPanel::OnIllegalTimer(wxTimerEvent& event)
{
    m_showIllegal = false;
    Refresh();
}
