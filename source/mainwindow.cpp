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

    constexpr Qt::GlobalColor kDefaultSceneBackgroundColor{Qt::white};
    constexpr QSize kDefaultBtnIconSize{30, 30};

}  // namespace

QSize defineWindowSize() {
    auto windowSize = detail::getScreenSize();
    return detail::calcWindowRelativeSize(windowSize, 0.15);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent},
      graphicsScene_(new QGraphicsScene{this}),
      stackedWidget_(new QStackedWidget{this}),
      modeButtonsToolBar_(new QToolBar{this}),
      windowSize_(defineWindowSize()),
      isModified_(false)
{
    setUpGraphicViews();
    setUpScreen();
    setUpScene();
    setUpLayout();
    setUpToolBarStyle();
}

MainWindow::~MainWindow() = default;

void MainWindow::setUpToolBarStyle() {
    QFile file(kToolBarStyleSheetPath.data());
    file.open(QFile::ReadOnly);
    QString strCSS = QLatin1String(file.readAll());
    modeButtonsToolBar_->setStyleSheet(strCSS);
    modeButtonsList_.front()->setChecked(true);
}

void MainWindow::addMode(std::string_view iconPath, int btnIndex) {
    auto* button = addToolBarButton(iconPath);
    connect(button, &QPushButton::clicked, this, [&, button, btnIndex]() {
        stackedWidget_->setCurrentIndex(btnIndex);
        for (auto* btn : modeButtonsList_) {
            btn->setChecked(false);
        }
        button->setChecked(true);
        graphicsScene_->clearSelection();
    });
}

QPushButton* MainWindow::addToolBarButton(std::string_view iconPath) {
    auto* button = new QPushButton{};
    modeButtonsList_.push_back(button);
    QPixmap pixmap{iconPath.data()};

    button->setIcon(QIcon{pixmap});
    button->setIconSize(kDefaultBtnIconSize);
    button->setCheckable(true);
    button->setChecked(false);

    modeButtonsToolBar_->addWidget(button);
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
    layout->addWidget(modeButtonsToolBar_);
    layout->addWidget(stackedWidget_);

    auto* centralWidget = new QWidget{};
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
}

void MainWindow::setUpScene() {
    graphicsScene_->setItemIndexMethod(QGraphicsScene::NoIndex);                    // setting up indexing of elements - https://doc.qt.io/qt-6/qgraphicsscene.html#itemIndexMethod-prop
    graphicsScene_->setBackgroundBrush(QBrush{kDefaultSceneBackgroundColor});
    QSize sceneSize = detail::calcWindowRelativeSize(windowSize_, 0.1);
    graphicsScene_->setSceneRect(0, 0, sceneSize.width(), sceneSize.height());
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

void MainWindow::changeSceneState() {
    isModified_ = true;
}
