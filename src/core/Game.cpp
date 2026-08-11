#include "Game.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <wx/string.h>
#include <wx/encconv.h>

static std::string WStringToUTF8(const std::wstring& ws)
{
    wxString wx(ws);
    return std::string(wx.utf8_str().data());
}

static std::string UTF8ToGBK(const std::string& utf8)
{
    if (utf8.empty())
        return "";
    wxString wx(wxString::FromUTF8(utf8.c_str()));
    if (wx.IsEmpty())
        return utf8;
    wxCharBuffer buf = wxCSConv(wxFONTENCODING_CP936).cWX2MB(wx.wc_str());
    if (!buf)
        return utf8;
    return std::string(buf.data(), buf.length());
}

static std::string GBKToUTF8(const std::string& gbk)
{
    if (gbk.empty())
        return "";
    wxWCharBuffer wbuf = wxCSConv(wxFONTENCODING_CP936).cMB2WC(
        gbk.c_str(), gbk.size(), nullptr);
    if (!wbuf)
        return gbk;
    wxString wx(wbuf);
    if (wx.IsEmpty())
        return gbk;
    return std::string(wx.utf8_str().data());
}

static std::vector<std::string> UTF8Chars(const std::string& s)
{
    std::vector<std::string> chars;
    for (size_t i = 0; i < s.size();)
    {
        unsigned char c = static_cast<unsigned char>(s[i]);
        size_t len = 1;
        if (c >= 0xF0) len = 4;
        else if (c >= 0xE0) len = 3;
        else if (c >= 0xC0) len = 2;
        chars.push_back(s.substr(i, len));
        i += len;
    }
    return chars;
}

static int ChineseNumToIndex(const std::string& ch)
{
    static const struct { const char* s; int v; } map[] = {
        {"一", 0}, {"二", 1}, {"三", 2}, {"四", 3}, {"五", 4},
        {"六", 5}, {"七", 6}, {"八", 7}, {"九", 8},
        {"１", 0}, {"２", 1}, {"３", 2}, {"４", 3}, {"５", 4},
        {"６", 5}, {"７", 6}, {"８", 7}, {"９", 8},
    };
    for (auto& m : map)
        if (ch == m.s) return m.v;
    if (ch.size() == 1 && ch[0] >= '1' && ch[0] <= '9')
        return ch[0] - '1';
    return -1;
}

static std::pair<PieceType, PieceColor> CharToPiece(const std::string& ch)
{
    if (ch == "帅" || ch == "帥") return {KING, RED};
    if (ch == "仕") return {ADVISOR, RED};
    if (ch == "相") return {ELEPHANT, RED};
    if (ch == "马" || ch == "馬" || ch == "傌") return {HORSE, RED};
    if (ch == "车" || ch == "車" || ch == "俥") return {CHARIOT, RED};
    if (ch == "炮" || ch == "砲" || ch == "包") return {CANNON, RED};
    if (ch == "兵") return {PAWN, RED};
    if (ch == "将" || ch == "將") return {KING, BLACK};
    if (ch == "士") return {ADVISOR, BLACK};
    if (ch == "象") return {ELEPHANT, BLACK};
    if (ch == "马" || ch == "馬") return {HORSE, BLACK};
    if (ch == "车" || ch == "車") return {CHARIOT, BLACK};
    if (ch == "炮" || ch == "砲" || ch == "包") return {CANNON, BLACK};
    if (ch == "卒") return {PAWN, BLACK};
    return {NONE, RED};
}

static bool IsDiagonalPiece(PieceType t)
{
    return t == HORSE || t == ADVISOR || t == ELEPHANT;
}

