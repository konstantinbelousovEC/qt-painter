// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include "graphics-view.h"

class RotationInfo;

class ModificationModeView final : public ApplicationGraphicsView {
 public:
    ModificationModeView(QGraphicsScene* scene, QSize viewSize);

 protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

 private:
    void setSelectionAreaProperties();
    void updateItemsSelection(QMouseEvent* event, const QRectF& rect);
    void moveSelectedItems(const QPointF& mousePos);
    void rotateSelectedItems(QMouseEvent* event);
    void rotateItem(QMouseEvent *event, QGraphicsItem* item, qreal startAngle);
    void updateSelectionArea(QMouseEvent* event, const QPointF& mouseCurrentPos);
    void handleMiddleButtonClick(QGraphicsItem* itemUnderCursor, const QPointF& currentCursorPos);
    void handleRightButtonClick(QMouseEvent* event, QGraphicsItem* itemUnderCursor);
    void handleLeftButtonClick(QMouseEvent* event,
                               QGraphicsItem* itemUnderCursor,
                               const QPointF& currentCursorPos);

 private:
    QGraphicsRectItem* selectionArea_;
    std::unique_ptr<RotationInfo> rotationInfo_;
    QPointF selectionStartPos_;
    QPointF lastClickPos_;
    QPointF initialCursorPosA_;
    bool isMoving_;
};
