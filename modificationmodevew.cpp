#include "modificationmodevew.h"

template<typename RectLikeItem>
QPointF getItemCenter(const RectLikeItem* rectItem) noexcept {
    return rectItem->boundingRect().center();
}

QPointF getTriangleCenter(const QGraphicsPolygonItem *triangleItem) {
    QPolygonF trianglePolygon = triangleItem->polygon();

    if (trianglePolygon.size() != 3) throw std::runtime_error("App supports only triangles, see you later");

    return (trianglePolygon.at(0) + trianglePolygon.at(1) + trianglePolygon.at(2)) / 3.0;
}

QPointF getGraphicsItemCenterPos(const QGraphicsItem* const item) {
    if (const QGraphicsRectItem* rectItem = qgraphicsitem_cast<const QGraphicsRectItem*>(item)) {
        return getItemCenter(rectItem);
    } else if (const QGraphicsEllipseItem* ellipseItem = qgraphicsitem_cast<const QGraphicsEllipseItem*>(item)) {
        return getItemCenter(ellipseItem);
    } else if (const QGraphicsPolygonItem* triangleItem = qgraphicsitem_cast<const QGraphicsPolygonItem*>(item)) {
        return getTriangleCenter(triangleItem);
    } else {
        throw std::runtime_error("Can not define figure's center: unknown figure type");
    }
}

qreal calculateAngle(const QPointF &O, const QPointF &A, const QPointF &B) noexcept {
    qreal angleAO = qAtan2(A.y() - O.y(), A.x() - O.x());
    qreal angleBO = qAtan2(B.y() - O.y(), B.x() - O.x());

    qreal angle = angleBO - angleAO;

    if (angle < 0) angle += 2 * M_PI;

    return qRadiansToDegrees(angle);
}

ModificationModeVew::ModificationModeVew(QGraphicsScene* graphic_scene, bool& isModified)
    : QGraphicsView(graphic_scene), selectionArea_(new QGraphicsRectItem()), isModified_(isModified)
{
    setSelectionAreaProperties();
    scene()->addItem(selectionArea_);
}

// ------------------------------------------------------------------------------------------------

void ModificationModeVew::mousePressEvent(QMouseEvent* event) {
    QPointF currentClickPosition = mapToScene(event->pos());
    QGraphicsItem *itemUnderCursor = scene()->itemAt(currentClickPosition, QTransform());

    if (isStartingRotatingEvent(event)) {
        handleRotatingEvent(itemUnderCursor, event);
    } else if (event->button() == Qt::RightButton) {
        handleRightButtonClick(itemUnderCursor, currentClickPosition);
        QGraphicsView::mousePressEvent(event);
    } else if (event->button() == Qt::LeftButton) {
        handleLeftButtonClick(itemUnderCursor, currentClickPosition);
        QGraphicsView::mousePressEvent(event);
    }
}

void ModificationModeVew::mouseMoveEvent(QMouseEvent* event) {
    QGraphicsView::mouseMoveEvent(event);

    QPointF mousePos = mapToScene(event->pos());

    if (event->buttons() & (Qt::LeftButton | Qt::RightButton)) {
        if (isRotating_) {
            rotateSelectedItems(event);
            isModified_ = true;
        } else if (isMoving_) {
            moveSelectedItems(mousePos);
            isModified_ = true;
        } else if (selectionArea_ && !isMoving_ && !isRotating_) {
            updateSelectionArea(mousePos);
        }
    }
}

void ModificationModeVew::mouseReleaseEvent(QMouseEvent* event) {
    isMoving_ = false;
    isRotating_ = false;
    selectionArea_->hide();
    QGraphicsView::mouseReleaseEvent(event);
}

void ModificationModeVew::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Control) {
        isCtrlPressed_ = true;
    } else if (event->key() == Qt::Key_D) {
        deleteSelectedItems();
        isModified_ = true;
    } else if (event->key() == Qt::Key_Shift) {
        isShiftPressed_ = true;
    } else {
        QGraphicsView::keyPressEvent(event);
    }
}

void ModificationModeVew::keyReleaseEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Control) {
        isCtrlPressed_ = false;
    } else if (event->key() == Qt::Key_Shift) {
        isShiftPressed_ = false;
    } else {
        QGraphicsView::keyPressEvent(event);
    }
}

// ------------------------------------------------------------------------------------------------

void ModificationModeVew::deleteSelectedItems() {
    foreach (QGraphicsItem *item, scene()->selectedItems()) {
        scene()->removeItem(item);
        delete item;
    }
}