static ChessMove ParseChineseNotation(const Board& board, PieceColor side, const std::string& notation)
{
    std::vector<std::string> chs = UTF8Chars(notation);
    if (chs.size() < 4)
        return ChessMove();

    size_t idx = 0;
    std::string prefix;
    if (chs[0] == "前" || chs[0] == "后" || chs[0] == "中")
    {
        prefix = chs[0];
        idx = 1;
    }
    if (chs.size() - idx < 4)
        return ChessMove();

    auto [type, color] = CharToPiece(chs[idx]);
    if (type == NONE || color != side)
        return ChessMove();
    idx++;

    std::string fileChar = chs[idx];
    std::string dirChar = chs[idx + 1];
    std::string targetChar = chs[idx + 2];

    bool isHorizontal = false;
    bool isAdvance = false;
    if (dirChar == "进")
        isAdvance = true;
    else if (dirChar == "退")
        isAdvance = false;
    else if (dirChar == "平")
        isHorizontal = true;
    else
        return ChessMove();

    int fromFile = ChineseNumToIndex(fileChar);
    if (fromFile < 0)
        return ChessMove();

    int targetNum = ChineseNumToIndex(targetChar);
    if (targetNum < 0)
        return ChessMove();

    std::vector<ChessMove> legal = board.GenerateLegalMoves(side);
    ChessMove bestMatch;
    int found = 0;

    for (const auto& mv : legal)
    {
        Piece piece = board.GetPiece(mv.fromRow, mv.fromCol);
        if (piece.type != type || piece.color != side)
            continue;

        int mvFromFile = (side == RED) ? (Board::COLS - 1 - mv.fromCol) : mv.fromCol;
        if (prefix.empty() && mvFromFile != fromFile)
            continue;

        int dRow = mv.toRow - mv.fromRow;
        int dCol = mv.toCol - mv.fromCol;

        bool mvIsAdvance, mvIsHorizontal;
        if (side == RED)
            mvIsAdvance = (dRow < 0);
        else
            mvIsAdvance = (dRow > 0);
        mvIsHorizontal = (dRow == 0 && dCol != 0);

        if (isHorizontal && !mvIsHorizontal)
            continue;
        if (!isHorizontal && mvIsHorizontal)
            continue;
        if (!isHorizontal && mvIsAdvance != isAdvance)
            continue;

        bool targetMatch = false;
        if (IsDiagonalPiece(type))
        {
            int mvToFile = (side == RED) ? (Board::COLS - 1 - mv.toCol) : mv.toCol;
            targetMatch = (mvToFile == targetNum);
        }
        else if (isHorizontal)
        {
            int mvToFile = (side == RED) ? (Board::COLS - 1 - mv.toCol) : mv.toCol;
            targetMatch = (mvToFile == targetNum);
        }
        else
        {
            int steps = (side == RED) ? -dRow : dRow;
            targetMatch = (steps == targetNum + 1);
        }

        if (!targetMatch)
            continue;

        bestMatch = mv;
        found++;
    }

    if (found == 1)
        return bestMatch;

    if (found > 1 && !prefix.empty())
    {
        std::vector<ChessMove> filtered;
        for (const auto& mv : legal)
        {
            Piece piece = board.GetPiece(mv.fromRow, mv.fromCol);
            if (piece.type != type || piece.color != side)
                continue;

            int mvFromFile = (side == RED) ? (Board::COLS - 1 - mv.fromCol) : mv.fromCol;
            if (mvFromFile != fromFile)
                continue;

            int dRow = mv.toRow - mv.fromRow;
            int dCol = mv.toCol - mv.fromCol;

            bool mvIsAdvance, mvIsHorizontal;
            if (side == RED)
                mvIsAdvance = (dRow < 0);
            else
                mvIsAdvance = (dRow > 0);
            mvIsHorizontal = (dRow == 0 && dCol != 0);

            if (isHorizontal && !mvIsHorizontal) continue;
            if (!isHorizontal && mvIsHorizontal) continue;
            if (!isHorizontal && mvIsAdvance != isAdvance) continue;

            bool targetMatch = false;
            if (IsDiagonalPiece(type))
            {
                int mvToFile = (side == RED) ? (Board::COLS - 1 - mv.toCol) : mv.toCol;
                targetMatch = (mvToFile == targetNum);
            }
            else if (isHorizontal)
            {
                int mvToFile = (side == RED) ? (Board::COLS - 1 - mv.toCol) : mv.toCol;
                targetMatch = (mvToFile == targetNum);
            }
            else
            {
                int steps = (side == RED) ? -dRow : dRow;
                targetMatch = (steps == targetNum + 1);
            }
            if (!targetMatch) continue;

            filtered.push_back(mv);
        }

        if (filtered.size() >= 2)
        {
            auto sorted = filtered;
            std::sort(sorted.begin(), sorted.end(),
                [side](const ChessMove& a, const ChessMove& b)
                {
                    return (side == RED) ? (a.fromRow > b.fromRow) : (a.fromRow < b.fromRow);
                });

            if (prefix == "前")
                return sorted[0];
            else if (prefix == "中")
                return sorted.size() >= 3 ? sorted[1] : sorted[0];
            else if (prefix == "后")
                return sorted.back();
        }
    }

    return ChessMove();
}

