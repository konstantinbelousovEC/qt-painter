#include "modificationmodeview.h"

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
    if (const auto* rectItem = qgraphicsitem_cast<const QGraphicsRectItem*>(item)) {
        return getItemCenter(rectItem);
    } else if (const auto* ellipseItem = qgraphicsitem_cast<const QGraphicsEllipseItem*>(item)) {
        return getItemCenter(ellipseItem);
    } else if (const auto* triangleItem = qgraphicsitem_cast<const QGraphicsPolygonItem*>(item)) {
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

ModificationModeView::ModificationModeView(QGraphicsScene* graphic_scene)
    : QGraphicsView(graphic_scene), selectionArea_(new QGraphicsRectItem())
{
    setSelectionAreaProperties();
    scene()->addItem(selectionArea_);
}

// ------------------------------------------------------------------------------------------------

void ModificationModeView::mousePressEvent(QMouseEvent* event) {
    QPointF currentClickPosition = mapToScene(event->pos());
    QGraphicsItem *itemUnderCursor = scene()->itemAt(currentClickPosition, QTransform());
    qDebug() << "press mouse coords:" << currentClickPosition;

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

void ModificationModeView::mouseMoveEvent(QMouseEvent* event) {
    QGraphicsView::mouseMoveEvent(event);

    QPointF mousePos = mapToScene(event->pos());
    qDebug() << "moving coords:" << mousePos;

    if (event->buttons() & (Qt::LeftButton | Qt::RightButton)) {
        if (isRotating_) {
            rotateSelectedItems(event);
        } else if (isMoving_) {
            moveSelectedItems(mousePos);
        } else if (selectionArea_ && !isMoving_ && !isRotating_) {
            updateSelectionArea(mousePos);
        }
    }
}

void ModificationModeView::mouseReleaseEvent(QMouseEvent* event) {
    isMoving_ = false;
    isRotating_ = false;
    selectionArea_->hide();
    qDebug() << "mouse release coords:" << mapToScene(event->pos());
    QGraphicsView::mouseReleaseEvent(event);
}

void ModificationModeView::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Control) {
        isCtrlPressed_ = true;
    } else if (event->key() == Qt::Key_D) {
        deleteSelectedItems();
    } else if (event->key() == Qt::Key_Shift) {
        isShiftPressed_ = true;
    } else {
        QGraphicsView::keyPressEvent(event);
    }
}

void ModificationModeView::keyReleaseEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Control) {
        isCtrlPressed_ = false;
    } else if (event->key() == Qt::Key_Shift) {
        isShiftPressed_ = false;
    } else {
        QGraphicsView::keyPressEvent(event);
    }
}

// ------------------------------------------------------------------------------------------------

void ModificationModeView::deleteSelectedItems() {
    foreach (QGraphicsItem *item, scene()->selectedItems()) {
        scene()->removeItem(item);
        delete item;
    }
}

