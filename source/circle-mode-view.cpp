// @copyright Copyright (c) 2023 by Konstantin Belousov

#include <QMouseEvent>
#include "../include/circle-mode-view.h"
#include "../include/graphics-items-detail.h"
#include "../include/rectangles-detail.h"
#include "../include/constants.h"

CircleModeView::CircleModeView(QGraphicsScene* scene, QSize viewSize)
    : DrawingGraphicsView(scene, viewSize),
      currentItem_(nullptr),
      startCursorPos_(constants::kZeroPointF) {}

void CircleModeView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        startCursorPos_ = mapToScene(event->pos());
        currentItem_ = scene()->addEllipse(
                QRectF{startCursorPos_, constants::kZeroSizeF},
                QPen{strokeColor_, strokeWidth_, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin},
                QBrush{fillColor_}
        );

        if (currentItem_ != nullptr)
            detail::makeItemSelectableAndMovable(currentItem_);
    }
}

void CircleModeView::mouseMoveEvent(QMouseEvent* event) {
    QPointF currentCursorPos = mapToScene(event->pos());
    emit cursorPositionChanged(currentCursorPos);
    if (currentItem_ != nullptr && event->buttons() & Qt::LeftButton) {
        if (event->modifiers() & Qt::ShiftModifier) {
            currentItem_->setRect(detail::makeSquare(startCursorPos_, currentCursorPos));
        } else {
            currentItem_->setRect(detail::makeRectangle(startCursorPos_, currentCursorPos));
        }
        emit changeStateOfScene();
    }
}

void CircleModeView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && currentItem_ != nullptr) {
        if (detail::shouldDeleteZeroSizeItem(currentItem_, startCursorPos_)) {
            detail::deleteItem(scene(), currentItem_);
        }
        currentItem_ = nullptr;
    }
}
