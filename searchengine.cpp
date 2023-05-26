
#include "searchengine.h"
#include "ui_searchengine.h"


SearchEngine::SearchEngine(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SearchEngine)
{
    ui->setupUi(this);

    //Если загрузим config.json - ok
    if (!loadConfig())
    {
        ui->centralwidget->hide();
    }
    else
    {
        //Выставим то, что прочитали
        this->setWindowTitle(config.name + " ver. " +  QString::number(config.version));
        ui->serverName->setText(config.name);
        ui->serverVer->setText(QString::number(config.version));
        ui->maxResponse->setText(QString::number(config.maxResponses));

        //Отобразим каждый файл из конфига, если он открывается, то говорим "ок"
        for (int i = 0; i < config.doscCount; i++)
        {
            QFileInfo fileInfo(config.filesPath[i]);
            QFile file(config.filesPath[i]);

            if (file.open(QFile::ReadOnly))
            {
                QString ok = "☑";
                QColor myColor(Qt::green);
                ok.prepend("<font color=\"" + myColor.name() + "\">");
                ok.append("</font>");
                ui->fileList->append(fileInfo.absoluteFilePath() + " - " + ok);
            }
            else
            {
                QString bad = "☒";
                QColor myColor(Qt::red);
                bad.prepend("<font color=\"" + myColor.name() + "\">");
                bad.append("</font>");
                ui->fileList->append(fileInfo.absoluteFilePath() + " - " + bad);
            }

            file.close();
        }

        showFreqDictionary();

        loadRequest();
        request.findOnlyUniqWords();
        showRequests();
        request.clearNotUniqWords();

        addRequestToAsnwers();

        findDocsForAnswer();

        countRelevance();

        writeAnswers();

        QTest::qExec(new TestDictionary);
    }


}

SearchEngine::~SearchEngine()
{
    delete ui;
}

bool SearchEngine::loadConfig()
{
    //Открываем config.json
    //Если не получилось то пишем в статусбар ошибку
    QFile configFile("config.json");
    if(!configFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui->statusbar->showMessage("Can't open config.json");
        return false;
    }


    //Проверяем файл на корректность
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(configFile.readAll(), &error);
    configFile.close();

    if (jsonDoc.isNull()) {
        ui->statusbar->showMessage("Failed to parse JSON:" + error.errorString());
        return false;
    }

    if (!jsonDoc.isObject()) {
        ui->statusbar->showMessage("JSON is not an object");
        return false;
    }

    // Получаем объект конфигурации
    QJsonObject configJson = jsonDoc.object().value("config").toObject();
    QString name = configJson.value("name").toString();
    QString version = configJson.value("version").toString();

    //Обязательно версия из конфига должна быть как мы задали, иначе не работаем
    if (version.toDouble() != config.version)
    {
        ui->statusbar->showMessage("Incorect config version");
        return false;
    }

    //Проверка корректности "ответов"
    if (configJson.value("max_responses").toInt() > 0 && configJson.value("max_responses").toInt() <= 10)
    {
        config.maxResponses = configJson.value("max_responses").toInt();
    }

    config.name = name;

    // Получаем массив файлов
    QJsonArray files = jsonDoc.object().value("files").toArray();

    for(const QJsonValue& value: files)
    {
        config.filesPath.push_back(value.toString());
    }

    readFiles();
    //checkFreq();
    config.doscCount = config.filesPath.size();

    return true;
}

void SearchEngine::readFiles()
{
    //Читаем каждый файл и записываем в request.docs для последующей индексации
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

    //Очищаем память, от документов
    //requests.clearDocs();
}

void SearchEngine::showFreqDictionary()
{
    for(QMap<QString, QVector<Entry>>::ConstIterator it = dictionary.getFreqDictionary().begin();
         it != dictionary.getFreqDictionary().end(); it++)
    {
        QString out;
        out = "index[\"" + it.key() + "\"]=";
        QVector<Entry> entr = it.value();
        for(int i = 0; i < entr.size(); i++)
        {
            out += "{" + QString::number(entr[i].docId) + "," + QString::number(entr[i].count) + "}";
            if (i != entr.size() - 1)
            {
                out += ", ";
            }
        }
        out += ";";
        ui->wordsIndex->append(out);
    }
}

void SearchEngine::loadRequest()
{
    QFile configFile("requests.json");
    if(!configFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui->statusbar->showMessage("Can't open requests.json");
        return;
    }

    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(configFile.readAll(), &error);
    configFile.close();

    if (jsonDoc.isNull()) {
        ui->statusbar->showMessage("Failed to parse JSON:" + error.errorString());
        return;
    }

    if (!jsonDoc.isObject()) {
        ui->statusbar->showMessage("JSON is not an object");
        return;
    }

    // Получаем объект конфигурации
    //QJsonObject configJson = jsonDoc.object().value("requests").toObject();
    // Получаем массив файлов
    QJsonArray files = jsonDoc.object().value("requests").toArray();
    int count = 1;
    for (const QJsonValue& value: files)
    {
        request.addWordsToRequests(value.toString(), count);
        count++;
    }
}

