#include "metromapwidget.h"
#include "mainwindow.h"
#include "selectablelinelabel.h"

// StationGraphicsItem
StationGraphicsItem::StationGraphicsItem(Station* station, QGraphicsItem* parent)
    : QGraphicsEllipseItem(parent), m_station(station)
{
    // 设置站点图形（圆形）
    setRect(-5, -5, 5, 5); // 5x5圆形
    setPos(station->position);
    updateAppearance();
    setZValue(2); // 确保站点显示在连接线上方
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable |
             QGraphicsItem::ItemSendsGeometryChanges);
}

void StationGraphicsItem::updateAppearance()
{
    // 设置站点颜色灰色
    setBrush(Qt::lightGray);
    setPen(QPen(Qt::darkGray, 1));
//    if (!m_station->lineIds.isEmpty())
//    {
//        setBrush(Qt::white);
//        setPen(QPen(Qt::black, 1));
//    }
//    else
//    {
//        setBrush(Qt::lightGray);
//        setPen(QPen(Qt::darkGray, 1));
//    }
}

void StationGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsEllipseItem::mousePressEvent(event);
    // 发出站点选择信号
    if (event->button() == Qt::LeftButton)
    {
        emit static_cast<MetroMapWidget*>(scene()->parent())->stationSelected(m_station);
    }
}

void StationGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsEllipseItem::mouseDoubleClickEvent(event);

    // 获取 MetroMapWidget 实例
    MetroMapWidget* mapWidget = qobject_cast<MetroMapWidget*>(scene()->parent());
    if (!mapWidget) return;
    qDebug() << "Double click event, first station ID:" << mapWidget->getFirstDoubleClickStationId();

    // 获取网络实例
    MetroNetwork* network = mapWidget->getNetwork();

    // 停止之前的计时器
    mapWidget->stopDoubleClickTimer();

    // 双击逻辑
    if (mapWidget->getFirstDoubleClickStationId() == -1) {
        // 第一次双击，记录站点ID
        mapWidget->setFirstDoubleClickStationId(m_station->id);

        // 高亮显示第一个站点
        setBrush(Qt::yellow);
        setPen(QPen(Qt::red, 2));

        // 启动计时器，如果超时则取消选择
        mapWidget->startDoubleClickTimer();
    } else {
        // 第二次双击
        int firstStationId = mapWidget->getFirstDoubleClickStationId();

        // 检查是否是同一个站点
        if (m_station->id == firstStationId) {
            // 双击了同一个站点，取消选择
            mapWidget->cancelDoubleClick();
            updateAppearance(); // 恢复外观
        } else {
            // 双击了不同的站点，尝试创建连接
            int stationId1 = firstStationId;
            int stationId2 = m_station->id;

            // 检查是否已存在连接
            bool connectionExists = false;
            for (auto it = network->connections.begin(); it != network->connections.end(); ++it) {
                const Connection& conn = it.value();
                if ((conn.stationId1 == stationId1 && conn.stationId2 == stationId2) ||
                    (conn.stationId1 == stationId2 && conn.stationId2 == stationId1)) {
                    connectionExists = true;
                    break;
                }
            }

            if (connectionExists) {
                // 连接已存在，显示消息
                QMessageBox::information(nullptr, "提示", "这两个站点之间已有连接");
            } else {
                // 创建新连接，默认通行时间3分钟
                int newConnectionId = network->addConnection(stationId1, stationId2, 3);
                if (newConnectionId != -1) {
                    // 成功创建连接
                    QMessageBox::information(nullptr, "成功", "已成功创建连接");

                    mapWidget->addConnectionItem(&(network->connections[newConnectionId]));
                } else {
                    // 创建连接失败
                    QMessageBox::warning(nullptr, "错误", "无法创建连接，可能是两个站点没有共同的线路");
                }
            }

            // 重置双击状态
            mapWidget->cancelDoubleClick();
        }
    }
}

// ConnectionGraphicsItem
ConnectionGraphicsItem::ConnectionGraphicsItem(Connection* connection, int lineId, QGraphicsItem* parent)
    : QGraphicsLineItem(parent), m_connection(connection), m_lineId(lineId) {
    updateAppearance();
    setZValue(1); // 连接线在站点下方
}

