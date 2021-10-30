#include "progressBarui.h"
#include "ui_progressBarui.h"

ProgressBarUI::ProgressBarUI(Context* context,QWidget *parent) : QDialog(parent),
    ui(new Ui::Scheduler),context(context),errored(false),confirmBox(this){
    ui->setupUi(this);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setWindowModality(Qt::ApplicationModal);

    confirmBox.setIcon(QMessageBox::Question);
    confirmBox.setWindowTitle(QApplication::applicationName());
    confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirmBox.setDefaultButton(QMessageBox::No);
    confirmBox.setButtonText(QMessageBox::Yes,"接收");
    confirmBox.setButtonText(QMessageBox::No,"拒绝");
    connect(&confirmBox,&QMessageBox::finished,this,&ProgressBarUI::response);

    context->setParent(this);
    connect(context,&Context::printLogMsg,this,&ProgressBarUI::printLog);
    connect(context,&Context::updateProgressBar,this,&ProgressBarUI::updateBar);
    connect(context,&Context::raiseErrorMsg,this,&ProgressBarUI::errorAppear);
    connect(context,&Context::metaDataReady,this,&ProgressBarUI::openConfirmDialog);

    // 结束传送，关闭当前窗口，发送打开下一个传送信号。
    connect(context,&Context::raiseEndSignal,this,&ProgressBarUI::transferFinish);

    context->exchange_key();
}

ProgressBarUI::~ProgressBarUI(){
    delete ui;
}

void ProgressBarUI::transferFinish(){
    emit openNextTask();
    done(QDialog::Accepted);
}

void ProgressBarUI::closeEvent(QCloseEvent *event){
    event->accept();
    emit transferInterruptedByUser();
}

void ProgressBarUI::response(int result){
    confirmBoxTimer.stop();
    bool res=result==QMessageBox::Yes;
    // 这里是一个虚函数，Sender的实现没有意义
    // 如果同意接收，Receiver开始根据传输队列创建文件，不同意接收则触发结束信号
    context->response(res);
    if(!res)
        hide();
}

void ProgressBarUI::updateBar(double progress){
    ui->progressBar->setValue(ui->progressBar->maximum() * progress);
}

void ProgressBarUI::errorAppear(const QString &err){
    if(errored)
        return;
    errored=true;
    if(isVisible()){
        informationBox=new QMessageBox(this);
        informationBox->setWindowTitle(QApplication::applicationName());
        informationBox->setText(err);
        informationBox->show();
        QTimer::singleShot(2000,this,[=](){
            informationBox->done(QDialog::Accepted);
            emit openNextTask();
            done(QDialog::Rejected);
        });
    }else{
        emit openNextTask();
        done(QDialog::Rejected);
    }
    // 这里会触发finish函数，finish和response函数连接在了一起
}

/**
 * @brief 只有接收端会用到下面的函数，弹窗询问是否接收文件。
 * @param metaData
 * @param totalSize
 * @param deviceName
 * @param keyDigest
 */
void ProgressBarUI::openConfirmDialog(const QVector<FileMetadata> &metaData, quint64 totalSize,
                                      const QString &deviceName, const QString &keyDigest){
    show();
    QString totalSizeStr=locale().formattedDataSize(totalSize,2,QLocale::DataSizeTraditionalFormat);
    QString hint;
    if(metaData.size()==1){
        hint=QString("%1 向您分享文件： \"%2\" 大小为： %3。")
                .arg(deviceName, metaData.front().filename, totalSizeStr);
    }else{
        hint = QString("%1 向您分享 %2 个文件，总大小为 %3.")
                        .arg(deviceName).arg(metaData.size()).arg(totalSizeStr);
    }
    hint += QString("\n确认在发送设备上显示了验证码： \"%1\" 。").arg(keyDigest);
    hint += QString("\n是否接收文件？");

    confirmBox.setText(hint);
    confirmBox.show();

    // 这里写定时操作，超过五秒钟自动拒绝接收
    autoRefuse=5;
    connect(&confirmBoxTimer,&QTimer::timeout,this,&ProgressBarUI::autoPick);
    confirmBoxTimer.start(1000);
}

void ProgressBarUI::printLog(const QString msg)
{
    ui->statusLabel->setText(msg);
    ui->history_log->addItem(new QListWidgetItem(msg));
}

void ProgressBarUI::autoPick()
{
    if(autoRefuse<0){
        confirmBoxTimer.stop();
        confirmBox.done(QDialog::Rejected);
    }else{
        confirmBox.setButtonText(QMessageBox::No,QString("拒绝（%1s）").arg(autoRefuse));
        autoRefuse-=1;
    }
}

