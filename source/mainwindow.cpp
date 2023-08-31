// @copyright Copyright (c) 2023 by Konstantin Belousov

#include <QCoreApplication>
#include <QGuiApplication>
#include <QGraphicsView>
#include <QToolBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStringView>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QScreen>
#include <string_view>
#include "../include/mainwindow.h"
#include "../include/rectanglemodeview.h"
#include "../include/modificationmodeview.h"
#include "../include/squaremodeview.h"
#include "../include/trianglemode.h"
#include "../include/circlemodeview.h"


namespace {
    using std::operator ""sv;

    constexpr auto kModificationModeIconPath{":/images/buttons/imgs/modimg.png"sv};
    constexpr auto kSquareModeIconPath{":/images/buttons/imgs/squareimg.png"sv};
    constexpr auto kRectangleModeIconPath{":/images/buttons/imgs/rectimg.png"sv};
    constexpr auto kTriangleModeIconPath{":/images/buttons/imgs/trianimg.png"sv};
    constexpr auto kCircleModeIconPath{":/images/buttons/imgs/circleimg.png"sv};

    constexpr auto kMenuName{"File"sv};
    constexpr auto kNewActionName{"New"sv};
    constexpr auto kLoadActionName{"Load"sv};
    constexpr auto kSaveActionName{"Save"sv};
    constexpr auto kSaveAsActionName{"Save as..."sv};
    constexpr auto kExitActionName{"Leave"sv};

    constexpr auto kSaveChangesTitle{"Save Changes"sv};
    constexpr auto kSaveChangesQuestion{"Do you want to save your changes?"sv};
    constexpr auto kOpenTitle{"Open File"sv};
    constexpr auto kPngJpeg{"Images (*.png *.jpg)"sv};
    constexpr auto kSaveImageTitle{"Save Image"sv};
    constexpr auto kPngJpegBmpAllFiles{"PNG Image (*.png);;JPEG Image (*.jpg *.jpeg);;BMP Image (*.bmp);;All Files (*)"sv};
    constexpr auto kSaveChangesAndExitTitle{"Save Changes and Exit"sv};
    constexpr auto kSaveChangesAndExitQuestion{"Do you want to save your changes before exiting?"sv};

    constexpr Qt::GlobalColor kDefaultSceneBackgroundColor{Qt::white};
    constexpr QSize kDefaultBtnIconSize{30, 30};

}  // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent},
      scene_(new QGraphicsScene{this}),
      stackedWidget_(new QStackedWidget{this}),
      toolBar_(new QToolBar{this}),
      isModified_(false)
{
    setUpGraphicViews();
    setUpScene();
    setUpScreen();
    setUpLayout();
    setUpMenuBar();
}

MainWindow::~MainWindow() {}

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
    setUpGraphicView<ModificationModeView>(kModificationModeIconPath);
    setUpGraphicView<SquareModeView>(kSquareModeIconPath);
    setUpGraphicView<RectangleModeView>(kRectangleModeIconPath);
    setUpGraphicView<TriangleModeView>(kTriangleModeIconPath);
    setUpGraphicView<CircleModeView>(kCircleModeIconPath);
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
}

void MainWindow::setUpScreen() {
    QScreen* screen = QGuiApplication::primaryScreen();
    QSize size = screen->size();
    int screenWidth = size.width();
    int screenHeight = size.height();
    int windowWidth = screenWidth - static_cast<int>(screenWidth * 0.15);
    int windowHeight = screenHeight - static_cast<int>(screenHeight * 0.15);
    setMinimumSize(windowWidth, windowHeight);
    setGeometry((screenWidth - windowWidth) / 2,
                (screenHeight - windowHeight) / 2,
                windowWidth,
                windowHeight);
}

void MainWindow::setUpMenuBar() {
    QMenu* menu = menuBar()->addMenu(kMenuName.data());
    addMenuAction(menu, kNewActionName, &MainWindow::newFile);
    addMenuAction(menu, kLoadActionName, &MainWindow::loadFile);
    addMenuAction(menu, kSaveActionName, &MainWindow::saveFile);
    addMenuAction(menu, kSaveAsActionName, &MainWindow::saveFileAs);
    addMenuAction(menu, kExitActionName, &MainWindow::exitApp);
}

void MainWindow::changeSceneState() {
    isModified_ = true;
}

void MainWindow::newFile() {
    if (isModified_) {
        auto answer = QMessageBox::warning(this,
                                           kSaveChangesTitle.data(),
                                           kSaveChangesQuestion.data(),
                                           QMessageBox::Save |
                                           QMessageBox::Discard |
                                           QMessageBox::Cancel);

        if (answer == QMessageBox::Save) saveFile();
        else if (answer == QMessageBox::Cancel) return;
    }

    scene_->clear();
    isModified_ = false;
}

void MainWindow::loadFile() {
    if (isModified_) {
        auto answer = QMessageBox::warning(this, kSaveChangesTitle.data(),
                                           kSaveChangesQuestion.data(),
                                           QMessageBox::Save |
                                           QMessageBox::Discard |
                                           QMessageBox::Cancel);

        if (answer == QMessageBox::Save) saveFile();
        else if (answer == QMessageBox::Cancel) return;
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
        auto answer = QMessageBox::warning(this,
                                           kSaveChangesAndExitTitle.data(),
                                           kSaveChangesAndExitQuestion.data(),
                                           QMessageBox::Save |
                                           QMessageBox::Discard |
                                           QMessageBox::Cancel);

        if (answer == QMessageBox::Save) saveFile();
        else if (answer == QMessageBox::Cancel) return;
    }
    QCoreApplication::exit();
}
