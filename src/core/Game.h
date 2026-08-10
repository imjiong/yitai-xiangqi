#ifndef GAME_H
#define GAME_H

#include "../core/Board.h"
#include <string>
#include <vector>
#include <map>

struct MoveRecord
{
    ChessMove move;
    std::string comment;
    std::string notation;

    MoveRecord() {}
    MoveRecord(const ChessMove& m) : move(m) {}
    MoveRecord(const ChessMove& m, const std::string& c) : move(m), comment(c) {}
};

struct GameInfo
{
    std::string eventName;
    std::string site;
    std::string date;
    std::string round;
    std::string redPlayer;
    std::string blackPlayer;
    std::string result;
    std::string redElo;
    std::string blackElo;
    std::string ecco;
    std::string fen;

    GameInfo() : result("*") {}
};

class Game
{
public:
    Game();

    void NewGame();
    void Clear();

    bool AddMove(const ChessMove& move, const std::string& comment = "");
    bool InsertMove(int index, const ChessMove& move, const std::string& comment = "");
    bool RemoveMove(int index);
    void SetComment(int index, const std::string& comment);

    MoveRecord* GetMove(int index);
    const MoveRecord* GetMove(int index) const;
    int GetMoveCount() const { return static_cast<int>(m_moves.size()); }

    int GetCurrentIndex() const { return m_currentIndex; }
    void SetCurrentIndex(int index);
    void GoToStart();
    void GoToEnd();
    void GoBack();
    void GoForward();

    Board& GetBoard() { return m_board; }
    const Board& GetBoard() const { return m_board; }

    GameInfo& GetGameInfo() { return m_info; }
    const GameInfo& GetGameInfo() const { return m_info; }

    bool LoadFromPGN(const std::string& filename);
    bool SaveToPGN(const std::string& filename) const;
    bool LoadFromBuffer(const std::string& content);
    bool SaveToBuffer(std::string& content) const;

    static std::string MoveToICCS(const ChessMove& move);
    static ChessMove ICCSToMove(const std::string& iccs);
    static std::string MoveToCoordinate(const ChessMove& move);
    static ChessMove CoordinateToMove(const std::string& coord);
    static std::string MoveToChinese(const ChessMove& move, PieceType type, PieceColor color, const Board& board);

private:
    std::vector<MoveRecord> m_moves;
    Board m_board;
    GameInfo m_info;
    int m_currentIndex;

    void RebuildBoardToIndex(int index);
    std::string GetTagValue(const std::string& tag) const;
    void SetTagValue(const std::string& tag, const std::string& value);
    std::map<std::string, std::string> m_tags;
};

#endif // GAME_H
