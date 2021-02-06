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
#ifndef CONSOLE_H
#define CONSOLE_H

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <sstream>
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif
#include "keys.h"
#include "ConsoleColor.h"

using namespace std;

class Console
{
private:
#ifdef _WIN32
    static int BGColor;
    static int FGColor;
    static HANDLE hConsole;
    //adel
    static WORD LastMode;
    static BYTE TextAttr,DefaultAttr;

#else
    static string BGColor;
    static string FGColor;
#endif
    static void SetColor();
public:
    static void Clear();
    static void Write(string str);
    static void Write(int v);
    static void Write(bool v);
    static void Write(char str[]);
    static void Write(char c);
    static void Write(double d);
    static void Write(float f);
    static void WriteLine();
    static void WriteLine(string str);
    static void WriteLine(char str[]);
    static int ReadKey();
    static bool KeyDown();
    static string ReadLine();
    static void SetBGColor(const short color);
    static void SetFGColor(const short color);
    static void SetTitle(const string str);
    static void SetTitle(const char str[]);
    static void SetCursorPosition(int x,int y);
    //adel
    static int GetCursorX();
    static int GetCursorY();
    static void initConsoleScreen();
    static void HighVideo();
    static void WriteEx(const string str,const short fcolor,const short bcolor=ColorBlack);
    static void WriteEx(char str[],const short fcolor,const short bcolor=ColorBlack);
};
#endif
