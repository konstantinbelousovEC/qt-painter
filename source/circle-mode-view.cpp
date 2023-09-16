// @copyright Copyright (c) 2023 by Konstantin Belousov

#include <QMouseEvent>
#include <QGraphicsItem>
#include "../include/circle-mode-view.h"
#include "../include/detail.h"
#include "../include/constants.h"

CircleModeView::CircleModeView(QGraphicsScene* scene)
    : DrawingGraphicsView(scene),
      currentItem_(nullptr),
      ellipseCenterPos_(constants::kZeroPointF) {}

void CircleModeView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        ellipseCenterPos_ = mapToScene(event->pos());
        currentItem_ = scene()->addEllipse(
                QRectF{ellipseCenterPos_, constants::kZeroSizeF},
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
    if (currentItem_ != nullptr && (event->buttons() & Qt::LeftButton)) {
        qreal radius = QLineF{ellipseCenterPos_, currentCursorPos}.length();
        QRectF describingRectangle{ellipseCenterPos_.x() - radius,
                                   ellipseCenterPos_.y() - radius,
                                   radius * 2,
                                   radius * 2};

        currentItem_->setRect(describingRectangle.normalized());
        emit changeStateOfScene();
    }
}

void CircleModeView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && currentItem_ != nullptr) {
        if (detail::shouldDeleteZeroSizeItem(currentItem_, ellipseCenterPos_)) {
            detail::deleteItem(scene(), currentItem_);
        }
        currentItem_ = nullptr;
    }
}
