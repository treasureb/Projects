#include "window.h"
#include<locale.h>
namespace client{
Window::Window()
{
    //对整个ncurese进行初始化
    initscr();
    //隐藏光标
    curs_set(0);
    //设置字符集,支持中文
    setlocale(LC_ALL,"");
};

Window::~Window()
{
    //对整个ncourese进行销毁
    endwin();
}

void Window::DrawHeader()
{
    //LINES是ncurese提供的宏，获取当前屏幕最大行数
    int h = LINES;
    //COLS也是一个宏
    int w = COLS;
    //表示左上角的位置
    int y = 0;
    int x = 0;
    _header_win = newwin(h,w,y,x);
    std::string title = "Welcome Chat!";
    PutStrToWin(_header_win,h/2,w/2 - title.size()/2,title);
    box(_header_win,'|','-');
    wrefresh(_header_win);
}


void Window:: PutStrToWin(WINDOW* win,int y ,int x,const std::string& str)
{
    mvwaddstr(win,y,x,str.c_str());
}

void Window::GetStrFromWin(WINDOW* win,std::string* str)
{
    char buf[1024 * 10] = {0};
    wgetnstr(win,buf,sizeof(buf)-1);
    //多一次拷贝的过程，可以直接传str的c风格指针进去
    *str = buf;
}

void Window::DrawInput()
{
    //LINES是ncurese提供的宏，获取当前屏幕最大行数
    int h = LINES;
    //COLS也是一个宏
    int w = COLS;
    //表示左上角的位置
    int y = LINES * 4 /5;
    int x = 0;
    _input_win = newwin(h,w,y,x);
    std::string title = "Please Enter:";
    //此时的坐标是窗口左上角为原点的坐标系
    PutStrToWin(_input_win,1,1,title);
    box(_input_win,'|','-');
    wrefresh(_input_win);
}

void Window::DrawOutput()
{
    int h = LINES * 3 / 5;
    int w = COLS * 3 / 4;
    int y = LINES / 5;
    int x = 0;
    _output_win = newwin(h,w,y,x);
    box(_output_win,'|','-');
    wrefresh(_output_win);
}

}//end client
#include<unistd.h>
int main()
{
    client::Window win;
    win.DrawHeader();
    sleep(5);
    return 0;
}
