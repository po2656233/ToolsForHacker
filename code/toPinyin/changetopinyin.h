#ifndef CHANGETOPINYIN_H
#define CHANGETOPINYIN_H

#include <QWidget>
#include <QMap>
namespace Ui {
class ChangeToPinYin;
}

class ChangeToPinYin : public QWidget
{
    Q_OBJECT

public:
    explicit ChangeToPinYin(QWidget *parent = 0);
    ~ChangeToPinYin();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::ChangeToPinYin *ui;
    QStringList m_fileNames;
    QMap<QString,QString> m_mapfileNames;
};

#endif // CHANGETOPINYIN_H
