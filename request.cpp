
#include "request.h"

Request::Request()
{

}

void Request::addWordsToRequests(QString words, int count)
{
    QRegularExpression rx("\\s+");
    QStringList wordsList = words.split(rx, Qt::SkipEmptyParts);

    Requests temp;

    if (count < 10)
    {
        temp.name = "request00" + QString::number(count);
    }
    else if (count >= 10 && count < 100)
    {
        temp.name = "request0" + QString::number(count);
    }
    else if (count >= 100)
    {
        temp.name = "request" + QString::number(count);
    }

    foreach(QString word, wordsList)
    {
        temp.words.append(word);
    }
    request.push_back(temp);
}

QVector<Requests> *Request::getRequests()
{
    return &request;
}

void Request::findOnlyUniqWords()
{
    for (auto req = request.begin(); req != request.end(); req++)
    {
        QSet<QString> uniqueWordsSet;

        foreach (auto word, req->words)
        {
            uniqueWordsSet.insert(word);
        }

        foreach (auto uniqWord, uniqueWordsSet)
        {
            req->uniqWords.append(uniqWord);
        }
    }
}

void Request::clearNotUniqWords()
{
    for (auto req = request.begin(); req != request.end(); req++)
    {
        req->words.clear();
    }
}
