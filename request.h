
#ifndef REQUEST_H
#define REQUEST_H

#include <QString>
#include <QRegularExpression>
#include <QVector>
#include <QMap>

struct Requests
{
    QString name;
    QStringList words;
    QStringList uniqWords;
};

class Request
{
public:
    Request();

    void addWordsToRequests(QString words, int count);
    QVector<Requests> *getRequests();
    void findOnlyUniqWords();
    void clearNotUniqWords();

private:
    QVector<Requests> request;
};

#endif // REQUEST_H
