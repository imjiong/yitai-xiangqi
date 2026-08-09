#include <wx/wx.h>
#include "gui/MainFrame.h"

/**
 * @brief ChessApp 类继承自 wxApp，是国际象棋应用程序的主应用程序类
 */
class ChessApp : public wxApp
{
public:
    /**
     * @brief 初始化应用程序，在程序启动时被调用
     * @return 初始化成功返回 true，否则返回 false
     */
    virtual bool OnInit() override;
};

wxIMPLEMENT_APP(ChessApp);

bool ChessApp::OnInit()
{
    if (!wxApp::OnInit())
        return false;

    wxImage::AddHandler(new wxPNGHandler);

    MainFrame* frame = new MainFrame();
    frame->Show(true);
    return true;
}
