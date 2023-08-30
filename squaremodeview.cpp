// @copyright Copyright (c) 2023 by Konstantin Belousov

#include "squaremodeview.h"
#include "detail.h"

#include <QMouseEvent>
#include <QGraphicsItem>

namespace {
    constexpr Qt::GlobalColor kDefaultSquareFillColor{Qt::red};
    constexpr Qt::GlobalColor kDefaultSquareStrokeColor{Qt::black};
}

SquareModeView::SquareModeView(QGraphicsScene* scene)
    : QGraphicsView(scene),
      currentItem_(nullptr),
      centerPos_(detail::kZeroPointF),
      fillColor_(kDefaultSquareFillColor),
      strokeColor_(kDefaultSquareStrokeColor)
{
    setRenderHint(QPainter::Antialiasing);
}

void SquareModeView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        centerPos_ = mapToScene(event->pos());
        currentItem_ = scene()->addRect(QRectF{centerPos_, detail::kZeroSizeF},
                                        QPen{strokeColor_},
                                        QBrush{fillColor_});
        if (currentItem_ != nullptr) detail::makeItemSelectableAndMovable(currentItem_);
    }
}

void SquareModeView::mouseMoveEvent(QMouseEvent* event) {
    if (currentItem_ != nullptr && (event->buttons() & Qt::LeftButton) ) {
        QPointF currentCursorPos = mapToScene(event->pos());
        qreal halfDistance = QLineF{centerPos_, currentCursorPos}.length() / 2;
        QRectF updatedRectangle{centerPos_.x() - halfDistance / 2, centerPos_.y() - halfDistance / 2, halfDistance, halfDistance};
        currentItem_->setRect(updatedRectangle.normalized());
        emit changeStateOfScene();
    }
}

void SquareModeView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && currentItem_ != nullptr) {
        if (detail::shouldDeleteZeroSizeItem(currentItem_, centerPos_)) {
            qDebug() << "deleting zero square";
            detail::deleteItem(scene(), currentItem_);
        }
        currentItem_ = nullptr;
    }
}

void SquareModeView::changeFillColor(const QColor& color) {
    fillColor_ = color;
}

void SquareModeView::changeStrokeColor(const QColor& color) {
    strokeColor_ = color;
}
