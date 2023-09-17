// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include <QMouseEvent>
#include "drawing-graphics-view.h"
#include "graphics-items-detail.h"
#include "rectangles-detail.h"
#include "constants.h"

template<typename ShapeType>
class RectangleLikeShapeModeView : public DrawingGraphicsView {
 public:
    RectangleLikeShapeModeView(QGraphicsScene* scene, QSize viewSize);

 protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

 private:
    ShapeType* currentItem_;
    QPointF startCursorPos_;
};

template<typename ShapeType>
RectangleLikeShapeModeView<ShapeType>::
    RectangleLikeShapeModeView(QGraphicsScene* scene, QSize viewSize)
            : DrawingGraphicsView(scene, viewSize),
              currentItem_(nullptr),
              startCursorPos_(constants::kZeroPointF) {}

template<typename ShapeType>
void RectangleLikeShapeModeView<ShapeType>::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        startCursorPos_ = mapToScene(event->pos());

        QRectF rectangle{startCursorPos_, constants::kZeroSizeF};
        QPen pen{strokeColor_, strokeWidth_, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin};
        QBrush brush{fillColor_};

        if constexpr (std::is_same_v<ShapeType, QGraphicsRectItem>) {
            currentItem_ = scene()->addRect(rectangle, pen, brush);
        } else if constexpr (std::is_same_v<ShapeType, QGraphicsEllipseItem>) {
            currentItem_ = scene()->addEllipse(rectangle, pen, brush);
        }

        if (currentItem_ != nullptr)
            detail::makeItemSelectableAndMovable(currentItem_);
    }
}

template<typename ShapeType>
void RectangleLikeShapeModeView<ShapeType>::mouseMoveEvent(QMouseEvent* event) {
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

template<typename ShapeType>
void RectangleLikeShapeModeView<ShapeType>::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && currentItem_ != nullptr) {
        if (detail::shouldDeleteZeroSizeItem(currentItem_, startCursorPos_)) {
            detail::deleteItem(scene(), currentItem_);
        }
        currentItem_ = nullptr;
    }
}
