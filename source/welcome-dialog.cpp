#include "../include/welcome-dialog.h"
#include "../ui/ui_welcome-dialog.h"

WelcomeDialog::WelcomeDialog(QWidget *parent)
        : QDialog(parent)
        , ui(new Ui::WelcomeDialog)
{
    ui->setupUi(this);

    connect(ui->pushButtonQuit_, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->pushButtonOk_, &QPushButton::clicked, this, &QDialog::accept);
}

WelcomeDialog::~WelcomeDialog()
{
    delete ui;
}

QSize WelcomeDialog::getViewSize() const noexcept {
    return {ui->widthSpinBox_->value(), ui->heightSpinBox_->value()};
}
