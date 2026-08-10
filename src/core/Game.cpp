#include "Game.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <wx/string.h>

static std::string WStringToUTF8(const std::wstring& ws)
{
    wxString wx(ws);
    std::string s = std::string(wx.utf8_str().data());
    return s;
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
    static const wchar_t* blackNums[] = { L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9" };
    static const wchar_t* pieceChars[] = { L"帥", L"仕", L"相", L"馬", L"車", L"炮", L"兵" };
    static const wchar_t* pieceCharsB[] = { L"將", L"士", L"象", L"馬", L"車", L"砲", L"卒" };

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
    std::ifstream file(filename);
    if (!file.is_open())
        return false;

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return LoadFromBuffer(buffer.str());
}

bool Game::SaveToPGN(const std::string& filename) const
{
    std::ofstream file(filename);
    if (!file.is_open())
        return false;

    std::string content;
    SaveToBuffer(content);
    file << content;
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

    std::istringstream moveStream(cleaned);
    std::string token;
    while (moveStream >> token)
    {
        if (token.empty())
            continue;

        if (token.find('.') != std::string::npos)
            continue;

        if (token == "1-0" || token == "0-1" || token == "1/2-1/2" || token == "*")
            continue;

        if (token.length() < 4)
            continue;

        ChessMove move = CoordinateToMove(token);
        if (move.fromRow < 0 || move.fromRow >= Board::ROWS || move.fromCol < 0 || move.fromCol >= Board::COLS)
        {
            move = ICCSToMove(token);
        }
        if (move.fromRow < 0 || move.fromRow >= Board::ROWS || move.fromCol < 0 || move.fromCol >= Board::COLS)
            continue;

        if (!m_board.IsLegalMove(move))
            continue;

        AddMove(move);
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
    addTag("Event", m_info.eventName);
    addTag("Site", m_info.site);
    addTag("Date", m_info.date);
    addTag("Round", m_info.round);
    addTag("Red", m_info.redPlayer);
    addTag("Black", m_info.blackPlayer);
    addTag("Result", m_info.result);
    addTag("RedElo", m_info.redElo);
    addTag("BlackElo", m_info.blackElo);
    addTag("Ecco", m_info.ecco);

    if (!m_info.fen.empty())
        addTag("FEN", m_info.fen);

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
