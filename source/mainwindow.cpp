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
#include <QLatin1String>
#include <string_view>
#include "../include/detail.h"
#include "../include/mainwindow.h"
#include "../include/rectanglemodeview.h"
#include "../include/modificationmodeview.h"
#include "../include/squaremodeview.h"
#include "../include/trianglemode.h"
#include "../include/circlemodeview.h"
#include "../include/brushmodeview.h"
#include "../include/linemodeview.h"
#include "../include/polygonmodeview.h"


namespace {
    using std::operator ""sv;

    constexpr auto kModificationModeIconPath{":/images/buttons/imgs/modimg.png"sv};
    constexpr auto kSquareModeIconPath{":/images/buttons/imgs/squareimg.png"sv};
    constexpr auto kRectangleModeIconPath{":/images/buttons/imgs/rectimg.png"sv};
    constexpr auto kTriangleModeIconPath{":/images/buttons/imgs/trianimg.png"sv};
    constexpr auto kCircleModeIconPath{":/images/buttons/imgs/circleimg.png"sv};
    constexpr auto kBrushModeIconPath{":/images/buttons/imgs/brushimg.png"sv};
    constexpr auto kLineModeIconPath{":/images/buttons/imgs/lineimg.png"sv};
    constexpr auto kPolygonModeIconPath{":/images/buttons/imgs/polygonimg.png"sv};

    constexpr auto kToolBarStyleSheetPath{":/styles/toolbarbtnstylesheet.qss"sv};

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

QSize defineWindowSize() {
    auto windowSize = detail::getScreenSize();
    return detail::calcWindowRelativeSize(windowSize, 0.15);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent},
      scene_(new QGraphicsScene{this}),
      stackedWidget_(new QStackedWidget{this}),
      toolBar_(new QToolBar{this}),
      windowSize_(defineWindowSize()),
      isModified_(false)
{
    setUpGraphicViews();
    setUpScreen();
    setUpScene();
    setUpLayout();
    setUpMenuBar();
    setUpToolBarStyle();
}

MainWindow::~MainWindow() = default;

void MainWindow::setUpToolBarStyle() {
    QFile file(kToolBarStyleSheetPath.data());
    file.open(QFile::ReadOnly);
    QString strCSS = QLatin1String(file.readAll());
    toolBar_->setStyleSheet(strCSS);
    btnList_.front()->setChecked(true);
}

void MainWindow::addMode(std::string_view iconPath, int btnIndex) {
    auto* button = addToolBarButton(iconPath);
    connect(button, &QPushButton::clicked, this, [&, button, btnIndex]() {
        stackedWidget_->setCurrentIndex(btnIndex);
        foreach(auto* btn, btnList_) {
            btn->setChecked(false);
        }
        button->setChecked(true);
        scene_->clearSelection();
    });
}

QPushButton* MainWindow::addToolBarButton(std::string_view iconPath) {
    auto* button = new QPushButton{};
    btnList_.push_back(button);
    QPixmap pixmap{iconPath.data()};

    button->setIcon(QIcon{pixmap});
    button->setIconSize(kDefaultBtnIconSize);
    button->setCheckable(true);
    button->setChecked(false);

    toolBar_->addWidget(button);
    return button;
}

void MainWindow::setUpGraphicViews() {
    setUpGraphicView<ModificationModeView>(kModificationModeIconPath);
    setUpGraphicView<SquareModeView>(kSquareModeIconPath);
    setUpGraphicView<RectangleModeView>(kRectangleModeIconPath);
    setUpGraphicView<TriangleModeView>(kTriangleModeIconPath);
    setUpGraphicView<PolygonModeView>(kPolygonModeIconPath);
    setUpGraphicView<CircleModeView>(kCircleModeIconPath);
    setUpGraphicView<LineModeView>(kLineModeIconPath);
    setUpGraphicView<BrushModeView>(kBrushModeIconPath);
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
    scene_->setItemIndexMethod(QGraphicsScene::NoIndex);                    // setting up indexing of elements - https://doc.qt.io/qt-6/qgraphicsscene.html#itemIndexMethod-prop
    scene_->setBackgroundBrush(QBrush{kDefaultSceneBackgroundColor});
    QSize sceneSize = detail::calcWindowRelativeSize(windowSize_, 0.1);
    scene_->setSceneRect(0, 0, sceneSize.width(), sceneSize.height());
}

void MainWindow::setUpScreen() {
    auto [screenWidth, screenHeight] = detail::getScreenSize();
    auto [windowWidth, windowHeight] =
            detail::calcWindowRelativeSize(QSize{screenWidth, screenHeight}, 0.15);

    setMinimumSize(windowWidth, windowHeight);
    setGeometry((screenWidth - windowWidth) / 2,
                (screenHeight - windowHeight) / 2,
                windowWidth,
                windowHeight);
}

void MainWindow::setUpMenuBar() {
    QMenu* menu = menuBar()->addMenu(kMenuName.data());
    addMenuAction(menu, kNewActionName, &MainWindow::newFile);
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

void MainWindow::saveFile() {
    if (currentFilePath_.isEmpty()) {
        saveFileAs();
    } else {
        scene_->clearSelection();
        QImage image{scene_->sceneRect().size().toSize(), QImage::Format_ARGB32};
        QPainter painter{&image};
        scene_->render(&painter);
        image.save(currentFilePath_);
        isModified_ = false;
    }
}

void MainWindow::saveFileAs() {
    const QString& filePath = QFileDialog::getSaveFileName(this,
                                                    kSaveImageTitle.data(),
                                                    QDir::homePath(),
                                                    kPngJpegBmpAllFiles.data());

    if (!filePath.isEmpty()) {
        QPixmap pixmap{scene_->sceneRect().size().toSize()};
        QPainter painter{&pixmap};
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
