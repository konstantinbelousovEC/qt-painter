// @copyright Copyright (c) 2023 by Konstantin Belousov

#include <QMouseEvent>
#include "../include/rectanglemodeview.h"
#include "../include/detail.h"

namespace {
    constexpr Qt::GlobalColor kDefaultRectFillColor{Qt::yellow};
    constexpr Qt::GlobalColor kDefaultRectStrokeColor{Qt::black};
}

RectangleModeView::RectangleModeView(QGraphicsScene* scene)
    : CustomGraphicsView(scene, kDefaultRectFillColor, kDefaultRectStrokeColor, detail::kDefaultStrokeWidth),
      currentItem_(nullptr),
      startCursorPos_(detail::kZeroPointF)
{
    setRenderHint(QPainter::Antialiasing);
}

void RectangleModeView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        startCursorPos_ = mapToScene(event->pos());
        currentItem_ = scene()->addRect(
                QRectF{startCursorPos_, detail::kZeroSizeF},
                QPen{strokeColor_.value(), static_cast<qreal>(strokeWidth_), Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin},
                QBrush{fillColor_.value()}
        );

        if (currentItem_ != nullptr)
            detail::makeItemSelectableAndMovable(currentItem_);
    }
}

void RectangleModeView::mouseMoveEvent(QMouseEvent* event) {
    if (currentItem_ != nullptr && event->buttons() & Qt::LeftButton) {
        QPointF currentCursorPos = mapToScene(event->pos());
        QRectF updatedRectangle = detail::updateRectangleSize(startCursorPos_,
                                                              currentCursorPos);

        currentItem_->setRect(updatedRectangle.normalized());
        emit changeStateOfScene();
    }
}

void RectangleModeView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && currentItem_ != nullptr) {
        if (detail::shouldDeleteZeroSizeItem(currentItem_, startCursorPos_)) {
            detail::deleteItem(scene(), currentItem_);
        }
        currentItem_ = nullptr;
    }
}