void ConnectionGraphicsItem::updateAppearance() {
    if (m_connection && m_connection->lineIds.contains(m_lineId)) {
        // 获取线路颜色
        QColor lineColor = Qt::gray; // 默认颜色，实际需要从网络中获取线路颜色

        // 设置线条样式
        setPen(QPen(lineColor, 3));
    } else {
        setPen(QPen(Qt::lightGray, 2, Qt::DashLine));
    }
}

void ConnectionGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    QGraphicsLineItem::mousePressEvent(event);
    // 发出连接线选择信号
    if (event->button() == Qt::LeftButton) {
        emit static_cast<MetroMapWidget*>(scene()->parent())->connectionSelected(m_connection);
    }
}

// MetroMapWidget
MetroMapWidget::MetroMapWidget(MetroNetwork* network, QWidget* parent)
    : QWidget(parent), m_network(network), m_scaleFactor(5.0)
{
    // 创建场景和视图
    m_scene = new QGraphicsScene(this);
    m_view = new QGraphicsView(m_scene, this);

    m_labelItems = QMap<int, QGraphicsTextItem*>();
    m_lineLabels = QMap<int, SelectableLineLabel*>();

    // 设置视图属性
    m_view->setRenderHint(QPainter::Antialiasing);

    m_view->setDragMode(QGraphicsView::RubberBandDrag); // RubberBandDrag模式，允许选择项目
    m_view->viewport()->installEventFilter(this);   // 事件过滤器

    m_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    m_view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    // 设置布局
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_view);
    setLayout(layout);

    // 初始化场景
    QTimer::singleShot(0, this, &MetroMapWidget::setupScene);

    // 创建双击计时器
    m_doubleClickTimer = new QTimer(this);
    m_doubleClickTimer->setSingleShot(true);
    m_doubleClickTimer->setInterval(CANCEL_DOUBLECLICK_TIME); // 5000毫秒后双击自动取消

    m_firstDoubleClickStationId = -1;

    // 连接计时器超时信号
    connect(m_doubleClickTimer, &QTimer::timeout, this, [this]() {
        // 超时后重置状态并清除所有高亮
        cancelDoubleClick();
        m_waitingForSecondClick = false;
        m_firstDoubleClickStationId = -1;
        clearAllHighlights();
    });
}

MetroMapWidget::~MetroMapWidget()
{
    // 清理图形项
//    qDeleteAll(m_stationItems);
//    qDeleteAll(m_connectionItems);
    // 由场景管理，不需要手动删除
    m_stationItems.clear();
    m_connectionItems.clear();
    m_labelItems.clear();

    // 清理线路标签
    for (auto it = m_lineLabels.begin(); it != m_lineLabels.end(); ++it)
    {
        delete it.value();
    }
    m_lineLabels.clear();

    delete m_scene;
}

void MetroMapWidget::centerOnItem(QGraphicsItem* item)
{
    if (!item || !m_view) return;

    QPointF itemCenter;

    Connection* connItem = dynamic_cast<Connection*>(item);
    if (connItem != nullptr) {
        int stationId1 = connItem->stationId1;
        int stationId2 = connItem->stationId2;

        QPointF pos1 = m_stationItems[stationId1]->sceneBoundingRect().center(); // 第一个站点的坐标
        QPointF pos2 = m_stationItems[stationId2]->sceneBoundingRect().center(); // 第二个站点的坐标
        itemCenter = QPointF( (pos1.x() + pos2.x()) / 2, (pos1.y() + pos2.y()) / 2 ); // 计算中点
    } else {
        itemCenter = item->sceneBoundingRect().center();
    }

    // 通过内部的QGraphicsView实现居中
    m_view->centerOn(itemCenter);

    double neededScale = 70 / m_scaleFactor;
    m_view->scale(neededScale, neededScale);
    m_scaleFactor = 70;

//    // 简单的动画效果
//    QPointF currentCenter = m_view->mapToScene(m_view->viewport()->rect().center());

//    // 创建一个定时器来实现平滑移动
//    QTimer* timer = new QTimer(this);
//    timer->setInterval(16); // 约60fps

//    int steps = 30; // 动画步数
//    int currentStep = 0;

//    connect(timer, &QTimer::timeout, [=]() mutable {
//        if (currentStep >= steps) {
//            timer->stop();
//            timer->deleteLater();
//            return;
//        }

//        // 计算当前步骤的插值
//        qreal progress = static_cast<qreal>(currentStep) / steps;
//        progress = progress * progress * (3 - 2 * progress); // 平滑缓动

//        // 计算当前中心点
//        QPointF newCenter = currentCenter + (itemCenter - currentCenter) * progress;

//        // 移动到新的中心点
//        m_view->centerOn(newCenter);

//        currentStep++;
//    });

//    timer->start();

}

