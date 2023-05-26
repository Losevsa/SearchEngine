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

    void addNewDoc(const QString &doc);
    void madeFreqDictionary();
    void clearDocs();
    int sizeFreqDictionary();
    const QMap<QString, QVector<Entry>>& getFreqDictionary();
    const QMap<int, int> findWord(const QString& word);

private:
    QVector<QString> docs;
    QMap<QString, QVector<Entry>>* freqDictionary;
    QMutex writeIntoFreq;
};

#endif // DICTIONARY_H
