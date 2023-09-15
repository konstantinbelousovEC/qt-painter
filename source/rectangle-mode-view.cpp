// @copyright Copyright (c) 2023 by Konstantin Belousov

#include <QMouseEvent>
#include "../include/rectangle-mode-view.h"
#include "../include/detail.h"
#include "../include/constants.h"

RectangleModeView::RectangleModeView(QGraphicsScene* scene)
    : DrawingGraphicsView(scene, constants::kDefaultColor, constants::kDefaultColor, constants::kDefaultStrokeWidth),
      currentItem_(nullptr),
      startCursorPos_(constants::kZeroPointF)
{
    setRenderHint(QPainter::Antialiasing);
}

void RectangleModeView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        startCursorPos_ = mapToScene(event->pos());
        currentItem_ = scene()->addRect(
                QRectF{startCursorPos_, constants::kZeroSizeF},
                QPen{strokeColor_, strokeWidth_, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin},
                QBrush{fillColor_}
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
