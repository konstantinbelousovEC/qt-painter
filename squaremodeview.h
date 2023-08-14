#pragma once

#include <QGraphicsView>
#include <QMouseEvent>
#include <QGraphicsRectItem>

class SquareModeView : public QGraphicsView {
public:
    SquareModeView(QGraphicsScene*);

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

private:
    QGraphicsRectItem* currentItem_;
    QPointF centerPos_;
    bool drawing_;

};
