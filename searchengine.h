
#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include <QMainWindow>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileInfo>

#include "dictionary.h"
#include "request.h"
#include "answer.h"
#include "jsonwriter.h"


struct Configuration
{
    QString name = "SkillboxSearchEngine";
    double version = 0.1;
    int maxResponses = 5;

    QVector<QString> filesPath;
    int doscCount = 0;
};



QT_BEGIN_NAMESPACE
namespace Ui { class SearchEngine; }
QT_END_NAMESPACE

class SearchEngine : public QMainWindow

{
    Q_OBJECT

public:
    SearchEngine(QWidget *parent = nullptr);
    ~SearchEngine();

    bool loadConfig();
    void readFiles();
    void showFreqDictionary();
    void loadRequest();
    void showRequests();
    void addRequestToAsnwers();
    void findDocsForAnswer();
    void countRelevance();
    void writeAnswers();


private:
    Ui::SearchEngine *ui;
    Configuration config;
    Dictionary dictionary;
    Request request;
    Answer answer;
    double maxRelevance = 0;
};

#endif // SEARCHENGINE_H
