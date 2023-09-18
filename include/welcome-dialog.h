// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class WelcomeDialog; }
QT_END_NAMESPACE

class WelcomeDialog : public QDialog
{
 Q_OBJECT

 public:
    explicit WelcomeDialog(QWidget *parent = nullptr);
    ~WelcomeDialog() override;

    [[nodiscard]] QSize getViewSize() const noexcept;

 protected:
    void keyPressEvent(QKeyEvent* event) override;

 private:
    Ui::WelcomeDialog *ui;
};
