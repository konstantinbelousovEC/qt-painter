// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include <QPointF>
#include <QRectF>

namespace detail {

    QRectF makeRectangle(const QPointF& startCursorPos, const QPointF& currentCursorPos) noexcept;
    QRectF makeSquare(const QPointF& startCursorPos, const QPointF& currentCursorPos) noexcept;

}
