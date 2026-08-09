#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <vector>

enum PieceType
{
    KING = 0,
    ADVISOR = 1,
    ELEPHANT = 2,
    HORSE = 3,
    CHARIOT = 4,
    CANNON = 5,
    PAWN = 6,
    NONE = 7
};

enum PieceColor
{
    RED = 0,
    BLACK = 1,
    COLORS_COUNT = 2
};

struct Piece
{
    PieceType type;
    PieceColor color;

    Piece() : type(NONE), color(RED) {}
    Piece(PieceType t, PieceColor c) : type(t), color(c) {}

    bool IsEmpty() const { return type == NONE; }
};

struct ChessMove
{
    int fromRow;
    int fromCol;
    int toRow;
    int toCol;

    ChessMove() : fromRow(0), fromCol(0), toRow(0), toCol(0) {}
    ChessMove(int fr, int fc, int tr, int tc) : fromRow(fr), fromCol(fc), toRow(tr), toCol(tc) {}

    bool operator==(const ChessMove& other) const
    {
        return fromRow == other.fromRow && fromCol == other.fromCol &&
               toRow == other.toRow && toCol == other.toCol;
    }
};

class Board
{
public:
    static const int ROWS = 10;
    static const int COLS = 9;

    Board();

    void Reset();
    bool FromFEN(const std::string& fen);
    std::string ToFEN() const;

    Piece GetPiece(int row, int col) const;
    PieceColor GetCurrentPlayer() const { return m_currentPlayer; }

    bool IsValidPosition(int row, int col) const;
    bool IsEmpty(int row, int col) const;
    bool IsAlly(int row, int col, PieceColor color) const;
    bool IsEnemy(int row, int col, PieceColor color) const;

    std::vector<ChessMove> GenerateMoves(PieceColor color) const;
    std::vector<ChessMove> GenerateLegalMoves(PieceColor color);
    std::vector<ChessMove> GenerateLegalMoves();

    bool IsLegalMove(const ChessMove& move);
    bool MakeMove(const ChessMove& move);
    void UndoMove();

    bool IsInCheck(PieceColor color) const;
    bool IsCheckmate(PieceColor color);
    bool IsStalemate(PieceColor color);
    bool KingsFaceEachOther() const;

    static bool InPalace(int row, int col, PieceColor color);
    static bool IsAcrossRiver(int row, PieceColor color);

private:
    Piece m_board[ROWS][COLS];
    PieceColor m_currentPlayer;

    struct UndoInfo
    {
        ChessMove move;
        Piece captured;
        PieceColor prevPlayer;
    };
    std::vector<UndoInfo> m_history;

    int FindKingRow(PieceColor color) const;
    int FindKingCol(PieceColor color) const;

    void AddSlidingMoves(std::vector<ChessMove>& moves, int row, int col, int dRow, int dCol) const;
    void AddKnightMoves(std::vector<ChessMove>& moves, int row, int col) const;
};

#endif // BOARD_H
