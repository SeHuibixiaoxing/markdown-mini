#ifndef QMATHDIALOG_H
#define QMATHDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QWebEngineView>
#include <QString>

class QMathDialog : public QDialog
{
public:
    QString* save;
    QPlainTextEdit textWidget;
    QWebEngineView webViewWidget;
    QHBoxLayout hBox;

    QMathDialog(QString* save, QWidget* parent = nullptr);
    ~QMathDialog();

    static QString creat();

public slots:
    void refresh();
};

#endif // QMATHDIALOG_H
