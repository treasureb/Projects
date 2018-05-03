#pragma once
#include"ncurses.h"
#include<string>
#include<queue>

namespace  client{
class Window
{
public:
    Window();
    ~Window();
    //绘制标题子窗口
    void DrawHeader();
    //绘制输入框
    void DrawInput();
    //绘制输出框
    void DrawOutput();
    //绘制好友列表
    void DrawFriendList();
    //往窗口中写字符串
    void PutStrToWin(WINDOW* win,int y ,int x,const std::string& str);
    //从窗口中获取字符串
    void GetStrFromWin(WINDOW* win,std::string* str);

private:
    WINDOW* _header_win;
    WINDOW* _input_win;
    WINDOW* _output_win;
    WINDOW* _friend_list__win;
    //保存当前显示的消息
    std::queue<std::string> msgs;
};

}//end client
