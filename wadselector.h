#ifndef WADSELECTOR_H
#define WADSELECTOR_H

#include <QMainWindow>
#include <QList>
#include <QFileDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class WadSelector; }
QT_END_NAMESPACE

class WadSelector : public QMainWindow
{
    Q_OBJECT

public:
    WadSelector(QWidget *parent = nullptr);
    ~WadSelector();
    QString pathToWads = getenv("HOME"), pathToIni = getenv("HOME"), defaults = getenv("HOME");
    QList<QFileInfo> fileList;
    QStringList selected;

    void grabWads();
    bool readAddedWads(QString);
    void addToIni(QString, QString);

private slots:
    void OnWadLoad();
    void OnWadSet();
    void OnIniSet();

private:
    Ui::WadSelector *ui;
};

#endif // WADSELECTOR_H