Game::Game()
{
    NewGame();
}

void Game::NewGame()
{
    m_board.Reset();
    m_moves.clear();
    m_currentIndex = -1;
    m_info = GameInfo();
    m_tags.clear();
}

void Game::Clear()
{
    NewGame();
}

bool Game::AddMove(const ChessMove& move, const std::string& comment)
{
    if (m_currentIndex + 1 < static_cast<int>(m_moves.size()))
        m_moves.erase(m_moves.begin() + m_currentIndex + 1, m_moves.end());

    Piece piece = m_board.GetPiece(move.fromRow, move.fromCol);

    MoveRecord record(move);
    record.comment = comment;
    record.notation = MoveToChinese(move, piece.type, piece.color, m_board);
    m_moves.push_back(record);

    m_board.MakeMove(move);
    m_currentIndex = static_cast<int>(m_moves.size()) - 1;

    return true;
}

bool Game::InsertMove(int index, const ChessMove& move, const std::string& comment)
{
    if (index < 0 || index > static_cast<int>(m_moves.size()))
        return false;

    MoveRecord record(move);
    record.comment = comment;
    record.notation = MoveToICCS(move);
    m_moves.insert(m_moves.begin() + index, record);

    if (index <= m_currentIndex)
        m_currentIndex++;

    RebuildBoardToIndex(m_currentIndex);
    return true;
}

bool Game::RemoveMove(int index)
{
    if (index < 0 || index >= static_cast<int>(m_moves.size()))
        return false;

    m_moves.erase(m_moves.begin() + index);

    if (m_currentIndex >= static_cast<int>(m_moves.size()))
        m_currentIndex = static_cast<int>(m_moves.size()) - 1;

    RebuildBoardToIndex(m_currentIndex);
    return true;
}

void Game::SetComment(int index, const std::string& comment)
{
    if (index >= 0 && index < static_cast<int>(m_moves.size()))
        m_moves[index].comment = comment;
}

MoveRecord* Game::GetMove(int index)
{
    if (index < 0 || index >= static_cast<int>(m_moves.size()))
        return nullptr;
    return &m_moves[index];
}

const MoveRecord* Game::GetMove(int index) const
{
    if (index < 0 || index >= static_cast<int>(m_moves.size()))
        return nullptr;
    return &m_moves[index];
}

void Game::SetCurrentIndex(int index)
{
    if (index < -1 || index >= static_cast<int>(m_moves.size()))
        return;

    m_currentIndex = index;
    RebuildBoardToIndex(index);
}

void Game::GoToStart()
{
    m_currentIndex = -1;
    m_board.Reset();
}

void Game::GoToEnd()
{
    if (m_moves.empty())
        return;

    m_currentIndex = static_cast<int>(m_moves.size()) - 1;
    RebuildBoardToIndex(m_currentIndex);
}

void Game::GoBack()
{
    if (m_currentIndex >= 0)
    {
        m_currentIndex--;
        RebuildBoardToIndex(m_currentIndex);
    }
}

void Game::GoForward()
{
    if (m_currentIndex + 1 < static_cast<int>(m_moves.size()))
    {
        m_currentIndex++;
        RebuildBoardToIndex(m_currentIndex);
    }
}

void Game::RebuildBoardToIndex(int index)
{
    m_board.Reset();

    for (int i = 0; i <= index && i < static_cast<int>(m_moves.size()); i++)
        m_board.MakeMove(m_moves[i].move);
}