void MetroMapWidget::centerOnItem(SelectableLineLabel* label)
{
    if (!label || !m_view) return;

    // 将 SelectableLineLabel* 转换为 QGraphicsItem*
    QGraphicsItem* item = dynamic_cast<QGraphicsItem*>(label);
    if (!item) return;

    QPointF itemCenter = item->sceneBoundingRect().center();

    m_view->centerOn(itemCenter);
}

bool MetroMapWidget::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == m_view->viewport()) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::RightButton) {
                m_view->setDragMode(QGraphicsView::ScrollHandDrag);
                // 模拟左键按下，以便开始拖拽
                QMouseEvent fakeEvent(mouseEvent->type(), mouseEvent->localPos(),
                                     Qt::LeftButton, Qt::LeftButton, mouseEvent->modifiers());
                QCoreApplication::sendEvent(m_view->viewport(), &fakeEvent);
                return true;
            }
        } else if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::RightButton) {
                m_view->setDragMode(QGraphicsView::RubberBandDrag);
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void MetroMapWidget::refreshMap()
{
    // 清除现有图形项
    qDeleteAll(m_stationItems);
    m_stationItems.clear();

    qDeleteAll(m_connectionItems);
    m_connectionItems.clear();

    // 清除标签映射（但不删除标签对象，因为它们由场景管理）
    m_labelItems.clear();
    m_lineLabels.clear();

    m_scene->clear();

    // 重新创建图形项
    setupScene();
}

void MetroMapWidget::setupScene()
{
    createConnectionItems();
    createStationItems();
    createLineLabels();

    // 设置场景大小
    if (!m_stationItems.isEmpty())
    {
        QRectF sceneRect;
        for (StationGraphicsItem* stationItem : m_stationItems)
        {
            sceneRect |= QRectF(stationItem->pos(), QSizeF(1, 1));
        }
        m_scene->setSceneRect(sceneRect.adjusted(-100, -100, 100, 100));
    }

    // 重置视图
    resetZoom();
}

void MetroMapWidget::createStationItems()
{
    for (auto it = m_network->stations.begin(); it != m_network->stations.end(); ++it)
    {
        Station* station = &it.value();
        addStationItem(station);
//        StationGraphicsItem* stationItem = new StationGraphicsItem(station);
//        m_stationItems.insert(station->id, stationItem);
//        m_scene->addItem(stationItem);

//        // 添加站点名称标签
//        QGraphicsTextItem* label = new QGraphicsTextItem(station->name);
//        label->setPos(station->position + QPointF(3, -3));
//        label->setZValue(3); // 标签显示在最上层

//        // 根据缩放因子调整文本大小
//        qreal textScale = qMax(0.1, 1.0 / m_scaleFactor);
//        label->setScale(textScale);

//        m_scene->addItem(label);

//        // 存储标签引用，以便在缩放时更新
//        m_labelItems.insert(station->id, label);
    }
}

void MetroMapWidget::addStationItem(Station* station)
{
    if (!station) return;

    // 创建站点图形项
    StationGraphicsItem* stationItem = new StationGraphicsItem(station);
    //stationItem->setPos(station->position);在构造函数里已经做了
    m_stationItems.insert(station->id, stationItem);
    m_scene->addItem(stationItem);

    // 添加站点名称标签
    QGraphicsTextItem* label = new QGraphicsTextItem(station->name);
    label->setPos(station->position + QPointF(3, -3));
    label->setZValue(3); // 标签显示在最上层

    // 根据缩放因子调整文本大小
    qreal textScale = qMax(0.1, 1.0 / m_scaleFactor);
    label->setScale(textScale);

    m_scene->addItem(label);

    // 存储标签引用，以便在缩放时更新
    m_labelItems.insert(station->id, label);

}

void MetroMapWidget::createConnectionItems()
{
    for (auto it = m_network->connections.begin(); it != m_network->connections.end(); ++it)
    {
        Connection* connection = &it.value();
        addConnectionItem(connection);
    }
}

void MetroMapWidget::addConnectionItem(Connection* connection)
{
    // 获取连接的站点
    Station* station1 = m_network->stations.contains(connection->stationId1) ?
                        &m_network->stations[connection->stationId1] : nullptr;
    Station* station2 = m_network->stations.contains(connection->stationId2) ?
                        &m_network->stations[connection->stationId2] : nullptr;

    if (station1 && station2)
    {
        // 计算连接线的方向向量
        QPointF direction = station2->position - station1->position;
        qreal length = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());

        // 如果长度为0，跳过这个连接（避免除以0）
        if (length < 1e-6) return;

        // 归一化方向向量
        QPointF unitDirection = direction / length;

        // 计算垂直于方向向量的单位法向量（用于偏移）
        QPointF normal(-unitDirection.y(), unitDirection.x());

        // 获取连接边所属的线路数量
        int lineCount = connection->lineIds.size();

        // 计算偏移量（根据线路数量对称分布）
        //qreal maxOffset = (lineCount - 1) * 2.0; // 最大偏移量（像素）

        // 为连接的每条线路创建一个图形项
        int lineIndex = 0;
        for (int lineId : connection->lineIds)
        {
            // 计算当前线路的偏移量（对称分布）
            qreal offset = (lineIndex - (lineCount - 1) / 2.0) * 2.0;

            // 计算偏移后的起点和终点
            QPointF offsetVector = normal * offset;
            QPointF startPoint = station1->position + offsetVector;
            QPointF endPoint = station2->position + offsetVector;

            // 创建连接图形项，指定线路ID
            ConnectionGraphicsItem* connectionItem = new ConnectionGraphicsItem(connection, lineId);

            // 使用组合键存储（连接ID + 线路ID）
            int compositeKey = connection->id * 1000 + lineId;
            m_connectionItems.insert(compositeKey, connectionItem);

            // 设置偏移后的连接线位置
            QLineF line(startPoint, endPoint);
            connectionItem->setLine(line);

            // 设置线路特定颜色
            if (m_network->lines.contains(lineId)) {
                QColor lineColor = m_network->lines[lineId].color;
                connectionItem->setPen(QPen(lineColor, 2)); // 稍微减小线宽，使多条线更清晰
            }

            m_scene->addItem(connectionItem);

            lineIndex++;
        }
    }
}

