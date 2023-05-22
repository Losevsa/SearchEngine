#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <QVector>
#include <QMap>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QTextBrowser>
#include <QThread>

struct Entry
{
    size_t docId, count;
};

class Dictionary
{
public:
    Dictionary();

    void addNewDoc(QString doc);
    void madeFreqDictionary();
    void clearDocs();
    int sizeFreqDictionary();
    QMap<QString, QVector<Entry>>* getFreqDictionary();
    QMap<int, int> findWord(QString word);

private:
    QVector<QString> docs;
    QMap<QString, QVector<Entry>>* freqDictionary;
    QMutex writeIntoFreq;
};

#endif // DICTIONARY_H
