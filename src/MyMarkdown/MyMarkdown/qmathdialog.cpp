#include "qmathdialog.h"

QMathDialog::QMathDialog(QString* save, QWidget* parent) : QDialog(parent)
{
    this->save = save;
    setWindowTitle("数学公式编辑器");

    hBox.addWidget(&textWidget);
    hBox.addWidget(&webViewWidget);

    hBox.setStretch(0, 1);
    hBox.setStretch(1, 1);

    this->setLayout(&hBox);
    connect(&textWidget, &QPlainTextEdit::textChanged, this, &QMathDialog::refresh);

}

QMathDialog::~QMathDialog()
{

}

void QMathDialog::refresh()
{
    *save = textWidget.toPlainText();
    webViewWidget.setUrl(QUrl("http://www.zhihu.com/equation?tex=" + QUrl::toPercentEncoding(*save)));

}

QString QMathDialog::creat()
{
    QString re;
    QMathDialog* mathDialog = new QMathDialog(&re);
    mathDialog->exec();
    delete mathDialog;
    return re;
}
