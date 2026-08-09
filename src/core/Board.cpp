#include "Board.h"
#include <sstream>
#include <algorithm>

Board::Board()
{
    Reset();
}

void Board::Reset()
{
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            m_board[r][c] = Piece();

    m_board[0][0] = Piece(CHARIOT, BLACK);
    m_board[0][1] = Piece(HORSE, BLACK);
    m_board[0][2] = Piece(ELEPHANT, BLACK);
    m_board[0][3] = Piece(ADVISOR, BLACK);
    m_board[0][4] = Piece(KING, BLACK);
    m_board[0][5] = Piece(ADVISOR, BLACK);
    m_board[0][6] = Piece(ELEPHANT, BLACK);
    m_board[0][7] = Piece(HORSE, BLACK);
    m_board[0][8] = Piece(CHARIOT, BLACK);
    m_board[2][1] = Piece(CANNON, BLACK);
    m_board[2][7] = Piece(CANNON, BLACK);
    m_board[3][0] = Piece(PAWN, BLACK);
    m_board[3][2] = Piece(PAWN, BLACK);
    m_board[3][4] = Piece(PAWN, BLACK);
    m_board[3][6] = Piece(PAWN, BLACK);
    m_board[3][8] = Piece(PAWN, BLACK);

    m_board[9][0] = Piece(CHARIOT, RED);
    m_board[9][1] = Piece(HORSE, RED);
    m_board[9][2] = Piece(ELEPHANT, RED);
    m_board[9][3] = Piece(ADVISOR, RED);
    m_board[9][4] = Piece(KING, RED);
    m_board[9][5] = Piece(ADVISOR, RED);
    m_board[9][6] = Piece(ELEPHANT, RED);
    m_board[9][7] = Piece(HORSE, RED);
    m_board[9][8] = Piece(CHARIOT, RED);
    m_board[7][1] = Piece(CANNON, RED);
    m_board[7][7] = Piece(CANNON, RED);
    m_board[6][0] = Piece(PAWN, RED);
    m_board[6][2] = Piece(PAWN, RED);
    m_board[6][4] = Piece(PAWN, RED);
    m_board[6][6] = Piece(PAWN, RED);
    m_board[6][8] = Piece(PAWN, RED);

    m_currentPlayer = RED;
    m_history.clear();
}

/**
 * 从FEN字符串初始化棋盘状态
 * @param fen FEN字符串，描述棋盘的初始状态
 * @return 初始化成功返回true，否则返回false
 */
bool Board::FromFEN(const std::string& fen)
{
    // 清空棋盘，所有位置初始化为空棋子
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            m_board[r][c] = Piece();

    std::istringstream iss(fen);
    std::string placement;
    iss >> placement;

    int row = 0, col = 0;
    for (char ch : placement)
    {
        if (ch == '/')
        {
            row++;
            col = 0;
        }
        else if (ch >= '1' && ch <= '9')
        {
            col += (ch - '0');
        }
        else
        {
            PieceColor color = (ch >= 'a' && ch <= 'z') ? BLACK : RED;
            PieceType type;
            switch (ch)
            {
                case 'K': case 'k': type = KING; break;
                case 'A': case 'a': type = ADVISOR; break;
                case 'B': case 'b': type = ELEPHANT; break;
                case 'N': case 'n': type = HORSE; break;
                case 'R': case 'r': type = CHARIOT; break;
                case 'C': case 'c': type = CANNON; break;
                case 'P': case 'p': type = PAWN; break;
                default: return false;
            }
            if (row < ROWS && col < COLS)
                m_board[row][col] = Piece(type, color);
            col++;
        }
    }

    std::string turn;
    iss >> turn;
    m_currentPlayer = (turn == "b") ? BLACK : RED;

    m_history.clear();
    return true;
}

std::string Board::ToFEN() const
{
    std::string fen;
    for (int r = 0; r < ROWS; r++)
    {
        int empty = 0;
        for (int c = 0; c < COLS; c++)
        {
            if (m_board[r][c].IsEmpty())
            {
                empty++;
            }
            else
            {
                if (empty > 0)
                {
                    fen += ('0' + empty);
                    empty = 0;
                }
                char ch;
                switch (m_board[r][c].type)
                {
                    case KING: ch = 'k'; break;
                    case ADVISOR: ch = 'a'; break;
                    case ELEPHANT: ch = 'b'; break;
                    case HORSE: ch = 'n'; break;
                    case CHARIOT: ch = 'r'; break;
                    case CANNON: ch = 'c'; break;
                    case PAWN: ch = 'p'; break;
                    default: ch = '?';
                }
                if (m_board[r][c].color == RED)
                    ch = toupper(ch);
                fen += ch;
            }
        }
        if (empty > 0)
            fen += ('0' + empty);
        if (r < ROWS - 1)
            fen += '/';
    }
    fen += (m_currentPlayer == RED) ? " w" : " b";
    fen += " - - 0 1";
    return fen;
}

