#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <vector>

/**
 * @brief 棋子类型枚举
 * 定义了中国象棋中所有可能的棋子类型
 */
enum PieceType
{
    KING = 0,      // 将/帅
    ADVISOR = 1,   // 士/仕
    ELEPHANT = 2,  // 象/相
    HORSE = 3,     // 马
    CHARIOT = 4,   // 车
    CANNON = 5,    // 炮
    PAWN = 6,      // 兵/卒
    NONE = 7       // 空位置
};

/**
 * @brief 棋子颜色枚举
 * 定义了棋子的两种颜色：红方和黑方
 */
enum PieceColor
{
    RED = 0,           // 红方
    BLACK = 1,         // 黑方
    COLORS_COUNT = 2    // 颜色总数（用于循环等）
};

/**
 * @brief 棋子结构体
 * 表示棋盘上的一个棋子，包含类型和颜色
 */
struct Piece
{
    PieceType type;   // 棋子类型
    PieceColor color;  // 棋子颜色

    /**
     * @brief 默认构造函数
     * 创建一个空棋子（红色）
     */
    Piece() : type(NONE), color(RED) {}
    
    /**
     * @brief 带参数的构造函数
     * @param t 棋子类型
     * @param c 棋子颜色
     */
    Piece(PieceType t, PieceColor c) : type(t), color(c) {}

    /**
     * @brief 判断是否为空位置
     * @return 如果棋子类型为NONE则返回true，否则返回false
     */
    bool IsEmpty() const { return type == NONE; }
};

/**
 * @brief 走法结构体
 * 表示一个棋子的移动，包含起始位置和目标位置
 */
struct ChessMove
{
    int fromRow;  // 起始行
    int fromCol;  // 起始列
    int toRow;    // 目标行
    int toCol;    // 目标列

    /**
     * @brief 默认构造函数
     * 创建一个无效的走法（所有位置为0）
     */
    ChessMove() : fromRow(0), fromCol(0), toRow(0), toCol(0) {}
    
    /**
     * @brief 带参数的构造函数
     * @param fr 起始行
     * @param fc 起始列
     * @param tr 目标行
     * @param tc 目标列
     */
    ChessMove(int fr, int fc, int tr, int tc) : fromRow(fr), fromCol(fc), toRow(tr), toCol(tc) {}

    /**
     * @brief 重载等于运算符
     * 用于比较两个走法是否相同
     * @param other 另一个走法
     * @return 如果两个走法相同则返回true，否则返回false
     */
    bool operator==(const ChessMove& other) const
    {
        return fromRow == other.fromRow && fromCol == other.fromCol &&
               toRow == other.toRow && toCol == other.toCol;
    }
};

/**
 * @brief 棋盘类
 * 表示中国象棋的棋盘状态，包括棋子的位置、当前玩家、走法历史等
 */
class Board
{
public:
    static const int ROWS = 10;  // 棋盘行数
    static const int COLS = 9;   // 棋盘列数

    /**
     * @brief 构造函数
     * 初始化一个新的棋盘
     */
    Board();

    /**
     * @brief 重置棋盘
     * 将棋盘恢复到初始状态
     */
    void Reset();
    
    /**
     * @brief 从FEN字符串初始化棋盘
     * @param fen FEN字符串，描述棋盘的初始状态
     * @return 初始化成功返回true，否则返回false
     */
    bool FromFEN(const std::string& fen);
    
    /**
     * @brief 将棋盘状态转换为FEN字符串
     * @return 描述当前棋盘状态的FEN字符串
     */
    std::string ToFEN() const;

    /**
     * @brief 获取指定位置的棋子
     * @param row 行坐标
     * @param col 列坐标
     * @return 指定位置的棋子
     */
    Piece GetPiece(int row, int col) const;
    
    /**
     * @brief 获取当前玩家
     * @return 当前应该走棋的玩家颜色
     */
    PieceColor GetCurrentPlayer() const { return m_currentPlayer; }

    /**
     * @brief 判断位置是否有效
     * @param row 行坐标
     * @param col 列坐标
     * @return 如果位置在棋盘范围内则返回true，否则返回false
     */
    bool IsValidPosition(int row, int col) const;
    
    /**
     * @brief 判断位置是否为空
     * @param row 行坐标
     * @param col 列坐标
     * @return 如果位置在棋盘范围内且为空则返回true，否则返回false
     */
    bool IsEmpty(int row, int col) const;
    
