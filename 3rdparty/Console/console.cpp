/*
FreeBSD License

Copyright (c) 2010, Stefan Gatu

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "console.h"
#include "ConsoleMode.h"
//Setting the default color
//if platform is Windows
#ifdef _WIN32
//set default values
int Console::BGColor = 0;
int Console::FGColor = FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED;
HANDLE Console::hConsole =  GetStdHandle(STD_OUTPUT_HANDLE);

BYTE Console::TextAttr=0;
BYTE Console::DefaultAttr=0;
WORD Console::LastMode=0;
//if the plaform is other than Windows
#else
string Console::BGColor = "30";
string Console::FGColor = "47";
#endif
//adel
void Console::initConsoleScreen()
{
    /* PCONSOLE_SCREEN_BUFFER_INFO p=(PCONSOLE_SCREEN_BUFFER_INFO)malloc(sizeof(CONSOLE_SCREEN_BUFFER_INFO));
     GetConsoleScreenBufferInfo(hConsole, p);
     TextAttr = p->wAttributes & 255;
     DefaultAttr = TextAttr;
     LastMode = CO80;
     delete p;*/
}
void Console::HighVideo()
{
    Console::initConsoleScreen();
    /*LastMode = TextAttr;
    TextAttr = TextAttr | 8;
    SetConsoleTextAttribute(hConsole, TextAttr);
    */

}



//end adel
//Clears the console to the background color
void Console::Clear()
{
#ifdef _WIN32
    system("cls");
#else
    cout << "\033[2J";
#endif
}
//Sets the color(background and foreground)
void Console::SetColor()
{
#ifdef _WIN32
    SetConsoleTextAttribute(hConsole, FGColor | BGColor);
#else
    string clr = "\033[";
    clr += BGColor;
    clr += ";";
    clr += FGColor;
    clr += "m";
    cout << clr;
#endif
}
//Print a string
void Console::Write(string str)
{
#ifdef _MSC_VER
    cout << str.c_str();
#else
    cout << str;
#endif
}
//Print a int
void Console::Write(int v)
{
    cout << v;
}
//Print a bool
void Console::Write(bool v)
{
    cout << (v?"TRUE":"FALSE");
}
//Print a char array
void Console::Write(char str[])
{
    cout << str;
}
//Print a char
void Console::Write(char c)
{
    cout << c;
}
void Console::Write(double d)
{
    cout << d;
}
void Console::Write(float f)
{
    cout << f;
}

//Print a string and moves the cursor to the next line
void Console::WriteLine(string str)
{
#ifdef _MSC_VER
    cout << str.c_str() << endl;
#else
    cout << str << endl;
#endif
}
//Moves the cursor to the next line
void Console::WriteLine()
{
    cout << endl;
}
//Print a char array and moves the cursor to the next line
void Console::WriteLine(char str[])
{
    cout << str <<endl;
}
//Read the first key user presses
int Console::ReadKey()
{
    int val = 0;
#ifdef _WIN32
#ifdef _MSC_VER
    val = (int)_getch();
#else
    val = (int)getch();
#endif
#else
    struct termios oldt,newt;
    tcgetattr( STDIN_FILENO, &oldt );
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );
    val = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
#endif
    if(val == 224)
        val += Console::ReadKey();
    return val;
}
bool Console::KeyDown()
{
#ifdef _WIN32
#ifdef _MSC_VER
    return _kbhit();
#else
    return kbhit();
#endif
#else
    struct timeval tv;
    fd_set rdfs;

    tv.tv_sec = 0;
    tv.tv_usec = 0;

    FD_ZERO(&rdfs);
    FD_SET (STDIN_FILENO, &rdfs);

    select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &rdfs);
#endif
       }

