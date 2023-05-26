#ifndef TESTDICTIONARY_H
#define TESTDICTIONARY_H

#include <QtTest/QTest>

#include "dictionary.h"


class TestDictionary: public QObject
{
    Q_OBJECT
public:
    TestDictionary();
/*
    void test()
    {
        Dictionary dictionary;

        for (int i = 0; i < config.filesPath.size(); i++)
        {
            QFile file(config.filesPath[i]);
            if(!file.open(QFile::ReadOnly | QFile::Text))
            {
                continue;
            }
            QString str = file.readAll();
            dictionary.addNewDoc(str);
        }

        //Индексируем все добавленные файлы
        dictionary.madeFreqDictionary();
    }
*/
};

#endif // TESTDICTIONARY_H
