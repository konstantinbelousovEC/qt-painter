#pragma once

#include <QGraphicsView>
#include <QMouseEvent>
#include <QGraphicsRectItem>

class CircleModeView : public QGraphicsView
{
public:
    CircleModeView(QGraphicsScene*, bool& isModified);

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

private:
    QGraphicsEllipseItem* currentItem_;
    QPointF centerPos_;
    bool drawing_;
    bool& isModified_;

};