//Wait for user input and saves the input in a string
       string Console::ReadLine()
{
    string str;
    char c = getchar();
    while(c != '\n')
    {
        if(c == '\r')
            continue;
        else
            str+=c;
        c = getchar();
    }
    while(c != '\n');
    return str;
}
//Set the background color
void Console::SetBGColor(const short color)
{
#ifdef _WIN32
    short val;
    switch(color)
    {
    default:
    case ColorBlack:
        val = 0;
        break;
    case ColorBlue:
        val = BACKGROUND_BLUE|BACKGROUND_INTENSITY;
        break;
    case ColorCyan:
        val =  BACKGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_INTENSITY;
        break;
    case ColorGreen:
        val = BACKGROUND_GREEN|BACKGROUND_INTENSITY;
        break;
    case ColorMagenta:
        val = BACKGROUND_BLUE|BACKGROUND_RED|BACKGROUND_INTENSITY;
        break;
    case ColorRed:
        val = BACKGROUND_RED|BACKGROUND_INTENSITY;
        break;
    case ColorWhite:
        val = BACKGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_RED|BACKGROUND_INTENSITY;
        break;
    case ColorSilver:
        val = BACKGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_RED;
        break;
    case ColorBrown:
        val = BACKGROUND_GREEN|BACKGROUND_BLUE;

        break;
    case ColorYellow:
        val = BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY;

        break;
    }
    BGColor = val;
#else
    string val;
    switch(color)
    {
    default:
    case ColorBlack:
        val = "40";
        break;
    case ColorBlue:
        val = "44";
        break;
    case ColorCyan:
        val = "46";
        break;
    case ColorGreen:
        val = "42";
        break;
    case ColorMagenta:
        val = "45";
        break;
    case ColorRed:
        val = "41";
        break;
    case ColorWhite:
        val = "47";
        break;
    case ColorYellow:
        val = "43";
        break;
    }
    BGColor = val;
#endif
    SetColor();
}
//Set the foreground color(or text color)
void Console::SetFGColor(const short color)
{

#ifdef _WIN32
    int val;
    switch(color)
    {
    default:
    case ColorBlack:
        val = 0;
        break;
    case ColorBlue:
        val = FOREGROUND_BLUE|FOREGROUND_INTENSITY;
        break;
    case ColorCyan:
        val =  FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
        break;
    case ColorGreen:
        val = FOREGROUND_GREEN|FOREGROUND_INTENSITY;
        break;
    case ColorMagenta:
        val = FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_INTENSITY;
        break;
    case ColorRed:
        val = FOREGROUND_RED|FOREGROUND_INTENSITY;
        break;
    case ColorWhite:
        val = FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY;
        break;
    case ColorSilver:
        val = FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED;
        break;
    case ColorBrown:
        val = FOREGROUND_GREEN|FOREGROUND_RED;
        break;
    case ColorYellow:
        val = FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY;
        break;
    }
    FGColor = val;
#else
    string val;
    switch(color)
    {
    case ColorBlack:
        val = "30";
        break;
    case ColorBlue:
        val = "34";
        break;
    case ColorCyan:
        val = "36";
        break;
    case ColorGreen:
        val = "32";
        break;
    case ColorMagenta:
        val = "35";
        break;
    case ColorRed:
        val = "31";
        break;
    default:
    case ColorWhite:
        val = "37";
        break;
    case ColorYellow:
        val = "33";
        break;
    }
    FGColor = val;
#endif

    SetColor();
}
//Sets the console title
void Console::SetTitle(const string str)
{
#ifdef _WIN32
    SetConsoleTitle(str.c_str());
#else
    cout << "\033]0;\0" << str << "\x07\0";
#endif
}
void Console::SetTitle(const char str[])
{
#ifdef _WIN32
    SetConsoleTitle(str);
#else
    cout << "\033]0;\0" << str << "\x07\0";
#endif
}
void Console::SetCursorPosition(int x,int y)
{
#ifdef _WIN32
    COORD pos  = {x,y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),pos);
#else
    stringstream str;
    str << "\033[" << x << ";" << y << "H";
    cout << str.str();
#endif
}
//adel
int Console::GetCursorX(void)
{
    PCONSOLE_SCREEN_BUFFER_INFO p=(PCONSOLE_SCREEN_BUFFER_INFO)malloc(sizeof(CONSOLE_SCREEN_BUFFER_INFO));
    GetConsoleScreenBufferInfo(hConsole, p);
    return p->dwCursorPosition.X;

}
int Console::GetCursorY(void)
{
    PCONSOLE_SCREEN_BUFFER_INFO p=(PCONSOLE_SCREEN_BUFFER_INFO)malloc(sizeof(CONSOLE_SCREEN_BUFFER_INFO));
    GetConsoleScreenBufferInfo(hConsole,p);
    return p->dwCursorPosition.Y;
}
void Console::WriteEx(const string str,const short fcolor,const short bcolor)
{
    short oldbcolor=BGColor;
    short oldfcolor=FGColor;

    SetFGColor(fcolor);
    SetBGColor(bcolor);
    Write(str);
    BGColor=oldbcolor;
    FGColor=oldfcolor;
    SetColor();
}
void Console::WriteEx(char str[],const short fcolor,const short bcolor)
{
    short oldbcolor=BGColor;
    short oldfcolor=FGColor;

    SetFGColor(fcolor);
    SetBGColor(bcolor);
    Write(str);
    BGColor=oldbcolor;
    FGColor=oldfcolor;
    SetColor();


}