QGraphicsItem* ModificationModeVew::cloneGraphicsItem(QGraphicsItem* originalItem) {
    QGraphicsItem* copiedItem = nullptr;

    if (QGraphicsRectItem* rectItem = qgraphicsitem_cast<QGraphicsRectItem*>(originalItem)) {
        QGraphicsRectItem* temporaryRectItem = new QGraphicsRectItem(rectItem->rect());
        temporaryRectItem->setPen(rectItem->pen());
        temporaryRectItem->setBrush(rectItem->brush());
        copiedItem = temporaryRectItem;
    } else if (QGraphicsEllipseItem* ellipseItem = qgraphicsitem_cast<QGraphicsEllipseItem*>(originalItem)) {
        QGraphicsEllipseItem* temporaryEllipseItem = new QGraphicsEllipseItem(ellipseItem->rect());
        temporaryEllipseItem->setPen(ellipseItem->pen());
        temporaryEllipseItem->setBrush(ellipseItem->brush());
        copiedItem = temporaryEllipseItem;
    } else if (QGraphicsPolygonItem* polygonItem = qgraphicsitem_cast<QGraphicsPolygonItem*>(originalItem)) {
        QGraphicsPolygonItem* temporaryPolygonItem = new QGraphicsPolygonItem(polygonItem->polygon());
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

void ModificationModeVew::rotateItem(QGraphicsItem* item, QMouseEvent *event) {
    QPointF pointO = getGraphicsItemCenterPos(item);
    QPointF pointB = mapToScene(event->pos());

    item->setTransformOriginPoint(pointO);

    qreal angle = calculateAngle(pointO, rotationPointA_, pointB);

    QTransform transform;
    transform.translate(pointO.x(), pointO.y());
    transform.rotate(angle);
    transform.translate(-pointO.x(), -pointO.y());

    qDebug() << "shape center" << pointO;
    qDebug() << "start pos" << rotationPointA_;
    qDebug() << "current pos" << pointB;

    item->setTransform(transform);
}

bool ModificationModeVew::isStartingRotatingEvent(QMouseEvent* event) {
    return event->button() == Qt::LeftButton && isCtrlPressed_ && isShiftPressed_;
}

void ModificationModeVew::setSelectionAreaProperties() {
    selectionArea_->setPen(QPen(Qt::black, 1, Qt::DashLine));
    selectionArea_->hide();
    selectionArea_->setFlag(QGraphicsItem::ItemIsSelectable, false);
    selectionArea_->setFlag(QGraphicsItem::ItemIsMovable, false);
    selectionArea_->setZValue(1.0);
}

QRectF ModificationModeVew::normalizeSelectionAreaRect(QPointF mousePos) {
    qreal x = qMin(selectionStartPos_.x(), mousePos.x());
    qreal y = qMin(selectionStartPos_.y(), mousePos.y());
    qreal width = qAbs(mousePos.x() - selectionStartPos_.x());
    qreal height = qAbs(mousePos.y() - selectionStartPos_.y());

    return QRectF{x, y, width, height};
}

void ModificationModeVew::handleRotatingEvent(QGraphicsItem* itemUnderCursor, QMouseEvent* event) {
    if (itemUnderCursor != nullptr) {
        isRotating_ = true;
        rotationPointA_ = mapToScene(event->pos());
    }
}

void ModificationModeVew::handleRightButtonClick(QGraphicsItem* itemUnderCursor, const QPointF& currentClickPosition) {
    if (itemUnderCursor != nullptr) {
        QList<QGraphicsItem*> clonedItems = cloneSelectedItems();
        updateSceneSelection(clonedItems);
        isMoving_ = true;
        lastMousePos_ = currentClickPosition;
    }
}

void ModificationModeVew::handleLeftButtonClick(QGraphicsItem* itemUnderCursor, const QPointF& currentClickPosition) {
    if (itemUnderCursor == nullptr) {
        handleSelectionClear(currentClickPosition);
    } else {
        isMoving_ = true;
        lastMousePos_ = currentClickPosition;
    }
}

QList<QGraphicsItem*> ModificationModeVew::cloneSelectedItems() {
    QList<QGraphicsItem*> clonedItems;
    foreach (QGraphicsItem* item, scene()->selectedItems()) {
        QGraphicsItem* clonedItem = cloneGraphicsItem(item);
        if (clonedItem) {
            clonedItem->setPos(item->pos());
            clonedItem->pos();
            clonedItems.append(clonedItem);
            scene()->addItem(clonedItem);
        }
    }
    return clonedItems;
}

void ModificationModeVew::updateSceneSelection(const QList<QGraphicsItem*>& items) {
    scene()->clearSelection();
    foreach (QGraphicsItem* item, items) {
        item->setSelected(true);
    }
}

void ModificationModeVew::handleSelectionClear(const QPointF& currentClickPosition) {
    if (!isCtrlPressed_) {
        scene()->clearSelection();
    }
    selectionStartPos_ = currentClickPosition;
}

void ModificationModeVew::rotateSelectedItems(QMouseEvent* event) {
    foreach (QGraphicsItem* item, scene()->selectedItems()) {
        rotateItem(item, event);
    }
}

void ModificationModeVew::moveSelectedItems(const QPointF& mousePos) {
    QPointF delta = mousePos - lastMousePos_;
    foreach (QGraphicsItem* item, scene()->selectedItems()) {
        item->setPos(item->pos() + delta);
    }
    lastMousePos_ = mousePos;
}

void ModificationModeVew::updateSelectionArea(const QPointF& mousePos) {
    selectionArea_->show();
    QRectF rect = normalizeSelectionAreaRect(mousePos);
    selectionArea_->setRect(rect);
    updateItemsSelection(rect);
}

void ModificationModeVew::updateItemsSelection(const QRectF &rect) {
    QList<QGraphicsItem*> itemsInRect = scene()->items(rect);
    foreach (QGraphicsItem *item, scene()->items()) {
        if (!isCtrlPressed_) {
            item->setSelected(itemsInRect.contains(item));
        } else {
            item->setSelected(item->isSelected() || itemsInRect.contains(item));
        }
    }
}
