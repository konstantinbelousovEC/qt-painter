// @copyright Copyright (c) 2023 by Konstantin Belousov

#pragma once

#include <QGraphicsView>

class TriangleModeView : public QGraphicsView {
    Q_OBJECT

 public:
    explicit TriangleModeView(QGraphicsScene* scene);

 protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

 private:
    void deleteTemporaryLines();
    bool addNewTemporaryLine(QMouseEvent* event);
    bool createPolygon();

 private slots:
    void changeFillColor(const QColor& color);
    void changeStrokeColor(const QColor& color);

 signals:
    void changeStateOfScene();

 private:
    QVector<QPointF> points_;
    QVector<QGraphicsLineItem*> lineItems_;
    QPointF lastClickPos_;
    QColor fillColor_;
    QColor strokeColor_;
    int clickCount_;
};
