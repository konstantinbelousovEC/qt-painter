#include "modificationmodeview.h"
#include "detail.h"
#include <math.h>


namespace {

    constexpr QColor kSelectionAreaBrush{0, 0, 200, 15};

}

ModificationModeView::ModificationModeView(QGraphicsScene* graphic_scene)
    : QGraphicsView(graphic_scene), selectionArea_(new QGraphicsRectItem())
{
    setSelectionAreaProperties();
    scene()->addItem(selectionArea_);
}

void ModificationModeView::mousePressEvent(QMouseEvent* event) {
    QPointF currentCursorPos = mapToScene(event->pos());
    QGraphicsItem *itemUnderCursor = scene()->itemAt(currentCursorPos, QTransform());
    qDebug() << "click position:" << currentCursorPos;

    if (event->button() == Qt::LeftButton && (event->modifiers() & Qt::ShiftModifier)) { // todo: копирование // Qt::MiddleButton
        qDebug() << "middle button click";

    } else if (event->button() == Qt::LeftButton) { // todo: выделение
        qDebug() << "left button click";
        QGraphicsView::mousePressEvent(event);
        handleLeftButtonClick(event, itemUnderCursor, currentCursorPos);
        qDebug() << "click event:" << scene()->selectedItems().size();
    } else if (event->button() == Qt::RightButton) { // todo: вращение
        qDebug() << "right button click";
    }
}

void ModificationModeView::mouseMoveEvent(QMouseEvent* event) {
    QPointF mouseCurrentPos = mapToScene(event->pos());
    if (isRotating_) {
        emit changeStateOfScene();
    } else if (isMoving_) {
        qDebug() << "move";
        emit changeStateOfScene();
    } else if (!isMoving_ && !isRotating_ && event->buttons() & Qt::LeftButton) {
        updateSelectionArea(event, mouseCurrentPos);
    }
}

void ModificationModeView::mouseReleaseEvent(QMouseEvent* event) {
    isMoving_ = false;
    isRotating_ = false;
    selectionArea_->setRect(0, 0, 0, 0);
    selectionArea_->hide();
}

void ModificationModeView::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_D) {
        detail::deleteSelectedItems(scene());
        emit changeStateOfScene();
    }
}

void ModificationModeView::setSelectionAreaProperties() {
    selectionArea_->setPen(QPen{Qt::blue});
    selectionArea_->setBrush(QBrush{kSelectionAreaBrush});
    selectionArea_->hide();
    selectionArea_->setFlag(QGraphicsItem::ItemIsSelectable, false);
    selectionArea_->setFlag(QGraphicsItem::ItemIsMovable, false);
    selectionArea_->setZValue(1.0);
}

void ModificationModeView::handleLeftButtonClick(QMouseEvent* event, QGraphicsItem* itemUnderCursor, const QPointF& currentCursorPos) {
    if (itemUnderCursor == nullptr) {
        if (!(event->modifiers() & Qt::ControlModifier)) scene()->clearSelection();

        qDebug() << "control not pressed:" << scene()->selectedItems().size();
        selectionStartPos_ = currentCursorPos;
    } else {
        if (event->modifiers() & Qt::ControlModifier) itemUnderCursor->setSelected(!itemUnderCursor->isSelected());
        isMoving_ = true;
        lastClickPos_ = currentCursorPos;
    }
}

void ModificationModeView::updateSelectionArea(QMouseEvent* event, const QPointF& mouseCurrentPos) {
    selectionArea_->show();
    QRectF updatedRectangle = detail::updateRectangleSize(selectionStartPos_, mouseCurrentPos);
    selectionArea_->setRect(updatedRectangle.normalized());
    updateItemsSelection(event, selectionArea_->rect());
}

void ModificationModeView::updateItemsSelection(QMouseEvent* event, const QRectF &rect) {
    QList<QGraphicsItem*> itemsInRect = scene()->items(rect);
    foreach (QGraphicsItem *item, scene()->items()) {
        if (!(event->modifiers() & Qt::ControlModifier)) {
            item->setSelected(itemsInRect.contains(item));
        } else {
            item->setSelected(item->isSelected() || itemsInRect.contains(item));
        }
    }
}

qreal calculateRotationAngle(const QPointF &O, const QPointF &A, const QPointF &B) noexcept {
    qreal angleAO = qAtan2(A.y() - O.y(), A.x() - O.x());
    qreal angleBO = qAtan2(B.y() - O.y(), B.x() - O.x());
    qreal angle = angleBO - angleAO;

    if (angle < 0) angle += 2 * M_PI;

    return qRadiansToDegrees(angle);
}

void updateSceneSelection(QGraphicsScene* scene, const QList<QGraphicsItem*>& items) {
    scene->clearSelection();
    foreach (QGraphicsItem* item, items) {
        item->setSelected(true);
    }
}

QGraphicsItem* cloneGraphicsItem(QGraphicsItem* originalItem) {
    QGraphicsItem* copiedItem = nullptr;

    if (auto* rectItem = qgraphicsitem_cast<QGraphicsRectItem*>(originalItem)) {
        auto* temporaryRectItem = new QGraphicsRectItem(rectItem->rect());
        temporaryRectItem->setPen(rectItem->pen());
        temporaryRectItem->setBrush(rectItem->brush());
        copiedItem = temporaryRectItem;
    } else if (auto* ellipseItem = qgraphicsitem_cast<QGraphicsEllipseItem*>(originalItem)) {
        auto* temporaryEllipseItem = new QGraphicsEllipseItem(ellipseItem->rect());
        temporaryEllipseItem->setPen(ellipseItem->pen());
        temporaryEllipseItem->setBrush(ellipseItem->brush());
        copiedItem = temporaryEllipseItem;
    } else if (auto* polygonItem = qgraphicsitem_cast<QGraphicsPolygonItem*>(originalItem)) {
        auto* temporaryPolygonItem = new QGraphicsPolygonItem(polygonItem->polygon());
        temporaryPolygonItem->setPen(polygonItem->pen());
        temporaryPolygonItem->setBrush(polygonItem->brush());
        copiedItem = temporaryPolygonItem;
    }

    if (copiedItem != nullptr) {
        copiedItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
        copiedItem->setFlag(QGraphicsItem::ItemIsMovable, true);
        copiedItem->setTransform(originalItem->transform());
    }

    return copiedItem;
}

QList<QGraphicsItem*> cloneSelectedItems(QGraphicsScene* scene) {
    QList<QGraphicsItem*> clonedItems;
            foreach (QGraphicsItem* item, scene->selectedItems()) {
            QGraphicsItem* clonedItem = cloneGraphicsItem(item);
            if (clonedItem) {
                clonedItem->setPos(item->pos());
                clonedItem->pos();
                clonedItems.append(clonedItem);
                scene->addItem(clonedItem);
            }
        }
    return clonedItems;
}

void ModificationModeView::handleMiddleButtonClick(QGraphicsItem* itemUnderCursor, const QPointF& currentCursorPos) {
    if (itemUnderCursor != nullptr) {
        QList<QGraphicsItem*> clonedItems = cloneSelectedItems(scene());
        updateSceneSelection(scene(), clonedItems);
        isMoving_ = true;
        lastClickPos_ = currentCursorPos;
    }
}


