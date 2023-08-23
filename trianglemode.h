// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include <QGraphicsView>

class TriangleModeView : public QGraphicsView {
    Q_OBJECT

public:
    TriangleModeView(QGraphicsScene* scene);

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
    QGraphicsLineItem* lineItem1_;
    QGraphicsLineItem* lineItem2_;
    QPointF startPoint_{};
    QPointF secondPoint_{};
    QColor fillColor_;
    QColor strokeColor_;
    int clickCount_{};
};
