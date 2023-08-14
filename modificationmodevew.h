#pragma once

#include <QApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPointF>
#include <QKeyEvent>

class ModificationModeVew : public QGraphicsView {
private:
    QGraphicsRectItem* selectionArea_;

    QPointF selectionStartPos_{};
    QPointF lastMousePos_{};
    QPointF rotationPointA_{};

    bool isCtrlPressed_{false};
    bool isShiftPressed_{false};
    bool isMoving_{false};
    bool isRotating_{false};

public:
    ModificationModeVew(QGraphicsScene*);

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void keyPressEvent(QKeyEvent*) override;
    void keyReleaseEvent(QKeyEvent*) override;

private:
    void deleteSelectedItems();
    void rotateItem(QGraphicsItem* item, QMouseEvent* event);
    void setSelectionAreaProperties();
    void handleRotatingEvent(QGraphicsItem* itemUnderCursor, QMouseEvent* event);
    void handleRightButtonClick(QGraphicsItem* itemUnderCursor, const QPointF& currentClickPosition);
    void handleLeftButtonClick(QGraphicsItem* itemUnderCursor, const QPointF& currentClickPosition);
    void updateSceneSelection(const QList<QGraphicsItem*>& items);
    void handleSelectionClear(const QPointF& currentClickPosition);
    void rotateSelectedItems(QMouseEvent* event);
    void moveSelectedItems(const QPointF& mousePos);
    void updateSelectionArea(const QPointF& mousePos);
    void updateItemsSelection(const QRectF& rect);
    bool isStartingRotatingEvent(QMouseEvent* event);
    QRectF normalizeSelectionAreaRect(QPointF mousePos);QList<QGraphicsItem*> cloneSelectedItems();
    QGraphicsItem* cloneGraphicsItem(QGraphicsItem* originalItem);

};
