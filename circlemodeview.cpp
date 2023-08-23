// @copyright Copyright (c) 2023 by Konstantin Belousov

#include "circlemodeview.h"
#include "detail.h"

#include <QMouseEvent>
#include <QGraphicsItem>

namespace {
    constexpr QSizeF kZeroSizeF{0, 0};
    constexpr QPointF kZeroPointF{0, 0};
    constexpr Qt::GlobalColor kDefaultCircleFillColor{Qt::green};
    constexpr Qt::GlobalColor kDefaultCircleStrokeColor{Qt::black};
}

bool shouldDeleteZeroSizeItem(const QGraphicsEllipseItem* currentItem, const QPointF& startPos);

CircleModeView::CircleModeView(QGraphicsScene *scene)
    : QGraphicsView(scene),
      currentItem_(nullptr),
      ellipseCenterPos_(kZeroPointF),
      fillColor_(kDefaultCircleFillColor),
      strokeColor_(kDefaultCircleStrokeColor) {}

void CircleModeView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        ellipseCenterPos_ = mapToScene(event->pos());
        currentItem_ = scene()->addEllipse(QRectF{ellipseCenterPos_, kZeroSizeF},
                                           QPen{kDefaultCircleStrokeColor},
                                           QBrush{kDefaultCircleFillColor});
        detail::makeItemSelectableAndMovable(currentItem_);
    }
}

void CircleModeView::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        QPointF currentPos = mapToScene(event->pos());
        qreal radius = QLineF{ellipseCenterPos_, currentPos}.length();
        QRectF describingRectangle{ellipseCenterPos_.x() - radius, ellipseCenterPos_.y() - radius, radius * 2, radius * 2};
        currentItem_->setRect(describingRectangle.normalized());
        emit changeStateOfScene();
    }
}

void CircleModeView::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && currentItem_ != nullptr) {
        if (shouldDeleteZeroSizeItem(currentItem_, ellipseCenterPos_)) {
            qDebug() << "delete zero ellipse";
            detail::deleteItem(scene(), currentItem_);
        }
        currentItem_ = nullptr;
    }
}

void CircleModeView::changeFillColor(const QColor &color) {
    fillColor_ = color;
}

void CircleModeView::changeStrokeColor(const QColor &color) {
    strokeColor_ = color;
}

bool shouldDeleteZeroSizeItem(const QGraphicsEllipseItem* currentItem, const QPointF& startPos) {
    auto rect = currentItem->rect();
    return rect.center() == startPos && rect.size() == kZeroSizeF;
}

