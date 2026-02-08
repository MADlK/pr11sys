
#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

#define MAX_LOGS 1000    // Максимальное количество записей в журнале
#define MAX_TEXT 128     // Максимальная длина сообщения в журнале

// Структура одной записи журнала
struct LogRecord {
    DWORD threadId;       // ID потока, создавшего запись
    int priority;         // Приоритет потока
    DWORD tick;           // Время в миллисекундах с запуска системы
    char message[MAX_TEXT]; // Сообщение
};

// Структура журнала событий
struct LogBuffer {
    LogRecord records[MAX_LOGS]; // Массив записей
    LONG index;                  // Следующий свободный индекс
};

// Общие данные
LogBuffer logBuffer;        // Журнал событий
CRITICAL_SECTION cs;        // Объект синхронизации для защиты logBuffer
bool stopThreads = false;   // Флаг завершения работы потоков
bool Cs = false;

// Генерация случайной задержки в миллисекундах
DWORD getRandomDelay(int minMs, int maxMs) {
    return minMs + rand() % (maxMs - minMs + 1);
}

// Функция записи сообщения в журнал
void logMessage(int priority, const char* msg) {
    // Входим в критическую секцию

    if (logBuffer.index < MAX_LOGS) {
        LogRecord& rec = logBuffer.records[logBuffer.index];
        rec.threadId = GetCurrentThreadId();
        rec.priority = priority;
        rec.tick = GetTickCount();
        strncpy_s(rec.message, msg, MAX_TEXT - 1);
        rec.message[MAX_TEXT - 1] = '\0';
        logBuffer.index++; // Переходим к следующему индексу
    }

    // Выходим из критической секции
}

// Функция чтения последних 5 записей журнала
void readLastLogs() {
     // Защищаем чтение
    
    int start =  logBuffer.index - 5; // Берем последние 5 записей
    for (LONG i = start; i < logBuffer.index; ++i) {
        LogRecord& rec = logBuffer.records[i];
        std::cout << "[" << rec.tick << "] ThreadID: " << rec.threadId
            << " Priority: " << rec.priority
            << " Msg: " << rec.message << std::endl;
    }
    
     // Выходим из критической секции
}

// Функция потока-логгера
DWORD WINAPI loggerThread(LPVOID param) {
    int priority = *(int*)param; 
    

    while (!stopThreads) {
        char msg[MAX_TEXT];
        if (Cs)
            EnterCriticalSection(&cs);
        sprintf_s(msg, "Log message %d from priority %d", logBuffer.index, priority);
        
            
        logMessage(priority, msg); // Записываем сообщение в журнал
        if(Cs)
            LeaveCriticalSection(&cs);
        Sleep(getRandomDelay(10, 100)); // Задержка 10-100 мс
    }
    return 0;
}



// Функция потока-наблюдателя
DWORD WINAPI observerThread(LPVOID) {
    while (!stopThreads) {
        Sleep(100);     // Задержка 100 мс
        if (Cs)
            EnterCriticalSection(&cs);
        cout << "-----------------------------------------------------" << endl;

        readLastLogs(); // Читаем последние 5 записей
        
        if (Cs)
            LeaveCriticalSection(&cs);
    }
    return 0;
}

int main() {
    setlocale(LC_ALL, "Russian");
    srand((unsigned int)time(nullptr)); // Инициализация генератора случайных чисел
    char c;
    cout << "1 - с крит секцией \n 2 - без крит секции" << endl;
    cin >> c;
        
    if (c == '1')
        Cs = true;
    else
        Cs = false;
        
    if (Cs)
        InitializeCriticalSection(&cs); // Инициализация критической секции
    logBuffer.index = 0;            // Изначально журнал пуст

    // Приоритеты потоков
    int pHigh = THREAD_PRIORITY_HIGHEST;

    int pNormal = THREAD_PRIORITY_NORMAL;
    int pLow = THREAD_PRIORITY_BELOW_NORMAL;
    int pObs = THREAD_PRIORITY_LOWEST;

    // Массив дескрипторов потоков
    HANDLE hThreads[4];

    // Создаем три потока-логгера
    hThreads[0] = CreateThread(nullptr, 0, loggerThread, &pHigh, 0, nullptr);
    hThreads[1] = CreateThread(nullptr, 0, loggerThread, &pNormal, 0, nullptr);
    hThreads[2] = CreateThread(nullptr, 0, loggerThread, &pLow, 0, nullptr);
    // Создаем поток-наблюдателя
    hThreads[3] = CreateThread(nullptr, 0, observerThread, nullptr, 0, nullptr);

    // Устанавливаем приоритеты потоков
    SetThreadPriority(hThreads[0], pHigh);
    SetThreadPriority(hThreads[1], pNormal);
    SetThreadPriority(hThreads[2], pLow);
    SetThreadPriority(hThreads[3], pObs);

    Sleep(50000);        // Работаем 5 секунд
    stopThreads = true; // Сообщаем потокам завершиться

    WaitForMultipleObjects(4, hThreads, TRUE, INFINITE); // Ждем завершения всех потоков
    for (int i = 0; i < 4; i++) CloseHandle(hThreads[i]); // Закрываем дескрипторы потоков
    if (Cs)
        DeleteCriticalSection(&cs);                            // Удаляем критическую секцию

    return 0;
}