    /**
     * @brief 判断位置是否为指定颜色的盟友
     * @param row 行坐标
     * @param col 列坐标
     * @param color 要检查的颜色
     * @return 如果位置在棋盘范围内且为指定颜色的棋子则返回true，否则返回false
     */
    bool IsAlly(int row, int col, PieceColor color) const;
    
    /**
     * @brief 判断位置是否为指定颜色的敌人
     * @param row 行坐标
     * @param col 列坐标
     * @param color 要检查的颜色
     * @return 如果位置在棋盘范围内且为对方颜色的棋子则返回true，否则返回false
     */
    bool IsEnemy(int row, int col, PieceColor color) const;

    /**
     * @brief 生成指定颜色的所有可能走法
     * @param color 要生成走法的颜色
     * @return 所有可能走法的列表
     */
    std::vector<ChessMove> GenerateMoves(PieceColor color) const;
    
    /**
     * @brief 生成指定颜色的所有合法走法
     * @param color 要生成走法的颜色
     * @return 所有合法走法的列表
     */
    std::vector<ChessMove> GenerateLegalMoves(PieceColor color);
    
    /**
     * @brief 生成当前玩家的所有合法走法
     * @return 所有合法走法的列表
     */
    std::vector<ChessMove> GenerateLegalMoves();

    /**
     * @brief 判断走法是否合法
     * @param move 要判断的走法
     * @return 如果走法合法则返回true，否则返回false
     */
    bool IsLegalMove(const ChessMove& move);
    
    /**
     * @brief 执行走法
     * @param move 要执行的走法
     * @return 如果走法合法并执行成功则返回true，否则返回false
     */
    bool MakeMove(const ChessMove& move);
    
    /**
     * @brief 撤销上一步走法
     */
    void UndoMove();

    /**
     * @brief 判断指定颜色是否被将军
     * @param color 要检查的颜色
     * @return 如果该颜色的王被将军则返回true，否则返回false
     */
    bool IsInCheck(PieceColor color) const;
    
    /**
     * @brief 判断指定颜色是否被将死
     * @param color 要检查的颜色
     * @return 如果该颜色被将死则返回true，否则返回false
     */
    bool IsCheckmate(PieceColor color);
    
    /**
     * @brief 判断是否为和棋
     * @param color 要检查的颜色
     * @return 如果为和棋则返回true，否则返回false
     */
    bool IsStalemate(PieceColor color);
    
    /**
     * @brief 判断两方王是否面对面
     * @return 如果两方王在同一列且中间无棋子阻挡则返回true，否则返回false
     */
    bool KingsFaceEachOther() const;

    /**
     * @brief 判断位置是否在九宫格内
     * @param row 行坐标
     * @param col 列坐标
     * @param color 颜色（九宫格位置与颜色相关）
     * @return 如果位置在指定颜色的九宫格内则返回true，否则返回false
     */
    static bool InPalace(int row, int col, PieceColor color);
    
    /**
     * @brief 判断位置是否过河
     * @param row 行坐标
     * @param color 颜色（过河位置与颜色相关）
     * @return 如果位置过河则返回true，否则返回false
     */
    static bool IsAcrossRiver(int row, PieceColor color);

private:
    Piece m_board[ROWS][COLS];  // 棋盘数组
    PieceColor m_currentPlayer;  // 当前玩家

    /**
     * @brief 撤销信息结构体
     * 用于保存走法历史，以便撤销
     */
    struct UndoInfo
    {
        ChessMove move;      // 执行的走法
        Piece captured;      // 被吃的棋子
        PieceColor prevPlayer; // 上一个玩家
    };
    std::vector<UndoInfo> m_history;  // 走法历史

    /**
     * @brief 查找指定颜色的王所在的行
     * @param color 要查找的颜色
     * @return 王所在的行，如果找不到则返回-1
     */
    int FindKingRow(PieceColor color) const;
    
    /**
     * @brief 查找指定颜色的王所在的列
     * @param color 要查找的颜色
     * @return 王所在的列，如果找不到则返回-1
     */
    int FindKingCol(PieceColor color) const;

    /**
     * @brief 添加滑动棋子的可能走法
     * @param moves 走法列表（将被修改）
     * @param row 起始行
     * @param col 起始列
     * @param dRow 行方向增量
     * @param dCol 列方向增量
     */
    void AddSlidingMoves(std::vector<ChessMove>& moves, int row, int col, int dRow, int dCol) const;
    
    /**
     * @brief 添加马的走法
     * @param moves 走法列表（将被修改）
     * @param row 起始行
     * @param col 起始列
     */
    void AddKnightMoves(std::vector<ChessMove>& moves, int row, int col) const;
};

#endif // BOARD_H
