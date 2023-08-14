#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QPolygonF>
#include <QMouseEvent>

class TriangleModeView : public QGraphicsView {
public:
    TriangleModeView(QGraphicsScene*, bool& isModified);

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;

private:
    QGraphicsLineItem* lineItem1_;
    QGraphicsLineItem* lineItem2_;
    QPointF startPoint_{};
    QPointF secondPoint_{};
    int clickCount_{};
    bool& isModified_;

private:
    void resetTemporaryItems();
    void createTriangle(const QPointF &thirdPoint);
};
