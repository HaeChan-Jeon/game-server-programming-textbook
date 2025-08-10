#include <iostream>
#include <windows.h>
#include <thread>

using namespace std;

class CriticalSection
{
    CRITICAL_SECTION m_critSec;
public:
    CriticalSection();
    ~CriticalSection();

    void Lock();
    void UnLock();
};

class CriticalSectionLock
{
    CriticalSection* m_pCritSec;
public:
    CriticalSectionLock(CriticalSection& critSec);
    ~CriticalSectionLock();
};

CriticalSection::CriticalSection()
{
    InitializeCriticalSectionEx(&m_critSec, 0, 0);
}

CriticalSection::~CriticalSection()
{
    DeleteCriticalSection(&m_critSec);
}

void CriticalSection::Lock()
{
    EnterCriticalSection(&m_critSec);
}

void CriticalSection::UnLock()
{
    LeaveCriticalSection(&m_critSec);
}

CriticalSectionLock::CriticalSectionLock(CriticalSection& critSec)
{
    m_pCritSec = &critSec;
    m_pCritSec->Lock();
}

CriticalSectionLock::~CriticalSectionLock()
{
    m_pCritSec->UnLock();
}

int a;
CriticalSection a_mutex;
int b;
CriticalSection b_mutex;

int main()
{
    // t1 스레드를 시작한다.
    thread t1([]()
    {
       while (1)
       {
           CriticalSectionLock Lock(a_mutex);
           a++;
           CriticalSectionLock Lock2(b_mutex);
           b++;
           cout << "t1 done. \n";
       }
    });

    // t2 스레드를 시작한다.

    thread t2([]()
        {
            while (1)
            {
                CriticalSectionLock Lock(b_mutex);
                b++;
                CriticalSectionLock Lock2(a_mutex);
                a++;
                cout << "t2 done. \n";
            }
        });

    // 스레드들의 일이 끝날 때까지 기다린다.
    // 사실상 무한 반복이므로 끝나지 않는다.
    t1.join();
    t2.join();

    return 0;
}