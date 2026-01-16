// pr11Sys.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <Windows.h>
using namespace std;

#define MAX_LOGS  1000
#define MAX_TEXT  128

struct LogRecord
{
    DWORD ThreadId;
    int prioriti;
    DWORD tick;
    char message[MAX_TEXT];
};




struct LogBuffer
{
    LogRecord record[MAX_LOGS];
    LONG index;
};


LogRecord lrHigest;
LogBuffer lbHigest;
//logger
DWORD WINAPI THREAD__PRIORITY_HIGHEST(LPVOID q)
{
    
    lrHigest.ThreadId = GetCurrentThreadId();
    lrHigest.prioriti=THREAD_PRIORITY_HIGHEST;
    lrHigest.tick = GetTickCount();
    strcpy(lrHigest.message, "asd");

}


//logger
DWORD WINAPI THREAD__PRIORITY_NORMAL(LPVOID q)
{

}

//logger
DWORD WINAPI THREAD__PRIORITY_BELOW_NORMAL(LPVOID q)
{

}

//observer
DWORD WINAPI THREAD__PRIORITY_LOWEST(LPVOID q)
{

}

int main()
{

    
    
    HANDLE hLog = CreateThread(NULL, 0, THREAD__PRIORITY_HIGHEST, NULL, 0, NULL);
    SetThreadPriority(&hLog,THREAD_PRIORITY_HIGHEST);
}