void MetroMapWidget::createLineLabels()
{
    // 为每条线路创建一个标签
    for (auto it = m_network->lines.begin(); it != m_network->lines.end(); ++it)
    {
        MetroLine* line = &it.value();

        // 检查线路是否有站点
        if (line->stationIds.empty())
            continue;

        // 获取线路的第一个站点
        int firstStationId = *(line->stationIds).begin();

        // 确保站点存在
        if (!m_network->stations.contains(firstStationId))
            continue;

        // 使用第一个站的位置
        QPointF startPosition = m_network->stations[firstStationId].position;
        const QPointF labelOffset(20, -20);
        QPointF labelPosition = startPosition + labelOffset;

        // 自定义线路标签
        SelectableLineLabel* label = new SelectableLineLabel(line);
        label->setPlainText(line->name); // 确保文本已设置，以便计算边界

        // 检查是否与现有标签重叠
        bool hasOverlap = true;
        int attempt = 0;
        const int maxAttempts = 10; // 最大尝试次数

        while (hasOverlap && attempt < maxAttempts)
        {
            hasOverlap = false;

            // 检查与所有现有标签的重叠
            for (auto existingLabel : m_lineLabels)
            {
                if (checkLabelOverlap(label, labelPosition, existingLabel))
                {
                    hasOverlap = true;

                    // 尝试不同的偏移方向
                    switch (attempt % 4)
                    {
                    case 0: // 向右移动
                        labelPosition += QPointF(30, 0);
                        break;
                    case 1: // 向下移动
                        labelPosition += QPointF(0, 30);
                        break;
                    case 2: // 向左移动
                        labelPosition += QPointF(-30, 0);
                        break;
                    case 3: // 向上移动
                        labelPosition += QPointF(0, -30);
                        break;
                    }

                    attempt++;
                    break; // 退出内部循环，重新检查所有标签
                }
            }

            // 如果没有重叠，退出循环
            if (!hasOverlap)
                break;
        }

        // 设置标签位置
        label->setPos(labelPosition);
        label->setZValue(4); // 标签显示在最上层

        // 连接点击信号
        connect(label, &SelectableLineLabel::clicked, this, &MetroMapWidget::onLineLabelClicked);

        // 根据缩放因子调整文本大小
        qreal textScale = qMax(0.1, 1.0 / m_scaleFactor);
        label->setScale(textScale);

        m_scene->addItem(label);
        m_lineLabels.insert(line->id, label);
    }
}

