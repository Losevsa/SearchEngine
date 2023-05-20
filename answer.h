
#ifndef ANSWER_H
#define ANSWER_H

#include <QString>
#include <QMap>
#include <QVector>

struct Answers
{
    QString request = "";
    QStringList words;

    bool result = false;
    QMap<int, int> maxCount;
    //int - номер документа, double - его ранг
    QMap<int,double> relevance;
    QMap<double, int> sortedRelevance;
};


class Answer
{
public:
    Answer();

    void addRequests(Answers answer);
    int getSizeOfAnswer();
    Answers *getAnswer(int index);

private:
    QVector<Answers> answer;
};

#endif // ANSWER_H
