// @copyright Copyright (c) 2023 by Konstantin Belousov

#include <QMouseEvent>
#include "../include/rectangle-mode-view.h"
#include "../include/detail.h"
#include "../include/constants.h"

RectangleModeView::RectangleModeView(QGraphicsScene* scene)
    : DrawingGraphicsView(scene),
      currentItem_(nullptr),
      startCursorPos_(constants::kZeroPointF) {}

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
    QPointF currentCursorPos = mapToScene(event->pos());
    emit cursorPositionChanged(currentCursorPos);
    if (currentItem_ != nullptr && event->buttons() & Qt::LeftButton) {
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
