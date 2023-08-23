// @copyright Copyright (c) 2023 by Konstantin Belousov

#include "mainwindow.h"
#include "rectanglemodeview.h"
#include "modificationmodeview.h"
#include "circlemodeview.h"
#include "squaremodeview.h"
#include "trianglemode.h"

#include <QCoreApplication>
#include <QGraphicsView>
#include <QToolBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStringView>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>

namespace {
    using namespace std::string_view_literals;

    constexpr std::string_view kModificationModeIconPath{":/images/buttons/imgs/modimg.png"sv};
    constexpr std::string_view kSquareModeIconPath{":/images/buttons/imgs/squareimg.png"};
    constexpr std::string_view kRectangleModeIconPath{":/images/buttons/imgs/rectimg.png"};
    constexpr std::string_view kTriangleModeIconPath{":/images/buttons/imgs/trianimg.png"};
    constexpr std::string_view kCircleModeIconPath{":/images/buttons/imgs/circleimg.png"};

    constexpr std::string_view kMenuName{"File"};
    constexpr std::string_view kNewActionName{"New"};
    constexpr std::string_view kLoadActionName{"Load"};
    constexpr std::string_view kSaveActionName{"Save"};
    constexpr std::string_view kSaveAsActionName{"Save as..."};
    constexpr std::string_view kExitActionName{"Leave"};

    constexpr std::string_view kSaveChangesTitle{"Save Changes"};
    constexpr std::string_view kSaveChangesQuestion{"Do you want to save your changes?"};
    constexpr std::string_view kOpenTitle{"Open File"};
    constexpr std::string_view kPngJpeg{"Images (*.png *.jpg)"};
    constexpr std::string_view kSaveImageTitle{"Save Image"};
    constexpr std::string_view kPngJpegBmpAllFiles{"PNG Image (*.png);;JPEG Image (*.jpg *.jpeg);;BMP Image (*.bmp);;All Files (*)"};
    constexpr std::string_view kSaveChagesAndExitTitle{"Save Changes and Exit"};
    constexpr std::string_view kSaveChagesAndExitQuestion{"Do you want to save your changes before exiting?"};

    constexpr Qt::GlobalColor kDefaultSceneBackgroundColor{Qt::white};
    constexpr QSize kDefaultBtnIconSize{30,30};
    constexpr int kMinimumWidth{1000};
    constexpr int kMinimumHeight{700};
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent},
      scene_(new QGraphicsScene{this}),
      stackedWidget_(new QStackedWidget{this}),
      toolBar_(new QToolBar{this}),
      isModified_(false)
{
    setMinimumSize(kMinimumWidth, kMinimumHeight);
    setUpGraphicViews();
    setUpScene();
    setUpLayout();
    setUpMenuBar();
}

MainWindow::~MainWindow() {
    delete scene_;
    delete stackedWidget_;
    delete toolBar_;
}

void MainWindow::addMode(std::string_view iconPath, int btnIndex) {
    auto* button = new QPushButton{};
    QPixmap pixmap{iconPath.data()};
    button->setIcon(QIcon{pixmap});
    button->setIconSize(kDefaultBtnIconSize);
    toolBar_->addWidget(button);
    connect(button, &QPushButton::clicked, this, [=]() {
        stackedWidget_->setCurrentIndex(btnIndex);
    });
}

void MainWindow::setUpGraphicViews() {
    auto modificationSceneIndex = stackedWidget_->addWidget(new ModificationModeView{scene_});
    addMode(kModificationModeIconPath, modificationSceneIndex);
    auto squareSceneIndex = stackedWidget_->addWidget(new SquareModeView{scene_});
    addMode(kSquareModeIconPath, squareSceneIndex);
    auto rectangleSceneIndex = stackedWidget_->addWidget(new RectangleModeView{scene_});
    addMode(kRectangleModeIconPath, rectangleSceneIndex);
    auto triangleSceneIndex = stackedWidget_->addWidget(new TriangleModeView{scene_});
    addMode(kTriangleModeIconPath, triangleSceneIndex);
    auto circleSceneIndex = stackedWidget_->addWidget(new CircleModeView{scene_});
    addMode(kCircleModeIconPath, circleSceneIndex);
}

