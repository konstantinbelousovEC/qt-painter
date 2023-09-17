// @copyright Copyright (c) 2023 by Konstantin Belousov

#include "../include/rectangles-detail.h"

namespace detail {

    QRectF makeRectangle(const QPointF& startCursorPos, const QPointF& currentCursorPos) noexcept {
        auto currentWidth = currentCursorPos.x() - startCursorPos.x();
        auto currentHeight = currentCursorPos.y() - startCursorPos.y();
        QRectF rectangle{startCursorPos, QSizeF{currentWidth, currentHeight}};
        return rectangle.normalized();
    }

    QRectF makeSquare(const QPointF& startCursorPos, const QPointF& currentCursorPos) noexcept {
        auto currentWidth = currentCursorPos.x() - startCursorPos.x();
        auto currentHeight = currentCursorPos.y() - startCursorPos.y();
        QRectF rectangle;
        if (std::abs(currentWidth) < std::abs(currentHeight)) {
            if ((currentWidth < 0 && currentHeight < 0) || (currentWidth >= 0 && currentHeight >= 0)) rectangle = QRectF{startCursorPos, QSizeF{currentWidth, currentWidth}};
            else rectangle = QRectF{startCursorPos, QSizeF{currentWidth, -currentWidth}};
        } else {
            if ((currentWidth < 0 && currentHeight < 0) || (currentWidth >= 0 && currentHeight >= 0)) rectangle = QRectF{startCursorPos, QSizeF{currentHeight, currentHeight}};
            else rectangle = QRectF{startCursorPos, QSizeF{-currentHeight, currentHeight}};
        }
        return rectangle.normalized();
    }

}  // namespace
