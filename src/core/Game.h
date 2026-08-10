#ifndef GAME_H
#define GAME_H

#include "../core/Board.h"
#include <string>
#include <vector>
#include <map>

/**
 * @brief 走法记录结构体
 * 保存一个走法的详细信息，包括走法本身、注释和记谱
 */
struct MoveRecord
{
    ChessMove move;        // 走法
    std::string comment;  // 注释
    std::string notation; // 记谱（如"炮二平五"）

    /**
     * @brief 默认构造函数
     */
    MoveRecord() {}
    
    /**
     * @brief 带走法的构造函数
     * @param m 走法
     */
    MoveRecord(const ChessMove& m) : move(m) {}
    
    /**
     * @brief 带走法和注释的构造函数
     * @param m 走法
     * @param c 注释
     */
    MoveRecord(const ChessMove& m, const std::string& c) : move(m), comment(c) {}
};

/**
 * @brief 游戏信息结构体
 * 保存对局的基本信息，如比赛名称、地点、日期、选手等
 */
struct GameInfo
{
    std::string eventName;  // 比赛名称
    std::string site;       // 比赛地点
    std::string date;       // 比赛日期
    std::string round;      // 回合数
    std::string redPlayer;  // 红方选手
    std::string blackPlayer;// 黑方选手
    std::string result;     // 比赛结果（1-0, 0-1, 1/2-1/2, *）
    std::string redElo;     // 红方Elo等级分
    std::string blackElo;  // 黑方Elo等级分
    std::string ecco;       // 开局代码
    std::string fen;        // 初始FEN字符串

    /**
     * @brief 构造函数
     * 默认结果为未结束（*）
     */
    GameInfo() : result("*") {}
};

/**
 * @brief 游戏类
 * 管理一局象棋游戏的状态、历史记录和游戏信息
 */
class Game
{
public:
    /**
     * @brief 构造函数
     * 创建一个新的游戏对象
     */
    Game();

    /**
     * @brief 开始新游戏
     * 清空所有走法，重置棋盘和游戏信息
     */
    void NewGame();
    
    /**
     * @brief 清空游戏
     * 清空所有数据和状态
     */
    void Clear();

    /**
     * @brief 添加走法
     * 在当前游戏末尾添加一个走法
     * @param move 要添加的走法
     * @param comment 走法的注释（可选）
     * @return 添加成功返回true，否则返回false
     */
    bool AddMove(const ChessMove& move, const std::string& comment = "");
    
    /**
     * @brief 插入走法
     * 在指定位置插入一个走法，后续走法后移
     * @param index 插入位置
     * @param move 要插入的走法
     * @param comment 走法的注释（可选）
     * @return 插入成功返回true，否则返回false
     */
    bool InsertMove(int index, const ChessMove& move, const std::string& comment = "");
    
    /**
     * @brief 删除走法
     * 删除指定位置的走法及其后续走法
     * @param index 要删除的走法位置
     * @return 删除成功返回true，否则返回false
     */
    bool RemoveMove(int index);
    
    /**
     * @brief 设置走法注释
     * 为指定位置的走法添加或修改注释
     * @param index 走法位置
     * @param comment 注释内容
     */
    void SetComment(int index, const std::string& comment);

    /**
     * @brief 获取走法记录
     * @param index 走法位置
     * @return 指定位置的走法记录指针
     */
    MoveRecord* GetMove(int index);
    
    /**
     * @brief 走法记录（常量版本）
     * @param index 走法位置
     * @return 指定位置的走法记录指针（常量）
     */
    const MoveRecord* GetMove(int index) const;
    
    /**
     * @brief 获取走法数量
     * @return 当前游戏中的走法总数
     */
    int GetMoveCount() const { return static_cast<int>(m_moves.size()); }

    /**
     * @brief 获取当前索引
     * @return 当前所在的游戏步骤索引
     */
    int GetCurrentIndex() const { return m_currentIndex; }
    
