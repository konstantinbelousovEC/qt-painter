// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include <QGraphicsView>

class CircleModeView : public QGraphicsView {
    Q_OBJECT

 public:
    explicit CircleModeView(QGraphicsScene* scene);

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
    QGraphicsEllipseItem* currentItem_;
    QPointF ellipseCenterPos_;
    QColor fillColor_;
    QColor strokeColor_;
};
