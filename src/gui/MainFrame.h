#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/wx.h>
#include <wx/aui/aui.h>
#include "BoardPanel.h"
#include "MoveRecordPanel.h"
#include "../core/Game.h"

/**
 * @brief 主框架类
 * 应用程序的主窗口，包含棋盘面板、走法记录面板，并管理菜单和工具栏
 */
class MainFrame : public wxFrame
{
public:
    /**
     * @brief 构造函数
     * 创建主窗口及其子组件
     */
    MainFrame();
    
    /**
     * @brief 析构函数
     */
    ~MainFrame();

private:
    // 界面设置函数
    /**
     * @brief 创建菜单栏
     * 设置应用程序的菜单项
     */
    void CreateMenuBar();
    
    /**
     * @brief 设置工具栏
     * 设置应用程序的工具栏按钮
     */
    void SetupToolBar();
    
    /**
     * @brief 设置状态栏
     * 设置应用程序的状态栏
     */
    void SetupStatusBar();

    // 事件处理函数
    /**
     * @brief 翻转视图事件处理
     * @param event 菜单事件
     */
    void OnFlipView(wxCommandEvent& event);
    
    /**
     * @brief 重置棋盘事件处理
     * @param event 菜单事件
     */
    void OnResetBoard(wxCommandEvent& event);
    
    /**
     * @brief 后退一步事件处理
     * @param event 菜单事件
     */
    void OnGoBack(wxCommandEvent& event);
    
    /**
     * @brief 前进一步事件处理
     * @param event 菜单事件
     */
    void OnGoForward(wxCommandEvent& event);
    
    /**
     * @brief 跳转到开始事件处理
     * @param event 菜单事件
     */
    void OnGoToStart(wxCommandEvent& event);
    
    /**
     * @brief 跳转到结束事件处理
     * @param event 菜单事件
     */
    void OnGoToEnd(wxCommandEvent& event);
    
    /**
     * @brief 播放/暂停事件处理
     * @param event 菜单事件
     */
    void OnPlay(wxCommandEvent& event);
    
    /**
     * @brief 切换播放状态
     * 在播放和暂停之间切换
     */
    void TogglePlay();

    static void OnBoardMoveMade(void* userData);

    /**
     * @brief 打开文件事件处理
     * @param event 菜单事件
     */
    void OnOpen(wxCommandEvent& event);
    
    /**
     * @brief 保存文件事件处理
     * @param event 菜单事件
     */
    void OnSave(wxCommandEvent& event);
    
    /**
     * @brief 另存为事件处理
     * @param event 菜单事件
     */
    void OnSaveAs(wxCommandEvent& event);
    
    /**
     * @brief 新建游戏事件处理
     * @param event 菜单事件
     */
    void OnNew(wxCommandEvent& event);

    /**
     * @brief 播放计时器事件处理
     * @param event 计时器事件
     */
    void OnPlayTimer(wxTimerEvent& event);

    // 成员变量
    wxAuiManager m_auiManager;      // AUI管理器，用于管理面板布局
    BoardPanel* m_boardPanel;       // 棋盘面板
    MoveRecordPanel* m_movePanel;    // 走法记录面板
    Game m_game;                     // 游戏对象

    wxMenuItem* m_playMenuItem;     // 播放菜单项
    wxToolBarToolBase* m_playTool;   // 播放工具栏按钮
    wxTimer* m_playTimer;            // 播放计时器
    bool m_isPlaying;                 // 是否正在播放
    wxString m_currentFile;          // 当前文件路径（用于保存）

    wxDECLARE_EVENT_TABLE();         // 声明事件表
};

#endif // MAINFRAME_H
