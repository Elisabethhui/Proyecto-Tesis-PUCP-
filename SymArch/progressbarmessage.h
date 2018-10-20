#ifndef PROGRESSBARMESSAGE_H
#define PROGRESSBARMESSAGE_H

#include <QtWidgets>
#include <QWidget>

class ProgressBarMessage : public QWidget
{
    Q_OBJECT
public:
    explicit ProgressBarMessage(QWidget *parent = 0);

public Q_SLOTS:
    void setValue(int value, const QString& message);
    void reset();

private:
    QLabel* m_labelMessage;
    QProgressBar* m_progressBar;

};

#endif // PROGRESSBARMESSAGE_H