    /**
     * @brief 设置当前索引
     * @param index 要设置的索引
     */
    void SetCurrentIndex(int index);
    
    /**
     * @brief 跳转到游戏开始
     */
    void GoToStart();
    
    /**
     * @brief 跳转到游戏结束
     */
    void GoToEnd();
    
    /**
     * @brief 回退一步
     */
    void GoBack();
    
    /**
     * @brief 前进一步
     */
    void GoForward();

    /**
     * @brief 获取棋盘（可修改版本）
     * @return 游戏棋盘的引用
     */
    Board& GetBoard() { return m_board; }
    
    /**
     * @brief 获取棋盘（常量版本）
     * @return 游戏棋盘的常量引用
     */
    const Board& GetBoard() const { return m_board; }

    /**
     * @brief 获取游戏信息（可修改版本）
     * @return 游戏信息的引用
     */
    GameInfo& GetGameInfo() { return m_info; }
    
    /**
     * @brief 获取游戏信息（常量版本）
     * @return 游戏信息的常量引用
     */
    const GameInfo& GetGameInfo() const { return m_info; }

    /**
     * @brief 从PGN文件加载游戏
     * @param filename PGN文件名
     * @return 加载成功返回true，否则返回false
     */
    bool LoadFromPGN(const std::string& filename);
    
    /**
     * @brief 保存游戏到PGN文件
     * @param filename 要保存的文件名
     * @return 保存成功返回true，否则返回false
     */
    bool SaveToPGN(const std::string& filename) const;
    
    /**
     * @brief 从缓冲区加载游戏
     * @param content 包含PGN格式内容的字符串
     * @return 加载成功返回true，否则返回false
     */
    bool LoadFromBuffer(const std::string& content);
    
    /**
     * @brief 保存游戏到缓冲区
     * @param content 用于保存结果的字符串引用
     * @return 保存成功返回true，否则返回false
     */
    bool SaveToBuffer(std::string& content) const;

    /**
     * @brief 将走法转换为ICCS格式
     * @param move 要转换的走法
     * @return ICCS格式的字符串
     */
    static std::string MoveToICCS(const ChessMove& move);
    
    /**
     * @brief 从ICCS字符串解析走法
     * @param iccs ICCS格式的字符串
     * @return 解析出的走法
     */
    static ChessMove ICCSToMove(const std::string& iccs);
    
    /**
     * @brief 将走法转换为坐标格式
     * @param move 要转换的走法
     * @return 坐标格式的字符串（如"e2e4"）
     */
    static std::string MoveToCoordinate(const ChessMove& move);
    
    /**
     * @brief 从坐标字符串解析走法
     * @param coord 坐标格式的字符串
     * @return 解析出的走法
     */
    static ChessMove CoordinateToMove(const std::string& coord);
    
    /**
     * @brief 将走法转换为中文记谱
     * @param move 要转换的走法
     * @param type 棋子类型
     * @param color 棋子颜色
     * @param board 当前棋盘状态
     * @return 中文记谱字符串（如"炮二平五"）
     */
    static std::string MoveToChinese(const ChessMove& move, PieceType type, PieceColor color, const Board& board);

private:
    std::vector<MoveRecord> m_moves; // 走法记录列表
    Board m_board;                   // 游戏棋盘
    GameInfo m_info;                 // 游戏信息
    int m_currentIndex;              // 当前索引

    /**
     * @brief 重建棋盘到指定索引
     * 根据走法记录重建棋盘到指定步骤
     * @param index 目标索引
     */
    void RebuildBoardToIndex(int index);
    
    /**
     * @brief 获取PGN标签值
     * @param tag 标签名
     * @return 标签对应的值
     */
    std::string GetTagValue(const std::string& tag) const;
    
    /**
     * @brief 设置PGN标签值
     * @param tag 标签名
     * @param value 标签值
     */
    void SetTagValue(const std::string& tag, const std::string& value);
    std::map<std::string, std::string> m_tags; // PGN标签映射
};

#endif // GAME_H