static int CountSameTypeOnFile(const Board& board, PieceType type, PieceColor color, int col, int excludeRow)
{
    int count = 0;
    for (int r = 0; r < Board::ROWS; r++)
    {
        if (r == excludeRow) continue;
        Piece p = board.GetPiece(r, col);
        if (!p.IsEmpty() && p.type == type && p.color == color)
            count++;
    }
    return count;
}

static std::wstring GetDisambigPrefix(const Board& board, const ChessMove& move, PieceType type, PieceColor color)
{
    int sameOnFile = CountSameTypeOnFile(board, type, color, move.fromCol, move.fromRow);
    if (sameOnFile == 0)
        return L"";

    struct SamePiece { int row; int col; };
    std::vector<SamePiece> samePieces;
    samePieces.push_back({move.fromRow, move.fromCol});
    for (int r = 0; r < Board::ROWS; r++)
    {
        if (r == move.fromRow) continue;
        Piece p = board.GetPiece(r, move.fromCol);
        if (!p.IsEmpty() && p.type == type && p.color == color)
            samePieces.push_back({r, move.fromCol});
    }

    if (samePieces.size() == 2)
    {
        bool isFirst = (color == RED) ? (samePieces[0].row < samePieces[1].row)
                                      : (samePieces[0].row > samePieces[1].row);
        return isFirst ? L"前" : L"后";
    }

    std::sort(samePieces.begin(), samePieces.end(),
        [color](const SamePiece& a, const SamePiece& b)
        {
            return (color == RED) ? (a.row < b.row) : (a.row > b.row);
        });

    static const wchar_t* threePrefixes[] = { L"前", L"中", L"后" };
    for (size_t i = 0; i < samePieces.size(); i++)
    {
        if (samePieces[i].row == move.fromRow)
            return threePrefixes[i];
    }
    return L"";
}

std::string Game::MoveToChinese(const ChessMove& move, PieceType type, PieceColor color, const Board& board)
{
    static const wchar_t* redNums[] = { L"一", L"二", L"三", L"四", L"五", L"六", L"七", L"八", L"九" };
    static const wchar_t* blackNums[] = { L"１", L"２", L"３", L"４", L"５", L"６", L"７", L"８", L"９" };
    static const wchar_t* pieceChars[] = { L"帅", L"仕", L"相", L"马", L"车", L"炮", L"兵" };
    static const wchar_t* pieceCharsB[] = { L"将", L"士", L"象", L"马", L"车", L"炮", L"卒" };

    const wchar_t** nums = (color == RED) ? redNums : blackNums;
    const wchar_t** pieces = (color == RED) ? pieceChars : pieceCharsB;

    int fromFile = (color == RED) ? (Board::COLS - 1 - move.fromCol) : move.fromCol;
    int toFile = (color == RED) ? (Board::COLS - 1 - move.toCol) : move.toCol;

    int dRow = move.toRow - move.fromRow;
    int dCol = move.toCol - move.fromCol;

    bool isAdvance, isHorizontal;
    if (color == RED)
    {
        isAdvance = (dRow < 0);
    }
    else
    {
        isAdvance = (dRow > 0);
    }
    isHorizontal = (dRow == 0 && dCol != 0);

    wchar_t dirChar;
    std::wstring targetStr;

    if (isHorizontal)
    {
        dirChar = L'平';
        targetStr = nums[toFile];
    }
    else if (isAdvance)
    {
        dirChar = L'进';
        if (type == HORSE || type == ADVISOR || type == ELEPHANT)
        {
            targetStr = nums[toFile];
        }
        else
        {
            int steps = (color == RED) ? (-dRow) : dRow;
            targetStr = nums[steps - 1];
        }
    }
    else
    {
        dirChar = L'退';
        if (type == HORSE || type == ADVISOR || type == ELEPHANT)
        {
            targetStr = nums[toFile];
        }
        else
        {
            int steps = (color == RED) ? dRow : (-dRow);
            targetStr = nums[steps - 1];
        }
    }

    std::wstring disambig = GetDisambigPrefix(board, move, type, color);

    std::wstring result;
    if (!disambig.empty())
    {
        result += disambig;
        result += pieces[type];
    }
    else
    {
        result += pieces[type];
        result += nums[fromFile];
    }
    result += dirChar;
    result += targetStr;

    return WStringToUTF8(result);
}

