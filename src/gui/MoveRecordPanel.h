#ifndef MOVERECORDPANEL_H
#define MOVERECORDPANEL_H

#include <wx/wx.h>
#include <wx/listctrl.h>
#include "../core/Game.h"

/**
 * @brief 走法记录面板类
 * 继承自wxPanel，用于显示和交互走法记录列表
 * 允许用户浏览历史走法，点击跳转到特定步骤
 */
class MoveRecordPanel : public wxPanel
{
public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    MoveRecordPanel(wxWindow* parent);

    /**
     * @brief 设置游戏
     * @param game 游戏对象指针
     */
    void SetGame(Game* game);

    /**
     * @brief 刷新列表
     * 更新走法记录列表的显示
     */
    void RefreshList();
    
    /**
     * @brief 跳转到指定走法
     * @param index 目标走法索引
     */
    void GoToMove(int index);
    
    /**
     * @brief 获取选中的索引
     * @return 当前选中的走法索引
     */
    int GetSelectedIndex() const;

private:
    // 事件处理函数
    /**
     * @brief 项目选中事件处理
     * @param event 列表事件
     */
    void OnItemSelected(wxListEvent& event);
    
    /**
     * @brief 项目激活事件处理（双击）
     * @param event 列表事件
     */
    void OnItemActivated(wxListEvent& event);

    // 成员变量
    wxListCtrl* m_listCtrl;  // 列表控件
    Game* m_game;           // 游戏对象指针

    wxDECLARE_EVENT_TABLE(); // 声明事件表
};

#endif // MOVERECORDPANEL_H
