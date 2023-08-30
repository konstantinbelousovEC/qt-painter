// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include <QGraphicsView>

class ModificationModeView : public QGraphicsView {
    Q_OBJECT

 public:
    explicit ModificationModeView(QGraphicsScene* scene);

 protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

 signals:
    void changeStateOfScene();

 private:
    void setSelectionAreaProperties();
    void updateItemsSelection(QMouseEvent* event, const QRectF& rect);
    void moveSelectedItems(const QPointF& mousePos);
    void rotateSelectedItems(QMouseEvent* event);
    void rotateItem(QMouseEvent *event, QGraphicsItem* item);
    void handleLeftButtonClick(QMouseEvent* event,
                               QGraphicsItem* itemUnderCursor,
                               const QPointF& currentCursorPos);

    void handleMiddleButtonClick(QGraphicsItem* itemUnderCursor,
                                 const QPointF& currentCursorPos);

    void handleRightButtonClick(QMouseEvent* event,
                                QGraphicsItem* itemUnderCursor);

    void updateSelectionArea(QMouseEvent* event,
                             const QPointF& mouseCurrentPos);

 private:
    QGraphicsRectItem* selectionArea_;
    QPointF selectionStartPos_;
    QPointF lastClickPos_;
    QPointF rotationPointA_;
    bool isMoving_;
    bool isRotating_;
};
