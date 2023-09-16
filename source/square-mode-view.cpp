// @copyright Copyright (c) 2023 by Konstantin Belousov

#include <QMouseEvent>
#include <QGraphicsItem>
#include "../include/square-mode-view.h"
#include "../include/graphics-items-detail.h"

SquareModeView::SquareModeView(QGraphicsScene* scene, QSize viewSize)
    : DrawingGraphicsView(scene, viewSize),
      currentItem_(nullptr),
      centerPos_(constants::kZeroPointF) {}

void SquareModeView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        centerPos_ = mapToScene(event->pos());
        currentItem_ = scene()->addRect(
                QRectF{centerPos_, constants::kZeroSizeF},
                QPen{strokeColor_, strokeWidth_, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin},
                QBrush{fillColor_}
        );

        if (currentItem_ != nullptr)
            detail::makeItemSelectableAndMovable(currentItem_);
    }
}

void SquareModeView::mouseMoveEvent(QMouseEvent* event) {
    QPointF currentCursorPos = mapToScene(event->pos());
    emit cursorPositionChanged(currentCursorPos);
    if (currentItem_ != nullptr && event->buttons() & Qt::LeftButton) {
        qreal halfDistance = QLineF{centerPos_, currentCursorPos}.length() / 2;
        QRectF updatedRectangle{centerPos_.x() - halfDistance / 2,
                                centerPos_.y() - halfDistance / 2,
                                halfDistance,
                                halfDistance};

        currentItem_->setRect(updatedRectangle.normalized());
        emit changeStateOfScene();
    }
}

void SquareModeView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && currentItem_ != nullptr) {
        if (detail::shouldDeleteZeroSizeItem(currentItem_, centerPos_)) {
            detail::deleteItem(scene(), currentItem_);
        }
        currentItem_ = nullptr;
    }
}