#pragma once

#include <QGraphicsView>
#include <QMouseEvent>
#include <QGraphicsRectItem>

class RectangleModeView : public QGraphicsView {
    Q_OBJECT
public:
    RectangleModeView(QGraphicsScene* scene);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
    void changeFillColor(const QColor& color);
    void changeStrokeColor(const QColor& color);

signals:
    void changeStateOfScene();

private:
    QGraphicsRectItem* currentItem_;
    QPointF startPos_;
    QColor fillColor_;
    QColor strokeColor_;

};
