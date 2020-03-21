#include "changetopinyin.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ChangeToPinYin w;
    w.show();

    return a.exec();
}
