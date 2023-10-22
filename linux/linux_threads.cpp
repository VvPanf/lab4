// Пример © Варфоломеев В.А.
// Доработан © Панферов В.Д.
// Разделяемая переменная: F
// Переменная-дублер: Fcontr
// Операция 1: при нажатии клавиши "1" -> F=F+1
// Операция 2: при нажатии клавиши "0" -> F=F-1

#include <pthread.h>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include "coniolibs.cpp"

using namespace std;

pthread_t  thread;  // дескриптор создаваемого потока
//функция потока (параметр- указатель на структуру log1)
void* ThreadProc(void* param);

const int KEY_UP = 49;           // код нажатия клавиши "1"
const int KEY_DOWN = 48;         // код нажатия клавиши "0"
const clock_t wtime=100 * 1000;  // время задержки (nc)

int F=0;                         // разделяемая глобальная переменная
// Журнал
struct log1 {
    int nom;		// порядковый номер операции
    int key;		// код нажатой клавиши
    int ID;		    // идентификатор созданного потока
    clock_t t1;	    // время начала операции
    clock_t t2;	    // время завершения операции
    int F;		    // результат операции в дочернем потоке
                    // (разделяемая переменная)
    int Fc;		    // результат операции главном потоке
                    // (переменная-дублер)
} mylog[1000];      // не более 1000 строк в журнале

//=================================================
int main()	// главный поток
{
    int i=0; 		// счетчик операций
    int j=1;     	// счетчик вывода строк журнала
    int key=1;		// код нажатой клавиши
    int flag=1;	    // признак завершения программы
    int Fcontr;	    // переменная-дублер
    mylog[1].t2=0;  // признак того, что поток не завершен
    Fcontr=F;
    cout << "№ key id start end f fc" << endl;
    // повторять пока flag !=0 или i<1000
    while (flag || i >=1000)
    {
        //Прием кода нажатой клавиши
        if (kbhit_()!= 0)
        {
            key=getch_();
            // формирование переменной-дублера
            if (key == KEY_UP) Fcontr++;
            if (key == KEY_DOWN) Fcontr--;
            //Проверка кода нажатой клавиши
            switch (key)
            {
                case 27 : {flag=0; break;}  // признак выхода из программы (Esc)
                case KEY_UP :		        // нажата клавиша "ВВЕРХ"
                case KEY_DOWN : 	        // нажата клавиша "ВНИЗ"
                {
                    i=++i; //следующая операция
                    // формируем строку журнала
                    mylog[i].nom=i;
                    mylog[i].key=key;
                    mylog[i].t2=0;
                    mylog[i].F=0;
                    mylog[i].Fc=Fcontr;
                    mylog[i].t1=clock();
                    // создание потока без немедленного запуска
                    mylog[i].ID=pthread_create(&thread,NULL,ThreadProc,&mylog[i]);

                }
            }
        }
        // завершение работы потока проверяется по
        // сформированному в функции
        // потока значению времени t2
        if (mylog[j].t2!=0)
        {
            // вывод j-й строки журнала
            cout << j << "  " << mylog[j].key << "  " << mylog[j].ID << "  " << mylog[j].t1 << "  " << mylog[j].t2 << "  " << mylog[j].F << "  " << mylog[j].Fc <<endl;
            j=++j;   // следующая строка журнала
        }
    } // повторять пока flag!=0 или i<1000
    getch_();
    return 0;
}

// функция потока: имитация "чтения", корректировки
// и "записи" разделяемой переменной F
// параметр - указатель на текущую строку журнала
void* ThreadProc(void* param)
{
    log1* p = (log1*) param;
    int Fbuf;
    // чтение разделяемой переменной в буферную переменную
    Fbuf=F;
    usleep(wtime);  // задержка
    // корректировка буферной переменной
    if ((*p).key == KEY_UP) Fbuf=++Fbuf;
    if ((*p).key == KEY_DOWN) Fbuf=--Fbuf;
    usleep(wtime);  // задержка
    // запись из буферной переменной в разделяемую переменную
    F=Fbuf;
    // запись результата в текущую строку журнала
    (*p).F=F;
    (*p).t2=clock(); // время окончания работы потока
    return 0;
}

