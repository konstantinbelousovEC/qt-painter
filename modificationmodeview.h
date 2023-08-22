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

class ModificationModeView : public QGraphicsView {
    Q_OBJECT
public:
    ModificationModeView(QGraphicsScene* scene);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

signals:
    void changeStateOfScene();

private:
    void setSelectionAreaProperties();
    void handleLeftButtonClick(QMouseEvent* event, QGraphicsItem* itemUnderCursor, const QPointF& currentCursorPos);
    void handleMiddleButtonClick(QGraphicsItem* itemUnderCursor, const QPointF& currentCursorPos);
    void updateSelectionArea(QMouseEvent* event, const QPointF& mouseCurrentPos);
    void updateItemsSelection(QMouseEvent* event, const QRectF& rect);

private:
    QGraphicsRectItem* selectionArea_;
    QPointF selectionStartPos_{};
    QPointF lastClickPos_{};
    QPointF rotationPointA_{};
    bool isMoving_{false};
    bool isRotating_{false};

};
