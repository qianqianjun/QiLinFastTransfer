#ifndef PROGRESSBARUI_H
#define PROGRESSBARUI_H

#include <QDialog>
#include <context.h>
#include <QMessageBox>
#include <util.h>
#include <QListWidget>
#include <QListWidgetItem>
#include <QCloseEvent>
namespace Ui {
class Scheduler;
}

class ProgressBarUI : public QDialog
{
    Q_OBJECT
private:
    Ui::Scheduler *ui;
    Context *context;
    bool errored;
    int autoRefuse;
    QTimer confirmBoxTimer;
    QMessageBox confirmBox;
    QMessageBox* informationBox;
private slots:
    void response(int result);
    void updateBar(double progress);
    void errorAppear(const QString& err);
    void openConfirmDialog(const QVector<FileMetadata> &metaData,quint64 totalSize,
                           const QString &deviceName,const QString &keyDigest);
    void printLog(const QString msg);
    void autoPick();
protected:
    void closeEvent(QCloseEvent* event);
public:
    explicit ProgressBarUI(Context *context,QWidget *parent = nullptr);
    ~ProgressBarUI();
public slots:
    void transferFinish();
signals:
    void transferInterruptedByUser();
    void openNextTask();
};

#endif // PROGRESSBARUI_H
