#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class WelcomeDialog; }
QT_END_NAMESPACE

class WelcomeDialog : public QDialog
{
 Q_OBJECT

 public:
    WelcomeDialog(QWidget *parent = nullptr);
    ~WelcomeDialog();

    [[nodiscard]] QSize getViewSize() const noexcept;

 private:
    Ui::WelcomeDialog *ui;
};
