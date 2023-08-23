// @copyright Copyright (c) 2023 by Konstantin Belousov

#include "squaremodeview.h"
#include "detail.h"

#include <QMouseEvent>
#include <QGraphicsItem>

namespace {
    constexpr QSizeF kZeroSizeF{0, 0};
    constexpr QPointF kZeroPointF{0, 0};
    constexpr Qt::GlobalColor kDefaultSquareFillColor{Qt::red};
    constexpr Qt::GlobalColor kDefaultSquareStrokeColor{Qt::black};
}

SquareModeView::SquareModeView(QGraphicsScene* scene)
    : QGraphicsView(scene),
      currentItem_(nullptr),
      centerPos_(kZeroPointF),
      fillColor_(kDefaultSquareFillColor),
      strokeColor_(kDefaultSquareStrokeColor) {}

void SquareModeView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        centerPos_ = mapToScene(event->pos());
        currentItem_ = scene()->addRect(QRectF{centerPos_, kZeroSizeF},
                                        QPen{strokeColor_},
                                        QBrush{fillColor_});
        detail::makeItemSelectableAndMovable(currentItem_);
    }
}

void SquareModeView::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() & Qt::LeftButton) {
        QPointF currentCursorPos = mapToScene(event->pos());
        qreal halfDistance = QLineF{centerPos_, currentCursorPos}.length() / 2;
        QRectF updatedRectangle{centerPos_.x() - halfDistance / 2, centerPos_.y() - halfDistance / 2, halfDistance, halfDistance};
        currentItem_->setRect(updatedRectangle.normalized());
        emit changeStateOfScene();
    }
}

static bool shouldDeleteZeroSizeItem(QGraphicsRectItem* currentItem, const QPointF& startPos) {
    auto rect = currentItem->rect();
    return rect.topLeft() == startPos && rect.size() == kZeroSizeF;
}

void SquareModeView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && currentItem_ != nullptr) {
        if (shouldDeleteZeroSizeItem(currentItem_, centerPos_)) {
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

