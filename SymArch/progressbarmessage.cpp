#include "progressbarmessage.h"

ProgressBarMessage::ProgressBarMessage(QWidget *parent) : QWidget(parent)
{
    m_labelMessage = new QLabel(this);
    m_labelMessage->setMinimumWidth(200);
    m_labelMessage->setAlignment(Qt::AlignLeft);
    m_labelMessage->setText(tr("Ready"));

    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0,100);

    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(m_labelMessage);
    layout->addWidget(m_progressBar);

    setLayout(layout);
}

void ProgressBarMessage::setValue(int value, const QString &message){
    m_labelMessage->setText(message);
    m_progressBar->setValue(value);
}

void ProgressBarMessage::reset(){
    m_labelMessage->setText(tr("Ready"));
    m_progressBar->reset();
}
