#pragma once

#include <QGraphicsView>
#include <QMouseEvent>
#include <QGraphicsRectItem>

class RectangleModeView : public QGraphicsView {
public:
    RectangleModeView(QGraphicsScene*, bool& isModified);

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

private:
    QGraphicsRectItem *currentItem_;
    QPointF startPos_;
    bool drawing_;
    bool& isModified_;

};