QGraphicsItem* ModificationModeView::cloneGraphicsItem(QGraphicsItem* originalItem) {
    QGraphicsItem* copiedItem = nullptr;
    //qDebug() << getGraphicsItemCenterPos(originalItem);
    if (auto* rectItem = qgraphicsitem_cast<QGraphicsRectItem*>(originalItem)) {
        auto* temporaryRectItem = new QGraphicsRectItem();
        temporaryRectItem->setRect(rectItem->rect().x(),rectItem->rect().y(), rectItem->rect().width(), rectItem->rect().height());
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

void ModificationModeView::rotateItem(QGraphicsItem* item, QMouseEvent *event) {
    QPointF pointO = getGraphicsItemCenterPos(item);
    QPointF pointB = mapToScene(event->pos());

    item->setTransformOriginPoint(pointO);

    qreal angle = calculateAngle(pointO, rotationPointA_, pointB);

    QTransform transform;
    transform.translate(pointO.x(), pointO.y());
    transform.rotate(angle);
    transform.translate(-pointO.x(), -pointO.y());

    item->setTransform(transform);
}

bool ModificationModeView::isStartingRotatingEvent(QMouseEvent* event) {
    return event->button() == Qt::LeftButton && isCtrlPressed_ && isShiftPressed_;
}

void ModificationModeView::setSelectionAreaProperties() {
    selectionArea_->setPen(QPen(Qt::black, 1, Qt::DashLine));
    selectionArea_->hide();
    selectionArea_->setFlag(QGraphicsItem::ItemIsSelectable, false);
    selectionArea_->setFlag(QGraphicsItem::ItemIsMovable, false);
    selectionArea_->setZValue(1.0);
}

QRectF ModificationModeView::normalizeSelectionAreaRect(QPointF mousePos) {
    qreal x = qMin(selectionStartPos_.x(), mousePos.x());
    qreal y = qMin(selectionStartPos_.y(), mousePos.y());
    qreal width = qAbs(mousePos.x() - selectionStartPos_.x());
    qreal height = qAbs(mousePos.y() - selectionStartPos_.y());

    return QRectF{x, y, width, height};
}

void ModificationModeView::handleRotatingEvent(QGraphicsItem* itemUnderCursor, QMouseEvent* event) {
    if (itemUnderCursor != nullptr) {
        isRotating_ = true;
        rotationPointA_ = mapToScene(event->pos());
    }
}

void ModificationModeView::handleRightButtonClick(QGraphicsItem* itemUnderCursor, const QPointF& currentClickPosition) {
    if (itemUnderCursor != nullptr) {
        QList<QGraphicsItem*> clonedItems = cloneSelectedItems();
        updateSceneSelection(clonedItems);
        isMoving_ = true;
        lastMousePos_ = currentClickPosition;
    }
}

void ModificationModeView::handleLeftButtonClick(QGraphicsItem* itemUnderCursor, const QPointF& currentClickPosition) {
    if (itemUnderCursor == nullptr) {
        handleSelectionClear(currentClickPosition);
    } else {
        isMoving_ = true;
        lastMousePos_ = currentClickPosition;
    }
}

QList<QGraphicsItem*> ModificationModeView::cloneSelectedItems() {
    QList<QGraphicsItem*> clonedItems;
    foreach (QGraphicsItem* item, scene()->selectedItems()) {
        QGraphicsItem* clonedItem = cloneGraphicsItem(item);
        if (clonedItem) {
            clonedItems.append(clonedItem);
            scene()->addItem(clonedItem);
        }
    }
    return clonedItems;
}

void ModificationModeView::updateSceneSelection(const QList<QGraphicsItem*>& items) {
    scene()->clearSelection();
    foreach (QGraphicsItem* item, items) {
        item->setSelected(true);
    }
}

void ModificationModeView::handleSelectionClear(const QPointF& currentClickPosition) {
    if (!isCtrlPressed_) {
        scene()->clearSelection();
    }
    selectionStartPos_ = currentClickPosition;
}

void ModificationModeView::rotateSelectedItems(QMouseEvent* event) {
    foreach (QGraphicsItem* item, scene()->selectedItems()) {
        rotateItem(item, event);
    }
}

void ModificationModeView::moveSelectedItems(const QPointF& mousePos) {
    QPointF delta = mousePos - lastMousePos_;
    foreach (QGraphicsItem* item, scene()->selectedItems()) {
        qDebug() << "scene pos" << item->scenePos();
        item->setPos(item->scenePos() + delta);
    }
    lastMousePos_ = mousePos;
}

void ModificationModeView::updateSelectionArea(const QPointF& mousePos) {
    selectionArea_->show();
    QRectF rect = normalizeSelectionAreaRect(mousePos);
    selectionArea_->setRect(rect);
    updateItemsSelection(rect);
}

void ModificationModeView::updateItemsSelection(const QRectF &rect) {
    QList<QGraphicsItem*> itemsInRect = scene()->items(rect);
    foreach (QGraphicsItem *item, scene()->items()) {
        if (!isCtrlPressed_) {
            item->setSelected(itemsInRect.contains(item));
        } else {
            item->setSelected(item->isSelected() || itemsInRect.contains(item));
        }
    }
}
