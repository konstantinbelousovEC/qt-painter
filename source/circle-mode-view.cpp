// @copyright Copyright (c) 2023 by Konstantin Belousov

#include <QMouseEvent>
#include <QGraphicsItem>
#include "../include/circle-mode-view.h"
#include "../include/detail.h"

namespace {
    constexpr Qt::GlobalColor kDefaultCircleFillColor{Qt::green};
    constexpr Qt::GlobalColor kDefaultCircleStrokeColor{Qt::black};
}

CircleModeView::CircleModeView(QGraphicsScene* scene)
    : DrawingGraphicsView(scene, kDefaultCircleFillColor, kDefaultCircleStrokeColor, detail::kDefaultStrokeWidth),
      currentItem_(nullptr),
      ellipseCenterPos_(detail::kZeroPointF)
{
    setRenderHint(QPainter::Antialiasing);
}

void CircleModeView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        ellipseCenterPos_ = mapToScene(event->pos());
        currentItem_ = scene()->addEllipse(
                QRectF{ellipseCenterPos_, detail::kZeroSizeF},
                QPen{strokeColor_, strokeWidth_, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin},
                QBrush{fillColor_.value()}
        );

        if (currentItem_ != nullptr)
            detail::makeItemSelectableAndMovable(currentItem_);
    }
}

void CircleModeView::mouseMoveEvent(QMouseEvent* event) {
    if (currentItem_ != nullptr && (event->buttons() & Qt::LeftButton)) {
        QPointF currentPos = mapToScene(event->pos());
        qreal radius = QLineF{ellipseCenterPos_, currentPos}.length();
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
