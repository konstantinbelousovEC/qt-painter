// @copyright Copyright (c) 2023 by Konstantin Belousov

#include <QMouseEvent>
#include <QGraphicsItem>
#include "../include/square-mode-view.h"
#include "../include/detail.h"

namespace {
    constexpr Qt::GlobalColor kDefaultSquareFillColor{Qt::red};
    constexpr Qt::GlobalColor kDefaultSquareStrokeColor{Qt::black};
}

SquareModeView::SquareModeView(QGraphicsScene* scene)
    : DrawingGraphicsView(scene, kDefaultSquareFillColor, kDefaultSquareStrokeColor, detail::kDefaultStrokeWidth),
      currentItem_(nullptr),
      centerPos_(detail::kZeroPointF)
{
    setRenderHint(QPainter::Antialiasing);
}

void SquareModeView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        centerPos_ = mapToScene(event->pos());
        currentItem_ = scene()->addRect(
                QRectF{centerPos_, detail::kZeroSizeF},
                QPen{strokeColor_, strokeWidth_, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin},
                QBrush{fillColor_.value()}
        );

        if (currentItem_ != nullptr)
            detail::makeItemSelectableAndMovable(currentItem_);
    }
}

void SquareModeView::mouseMoveEvent(QMouseEvent* event) {
    if (currentItem_ != nullptr && event->buttons() & Qt::LeftButton) {
        QPointF currentCursorPos = mapToScene(event->pos());
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