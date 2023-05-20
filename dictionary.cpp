#include "dictionary.h"

void addWordsToFreqDictionary(size_t docId, QMap<QString, QVector<Entry>>* freqDictionary, QMutex* mutex, QString doc);

Dictionary::Dictionary()
{
    freqDictionary = new QMap<QString, QVector<Entry>>;
}

void Dictionary::addNewDoc(QString doc)
{
    docs.push_back(doc);
}

void Dictionary::madeFreqDictionary()
{
    //Будем индексировать в 5 потоков
    QVector<QFuture<void>> threads;
    threads.reserve(5);

    QFutureWatcher<void> watcher;

    int threadsCount = 0;
    for (int i = 0; i < docs.size(); i++)
    {
        //Добавляем в вектор "потоки"
        threads.append(QtConcurrent::run(addWordsToFreqDictionary, i, freqDictionary, &writeIntoFreq, docs[i]));
        threadsCount++;

        //Если добавленных "потоков" = 5, то заходим, запускаем и ждем
        //когда закончится индексация и дальше покругу
        if (threadsCount == 5)
        {
            for (auto& thread : threads)
            {
                watcher.setFuture(thread);
            }

            watcher.waitForFinished();

            threadsCount = 0;
            threads.clear();
        }
    }

    //Если после завершения цикла остались не завершенные "потоки"
    //то запускаем и ждем окончания
    //Больше 4х потоков не будет
    for (auto& thread : threads)
    {
        watcher.setFuture(thread);
    }

    watcher.waitForFinished();
}

void Dictionary::clearDocs()
{
    docs.clear();
}

int Dictionary::sizeFreqDictionary()
{
    return freqDictionary->size();
}

QMap<QString, QVector<Entry> > *Dictionary::getFreqDictionary()
{
    return freqDictionary;
}

QMap<int, int> Dictionary::findWord(QString word)
{
    QMap<int, int> tempMap;

    if(freqDictionary->contains(word))
    {
        QVector<Entry>& entry = (*freqDictionary)[word];

        foreach (auto var, entry)
        {
            tempMap.insert(var.docId, var.count);
        }
        return tempMap;
    }
    return tempMap;
}


void addWordsToFreqDictionary(size_t docId, QMap<QString, QVector<Entry>>* freqDictionary, QMutex* mutex, QString doc)
{
    //"выписываем" все слова из doc
    // "\\s+" - означает, что мы пропускаем любые пробелы
    QRegularExpression rx("\\s+");
    QStringList words = doc.split(rx, Qt::SkipEmptyParts);

    QMap<QString, QVector<Entry>> tempDict;

    //Добавим каждое слово в новый словарь
    foreach(QString word, words)
    {
        bool haveKeys = false;
        if (tempDict.contains(word))
        {
            haveKeys = true;
            QVector<Entry>& entry = tempDict[word];

            bool haveDocId = false;

            for (QVector<Entry>::iterator it = entry.begin(); it != entry.end(); ++it)
            {
                if (it->docId == docId)
                {
                    haveDocId = true;
                    it->count++;
                }
            }
            if (!haveDocId)
            {
                Entry entr;
                entr.docId = docId;
                entr.count = 1;
                entry.append(entr);
            }
        }
        else
        {
            if (!haveKeys)
            {
                Entry entr;
                entr.docId = docId;
                entr.count = 1;
                QVector<Entry> newValue;
                newValue.append(entr);

                tempDict.insert(word, newValue);
            }
        }
    }

    //лочим главный словарь, что бы его никто не менял...
    mutex->lock();
    //Поочереди идем по новому словарю
    for(QMap<QString, QVector<Entry>>::Iterator tempDictIt = tempDict.begin(); tempDictIt != tempDict.end(); tempDictIt++)
    {
        //Проверяем, есть ли значение нового словаря в главном словаре
        if (freqDictionary->contains(tempDictIt.key()))
        {
            (*freqDictionary)[tempDictIt.key()].append(tempDictIt.value());

        }
        else
        {
            freqDictionary->insert(tempDictIt.key(), tempDictIt.value());
        }
    }
    mutex->unlock();
}