
#include "searchengine.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SearchEngine w;
    w.show();
    return a.exec();
}
