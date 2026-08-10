#ifndef BOARDPANEL_H
#define BOARDPANEL_H

#include <wx/wx.h>
#include "../core/Board.h"
#include "../core/Game.h"

/**
 * @brief 棋盘面板类
 * 继承自wxPanel，用于显示和交互象棋棋盘
 * 负责绘制棋盘、棋子，处理用户输入，并显示可能的走法
 */
class BoardPanel : public wxPanel
{
public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    BoardPanel(wxWindow* parent);
    
    /**
     * @brief 析构函数
     */
    ~BoardPanel();

    /**
     * @brief 重置棋盘
     * 将棋盘恢复到初始状态
     */
    void ResetBoard();
    
    /**
     * @brief 翻转视图
     * 将棋盘视图上下翻转（改变红黑方位置）
     */
    void FlipView();
    
    /**
     * @brief 检查视图是否翻转
     * @return 如果视图已翻转则返回true，否则返回false
     */
    bool IsFlipped() const { return m_flipped; }

    /**
     * @brief 设置游戏
     * @param game 游戏对象指针
     */
    void SetGame(Game* game);
    
    /**
     * @brief 获取游戏
     * @return 游戏对象指针
     */
    Game* GetGame() { return m_game; }

    /**
     * @brief 与游戏同步
     * 根据当前游戏状态更新棋盘显示
     */
    void SyncWithGame();

    typedef void (*MoveCallback)(void* userData);
    void SetMoveCallback(MoveCallback cb, void* userData);

private:
    // 事件处理函数
    /**
     * @brief 绘制事件处理
     * @param event 绘制事件
     */
    void OnPaint(wxPaintEvent& event);
    
    /**
     * @brief 鼠标左键按下事件处理
     * @param event 鼠标事件
     */
    void OnLeftDown(wxMouseEvent& event);
    
    /**
     * @brief 窗口大小改变事件处理
     * @param event 大小事件
     */
    void OnSize(wxSizeEvent& event);
    
    /**
     * @brief 非法走法计时器事件处理
     * @param event 计时器事件
     */
    void OnIllegalTimer(wxTimerEvent& event);

    // 绘制函数
    /**
     * @brief 计算布局
     * 根据窗口大小计算棋盘、棋子的尺寸和位置
     */
    void CalculateLayout();
    
    /**
     * @brief 绘制棋盘
     * @param dc 设备上下文
     */
    void DrawBoard(wxDC& dc);
    
    /**
     * @brief 绘制所有棋子
     * @param dc 设备上下文
     */
    void DrawPieces(wxDC& dc);
    
    /**
     * @brief 绘制单个棋子
     * @param dc 设备上下文
     * @param row 行坐标
     * @param col 列坐标
     * @param piece 要绘制的棋子
     */
    void DrawPiece(wxDC& dc, int row, int col, const Piece& piece);
    
    /**
     * @brief 绘制选中状态
     * @param dc 设备上下文
     */
    void DrawSelection(wxDC& dc);
    
    /**
     * @brief 绘制走法提示
     * 显示当前选中棋子的所有合法走法
     * @param dc 设备上下文
     */
    void DrawMoveHints(wxDC& dc);
    
    /**
     * @brief 绘制非法走法提示
     * 当用户尝试非法走法时显示提示
     * @param dc 设备上下文
     */
    void DrawIllegalText(wxDC& dc);

    // 坐标转换函数
    /**
     * @brief 棋盘坐标转屏幕坐标
     * @param row 行坐标
     * @param col 列坐标
     * @return 对应的屏幕坐标
     */
    wxPoint BoardToScreen(int row, int col) const;
    
    /**
     * @brief 屏幕坐标转棋盘坐标
     * @param x 屏幕x坐标
     * @param y 屏幕y坐标
     * @param row 输出：棋盘行坐标
     * @param col 输出：棋盘列坐标
     * @return 如果坐标有效则返回true，否则返回false
     */
    bool ScreenToBoard(int x, int y, int& row, int& col) const;

    // 成员变量
    Board m_board;                   // 棋盘对象
    Game* m_game;                    // 游戏对象指针
    bool m_flipped;                  // 视图是否翻转

    int m_selectedRow;               // 选中棋子的行
    int m_selectedCol;               // 选中棋子的列
    std::vector<ChessMove> m_legalMoves; // 合法走法列表

    bool m_showIllegal;              // 是否显示非法走法提示
    wxTimer* m_illegalTimer;         // 非法走法提示计时器

    int m_cellSize;                  // 格子大小
    int m_marginX;                   // 水平边距
    int m_marginY;                   // 垂直边距
    int m_pieceRadius;               // 棋子半径

    MoveCallback m_moveCallback;
    void* m_moveCallbackData;

    wxDECLARE_EVENT_TABLE();        // 声明事件表
};

#endif // BOARDPANEL_H