void MetroMapWidget::addLineLabel(MetroLine* line)
{
    if (!line || line->stationIds.empty()) return;

    // 获取线路的第一个站点
    int firstStationId = *(line->stationIds).begin();

    // 确保站点存在
    if (!m_network->stations.contains(firstStationId))
        return;

    // 使用起点站的位置
    QPointF startPosition = m_network->stations[firstStationId].position;
    const QPointF labelOffset(20, -20);

    // 自定义线路标签
    SelectableLineLabel* label = new SelectableLineLabel(line);
    label->setPos(startPosition + labelOffset);
    label->setZValue(4); // 标签显示在最上层

    // 连接点击信号
    connect(label, &SelectableLineLabel::clicked, this, &MetroMapWidget::onLineLabelClicked);

    // 根据缩放因子调整文本大小
    qreal textScale = qMax(0.1, 1.0 / m_scaleFactor);
    label->setScale(textScale);

    m_scene->addItem(label);
    m_lineLabels.insert(line->id, label);
}

bool MetroMapWidget::checkLabelOverlap(SelectableLineLabel* label, const QPointF& position, SelectableLineLabel* existingLabel)
{
    if (!label || !existingLabel) return false;

    // 获取标签的边界矩形（考虑缩放）
    QRectF newLabelRect = label->boundingRect();
    newLabelRect.moveTopLeft(position);
    newLabelRect.setWidth(newLabelRect.width() * label->scale());
    newLabelRect.setHeight(newLabelRect.height() * label->scale());

    // 获取现有标签的边界矩形（考虑缩放）
    QRectF existingLabelRect = existingLabel->boundingRect();
    existingLabelRect.moveTopLeft(existingLabel->pos());
    existingLabelRect.setWidth(existingLabelRect.width() * existingLabel->scale());
    existingLabelRect.setHeight(existingLabelRect.height() * existingLabel->scale());

    // 检查是否重叠（添加一些边距）
    QRectF expandedExistingRect = existingLabelRect.adjusted(-5, -5, 5, 5);

    return expandedExistingRect.intersects(newLabelRect);
}

void MetroMapWidget::zoomIn()
{
    m_scaleFactor *= 1.2;
    m_view->scale(1.2, 1.2);
    updateTextScale();
}

void MetroMapWidget::zoomOut()
{
    m_scaleFactor /= 1.2;
    m_view->scale(1 / 1.2, 1 / 1.2);
    updateTextScale();
}

void MetroMapWidget::resetZoom()
{
    m_view->resetTransform();
    m_scaleFactor = 5.0;

    // 居中显示
    if (!m_scene->items().isEmpty())
    {
        m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
    }

    updateTextScale();
}