Piece Board::GetPiece(int row, int col) const
{
    if (!IsValidPosition(row, col))
        return Piece();
    return m_board[row][col];
}

bool Board::IsValidPosition(int row, int col) const
{
    return row >= 0 && row < ROWS && col >= 0 && col < COLS;
}

bool Board::IsEmpty(int row, int col) const
{
    return IsValidPosition(row, col) && m_board[row][col].IsEmpty();
}

bool Board::IsAlly(int row, int col, PieceColor color) const
{
    return IsValidPosition(row, col) && !m_board[row][col].IsEmpty() &&
           m_board[row][col].color == color;
}

bool Board::IsEnemy(int row, int col, PieceColor color) const
{
    return IsValidPosition(row, col) && !m_board[row][col].IsEmpty() &&
           m_board[row][col].color != color;
}

bool Board::InPalace(int row, int col, PieceColor color)
{
    if (col < 3 || col > 5) return false;
    if (color == RED) return row >= 7 && row <= 9;
    return row >= 0 && row <= 2;
}

bool Board::IsAcrossRiver(int row, PieceColor color)
{
    if (color == RED) return row <= 4;
    return row >= 5;
}

int Board::FindKingRow(PieceColor color) const
{
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            if (m_board[r][c].type == KING && m_board[r][c].color == color)
                return r;
    return -1;
}

int Board::FindKingCol(PieceColor color) const
{
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            if (m_board[r][c].type == KING && m_board[r][c].color == color)
                return c;
    return -1;
}

bool Board::KingsFaceEachOther() const
{
    int redKingRow = FindKingRow(RED);
    int redKingCol = FindKingCol(RED);
    int blackKingRow = FindKingRow(BLACK);
    int blackKingCol = FindKingCol(BLACK);

    if (redKingCol != blackKingCol) return false;

    for (int r = blackKingRow + 1; r < redKingRow; r++)
        if (!m_board[r][redKingCol].IsEmpty()) return false;

    return true;
}

void Board::AddSlidingMoves(std::vector<ChessMove>& moves, int row, int col, int dRow, int dCol) const
{
    int r = row + dRow;
    int c = col + dCol;
    while (IsValidPosition(r, c))
    {
        if (IsEmpty(r, c))
        {
            moves.push_back(ChessMove(row, col, r, c));
        }
        else
        {
            if (IsEnemy(r, c, m_board[row][col].color))
                moves.push_back(ChessMove(row, col, r, c));
            break;
        }
        r += dRow;
        c += dCol;
    }
}

void Board::AddKnightMoves(std::vector<ChessMove>& moves, int row, int col) const
{
    static const int horseOffsets[8][2] = {{-2,-1},{-2,1},{-1,-2},{-1,2},{1,-2},{1,2},{2,-1},{2,1}};
    static const int legOffsets[8][2] = {{-1,0},{-1,0},{0,-1},{0,1},{0,-1},{0,1},{1,0},{1,0}};

    for (int i = 0; i < 8; i++)
    {
        int nr = row + horseOffsets[i][0];
        int nc = col + horseOffsets[i][1];
        int lr = row + legOffsets[i][0];
        int lc = col + legOffsets[i][1];

        if (IsValidPosition(nr, nc) && IsEmpty(lr, lc) && !IsAlly(nr, nc, m_board[row][col].color))
            moves.push_back(ChessMove(row, col, nr, nc));
    }
}

