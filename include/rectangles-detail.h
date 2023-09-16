// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include <QPointF>
#include <QRectF>

namespace detail {

    QRectF updateRectangleSize(const QPointF& startCursorPos, const QPointF& currentCursorPos) noexcept;

}