std::string Game::MoveToICCS(const ChessMove& move)
{
    char fromCol = 'a' + move.fromCol;
    char fromRow = '0' + (9 - move.fromRow);
    char toCol = 'a' + move.toCol;
    char toRow = '0' + (9 - move.toRow);

    std::string result;
    result += fromCol;
    result += fromRow;
    result += '-';
    result += toCol;
    result += toRow;
    return result;
}

ChessMove Game::ICCSToMove(const std::string& iccs)
{
    if (iccs.length() < 5)
        return ChessMove();

    int fromCol = iccs[0] - 'a';
    int fromRow = 9 - (iccs[1] - '0');
    int toCol = iccs[3] - 'a';
    int toRow = 9 - (iccs[4] - '0');

    if (fromCol < 0 || fromCol >= Board::COLS || toCol < 0 || toCol >= Board::COLS)
        return ChessMove();
    if (fromRow < 0 || fromRow >= Board::ROWS || toRow < 0 || toRow >= Board::ROWS)
        return ChessMove();

    return ChessMove(fromRow, fromCol, toRow, toCol);
}

std::string Game::MoveToCoordinate(const ChessMove& move)
{
    std::string result;
    result += static_cast<char>('a' + move.fromCol);
    result += std::to_string(9 - move.fromRow);
    result += static_cast<char>('a' + move.toCol);
    result += std::to_string(9 - move.toRow);
    return result;
}

ChessMove Game::CoordinateToMove(const std::string& coord)
{
    if (coord.length() < 4)
        return ChessMove();

    int fromCol = coord[0] - 'a';
    int fromRow = 9 - (coord[1] - '0');
    int toCol = coord[2] - 'a';
    int toRow = 9 - (coord[3] - '0');

    if (fromCol < 0 || fromCol >= Board::COLS || toCol < 0 || toCol >= Board::COLS)
        return ChessMove();
    if (fromRow < 0 || fromRow >= Board::ROWS || toRow < 0 || toRow >= Board::ROWS)
        return ChessMove();

    return ChessMove(fromRow, fromCol, toRow, toCol);
}



bool Game::LoadFromPGN(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
        return false;

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    std::string raw = buffer.str();

    if (!raw.empty() && (static_cast<unsigned char>(raw[0]) == 0xFF ||
                         (raw.size() > 1 && static_cast<unsigned char>(raw[0]) == 0xEF &&
                          static_cast<unsigned char>(raw[1]) == 0xBB)))
    {
        return LoadFromBuffer(raw);
    }

    std::string utf8 = GBKToUTF8(raw);
    if (utf8.empty() && !raw.empty())
        utf8 = raw;

    return LoadFromBuffer(utf8);
}

bool Game::SaveToPGN(const std::string& filename) const
{
    std::string content;
    SaveToBuffer(content);

    std::string gbk = UTF8ToGBK(content);

    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open())
        return false;

    file << gbk;
    file.close();

    return true;
}