void SearchEngine::showRequests()
{
    for(QVector<Requests>::ConstIterator it = request.getRequests().begin(); it != request.getRequests().end(); it++)
    {
        QString fullRequest = it->name + ": ";

        foreach (auto word, it->words)
        {
            fullRequest += word + ", ";
        }

        ui->requests->append(fullRequest);

        QString uniqRequest = it->name + ": ";

        foreach (auto word, it->uniqWords)
        {
            uniqRequest += word + ", ";
        }

        ui->uniqWords->append(uniqRequest);
    }
}

void SearchEngine::addRequestToAsnwers()
{
    for(QVector<Requests>::ConstIterator it = request.getRequests().begin(); it != request.getRequests().end(); it++)
    {
        Answers temp;

        temp.request = it->name;
        temp.words = it->uniqWords;

        answer.addRequests(temp);
    }
}

void SearchEngine::findDocsForAnswer()
{
    //Загружаем каждый "ответ"
    for (int i = 0; i < answer.getSizeOfAnswer(); i++)
    {
        Answers temp = answer.getAnswer(i);

        //Смотрим на каждое слово в нашем ответе
        foreach (auto word, temp.words)
        {
            //отправляем запрос на совпадения по этому слову в словаре
            QMap<int, int> docWithCount =  dictionary.findWord(word);

            for (QMap<int, int>::Iterator it = docWithCount.begin(); it != docWithCount.end(); ++it)
            {
                int newValue = it.value() + temp.maxCount[it.key()];

                if(maxRelevance < newValue)
                {
                    maxRelevance = newValue;
                }

                temp.maxCount.insert(it.key(),newValue);
                temp.result = true;
            }
        }

        if (temp.result)
        {
            ui->answer->append(temp.request);

            for (QMap<int, int>::Iterator it = temp.maxCount.begin(); it != temp.maxCount.end(); ++it)
            {
                ui->answer->append("    " + QString::number(it.key()) + " " + QString::number(it.value()));
            }
        }
        else
        {
            ui->answer->append(temp.request);
            ui->answer->append("    Not found");
        }
    }
}

void SearchEngine::countRelevance()
{
    for (int i = 0; i < answer.getSizeOfAnswer(); i++)
    {
        Answers temp = answer.getAnswer(i);

        for (QMap<int, int>::Iterator it = temp.maxCount.begin(); it != temp.maxCount.end(); ++it)
        {
            double relevance;
            relevance = it.value() / maxRelevance;
            temp.relevance.insert(it.key(),relevance);
            temp.sortedRelevance.insert(relevance, it.key());
        }
    }
}

void SearchEngine::writeAnswers()
{
    QFile answerFile("answers.json");
    if(answerFile.open(QFile::WriteOnly | QIODevice::Text))
    {
        QTextStream fileStream(&answerFile);
        fileStream << "{\n";
        fileStream << "\t\"answers\": {\n";

        for(int i = 0; i < answer.getSizeOfAnswer();i++)
        {
            Answers tempAnswer = answer.getAnswer(i);

            fileStream << "\t\t\"" + tempAnswer.request + "\": {\n";
            if (tempAnswer.result)
            {
                fileStream << "\t\t\t\"result\": \"true\",\n";
            }
            else
            {
                fileStream << "\t\t\t\"result\": \"false\"\n";
                if (i + 1 >= answer.getSizeOfAnswer())
                {
                    fileStream << "\t\t}\n";
                }
                else
                {
                    fileStream << "\t\t},\n";
                }

                continue;
            }
            if (tempAnswer.sortedRelevance.size() > 1)
            {
                fileStream << "\t\t\t\"relevance\":{ \n";

                int currentRow = 1;

                QList<double> keys = tempAnswer.sortedRelevance.keys();
                QList<int> values = tempAnswer.sortedRelevance.values();

                for(int i = keys.size() - 1; i >= 0; i--)
                {
                    fileStream << "\t\t\t\t\"docid\": " + QString::number(values[i]) + ", \"rank\" : " + QString::number(keys[i], 'f', 3);

                    if (currentRow < tempAnswer.sortedRelevance.size() && currentRow < config.maxResponses)
                    {
                        fileStream << ", \n";
                    }
                    else
                    {
                        fileStream << "\n \t\t\t} \n";
                        break;
                    }

                    currentRow++;
                }

                fileStream << "\t\t},\n";
            }
            else
            {
                QList<double> keys = tempAnswer.sortedRelevance.keys();
                QList<int> values = tempAnswer.sortedRelevance.values();

                for(int i = keys.size() - 1; i >= 0; i--)
                {
                    fileStream << "\t\t\t\"docid\": " + QString::number(values[i])
                                      + ", \"rank\" : " + QString::number(keys[i], 'f', 3) + "\n";
                    fileStream << "\t\t},\n";
                }
            }
        }
        fileStream << "\t}\n";
        fileStream << "}";
    }
}

