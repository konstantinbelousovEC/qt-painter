// @copyright Copyright (c) 2023 by Konstantin Belousov

#include <QScreen>
#include <QGuiApplication>
#include "../include/detail.h"

namespace detail {

    QSize getScreenSize() {
        QScreen* screen = QGuiApplication::primaryScreen();
        return screen->size();
    }

    QSize calcWindowRelativeSize(QSize wSize, double x) {
        auto [screenWidth, screenHeight] = wSize;
        int windowWidth = screenWidth - static_cast<int>(screenWidth * 0.15);
        int windowHeight = screenHeight - static_cast<int>(screenHeight * 0.15);
        return {windowWidth, windowHeight};
    }

    QRectF updateRectangleSize(const QPointF& startCursorPos, const QPointF& currentCursorPos) noexcept {
        qreal currentWidth = currentCursorPos.x() - startCursorPos.x();
        qreal currentHeight = currentCursorPos.y() - startCursorPos.y();
        QRectF rectangle{startCursorPos, QSizeF{currentWidth, currentHeight}};
        return rectangle;
    }

}  // namespace detail
