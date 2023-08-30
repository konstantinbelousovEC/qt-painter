// @copyright Copyright (c) 2023 by Konstantin Belousov

#include "../include/detail.h"

namespace detail {

    QRectF updateRectangleSize(const QPointF& startCursorPos, const QPointF& currentCursorPos) noexcept {
        qreal currentWidth = currentCursorPos.x() - startCursorPos.x();
        qreal currentHeight = currentCursorPos.y() - startCursorPos.y();
        QRectF rectangle{startCursorPos, QSizeF{currentWidth, currentHeight}};
        return rectangle;
    }

}