void MainWindow::setUpLayout() {
    auto* layout = new QVBoxLayout{};
    layout->addWidget(toolBar_);
    layout->addWidget(stackedWidget_);

    auto* centralWidget = new QWidget{};
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
}

void MainWindow::setUpScene() {
    scene_->setBackgroundBrush(QBrush{kDefaultSceneBackgroundColor});
    auto* item = scene_->addRect(QRectF{QPointF{0,0}, QSizeF(1, 1)},
                                                         QPen{Qt::black},
                                                         QBrush{Qt::black}); // todo: center for tests
}

void MainWindow::setUpMenuBar() {
    QMenu* menu = menuBar()->addMenu(kMenuName.data());
    addMenuAction(menu, kNewActionName, &MainWindow::newFile);
    addMenuAction(menu, kLoadActionName, &MainWindow::loadFile);
    addMenuAction(menu, kSaveActionName, &MainWindow::saveFile);
    addMenuAction(menu, kSaveAsActionName, &MainWindow::saveFileAs);
    addMenuAction(menu, kExitActionName, &MainWindow::exitApp);
}

void MainWindow::newFile() {
    if (isModified_) {
        auto answer = QMessageBox::warning(this,
                                           kSaveChangesTitle.data(),
                                           kSaveChangesQuestion.data(),
                                           QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (answer == QMessageBox::Save) {
            saveFile();
        } else if (answer == QMessageBox::Cancel) {
            return;
        }
    }

    scene_->clear();
    isModified_ = false;
}

void MainWindow::loadFile() {
    if (isModified_) {
        auto answer = QMessageBox::warning(this, kSaveChangesTitle.data(),
                                           kSaveChangesQuestion.data(),
                                           QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (answer == QMessageBox::Save) {
            saveFile();
        } else if (answer == QMessageBox::Cancel) {
            return;
        }
    }

    QString filePath = QFileDialog::getOpenFileName(this,
                                                    kOpenTitle.data(),
                                                    QDir::homePath(),
                                                    kPngJpeg.data());
    if (!filePath.isEmpty()) {
        QImage image(filePath);
        if (!image.isNull()) {
            scene_->clear();
            scene_->addPixmap(QPixmap::fromImage(image));
            isModified_ = false;
        }
    }
}

void MainWindow::saveFile() {
    if (currentFilePath_.isEmpty()) {
        saveFileAs();
    } else {
        scene_->clearSelection();
        QImage image(scene_->sceneRect().size().toSize(), QImage::Format_ARGB32);
        image.fill(Qt::white);
        QPainter painter(&image);
        scene_->render(&painter);
        image.save(currentFilePath_);
        isModified_ = false;
    }
}

void MainWindow::saveFileAs() {
    QString filePath = QFileDialog::getSaveFileName(this,
                                                    kSaveImageTitle.data(),
                                                    QDir::homePath(),
                                                    kPngJpegBmpAllFiles.data());
    if (!filePath.isEmpty()) {
        QPixmap pixmap(scene_->sceneRect().size().toSize());
        QPainter painter(&pixmap);
        scene_->render(&painter);
        pixmap.save(filePath);
        isModified_ = false;
    }
}

void MainWindow::exitApp() {
    if (isModified_) {
        auto answer = QMessageBox::warning(this, kSaveChagesAndExitTitle.data(),
                                           kSaveChagesAndExitQuestion.data(),
                                           QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (answer == QMessageBox::Save) {
            saveFile();
        } else if (answer == QMessageBox::Cancel) {
            return;
        }
    }
    QCoreApplication::exit();
}