bool Game::LoadFromBuffer(const std::string& content)
{
    NewGame();

    std::istringstream stream(content);
    std::string line;
    std::string movetext;
    bool inTags = true;

    while (std::getline(stream, line))
    {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        if (line.empty())
            continue;

        if (inTags && line[0] == '[')
        {
            size_t spacePos = line.find(' ');
            size_t quoteStart = line.find('"');
            size_t quoteEnd = line.find('"', quoteStart + 1);

            if (spacePos != std::string::npos && quoteStart != std::string::npos && quoteEnd != std::string::npos)
            {
                std::string tag = line.substr(1, spacePos - 1);
                std::string value = line.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
                SetTagValue(tag, value);
            }
        }
        else
        {
            inTags = false;
            movetext += line;
            movetext += ' ';
        }
    }

    m_info.eventName = GetTagValue("Event");
    m_info.site = GetTagValue("Site");
    m_info.date = GetTagValue("Date");
    m_info.round = GetTagValue("Round");
    m_info.redPlayer = GetTagValue("Red");
    m_info.blackPlayer = GetTagValue("Black");
    m_info.result = GetTagValue("Result");
    m_info.redElo = GetTagValue("RedElo");
    m_info.blackElo = GetTagValue("BlackElo");
    m_info.ecco = GetTagValue("Ecco");
    m_info.fen = GetTagValue("FEN");

    if (!m_info.fen.empty())
        m_board.FromFEN(m_info.fen);

    std::string cleaned;
    bool inComment = false;
    for (char ch : movetext)
    {
        if (ch == '{')
        {
            inComment = true;
            continue;
        }
        if (ch == '}')
        {
            inComment = false;
            continue;
        }
        if (!inComment)
            cleaned += ch;
    }

    std::string format = GetTagValue("Format");
    bool useChinese = (format.empty() || format == "Chinese");

    std::istringstream moveStream(cleaned);
    std::string token;
    PieceColor currentSide = RED;
    if (!m_info.fen.empty())
    {
        size_t sp = m_info.fen.find(' ');
        if (sp != std::string::npos && m_info.fen[sp + 1] == 'b')
            currentSide = BLACK;
    }

    while (moveStream >> token)
    {
        if (token.empty())
            continue;

        if (token.find('.') != std::string::npos)
            continue;

        if (token == "1-0" || token == "0-1" || token == "1/2-1/2" || token == "*")
            continue;

        ChessMove move;

        if (useChinese)
        {
            move = ParseChineseNotation(m_board, currentSide, token);
        }
        else
        {
            if (token.length() >= 4 && token[2] == '-')
                move = ICCSToMove(token);
            else if (token.length() >= 4)
                move = CoordinateToMove(token);
        }

        if (move.fromRow < 0 || move.fromRow >= Board::ROWS || move.fromCol < 0 || move.fromCol >= Board::COLS)
            continue;

        if (!m_board.IsLegalMove(move))
            continue;

        AddMove(move);
        currentSide = (currentSide == RED) ? BLACK : RED;
    }

    m_currentIndex = -1;
    m_board.Reset();
    if (!m_info.fen.empty())
        m_board.FromFEN(m_info.fen);

    return true;
}

bool Game::SaveToBuffer(std::string& content) const
{
    content.clear();

    auto addTag = [&content](const std::string& tag, const std::string& value)
    {
        if (!value.empty())
            content += "[" + tag + " \"" + value + "\"]\n";
    };

    addTag("Game", "Chinese Chess");
    addTag("Format", "Chinese");
    addTag("Event", m_info.eventName);
    addTag("Site", m_info.site);
    addTag("Date", m_info.date);
    addTag("Round", m_info.round);
    addTag("RedTeam", GetTagValue("RedTeam"));
    addTag("Red", m_info.redPlayer);
    addTag("BlackTeam", GetTagValue("BlackTeam"));
    addTag("Black", m_info.blackPlayer);
    addTag("Result", m_info.result);
    addTag("RedElo", m_info.redElo);
    addTag("BlackElo", m_info.blackElo);
    addTag("ECCO", m_info.ecco);
    addTag("Opening", GetTagValue("Opening"));
    addTag("Variation", GetTagValue("Variation"));

    {
        Board fenBoard;
        std::string fen = m_info.fen;
        if (fen.empty())
        {
            fenBoard.Reset();
            fen = fenBoard.ToFEN();
        }
        addTag("FEN", fen);
    }

    content += "\n";

    int moveNum = 1;
    for (int i = 0; i < static_cast<int>(m_moves.size()); i++)
    {
        if (i % 2 == 0)
        {
            content += std::to_string(moveNum);
            content += ". ";
            moveNum++;
        }

        content += m_moves[i].notation;
        content += ' ';

        if (!m_moves[i].comment.empty())
        {
            content += "{";
            content += m_moves[i].comment;
            content += "} ";
        }
    }

    if (!m_info.result.empty())
        content += m_info.result;

    content += "\n";
    return true;
}

std::string Game::GetTagValue(const std::string& tag) const
{
    auto it = m_tags.find(tag);
    if (it != m_tags.end())
        return it->second;
    return "";
}

void Game::SetTagValue(const std::string& tag, const std::string& value)
{
    m_tags[tag] = value;
}