std::vector<ChessMove> Board::GenerateMoves(PieceColor color) const
{
    std::vector<ChessMove> moves;

    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            if (m_board[r][c].IsEmpty() || m_board[r][c].color != color)
                continue;

            switch (m_board[r][c].type)
            {
                case KING:
                {
                    static const int offsets[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
                    for (auto& off : offsets)
                    {
                        int nr = r + off[0], nc = c + off[1];
                        if (InPalace(nr, nc, color) && !IsAlly(nr, nc, color))
                            moves.push_back(ChessMove(r, c, nr, nc));
                    }
                    break;
                }
                case ADVISOR:
                {
                    static const int offsets[4][2] = {{-1,-1},{-1,1},{1,-1},{1,1}};
                    for (auto& off : offsets)
                    {
                        int nr = r + off[0], nc = c + off[1];
                        if (InPalace(nr, nc, color) && !IsAlly(nr, nc, color))
                            moves.push_back(ChessMove(r, c, nr, nc));
                    }
                    break;
                }
                case ELEPHANT:
                {
                    static const int offsets[4][2] = {{-2,-2},{-2,2},{2,-2},{2,2}};
                    static const int eyeOffsets[4][2] = {{-1,-1},{-1,1},{1,-1},{1,1}};
                    for (int i = 0; i < 4; i++)
                    {
                        int nr = r + offsets[i][0], nc = c + offsets[i][1];
                        int er = r + eyeOffsets[i][0], ec = c + eyeOffsets[i][1];
                        if (IsValidPosition(nr, nc) && IsEmpty(er, ec) && !IsAcrossRiver(nr, color) && !IsAlly(nr, nc, color))
                            moves.push_back(ChessMove(r, c, nr, nc));
                    }
                    break;
                }
                case HORSE:
                    AddKnightMoves(moves, r, c);
                    break;
                case CHARIOT:
                    AddSlidingMoves(moves, r, c, -1, 0);
                    AddSlidingMoves(moves, r, c, 1, 0);
                    AddSlidingMoves(moves, r, c, 0, -1);
                    AddSlidingMoves(moves, r, c, 0, 1);
                    break;
                case CANNON:
                {
                    static const int dirs[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
                    for (auto& d : dirs)
                    {
                        int nr = r + d[0], nc = c + d[1];
                        while (IsValidPosition(nr, nc) && IsEmpty(nr, nc))
                        {
                            moves.push_back(ChessMove(r, c, nr, nc));
                            nr += d[0];
                            nc += d[1];
                        }
                        if (IsValidPosition(nr, nc))
                        {
                            nr += d[0];
                            nc += d[1];
                            while (IsValidPosition(nr, nc))
                            {
                                if (!IsEmpty(nr, nc))
                                {
                                    if (IsEnemy(nr, nc, color))
                                        moves.push_back(ChessMove(r, c, nr, nc));
                                    break;
                                }
                                nr += d[0];
                                nc += d[1];
                            }
                        }
                    }
                    break;
                }
                case PAWN:
                {
                    int forward = (color == RED) ? -1 : 1;
                    int nr = r + forward;
                    if (IsValidPosition(nr, c) && !IsAlly(nr, c, color))
                        moves.push_back(ChessMove(r, c, nr, c));
                    if (IsAcrossRiver(r, color))
                    {
                        if (IsValidPosition(r, c - 1) && !IsAlly(r, c - 1, color))
                            moves.push_back(ChessMove(r, c, r, c - 1));
                        if (IsValidPosition(r, c + 1) && !IsAlly(r, c + 1, color))
                            moves.push_back(ChessMove(r, c, r, c + 1));
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }

    return moves;
}

bool Board::IsInCheck(PieceColor color) const
{
    int kingRow = FindKingRow(color);
    int kingCol = FindKingCol(color);
    if (kingRow < 0 || kingCol < 0) return false;

    PieceColor opponent = (color == RED) ? BLACK : RED;
    std::vector<ChessMove> opponentMoves = GenerateMoves(opponent);

    for (const auto& move : opponentMoves)
        if (move.toRow == kingRow && move.toCol == kingCol)
            return true;

    return KingsFaceEachOther();
}

bool Board::IsLegalMove(const ChessMove& move)
{
    Piece piece = GetPiece(move.fromRow, move.fromCol);
    if (piece.IsEmpty() || piece.color != m_currentPlayer)
        return false;

    std::vector<ChessMove> legalMoves = GenerateLegalMoves();
    return std::find(legalMoves.begin(), legalMoves.end(), move) != legalMoves.end();
}

std::vector<ChessMove> Board::GenerateLegalMoves(PieceColor color)
{
    std::vector<ChessMove> pseudoMoves = GenerateMoves(color);
    std::vector<ChessMove> legalMoves;

    for (const auto& move : pseudoMoves)
    {
        Piece savedFrom = m_board[move.fromRow][move.fromCol];
        Piece savedTo = m_board[move.toRow][move.toCol];

        m_board[move.toRow][move.toCol] = savedFrom;
        m_board[move.fromRow][move.fromCol] = Piece();

        if (!IsInCheck(color) && !KingsFaceEachOther())
            legalMoves.push_back(move);

        m_board[move.fromRow][move.fromCol] = savedFrom;
        m_board[move.toRow][move.toCol] = savedTo;
    }

    return legalMoves;
}

std::vector<ChessMove> Board::GenerateLegalMoves()
{
    return GenerateLegalMoves(m_currentPlayer);
}

bool Board::MakeMove(const ChessMove& move)
{
    if (!IsLegalMove(move))
        return false;

    UndoInfo info;
    info.move = move;
    info.captured = m_board[move.toRow][move.toCol];
    info.prevPlayer = m_currentPlayer;

    m_board[move.toRow][move.toCol] = m_board[move.fromRow][move.fromCol];
    m_board[move.fromRow][move.fromCol] = Piece();

    m_currentPlayer = (m_currentPlayer == RED) ? BLACK : RED;
    m_history.push_back(info);

    return true;
}

void Board::UndoMove()
{
    if (m_history.empty()) return;

    UndoInfo info = m_history.back();
    m_history.pop_back();

    m_board[info.move.fromRow][info.move.fromCol] = m_board[info.move.toRow][info.move.toCol];
    m_board[info.move.toRow][info.move.toCol] = info.captured;

    m_currentPlayer = info.prevPlayer;
}

bool Board::IsCheckmate(PieceColor color)
{
    return IsInCheck(color) && GenerateLegalMoves(color).empty();
}

bool Board::IsStalemate(PieceColor color)
{
    return !IsInCheck(color) && GenerateLegalMoves(color).empty();
}