void MetroMapWidget::highlightRoute(const RouteResult& route)
{
    clearHighlight();

    // 高亮路径上的站点
    for (int stationId : route.stationPath)
    {
        if (m_stationItems.contains(stationId))
        {
            StationGraphicsItem* item = m_stationItems[stationId];
            item->setBrush(Qt::yellow);
            item->setPen(QPen(Qt::red, 2));
        }
    }

    // 高亮路径上的连接线
    // 根据路径中的站点顺序找到对应的连接线
    for (int i = 0; i < route.stationPath.size() - 1; i++)
    {
        int stationId1 = route.stationPath[i];
        int stationId2 = route.stationPath[i + 1];

        // 查找连接这两个站点的连接线
        for (auto it = m_connectionItems.begin(); it != m_connectionItems.end(); ++it)
        {
            Connection* conn = it.value()->getConnection();
            if ((conn->stationId1 == stationId1 && conn->stationId2 == stationId2) ||
                (conn->stationId1 == stationId2 && conn->stationId2 == stationId1))
            {
                it.value()->setPen(QPen(Qt::red, 4));
                break;
            }
        }
    }
}

void MetroMapWidget::clearHighlight()
{
//    for (StationGraphicsItem* item : m_stationItems)
//    {
//        item->updateAppearance();
//    }

//    for (ConnectionGraphicsItem* item : m_connectionItems)
//    {
//        item->updateAppearance();
//    }

    refreshMap();
}

StationGraphicsItem* MetroMapWidget::findStationItem(int stationId) const
{
    return m_stationItems.value(stationId, nullptr);
}

ConnectionGraphicsItem* MetroMapWidget::findConnectionItem(int connectionId) const
{
    return m_connectionItems.value(connectionId, nullptr);
}

void MetroMapWidget::updateTextScale()
{
    // 根据当前缩放因子调整所有文本的大小
    qreal textScale = qMax(0.5, 1.0 / m_scaleFactor);

    for (auto it = m_labelItems.begin(); it != m_labelItems.end(); ++it) {
        it.value()->setScale(textScale);
    }

    for (auto it = m_lineLabels.begin(); it != m_lineLabels.end(); ++it) {
        it.value()->setScale(textScale);
    }
}

void MetroMapWidget::optimizeLabelPositions()
{
    // 简单的标签位置优化算法，可以避免标签重叠

    QMap<int, QGraphicsTextItem*> labels = m_labelItems;
    QList<int> stationIds = labels.keys();

    // 按位置排序站点
    std::sort(stationIds.begin(), stationIds.end(), [this](int a, int b) {
        return m_network->stations[a].position.y() < m_network->stations[b].position.y();
    });

    // 调整标签位置以避免重叠
    for (int i = 1; i < stationIds.size(); ++i) {
        int prevId = stationIds[i-1];
        int currId = stationIds[i];

        QGraphicsTextItem* prevLabel = labels[prevId];
        QGraphicsTextItem* currLabel = labels[currId];

        QRectF prevRect = prevLabel->boundingRect();
        prevRect.moveTopLeft(prevLabel->pos());
        prevRect = prevLabel->mapToScene(prevRect).boundingRect();

        QRectF currRect = currLabel->boundingRect();
        currRect.moveTopLeft(currLabel->pos());
        currRect = currLabel->mapToScene(currRect).boundingRect();

        // 如果标签重叠，调整位置
        if (prevRect.intersects(currRect)) {
            // 简单地将当前标签向下移动
            QPointF newPos = currLabel->pos() + QPointF(0, prevRect.height() * 0.5);
            currLabel->setPos(newPos);
        }
    }
}

void MetroMapWidget::onLineLabelClicked(MetroLine* line)
{
    // 取消之前选中的标签
    for (auto it = m_lineLabels.begin(); it != m_lineLabels.end(); ++it)
    {
        SelectableLineLabel* label = it.value();
        if (label && label->line()->id != line->id)
        {
            label->setSelected(false);
        }
    }

    // 选中当前点击的标签
    if (m_lineLabels.contains(line->id))
    {
        SelectableLineLabel* label = m_lineLabels[line->id];
        if (label)
        {
            label->setSelected(true);
        }
    }

    // 发出选中信号
    emit lineSelected(line);
}
