#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "metromapwidget.h"
#include "selectablelinelabel.h"

extern QSqlDatabase db;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //以主界面为父窗口创建滚动条
    pScroll=new QScrollArea(this);

    //将包含所有UI信息的Widget放进滚动条区域
    pScroll->setWidgetResizable(true);
    pScroll->setWidget(ui->centralwidget);
    setCentralWidget(pScroll);

    m_currentStationId = -1;
    m_currentConnectionId = -1;
    m_currentLineId = -1;

    // 连接数据库
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("metro_data.db");
    bool ok = db.open();
    if (ok){
        QMessageBox::information(this, "infor", "已成功连接至地铁网络数据库");
        network.initShanghaiMetroData();
    }
    else {
        QMessageBox::information(this, "infor", "无法连接地铁网络数据库");
        qDebug()<<"error open database because"<<db.lastError().text();
    }

    // 获取 UI 中的地图容器
    QWidget *mapContainer = ui->mapContainer;

    metroMapWidget = new MetroMapWidget(&network, mapContainer);

    QVBoxLayout *containerLayout = new QVBoxLayout(mapContainer);
    containerLayout->addWidget(metroMapWidget);
    containerLayout->setContentsMargins(0, 0, 0, 0); // 移除边距

    mapContainer->setLayout(containerLayout);

    // 连接槽函数
    if (metroMapWidget)
    {
        connect(metroMapWidget, &MetroMapWidget::stationSelected,
                this, &MainWindow::onStationSelected);
        connect(metroMapWidget, &MetroMapWidget::connectionSelected,
                this, &MainWindow::onConnectionSelected);
        connect(metroMapWidget, &MetroMapWidget::lineSelected,
                this, &MainWindow::onLineSelected);
        connect(ui->LineColorPick, &ColorPickerButton::colorChanged,
                this, &MainWindow::onLineColorChanged);
    }

    // 布局设置
    // 创建主水平布局
    QHBoxLayout *mainLayout = new QHBoxLayout(ui->centralwidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    // 左侧面板（信息面板和添加站点线路部分）
    QWidget *leftPanel = new QWidget;
    leftPanel->setFixedWidth(550); // 固定宽度
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(10);

    // 添加信息面板
    leftLayout->addWidget(ui->InformationFrame);

    // 添加添加线路和站点区域
    leftLayout->addWidget(ui->frame3);

    // 确保内容顶部对齐
    leftLayout->addStretch();

    // 右侧面板（路线查找和地图）
    QWidget *rightPanel = new QWidget;
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(10);

    // 路线查找区域（上方）
    QWidget *routePanel = new QWidget;
    routePanel->setFixedHeight(250); // 固定高度
    QHBoxLayout *routePanelLayout = new QHBoxLayout(routePanel);
    routePanelLayout->setContentsMargins(0, 0, 0, 0);
    routePanelLayout->setSpacing(10);

    // 创建一个包装widget来包含FindRouteLayout
    QWidget *findRouteWidget = new QWidget;
    findRouteWidget->setLayout(ui->FindRouteLayout);
    findRouteWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    // 设置RouteDisplay的大小策略
    ui->RouteDisplay->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    // 添加到路线查找布局
    routePanelLayout->addWidget(findRouteWidget);
    routePanelLayout->addWidget(ui->RouteDisplay);

    // 设置拉伸比例
    routePanelLayout->setStretch(0, 1); // FindRouteLayout区域
    routePanelLayout->setStretch(1, 2); // RouteDisplay区域

    // 地图区域（下方）
    QWidget *mapPanel = new QWidget;
    QVBoxLayout *mapPanelLayout = new QVBoxLayout(mapPanel);
    mapPanelLayout->setContentsMargins(0, 0, 0, 0);
    mapPanelLayout->setSpacing(10);

    // 地图容器
    mapPanelLayout->addWidget(ui->mapContainer);

    // 缩放按钮区域固定高度
    QWidget *zoomPanel = new QWidget;
    zoomPanel->setFixedHeight(66);
    QHBoxLayout *zoomLayout = new QHBoxLayout(zoomPanel);
    zoomLayout->addWidget(ui->actionZoomIn);
    zoomLayout->addWidget(ui->actionZoomOut);
    zoomLayout->addWidget(ui->actionResetZoom);
    mapPanelLayout->addWidget(zoomPanel);

    // 设置地图区域的拉伸因子
    mapPanelLayout->setStretch(0, 1); // mapContainer占据所有额外空间

    // 将路线查找和地图区域添加到右侧布局
    rightLayout->addWidget(routePanel);
    rightLayout->addWidget(mapPanel);

    // 设置右侧布局的拉伸因子
    rightLayout->setStretch(0, 0); // 路线查找区域固定高度
    rightLayout->setStretch(1, 1); // 地图区域可扩展

    // 将左右面板添加到主布局
    mainLayout->addWidget(leftPanel);
    mainLayout->addWidget(rightPanel);

    // 设置主布局的拉伸因子
    mainLayout->setStretch(0, 0); // 左侧面板固定宽度
    mainLayout->setStretch(1, 1); // 右侧面板可扩展

    ui->centralwidget->setLayout(mainLayout);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Station
Station::Station(int id, QString name, QPointF position, const QSet<int>& lineIds)
    : id(id), name(name), position(position), lineIds(lineIds), transferTime(0) {
    connectedStationIds = QList<int>();
    connectedConnectionIds = QList<int>();
}

void Station::addLine(int lineId) {
    if (!lineIds.contains(lineId)) {
        lineIds.insert(lineId);
        // 如果现在属于多条线路，则为换乘站
        if (lineIds.size() > 1 && transferTime == 0) {
            // 设置默认换乘时间3分钟
            transferTime = DEFAULT_TRANSFER_TIME;
        }
    }
}

void Station::removeLine(int lineId) {
    if (lineIds.contains(lineId)) {
        lineIds.remove(lineId);
        // 如果现在只属于一条线路或没有线路，则不再是换乘站
        if (lineIds.size() <= 1) {
            transferTime = 0;
        }
    }
}

void Station::setTransferTime(int transferTime) {
    this->transferTime = transferTime;
}

bool Station::isTransferStation() {
    return lineIds.size() > 1;
}

const QList<int>& Station::getStations() const {
    return connectedStationIds;
}

const QList<int>& Station::getConnections() const {
    return connectedConnectionIds;
}

void Station::addStation(int stationId) {
    if (!connectedStationIds.contains(stationId)) {
        connectedStationIds.append(stationId);
    }
}

void Station::removeStation(int stationId) {
    connectedStationIds.removeAll(stationId);
}

void Station::addConnection(int connectionId) {
    if (!connectedConnectionIds.contains(connectionId)) {
        connectedConnectionIds.append(connectionId);
    }
}

void Station::removeConnection(int connectionId) {
    connectedConnectionIds.removeAll(connectionId);
}

// Connection
Connection::Connection(int id, int station1, int station2, const QSet<int>& lineIds, int time)
    : id(id), stationId1(station1), stationId2(station2), lineIds(lineIds), travelTime(time) {
}

void Connection::addLine(int lineId) {
    if (!lineIds.contains(lineId)) {
        lineIds.insert(lineId);
    }
}

void Connection::removeLine(int lineId) {
    if (lineIds.contains(lineId))
        lineIds.remove(lineId);
}

void Connection::setTravelTime(int travelTime) {
    // 确保通行时间大于0
    if (travelTime > 0) {
        this->travelTime = travelTime;
    } else {
        this->travelTime = DEFAULT_TRAVEL_TIME; // 默认通行时间3分钟
    }
}

bool Connection::connectsStation(int stationId) const {
    return (stationId1 == stationId) || (stationId2 == stationId);
}

int Connection::getOtherStation(int stationId) const {
    if (stationId1 == stationId) {
        return stationId2;
    } else if (stationId2 == stationId) {
        return stationId1;
    } else {
        // 如果给定的站点ID不属于此连接，返回-1表示无效
        return -1;
    }
}

// MetroLine
MetroLine::MetroLine(int id, QString name, QColor color)
    : id(id), name(name), color(color) {
}

void MetroLine::addStation(int stationId) {
    if (!stationIds.contains(stationId)) {
        stationIds.insert(stationId);
    }
}

void MetroLine::removeStation(int stationId) {
    if (stationIds.contains(stationId)) {
        stationIds.remove(stationId);

    }
}

void MetroLine::addConnection(int connectionId) {
    if (!connectionIds.contains(connectionId)) {
        connectionIds.insert(connectionId);
    }
}

void MetroLine::removeConnection(int connectionId) {
    if (connectionIds.contains(connectionId)) {
        connectionIds.remove(connectionId);
    }
}

bool MetroLine::containsStation(int stationId) const {
    return stationIds.contains(stationId);
}

bool MetroLine::containsConnection(int connectionId) const {
    return connectionIds.contains(connectionId);
}

// MetroNetwork
MetroNetwork::MetroNetwork()
    : nextStationId(1), nextLineId(1), nextConnectionId(1), routeResult(*new RouteResult()) {
    // 初始化图结构为空
    graph = QMap<int, QList<std::pair<int, int>>>();
}

int MetroNetwork::addStation(const QString& name, const QPointF& position, const QSet<int>& lineIds) {
    int id = nextStationId++;
    stations.insert(id, Station(id, name, position, lineIds));

    // 初始化该站点在图结构中的邻接表（空列表）
    graph[id] = QList<std::pair<int, int>>();

    // 将站点添加到所属线路
    for (int lineId : lineIds) {
        if (lines.contains(lineId)) {
            lines[lineId].addStation(id);
        }
    }

    return id;
}

bool MetroNetwork::removeStation(int stationId) {
    if (!stations.contains(stationId)) {
        return false;
    }

    Station& station = stations[stationId];

    // 从所有所属线路中移除站点
    for (int lineId : station.lineIds) {
        if (lines.contains(lineId)) {
            lines[lineId].removeStation(stationId);
        }
    }

    // 移除所有连接到该站点的连接
    QList<int> connectionsToRemove = station.connectedConnectionIds;
    for (int connectionId : connectionsToRemove) {
        removeConnection(connectionId);
    }

    // 从所有连接的站点中移除该站点
    for (int connectedStationId : station.connectedStationIds) {
        if (stations.contains(connectedStationId)) {
            stations[connectedStationId].removeStation(stationId);
        }
    }

    // 从图结构中移除该站点
    graph.remove(stationId);

    // 最后移除站点本身
    stations.remove(stationId);
    return true;
}

bool MetroNetwork::updateStation(int stationId, const QString& name, const QPointF& position) {
    if (!stations.contains(stationId)) {
        return false;
    }

    Station& station = stations[stationId];
    station.name = name;
    station.position = position;
    return true;
}

int MetroNetwork::addLine(const QString& name, const QColor& color) {
    int id = nextLineId++;
    lines.insert(id, MetroLine(id, name, color));
    return id;
}

bool MetroNetwork::removeLine(int lineId) {
    if (!lines.contains(lineId)) {
        return false;
    }

    MetroLine& line = lines[lineId];

    // 从所有站点中移除该线路
    for (int stationId : line.stationIds) {
        if (stations.contains(stationId)) {
            stations[stationId].removeLine(lineId);
        }
    }

    // 从所有连接中移除该线路
    for (int connectionId : line.connectionIds) {
        if (connections.contains(connectionId)) {
            removeLineFromConnection(connectionId, lineId);
        }
    }

    // 最后移除线路本身
    lines.remove(lineId);
    return true;
}

bool MetroNetwork::updateLine(int lineId, const QString& name, const QColor& color) {
    if (!lines.contains(lineId)) {
        return false;
    }

    MetroLine& line = lines[lineId];
    line.name = name;
    line.color = color;
    return true;
}

int MetroNetwork::addConnection(int stationId1, int stationId2, int travelTime) {
    if (!stations.contains(stationId1) || !stations.contains(stationId2)) {
        return -1;
    }

    // 获取两个站点所属线路的交集
    QSet<int> commonLineIds = stations[stationId1].lineIds;
    commonLineIds.intersect(stations[stationId2].lineIds);

    // 如果交集为空，添加失败
    if (commonLineIds.isEmpty()) {
        return -1;
    }

    // 验证连接有效性
    if (!isValidConnection(stationId1, stationId2, commonLineIds)) {
        return -1;
    }

    int id = nextConnectionId++;
    connections.insert(id, Connection(id, stationId1, stationId2, commonLineIds, travelTime));

    // 更新站点连接信息
    stations[stationId1].addConnection(id);
    stations[stationId1].addStation(stationId2);

    stations[stationId2].addConnection(id);
    stations[stationId2].addStation(stationId1);

    // 将连接添加到所属线路
    for (int lineId : commonLineIds) {
        if (lines.contains(lineId)) {
            lines[lineId].addConnection(id);
        }
    }

    // 更新图结构 - 添加双向连接
    graph[stationId1].append({stationId2, id});
    graph[stationId2].append({stationId1, id});

    return id;
}

bool MetroNetwork::removeConnection(int connectionId) {
    if (!connections.contains(connectionId)) {
        return false;
    }

    Connection& connection = connections[connectionId];

    // 从图结构中移除连接
    auto removeFromGraph = [this](int stationId, int targetId, int connId) {
        if (graph.contains(stationId)) {
            auto& neighbors = graph[stationId];
            for (int i = 0; i < neighbors.size(); ++i) {
                if (neighbors[i].first == targetId &&
                    connections.contains(neighbors[i].second) &&
                    connections[neighbors[i].second].id == connId) {
                    neighbors.removeAt(i);
                    break;
                }
            }
        }
    };

    removeFromGraph(connection.stationId1, connection.stationId2, connectionId);
    removeFromGraph(connection.stationId2, connection.stationId1, connectionId);

    // 从所有所属线路中移除连接
    for (int lineId : connection.lineIds) {
        if (lines.contains(lineId)) {
            lines[lineId].removeConnection(connectionId);
        }
    }

    // 从连接的站点中移除连接
    if (stations.contains(connection.stationId1)) {
        stations[connection.stationId1].removeConnection(connectionId);
        stations[connection.stationId1].removeStation(connection.stationId2);
    }

    if (stations.contains(connection.stationId2)) {
        stations[connection.stationId2].removeConnection(connectionId);
        stations[connection.stationId2].removeStation(connection.stationId1);
    }

    // 最后移除连接本身
    connections.remove(connectionId);
    return true;
}

bool MetroNetwork::updateConnection(int connectionId, int travelTime) {
    if (!connections.contains(connectionId)) {
        return false;
    }

    Connection& connection = connections[connectionId];
    connection.setTravelTime(travelTime);
    return true;
}

bool MetroNetwork::addStationToLine(int stationId, int lineId) {
    if (!stations.contains(stationId) || !lines.contains(lineId)) {
        return false;
    }

    Station& station = stations[stationId];
    MetroLine& line = lines[lineId];

    station.addLine(lineId);
    line.addStation(stationId);

    return true;
}

bool MetroNetwork::removeStationFromLine(int stationId, int lineId) {
    if (!stations.contains(stationId) || !lines.contains(lineId)) {
        return false;
    }

    Station& station = stations[stationId];
    MetroLine& line = lines[lineId];

    // 从线路移除站点
    station.removeLine(lineId);
    line.removeStation(stationId);

    // 处理与该站点相关的连接边
    QList<int> connectionsToCheck = station.connectedConnectionIds;
    for (int connectionId : connectionsToCheck) {
        if (connections.contains(connectionId)) {
            Connection& connection = connections[connectionId];

            // 如果连接边属于要移除的线路，从连接边中移除该线路
            if (connection.lineIds.contains(lineId)) {
                // 从连接边中移除线路
                connection.removeLine(lineId);

                // 从线路中移除连接边
                line.removeConnection(connectionId);

                // 检查连接边是否还有效（是否仍然属于至少一条线路）
                if (connection.lineIds.isEmpty()) {
                    // 连接边不再属于任何线路，删除它
                    removeConnection(connectionId);
                } else {
                    // 连接边仍然有效，检查它是否仍然连接两个属于共同线路的站点
                    int otherStationId = connection.getOtherStation(stationId);
                    if (otherStationId != -1 && stations.contains(otherStationId)) {
                        Station& otherStation = stations[otherStationId];

                        // 检查两个站点是否还有共同的线路
                        QSet<int> commonLines = station.lineIds;
                        commonLines.intersect(otherStation.lineIds);
                        commonLines.intersect(connection.lineIds);

                        if (commonLines.isEmpty()) {
                            // 连接边不再有效，删除它
                            removeConnection(connectionId);
                        }
                    }
                }
            }
        }
    }

    return true;
}

bool MetroNetwork::addConnectionToLine(int connectionId, int lineId) {
    if (!connections.contains(connectionId) || !lines.contains(lineId)) {
        return false;
    }

    Connection& connection = connections[connectionId];
    MetroLine& line = lines[lineId];

    // 检查连接边的两个端点站点是否都属于该线路
    if (!line.containsStation(connection.stationId1) || !line.containsStation(connection.stationId2)) {
        return false;
    }

    // 添加连接到线路
    connection.addLine(lineId);
    line.addConnection(connectionId);

    return true;
}

bool MetroNetwork::removeConnectionFromLine(int connectionId, int lineId) {
    if (!connections.contains(connectionId) || !lines.contains(lineId)) {
        return false;
    }

    Connection& connection = connections[connectionId];
    MetroLine& line = lines[lineId];

    connection.removeLine(lineId);
    line.removeConnection(connectionId);

    // 如果连接不再属于任何线路，则删除整个连接
    if (connection.lineIds.isEmpty()) {
        return removeConnection(connectionId);
    }

    return true;
}

bool MetroNetwork::removeLineFromConnection(int connectionId, int lineId)
{
    if (!connections.contains(connectionId)) {
        return false;
    }

    Connection& connection = connections[connectionId];

    connection.removeLine(lineId);

    // 如果连接不再属于任何线路，则删除整个连接
    if (connection.lineIds.isEmpty()) {
        return removeConnection(connectionId);
    }

    return true;
}

void MetroNetwork::findRoute(int startId, int endId, int priority) const {
    RouteResult &result = routeResult;
    result.clear();

    if (!stations.contains(startId) || !stations.contains(endId)) {
        return;
    }

    // 使用优先队列的Dijkstra
    QMap<int, int> dist; // 最短距离
    QMap<int, int> minTime; // 实际最短时间
    QMap<int, int> prevStation; // 前驱站点
    QMap<int, int> prevConnection; // 前驱连接边
    QMap<int, int> prevLine; // 前驱线路
    QMap<int, bool> visited;

    for (auto it = stations.begin(); it != stations.end(); ++it) {
        dist[it.key()] = INT_MAX;
        minTime[it.key()] = INT_MAX;
        visited[it.key()] = false;
    }

    dist[startId] = 0;
    minTime[startId] = 0;

    // 优先队列(最小堆)
    using Pair = std::pair<int, int>; // (距离, 站点ID)
    std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> pq;
    pq.push({0, startId});

    while (!pq.empty()) {
        int current = pq.top().second;
        pq.pop();

        if (visited[current]) continue;
        visited[current] = true;

        if (current == endId) break;

        // 遍历所有相邻站点
        for (const auto& neighbor : graph.value(current)) {
            int neighborId = neighbor.first;
            int connectionId = neighbor.second;

            if (!connections.contains(connectionId)) continue;

            const Connection& conn = connections[connectionId];
            int weight = conn.travelTime;
            int actualTime = conn.travelTime;

            // 确定当前连接应该使用的线路
            int currentLine = -1;
            bool isTransfer = false;

            // 如果有前驱站点，尝试使用与前驱相同的线路
            if (prevStation.contains(current) && prevLine.contains(current)) {
                int prevLineId = prevLine[current];

                // 检查当前连接是否支持前驱线路
                if (conn.lineIds.contains(prevLineId)) {
                    currentLine = prevLineId;
                } else {
                    // 需要换乘，选择任意一条可用线路
                    if (!conn.lineIds.isEmpty()) {
                        currentLine = *conn.lineIds.begin();
                    }

                    // 标记为换乘
                    isTransfer = true;

                    // 添加换乘惩罚（只影响路径选择，不影响实际时间）
                    weight += stations[current].transferTime;
                    actualTime += stations[current].transferTime;

                    // 如果优先考虑少换乘，增加换乘惩罚
                    if (priority == 2) {
                        weight += 300; // 5分钟
                    }
                }
            } else {
                // 起始点，选择任意一条可用线路
                if (!conn.lineIds.isEmpty()) {
                    currentLine = *conn.lineIds.begin();
                }
            }

            if (dist[current] + weight < dist[neighborId]) {
                dist[neighborId] = dist[current] + weight;
                minTime[neighborId] = minTime[current] + actualTime;
                prevStation[neighborId] = current;
                prevConnection[neighborId] = connectionId;

                if (isTransfer) {
                    prevLine[neighborId] = currentLine;
                } else if (prevLine.contains(current)) {
                    // 如果没有换乘，保持与前驱相同的线路
                    prevLine[neighborId] = prevLine[current];
                } else {
                    // 起始点，使用当前线路
                    prevLine[neighborId] = currentLine;
                }

                pq.push({dist[neighborId], neighborId});
            }
        }
    }

    // 回溯路径
    if (dist[endId] < INT_MAX) {
        int current = endId;
        QList<int> reverseStationPath;
        QList<int> reverseLinePath;

        QSet<int> processedConnections; // 记录已处理的连接
        QSet<int> processedTransfers; // 记录已处理的换乘点

        // 首先收集所有站点
        while (current != startId) {
            reverseStationPath.prepend(current);
            current = prevStation[current];
        }
        reverseStationPath.prepend(startId);
        result.stationPath = reverseStationPath;

        // 现在确定线路路径
        int lastLine = -1;
        for (int i = 0; i < reverseStationPath.size() - 1; i++) {
            int stationId = reverseStationPath[i];
            int nextStationId = reverseStationPath[i + 1];

            // 查找连接
            int connId = -1;
            for (const auto& neighbor : graph.value(stationId)) {
                if (neighbor.first == nextStationId) {
                    connId = neighbor.second;
                    break;
                }
            }

            if (connId != -1 && connections.contains(connId) && !processedConnections.contains(connId)) {
                processedConnections.insert(connId);
                const Connection& conn = connections[connId];

                // 确定当前段使用的线路
                int currentLine = -1;

                // 使用记录的前驱线路
                if (prevLine.contains(nextStationId)) {
                    currentLine = prevLine[nextStationId];
                } else if (!conn.lineIds.isEmpty()) {
                    // 否则选择第一条可用线路
                    currentLine = *conn.lineIds.begin();
                }

                // 如果线路发生变化，添加到线路路径
                if (currentLine != -1 && currentLine != lastLine) {
                    reverseLinePath.append(currentLine);
                    lastLine = currentLine;

                    // 如果是换乘站且尚未记录，记录换乘点
                    if (i > 0 && !processedTransfers.contains(stationId)) {
                        result.addTransferPoint(stationId, stations[stationId].transferTime);
                        processedTransfers.insert(stationId);
                    }
                }

                // 更新线路时间（只添加一次）
                if (currentLine != -1) {
                    result.lineTimes[currentLine] += conn.travelTime;
                }
            }
        }

        // 设置线路路径
        result.linePath = reverseLinePath;

        result.totalTime = minTime[endId];

        calculateDistance();
        result.calculateCost();
    }

    return;
}
bool MetroNetwork::isValidConnection(int stationId1, int stationId2, const QSet<int>& lineIds) const {
    // 检查站点是否存在
    if (!stations.contains(stationId1) || !stations.contains(stationId2)) {
        return false;
    }

    // 检查线路是否存在
    for (int lineId : lineIds) {
        if (!lines.contains(lineId)) {
            return false;
        }
    }

    // 检查两个站点是否有共同的线路
    QSet<int> commonLines = stations[stationId1].lineIds;
    commonLines.intersect(stations[stationId2].lineIds);
    commonLines.intersect(lineIds);

    return !commonLines.isEmpty();
}

// 检查是否可以添加站点到线路
//bool MetroNetwork::canAddStationToLine(int stationId, int lineId) const {
//    if (!stations.contains(stationId) || !lines.contains(lineId)) {
//        return false;
//    }

//    // 检查站点是否已经在线路中
//    if (lines[lineId].containsStation(stationId)) {
//        return false;
//    }

//    // 检查站点是否与线路中的至少一个站点相邻
//    const MetroLine& line = lines[lineId];
//    for (int lineStationId : line.stationIds) {
//        if (stations.contains(lineStationId)) {
//            const Station& lineStation = stations[lineStationId];
//            for (int connectedStationId : lineStation.connectedStationIds) {
//                if (connectedStationId == stationId) {
//                    return true;
//                }
//            }
//        }
//    }

//    return false;
//}

// 检查是否可以从线路移除站点
//bool MetroNetwork::canRemoveStationFromLine(int stationId, int lineId) const {
//    if (!stations.contains(stationId) || !lines.contains(lineId)) {
//        return false;
//    }

//    // 检查站点是否在线路中
//    if (!lines[lineId].containsStation(stationId)) {
//        return false;
//    }

//    // 检查移除站点是否会破坏线路的连通性
//    // 这是一个复杂的问题，简化实现：总是允许移除
//    return true;
//}

bool MetroNetwork::saveToFile() const {
    // TODO导出数据
    return true;
}

void MetroNetwork::initShanghaiMetroData()
{
    // 清除现有数据
    stations.clear();
    lines.clear();
    connections.clear();
    nextStationId = 1;
    nextLineId = 1;
    nextConnectionId = 1;

    // 检查数据库连接
    if (!db.isOpen()) {
        qDebug() << "Database is not open!";
        return;
    }

    // 获取上海市的ID
    int shanghaiCityId = -1;
    QSqlQuery cityQuery(db);
    QString queryStr = "SELECT id FROM citys WHERE cn_name = '上海市'";

    if (cityQuery.exec(queryStr)) {
        if (cityQuery.next()) {
            shanghaiCityId = cityQuery.value(0).toInt();
            qDebug() << "Found Shanghai city ID:" << shanghaiCityId;
        } else {
            qDebug() << "上海市 not found in citys table";
            return;
        }
    } else {
        qDebug() << "Failed to get Shanghai city ID:" << cityQuery.lastError().text();
        return;
    }

    // 获取上海的所有地铁线路
    QSqlQuery lineQuery(db);
    lineQuery.prepare("SELECT id, name FROM lines WHERE city_id = :city_id");
    lineQuery.bindValue(":city_id", shanghaiCityId);

    // 线路颜色映射
    QMap<int, QColor> lineColors = {
        {1, QColor(0xE4002B)},    // 1号线 - 红色
        {2, QColor(0x97D700)},    // 2号线 - 绿色
        {3, QColor(0xFFD100)},    // 3号线 - 黄色
        {4, QColor(0x5F259F)},    // 4号线 - 紫色
        {5, QColor(0xAC4FC6)},    // 5号线 - 紫红色
        {6, QColor(0xD9027D)},    // 6号线 - 粉色
        {7, QColor(0xFF6900)},    // 7号线 - 橙色
        {8, QColor(0x009B95)},    // 8号线 - 蓝绿色
        {9, QColor(0x71C5E8)},    // 9号线 - 浅蓝色
        {10, QColor(0xC9A0D0)},   // 10号线 - 淡紫色
        {11, QColor(0x762123)},   // 11号线 - 棕色
        {12, QColor(0x007C65)},   // 12号线 - 深绿色
        {13, QColor(0xEF95CF)},   // 13号线 - 粉红色
        {14, QColor(0x827A04)},   // 14号线 - 橄榄绿
        {15, QColor(0xBBA786)},   // 15号线 - 香槟色
        {16, QColor(0x94D4EF)},   // 16号线 - 水蓝色
        {17, QColor(0xB67770)},   // 17号线 - 玫瑰棕色
        {18, QColor(0xD6A01D)},   // 18号线 - 土黄色
    };

    // 数据库线路ID到实际线路ID的映射
    QMap<int, int> dbLineIdToActualId;

    if (lineQuery.exec()) {
        QSet<int> addedLineNumbers; // 用于记录已经添加的线路编号

        while (lineQuery.next()) {
            int dbLineId = lineQuery.value("id").toInt();
            QString lineName = lineQuery.value("name").toString();

            // 使用正则表达式从线路名称中提取数字
            QRegularExpression re("(\\d+)");
            QRegularExpressionMatch match = re.match(lineName);

            if (match.hasMatch()) {
                int lineNumber = match.captured(1).toInt();

                // 检查是否已经添加过这个编号的线路
                if (addedLineNumbers.contains(lineNumber)) {
                    qDebug() << "Skipping duplicate line:" << lineName << "(Number:" << lineNumber << ")";
                    continue; // 跳过重复线路
                }

                // 标记这个编号的线路已添加
                addedLineNumbers.insert(lineNumber);

                // 创建简单的线路名称（如"16号线"）
                QString simpleLineName = QString("%1号线").arg(lineNumber);

                // 添加线路到网络
                QColor lineColor = lineColors.contains(lineNumber) ?
                                  lineColors[lineNumber] :
                                  QColor(QRandomGenerator::global()->generate());

                int actualLineId = addLine(simpleLineName, lineColor);
                dbLineIdToActualId[dbLineId] = actualLineId;

                qDebug() << "Added line:" << simpleLineName << "DB ID:" << dbLineId << "Actual ID:" << actualLineId;
            } else {
                qDebug() << "Skipping line (no number):" << lineName;
            }
        }
    } else {
        qDebug() << "Failed to get lines:" << lineQuery.lastError().text();
        return;
    }

    // 获取所有站点
    QSqlQuery stationQuery(db);
    stationQuery.prepare("SELECT name, lat, lng, line_id FROM steps WHERE line_id IN (SELECT id FROM lines WHERE city_id = :city_id)");
    stationQuery.bindValue(":city_id", shanghaiCityId);

    // 站点名称到ID的映射
    QMap<QString, int> stationNameToId;
    // 线路ID到站点列表的映射
    QMap<int, QList<QPair<QString, QPointF>>> lineStations;

    if (stationQuery.exec()) {
        while (stationQuery.next()) {
            QString stationName = stationQuery.value("name").toString();
            double lat = stationQuery.value("lat").toDouble();
            double lng = stationQuery.value("lng").toDouble();
            int dbLineId = stationQuery.value("line_id").toInt();

            // 跳过不属于已添加线路的站点
            if (!dbLineIdToActualId.contains(dbLineId)) {
                continue;
            }

            int actualLineId = dbLineIdToActualId[dbLineId];

            // 将经纬度转换为平面坐标
            QPointF position(lat * POS_SCALE, -lng * POS_SCALE);

            // 如果站点已存在，更新其线路信息
            if (stationNameToId.contains(stationName)) {
                int stationId = stationNameToId[stationName];
                stations[stationId].lineIds.insert(actualLineId);

                // 更新线路的站点信息
                lines[actualLineId].addStation(stationId);
            } else {
                // 添加新站点
                QSet<int> lineIds = {actualLineId};
                int stationId = addStation(stationName, position, lineIds);
                stationNameToId[stationName] = stationId;

                // 添加到线路的站点列表
                lines[actualLineId].addStation(stationId);
            }

            // 添加到线路站点映射
            lineStations[actualLineId].append(qMakePair(stationName, position));
        }
    } else {
        qDebug() << "Failed to get stations:" << stationQuery.lastError().text();
        return;
    }

    // 为每条线路添加连接
    for (auto it = lineStations.begin(); it != lineStations.end(); ++it) {
        int lineId = it.key();
        const auto& stationsOnLine = it.value();

        for (int i = 0; i < stationsOnLine.size() - 1; i++) {
            QString station1Name = stationsOnLine[i].first;
            QString station2Name = stationsOnLine[i + 1].first;

            if (stationNameToId.contains(station1Name) && stationNameToId.contains(station2Name)) {
                int station1Id = stationNameToId[station1Name];
                int station2Id = stationNameToId[station2Name];

                // 检查连接是否已存在
                bool connectionExists = false;
                int existingConnectionId = -1;

                for (auto connIt = connections.begin(); connIt != connections.end(); ++connIt) {
                    const Connection& conn = connIt.value();
                    if ((conn.stationId1 == station1Id && conn.stationId2 == station2Id) ||
                        (conn.stationId1 == station2Id && conn.stationId2 == station1Id)) {
                        connectionExists = true;
                        existingConnectionId = connIt.key();
                        break;
                    }
                }

                if (connectionExists) {
                    // 连接已存在，添加线路信息
                    connections[existingConnectionId].addLine(lineId);
                    lines[lineId].addConnection(existingConnectionId);
                } else {
                    // 创建新连接
                    QSet<int> lineIds = {lineId};
                    int connectionId = addConnection(station1Id, station2Id, 3); // 默认通行时间3分钟

                    // 添加到线路
                    lines[lineId].addConnection(connectionId);

                    // 更新站点的连接信息
                    stations[station1Id].addConnection(connectionId);
                    stations[station2Id].addConnection(connectionId);

                    // 更新站点的相邻站点信息
                    stations[station1Id].addStation(station2Id);
                    stations[station2Id].addStation(station1Id);
                }
            }
        }
    }

    // 设置换乘站信息
    for (auto it = stations.begin(); it != stations.end(); ++it) {
        Station& station = it.value();
        if (station.lineIds.size() > 1) {
            // 这是一个换乘站
            station.setTransferTime(5); // 设置换乘时间为5分钟
        }
    }

    buildGraph();

    qDebug() << "Loaded" << stations.size() << "stations," << lines.size() << "lines," << connections.size() << "connections from database.";
}

void MainWindow::on_actionZoomIn_clicked()
{
    if (metroMapWidget) {
        metroMapWidget->zoomIn();
    }
}

void MainWindow::on_actionZoomOut_clicked()
{
    if (metroMapWidget) {
        metroMapWidget->zoomOut();
    }
}

void MainWindow::on_actionResetZoom_clicked()
{
    if (metroMapWidget) {
        metroMapWidget->resetZoom();
    }
}

void MainWindow::on_MainWindow_iconSizeChanged(const QSize &iconSize)
{
    if(pScroll)
    {
        // 设置滚动条区域大小时，需要减去状态栏的高度
        // 此处53是标题栏与任务栏的高度，是centralWidget相对于主界面左上角坐标的y值
        int y_0 = ui->menubar->geometry().y() + ui->menubar->height();
        pScroll->setGeometry(0,y_0,width(),height()-ui->statusbar->height()-y_0);
        ui->centralwidget->setMinimumSize(width(),height()-ui->statusbar->height()-y_0);
    }
}

void MainWindow::onStationSelected(Station* station)
{
    if (!station) return;

    m_currentStationId = station->id;
    m_currentConnectionId = -1;
    m_currentLineId = -1;

    m_originalStationName = station->name;
    m_originalStationX = station->position.x();
    m_originalStationY = station->position.y();

    // 更新UI显示站点信息
    ui->StationName->setText(station->name);
    ui->StationX->setText(QString::number(station->position.x()));
    ui->StationY->setText(QString::number(station->position.y()));

    // 清空之前的线路列表
    ui->StationLine->clear();

    // 添加所属线路
    for (int lineId : station->lineIds) {
        if (network.lines.contains(lineId)) {
            QString lineName = network.lines[lineId].name;
            ui->StationLine->addItem(lineName);
        }
    }

    ui->TransferTime->clear();
    //if(station->transferTime > 0)
        ui->TransferTime->setText(QString::number(station->transferTime));

    if (metroMapWidget && station) {
        if (metroMapWidget->findStationItem(station->id)) {
            metroMapWidget->centerOnItem(metroMapWidget->findStationItem(station->id));
        }
    }
}

void MainWindow::on_AddStationLine_clicked()
{
    if (m_currentStationId == -1) {
        QMessageBox::warning(this, "警告", "请先选择一个站点！");
        return;
    }

    // 在列表中添加一个新的空项，并设置为可编辑状态
    QListWidgetItem* newItem = new QListWidgetItem("点击输入线路名称");
    newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
    ui->StationLine->addItem(newItem);

    // 立即进入编辑状态
    ui->StationLine->editItem(newItem);
}

void MainWindow::on_StationLine_itemChanged(QListWidgetItem *item)
{
    // 防止递归调用
    static bool isProcessing = false;
    if (isProcessing) return;
    isProcessing = true;

    // 获取编辑后的文本
    QString newLineName = item->text();

    // 检查是否为空
    if (newLineName.isEmpty()) {
        // 如果为空，删除该项
        int row = ui->StationLine->row(item);
        delete ui->StationLine->takeItem(row);
        isProcessing = false;
        return;
    }

    // 检查线路是否存在
    bool lineExists = false;
    int lineId = -1;

    for (auto it = network.lines.begin(); it != network.lines.end(); ++it) {
        if (it.value().name == newLineName) {
            lineExists = true;
            lineId = it.key();
            break;
        }
    }

    if (!lineExists) {
        // 线路不存在，撤销更改
        QMessageBox::warning(this, "错误", "线路 '" + newLineName + "' 不存在！");

        // 恢复为原始文本或删除该项
        if (item->data(Qt::UserRole).isValid()) {
            // 如果是编辑现有项，恢复原始值
            QString originalText = item->data(Qt::UserRole).toString();
            item->setText(originalText);
        } else {
            // 如果是新增项，删除该项
            int row = ui->StationLine->row(item);
            delete ui->StationLine->takeItem(row);
        }
    } else {
        // 线路存在，更新站点所属线路
        if (network.stations.contains(m_currentStationId)) {
            Station& station = network.stations[m_currentStationId];

            // 检查是否已经是该线路的站点
            if (station.lineIds.contains(lineId)) {
                QMessageBox::information(this, "提示", "站点已经是线路 '" + newLineName + "' 的成员");
                // 删除该项
                int row = ui->StationLine->row(item);
                delete ui->StationLine->takeItem(row);
            } else {
                // 添加站点到线路
                if (network.addStationToLine(m_currentStationId, lineId)) {
                    // 保存原始文本到UserRole，以便后续编辑
                    item->setData(Qt::UserRole, newLineName);
                    QMessageBox::information(this, "成功", "已将站点添加到线路 '" + newLineName + "'");

                    // 检查线路是否有站点
                    if (metroMapWidget && metroMapWidget->m_network->lines.contains(lineId)) {
                        MetroLine& line = metroMapWidget->m_network->lines[lineId];

                        // 如果这是线路的第一个站点，创建标签
                        if (line.stationIds.size() == 1) { // 刚刚添加了第一个站点
                            // 创建线路标签
                            metroMapWidget->addLineLabel(&line);
                        }
                    }

                    // 刷新地图显示
                    if (metroMapWidget) {
                        //metroMapWidget->refreshMap();
                    }
                } else {
                    QMessageBox::warning(this, "错误", "无法将站点添加到线路 '" + newLineName + "'");
                    // 删除该项
                    int row = ui->StationLine->row(item);
                    delete ui->StationLine->takeItem(row);
                }
            }
        }
    }

    isProcessing = false;
}

void MainWindow::on_DeleteStationLine_clicked()
{
    if (m_currentStationId == -1) {
        QMessageBox::warning(this, "警告", "请先选择一个站点！");
        return;
    }

    // 获取当前选中的列表项
    QListWidgetItem* selectedItem = ui->StationLine->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "警告", "请先选择要删除的线路！");
        return;
    }

    // 获取线路名称查找对应的线路ID
    QString lineName = selectedItem->text();

    int lineId = -1;
    for (auto it = network.lines.begin(); it != network.lines.end(); ++it) {
        if (it.value().name == lineName) {
            lineId = it.key();
            break;
        }
    }

    if (lineId == -1) {
        QMessageBox::warning(this, "错误", "找不到对应的线路信息！");
        return;
    }

    // 确认删除
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认删除",
                                 "确定要从站点 '" + network.stations[m_currentStationId].name +
                                 "' 中移除线路 '" + lineName + "' 吗？",
                                 QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // 从站点中移除线路
        if (network.removeStationFromLine(m_currentStationId, lineId)) {
            // 从列表中移除项
            int row = ui->StationLine->row(selectedItem);
            delete ui->StationLine->takeItem(row);

            QMessageBox::information(this, "成功", "已从站点中移除线路 '" + lineName + "'");

            if (metroMapWidget) {
                metroMapWidget->refreshMap();
            }
        } else {
            QMessageBox::warning(this, "错误", "无法从站点中移除线路 '" + lineName + "'");
        }
    }
}

void MainWindow::on_DeleteStation_clicked()
{
    if (m_currentStationId == -1) {
        QMessageBox::warning(this, "警告", "请先选择一个站点！");
        return;
    }

    // 获取当前选中的站点
    if (!network.stations.contains(m_currentStationId)) {
        QMessageBox::warning(this, "错误", "选中的站点不存在！");
        return;
    }

    Station& station = network.stations[m_currentStationId];

    // 确认删除
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认删除",
                                 "确定要删除站点 '" + station.name + "' 吗？\n"
                                 "此操作将同时删除所有连接到该站点的连接边。",
                                 QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    // 删除站点
    if (network.removeStation(m_currentStationId)) {
        // 清除当前选中的站点
        m_currentStationId = -1;

        // 清空UI中的站点信息
        ui->StationName->clear();
        ui->StationX->clear();
        ui->StationY->clear();
        ui->TransferTime->clear();
        ui->StationLine->clear();

        if (metroMapWidget) {
            metroMapWidget->refreshMap();
        }

        QMessageBox::information(this, "成功", "站点已成功删除！");
    } else {
        QMessageBox::warning(this, "错误", "无法删除站点！");
    }
}

void MainWindow::on_StationName_editingFinished()
{
    if (m_currentStationId == -1) {
        return;
    }

    // 获取新的站点名称如果名称为空，不处理
    QString newName = ui->StationName->text();

    if (newName.isEmpty()) {
        return;
    }

    // 防止递归调用，因为会修改文本内容
    static bool isProcessing = false;
    if (isProcessing) return;
    isProcessing = true;

    // 检查名称是否与现有站点重复（除了当前站点）
    bool nameExists = false;
    for (auto it = network.stations.begin(); it != network.stations.end(); ++it) {
        if (it.key() != m_currentStationId && it.value().name == newName) {
            nameExists = true;
            break;
        }
    }

    if (nameExists) {
        // 名称重复，恢复原来的名称
        QString originalName = network.stations[m_currentStationId].name;
        ui->StationName->setText(originalName);
        QMessageBox::warning(this, "错误", "站点名称 '" + newName + "' 已存在，请使用不同的名称！");
    } else {
        // 名称唯一，更新站点名称
        if (network.updateStation(m_currentStationId, newName, network.stations[m_currentStationId].position)) {
            // 更新成功，刷新地图显示
            if (metroMapWidget) {
                metroMapWidget->refreshMap();
            }
        } else {
            // 更新失败，恢复原来的名称
            QString originalName = network.stations[m_currentStationId].name;
            ui->StationName->setText(originalName);
            QMessageBox::warning(this, "错误", "无法更新站点名称！");
        }
    }

    isProcessing = false;
}

void MainWindow::on_StationX_editingFinished()
{
    if (m_currentStationId == -1) {
        return;
    }

    // 防止递归调用
    static bool isProcessing = false;
    if (isProcessing) return;
    isProcessing = true;

    // 获取新的X坐标
    QString xText = ui->StationX->text();
    bool ok;
    double newX = xText.toDouble(&ok);

    if (!ok) {
        // 如果不是有效的数字，恢复原来的值
        double originalX = network.stations[m_currentStationId].position.x();
        ui->StationX->setText(QString::number(originalX));
        QMessageBox::warning(this, "错误", "请输入有效的数字！");
        isProcessing = false;
        return;
    }

    // 获取当前的Y坐标
    double currentY = network.stations[m_currentStationId].position.y();

    // 更新站点坐标
    if (network.updateStation(m_currentStationId,
                             network.stations[m_currentStationId].name,
                             QPointF(newX, currentY))) {
        if (metroMapWidget) {
            metroMapWidget->refreshMap();
        }
    } else {
        // 更新失败，恢复原来的值
        double originalX = network.stations[m_currentStationId].position.x();
        ui->StationX->setText(QString::number(originalX));
        QMessageBox::warning(this, "错误", "无法更新站点坐标！");
    }

    isProcessing = false;
}

void MainWindow::on_StationY_editingFinished()
{
    if (m_currentStationId == -1) {
        return;
    }

    // 防止递归调用
    static bool isProcessing = false;
    if (isProcessing) return;
    isProcessing = true;

    // 获取新的Y坐标
    QString yText = ui->StationY->text();
    bool ok;
    double newY = yText.toDouble(&ok);

    if (!ok) {
        // 如果不是有效的数字，恢复原来的值
        double originalY = network.stations[m_currentStationId].position.y();
        ui->StationY->setText(QString::number(originalY));
        QMessageBox::warning(this, "错误", "请输入有效的数字！");
        isProcessing = false;
        return;
    }

    // 获取当前的X坐标
    double currentX = network.stations[m_currentStationId].position.x();

    // 更新站点坐标
    if (network.updateStation(m_currentStationId,
                             network.stations[m_currentStationId].name,
                             QPointF(currentX, newY))) {
        if (metroMapWidget) {
            metroMapWidget->refreshMap();
        }
    } else {
        // 更新失败，恢复原来的值
        double originalY = network.stations[m_currentStationId].position.y();
        ui->StationY->setText(QString::number(originalY));
        QMessageBox::warning(this, "错误", "无法更新站点坐标！");
    }

    isProcessing = false;
}

void MainWindow::on_TransferTime_editingFinished()
{
    if (m_currentStationId == -1) {
        return;
    }

    // 防止递归调用
    static bool isProcessing = false;
    if (isProcessing) return;
    isProcessing = true;

    // 获取输入的换乘时间
    QString timeText = ui->TransferTime->text();

    // 如果文本为空，设置为0
    if (timeText.isEmpty()) {
        timeText = "0";
        ui->TransferTime->setText(timeText);
        isProcessing = false;
        return;
    }

    // 检查是否为有效的非负整数
    bool ok;
    int transferTime = timeText.toInt(&ok);

    if (!ok || transferTime < 0) {
        // 如果不是有效的非负整数，恢复原来的值
        ui->TransferTime->setText(QString::number(network.stations[m_currentStationId].transferTime));
        QMessageBox::warning(this, "错误", "请输入有效的非负整数！");
        isProcessing = false;
        return;
    }

    // 更新站点的换乘时间
    if (network.stations.contains(m_currentStationId)) {
        Station& station = network.stations[m_currentStationId];
        station.setTransferTime(transferTime);

        if (metroMapWidget) {
            //metroMapWidget->refreshMap();
        }
    }

    isProcessing = false;
}


void MainWindow::onConnectionSelected(Connection* connection)
{
    if (!connection) return;

    m_currentConnectionId = connection->id;
    m_currentStationId = -1;
    m_currentLineId = -1;

    // 获取连接边的两个站点信息
    QString station1Name, station2Name;
    if (network.stations.contains(connection->stationId1)) {
        station1Name = network.stations[connection->stationId1].name;
    }
    if (network.stations.contains(connection->stationId2)) {
        station2Name = network.stations[connection->stationId2].name;
    }

    // 更新UI显示连接边信息
    ui->ConnectionStation1->setText(station1Name);
    ui->ConnectionStation2->setText(station2Name);
    ui->ConnectionPassTime->setText(QString::number(connection->travelTime));

    // 清空之前的线路列表
    ui->ConnectionLine->clear();

    // 添加所属线路
    for (int lineId : connection->lineIds) {
        if (network.lines.contains(lineId)) {
            QString lineName = network.lines[lineId].name;
            ui->ConnectionLine->addItem(lineName);
        }
    }

    if (metroMapWidget && connection) {
        if (metroMapWidget->findConnectionItem(connection->id)) {
            metroMapWidget->centerOnItem(metroMapWidget->findConnectionItem(connection->id));
        }
    }
}

void MainWindow::on_AddConnectionLine_clicked()
{
    if (m_currentConnectionId == -1) {
        QMessageBox::warning(this, "警告", "请先选择一个连接边！");
        return;
    }

    // 在列表中添加一个新的空项，并设置为可编辑状态
    QListWidgetItem* newItem = new QListWidgetItem("点击输入线路名称");
    newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
    ui->ConnectionLine->addItem(newItem);

    // 立即进入编辑状态
    ui->ConnectionLine->editItem(newItem);
}

void MainWindow::on_ConnectionLine_itemChanged(QListWidgetItem *item)
{
    // 防止递归调用
    static bool isProcessing = false;
    if (isProcessing) return;
    isProcessing = true;

    // 获取编辑后的文本
    QString newLineName = item->text();

    if (newLineName.isEmpty()) {
        // 如果为空，删除该项
        int row = ui->ConnectionLine->row(item);
        delete ui->ConnectionLine->takeItem(row);
        isProcessing = false;
        return;
    }

    // 检查线路是否存在
    bool lineExists = false;
    int lineId = -1;

    for (auto it = network.lines.begin(); it != network.lines.end(); ++it) {
        if (it.value().name == newLineName) {
            lineExists = true;
            lineId = it.key();
            break;
        }
    }

    if (!lineExists) {
        // 线路不存在，撤销更改
        QMessageBox::warning(this, "错误", "线路 '" + newLineName + "' 不存在！");

        // 恢复为原始文本或删除该项
        if (item->data(Qt::UserRole).isValid()) {
            // 如果是编辑现有项，恢复原始值
            QString originalText = item->data(Qt::UserRole).toString();
            item->setText(originalText);
        } else {
            // 如果是新增项，删除该项
            int row = ui->ConnectionLine->row(item);
            delete ui->ConnectionLine->takeItem(row);
        }
    } else {
        // 线路存在，更新连接边所属线路
        if (network.connections.contains(m_currentConnectionId)) {
            Connection& connection = network.connections[m_currentConnectionId];

            // 检查是否已经是该线路的连接边
            if (connection.lineIds.contains(lineId)) {
                QMessageBox::information(this, "提示", "连接边已经是线路 '" + newLineName + "' 的成员");
                // 删除该项
                int row = ui->ConnectionLine->row(item);
                delete ui->ConnectionLine->takeItem(row);
            } else {
                // 添加连接边到线路
                if (network.addConnectionToLine(m_currentConnectionId, lineId)) {
                    // 保存原始文本到UserRole，以便后续编辑
                    item->setData(Qt::UserRole, newLineName);
                    QMessageBox::information(this, "成功", "已将连接边添加到线路 '" + newLineName + "'");

                    if (metroMapWidget) {
                        metroMapWidget->refreshMap();
                    }
                } else {
                    QMessageBox::warning(this, "错误", "无法将连接边添加到线路 '" + newLineName + "'");
                    // 删除该项
                    int row = ui->ConnectionLine->row(item);
                    delete ui->ConnectionLine->takeItem(row);
                }
            }
        }
    }

    isProcessing = false;
}

void MainWindow::on_DeleteConnectionLine_clicked()
{
    if (m_currentConnectionId == -1) {
        QMessageBox::warning(this, "警告", "请先选择一个连接边！");
        return;
    }

    // 获取当前选中的列表项
    QListWidgetItem* selectedItem = ui->ConnectionLine->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "警告", "请先选择要删除的线路！");
        return;
    }

    // 获取线路名称
    QString lineName = selectedItem->text();

    // 查找对应的线路ID
    int lineId = -1;
    for (auto it = network.lines.begin(); it != network.lines.end(); ++it) {
        if (it.value().name == lineName) {
            lineId = it.key();
            break;
        }
    }

    if (lineId == -1) {
        QMessageBox::warning(this, "错误", "找不到对应的线路信息！");
        return;
    }

    // 确认删除
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认删除",
                                 "确定要从连接边中移除线路 '" + lineName + "' 吗？",
                                 QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // 从连接边中移除线路
        if (network.removeConnectionFromLine(m_currentConnectionId, lineId)) {
            // 从列表中移除项
            int row = ui->ConnectionLine->row(selectedItem);
            delete ui->ConnectionLine->takeItem(row);

            QMessageBox::information(this, "成功", "已从连接边中移除线路 '" + lineName + "'");

            if (metroMapWidget) {
                metroMapWidget->refreshMap();
            }
        } else {
            QMessageBox::warning(this, "错误", "无法从连接边中移除线路 '" + lineName + "'");
        }
    }
}

void MainWindow::on_ConnectionPassTime_editingFinished()
{
    if (m_currentConnectionId == -1) {
        return;
    }

    // 防止递归调用
    static bool isProcessing = false;
    if (isProcessing) return;
    isProcessing = true;

    // 获取输入的通行时间，如果为空，则不改变
    QString timeText = ui->ConnectionPassTime->text();

    if (timeText.isEmpty()) {
        ui->ConnectionPassTime->setText(QString::number(network.connections[m_currentConnectionId].travelTime));
        isProcessing = false;
        return;
    }

    // 检查是否为有效的正整数
    bool ok;
    int passTime = timeText.toInt(&ok);

    if (!ok || passTime <= 0) {
        // 如果不是有效的正整数，恢复原来的值
        ui->ConnectionPassTime->setText(QString::number(network.connections[m_currentConnectionId].travelTime));
        QMessageBox::warning(this, "错误", "请输入有效的正整数！");
        isProcessing = false;
        return;
    }

    // 更新连接边的通行时间
    if (network.connections.contains(m_currentConnectionId)) {
        Connection& connection = network.connections[m_currentConnectionId];
        connection.setTravelTime(passTime);

        if (metroMapWidget) {
            //metroMapWidget->refreshMap();
        }
    }

    isProcessing = false;
}

void MainWindow::on_DeleteConnection_clicked()
{
    if (m_currentConnectionId == -1) {
        QMessageBox::warning(this, "警告", "请先选择一个连接边！");
        return;
    }

    // 获取当前选中的连接边
    if (!network.connections.contains(m_currentConnectionId)) {
        QMessageBox::warning(this, "错误", "选中的连接边不存在！");
        return;
    }

    Connection& connection = network.connections[m_currentConnectionId];

    // 获取连接边的两个站点名称
    QString station1Name, station2Name;
    if (network.stations.contains(connection.stationId1)) {
        station1Name = network.stations[connection.stationId1].name;
    }
    if (network.stations.contains(connection.stationId2)) {
        station2Name = network.stations[connection.stationId2].name;
    }

    // 确认删除
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认删除",
                                 "确定要删除连接边 '" + station1Name + " - " + station2Name + "' 吗？",
                                 QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    // 删除连接边
    if (network.removeConnection(m_currentConnectionId)) {
        // 清除当前选中的连接边
        m_currentConnectionId = -1;

        // 清空UI中的连接边信息
        ui->ConnectionStation1->clear();
        ui->ConnectionStation2->clear();
        ui->ConnectionPassTime->clear();
        ui->ConnectionLine->clear();

        if (metroMapWidget) {
            metroMapWidget->refreshMap();
        }

        QMessageBox::information(this, "成功", "连接边已成功删除！");
    } else {
        QMessageBox::warning(this, "错误", "无法删除连接边！");
    }
}

void MainWindow::onLineSelected(MetroLine* line)
{
    if (!line) return;

    m_currentStationId = -1;
    m_currentConnectionId = -1;
    m_currentLineId = line->id;

    // 在线路名称输入框中显示线路名称
    if (ui->LineName)
    {
        ui->LineName->setText(line->name);
    }

    // 在颜色选择按钮中显示线路颜色
    if (ui->LineColorPick)
    {
        ui->LineColorPick->setColor(line->color);
    }

    if (metroMapWidget && line) {
        if (metroMapWidget->m_lineLabels.contains(line->id)) {
            metroMapWidget->centerOnItem(metroMapWidget->m_lineLabels[line->id]);
        }
    }
}

void MainWindow::on_LineName_editingFinished()
{
    if (m_currentLineId == -1) {
        return;
    }

    // 获取新的线路名称
    QString newName = ui->LineName->text().trimmed();

    // 检查名称是否为空
    if (newName.isEmpty()) {
        QMessageBox::warning(this, "警告", "线路名称不能为空！");
        // 恢复原来的名称
        if (metroMapWidget && metroMapWidget->m_network->lines.contains(m_currentLineId)) {
            ui->LineName->setText(metroMapWidget->m_network->lines[m_currentLineId].name);
        }
        return;
    }

    // 检查名称是否已存在（排除当前线路）
    if (metroMapWidget) {
        for (auto it = metroMapWidget->m_network->lines.begin();
             it != metroMapWidget->m_network->lines.end(); ++it) {
            if (it.key() != m_currentLineId && it.value().name == newName) {
                QMessageBox::warning(this, "警告", "线路名称已存在！");
                // 恢复原来的名称
                ui->LineName->setText(metroMapWidget->m_network->lines[m_currentLineId].name);
                return;
            }
        }
    }

    // 更新线路名称
    if (metroMapWidget && metroMapWidget->m_network->lines.contains(m_currentLineId)) {
        metroMapWidget->m_network->lines[m_currentLineId].name = newName;

        // 更新地图上的标签显示
        if (metroMapWidget->m_lineLabels.contains(m_currentLineId)) {
            SelectableLineLabel* label = metroMapWidget->m_lineLabels[m_currentLineId];
            if (label) {
                label->setPlainText(newName);
            }
        }
    }
}

void MainWindow::onLineColorChanged(const QColor &color)
{
    if (m_currentLineId == -1) {
        return;
    }

    // 更新线路颜色
    if (metroMapWidget && metroMapWidget->m_network->lines.contains(m_currentLineId)) {
        // 检查线路颜色是否已存在
        if (metroMapWidget && metroMapWidget->m_network) {
            for (auto it = metroMapWidget->m_network->lines.begin();
                 it != metroMapWidget->m_network->lines.end(); ++it) {
                if (it.value().color == color) {
                    // 线路颜色已存在
                    QMessageBox::warning(this, "警告", "线路颜色已存在！");
                    ui->AddLineColorPick->setFocus();
                    return;
                }
                // 线路颜色不存在，更新线路数据中的颜色
                metroMapWidget->m_network->lines[m_currentLineId].color = color;

                if (metroMapWidget) {
                    metroMapWidget->refreshMap();
                }
            }
        }
    }    

}

void MainWindow::on_CommitAddLine_clicked()
{
    // 获取线路名称
    QString lineName = ui->AddLineName->text().trimmed();

    // 验证线路名称
    if (lineName.isEmpty()) {
        QMessageBox::warning(this, "警告", "线路名称不能为空！");
        ui->AddLineName->setFocus();
        return;
    }

    // 获取线路颜色
    QColor lineColor = ui->AddLineColorPick->color();

    // 检查线路名称和颜色是否已存在
    if (metroMapWidget && metroMapWidget->m_network) {
        for (auto it = metroMapWidget->m_network->lines.begin();
             it != metroMapWidget->m_network->lines.end(); ++it) {
            // 检查名称是否已存在
            if (it.value().name == lineName) {
                QMessageBox::warning(this, "警告", "线路名称已存在！");
                ui->AddLineName->setFocus();
                ui->AddLineName->selectAll();
                return;
            }

            // 检查颜色是否已存在
            if (it.value().color == lineColor) {
                QMessageBox::warning(this, "警告", "线路颜色已存在！");
                ui->AddLineColorPick->setFocus();
                return;
            }
        }

        int newLineId = network.nextStationId;

        // 创建新线路
        MetroLine newLine;
        newLine.id = newLineId;
        newLine.name = lineName;
        newLine.color = lineColor;
        // 新线路开始时没有站点，stationIds 为空

        // 添加到网络
        metroMapWidget->m_network->lines.insert(newLineId, newLine);

        // 清空输入框，准备下一次输入
        ui->AddLineName->clear();

        QMessageBox::information(this, "成功", "线路添加成功！");
    } else {
        QMessageBox::critical(this, "错误", "无法访问地铁网络数据！");
    }
}

void MainWindow::on_CommitAddStation_clicked()
{
    // 获取站点名称
    QString stationName = ui->AddStationName->text().trimmed();

    // 验证站点名称
    if (stationName.isEmpty()) {
        QMessageBox::warning(this, "警告", "站点名称不能为空！");
        ui->AddStationName->setFocus();
        return;
    }

    // 检查站点名称是否已存在
    if (metroMapWidget && metroMapWidget->m_network) {
        for (auto it = metroMapWidget->m_network->stations.begin();
             it != metroMapWidget->m_network->stations.end(); ++it) {
            if (it.value().name == stationName) {
                QMessageBox::warning(this, "警告", "站点名称已存在！");
                ui->AddStationName->setFocus();
                ui->AddStationName->selectAll();
                return;
            }
        }

        int newStationId = network.nextStationId;

        //QPointF defaultPosition(650000, -150000); // 左上角
        // 或者使用视图中心：
         QPointF defaultPosition = metroMapWidget->m_view->mapToScene(
             metroMapWidget->m_view->viewport()->rect().center());

        // 创建新站点
        Station newStation;
        newStation.id = newStationId;
        newStation.name = stationName;
        newStation.position = defaultPosition;
        newStation.lineIds = QSet<int>(); // 初始时不属于任何线路

        // 添加到网络
        metroMapWidget->m_network->stations.insert(newStationId, newStation);

        metroMapWidget->addStationItem(&metroMapWidget->m_network->stations[newStationId]);

        // 清空输入框，准备下一次输入
        ui->AddStationName->clear();

        // 选中新创建的站点
        emit onStationSelected(&metroMapWidget->m_network->stations[newStationId]);

        QMessageBox::information(this, "成功", "站点添加成功！\n请点击选中新站点，设置正确位置。");
    } else {
        QMessageBox::critical(this, "错误", "无法访问地铁网络数据！");
    }
}

// Web Mercator转WGS84
void webMercatorToWgs84(double x, double y, double& lat, double& lng) {
    // Web Mercator坐标范围校验
    if (x < -20037508.34 || x > 20037508.34 || y < -20037508.34 || y > 20037508.34) {
        lat = 0.0;
        lng = 0.0;
        return;
    }

    // 转换经度
    lng = x / 20037508.34 * 180.0;

    // 转换纬度
    double yRad = y / 20037508.34 * M_PI;
    lat = 180.0 / M_PI * (2 * atan(exp(yRad)) - M_PI / 2);
}

void MetroNetwork::calculateDistance() const {
    if (routeResult.stationPath.size() < 2) {
        qDebug() << "路径无效：站点数量不足（需至少包含起点和终点）";
        return;
    }

    int startStationId = routeResult.stationPath.first();
    int endStationId = routeResult.stationPath.last();

    if (!stations.contains(startStationId)) {
        qDebug() << QString("起点站ID不存在：%1").arg(startStationId);
        return;
    }
    if (!stations.contains(endStationId)) {
        qDebug() << QString("终点站ID不存在：%1").arg(endStationId);
        return;
    }

    const QPointF& startPos = stations[startStationId].position;
    const QPointF& endPos = stations[endStationId].position;

    // 反推原始经纬度坐标，原始逻辑：position(lat * scale, -lng * scale)
    double startX = startPos.x() / POS_SCALE;  // Web Mercator X坐标（米）
    double startY = -startPos.y() / POS_SCALE; // Web Mercator Y坐标（米）
    double endX = endPos.x() / POS_SCALE;      // Web Mercator X坐标（米）
    double endY = -endPos.y() / POS_SCALE;     // Web Mercator Y坐标（米）

    // 转换为WGS84经纬度（度）
    double startLat, startLng, endLat, endLng;
    webMercatorToWgs84(startX, startY, startLat, startLng);
    webMercatorToWgs84(endX, endY, endLat, endLng);

    // 将经纬度从 度 转换为 弧度
    double startLatRad = startLat * DEG_TO_RAD;
    double startLngRad = startLng * DEG_TO_RAD;
    double endLatRad = endLat * DEG_TO_RAD;
    double endLngRad = endLng * DEG_TO_RAD;

    // Haversine公式计算球面距离
    double latDiff = endLatRad - startLatRad; // 纬度差
    double lngDiff = endLngRad - startLngRad; // 经度差

    // Haversine公式核心计算
    double a = sin(latDiff / 2) * sin(latDiff / 2)
             + cos(startLatRad) * cos(endLatRad)
             * sin(lngDiff / 2) * sin(lngDiff / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a)); // 两点间的中心角（弧度）

    // 最终距离=地球半径x中心角
    routeResult.distance = EARTH_RADIUS * c;

    return;
}

RouteResult::RouteResult()
    : totalTime(0), distance(0.0), cost(0) {}

void RouteResult::clear() {
    stationPath.clear();
    linePath.clear();
    lineTimes.clear();
    transferTimes.clear();
    totalTime = 0;
    distance = 0.0;
    cost = 0;
}

void RouteResult::addTransferPoint(int stationId, int transferTime) {
    if (transferTime > 0) { // 只添加有效换乘时间
        transferTimes.append(qMakePair(stationId, transferTime));
    }
}

void RouteResult::calculateCost() {
/*
 * 起步价：0-6公里内，票价为3元。
 * 后续里程：每增加10公里，票价增加1元。例如，6-16公里为4元，16-26公里为5元，以此类推。
 * 最高票价：目前最高票价为9元
*/
    if (distance <= 6.0) {
        cost = 3;
    } else {
        double extraDistance = distance - 6.0;

        int extraFare = static_cast<int>(ceil(extraDistance / 10.0));

        cost = 3 + extraFare;

        if (cost > 9) {
            cost = 9;
        }
    }
}

QString MetroNetwork::getDescription() const {
    if (routeResult.stationPath.isEmpty()) {
        return "未找到有效路径";
    }

    QString description;

    QString startStation = stations.value(routeResult.stationPath.first()).name;
    QString endStation = stations.value(routeResult.stationPath.last()).name;

    // 第一行：起点和终点
    description += QString("起点：%1；终点：%2\n").arg(startStation).arg(endStation);

    // 第二行：详细线路
    description += "详细线路：";

    // 当前线路ID
    int j = 0;
    int currentLineId = routeResult.linePath[j];

    for (int i = 0; i < routeResult.stationPath.size(); i++) {
        int stationId = routeResult.stationPath[i];
        QString stationName = stations.value(stationId).name;

        // 如果是起点站
        if (i == 0) {
            description += stationName;
            continue;
        }

        // 检查是否是换乘点
        bool isTransfer = false;
        int transferTime = 0;
        for (const auto& transfer : routeResult.transferTimes) {
            if (transfer.first == stationId) {
                isTransfer = true;
                transferTime = transfer.second;
                qDebug() << "发现换乘点:" << stationName << "ID:" << stationId << "换乘时间:" << transferTime;
                break;
            }
        }

        // 如果线路发生变化，或者到达终点
        if (isTransfer || i == routeResult.stationPath.size() - 1) {
            if (currentLineId != -1) {
                int lineTime = routeResult.lineTimes.value(currentLineId);
                description += QString("——%1（%2分钟）").arg(lines.value(currentLineId).name).arg(lineTime);
            }

            // 如果是换乘点，添加换乘信息
            if (isTransfer && i < routeResult.stationPath.size() - 1) {
                description += QString("——%1（换乘%2分钟）").arg(stationName).arg(transferTime);
                j++;
                currentLineId = routeResult.linePath[j];
            }

            // 如果是最后一站，添加终点站
            if (i == routeResult.stationPath.size() - 1) {
                description += QString("——%1").arg(stationName);
            }
        }
    }

    // 第三行：总时间
    int hours = routeResult.totalTime / 60;
    int minutes = routeResult.totalTime % 60;

    if (hours > 0) {
        description += QString("\n总时间：%1小时%2分钟").arg(hours).arg(minutes);
    } else {
        description += QString("\n总时间：%1分钟").arg(minutes);
    }

    if (routeResult.distance > 0) {
        description += QString("，距离：%1公里").arg(routeResult.distance, 0, 'f', 1);
    }

    if (routeResult.cost > 0) {
        description += QString("，费用：%1元").arg(routeResult.cost);
    }

    return description;
}

void MainWindow::on_FindPath_clicked()
{
    QString startName = ui->StartEdit->text().trimmed();
    QString endName = ui->EndEdit->text().trimmed();

    if (startName.isEmpty() || endName.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入起点和终点站名");
        return;
    }

    // 查找起点和终点站ID
    int startId = -1;
    int endId = -1;

    for (auto it = network.stations.begin(); it != network.stations.end(); ++it) {
        if (it.value().name == startName) {
            startId = it.key();
        }
        if (it.value().name == endName) {
            endId = it.key();
        }
    }

    if (startId == -1) {
        QMessageBox::warning(this, "提示", QString("未找到起点站: %1").arg(startName));
        return;
    }
    if (endId == -1) {
        QMessageBox::warning(this, "提示", QString("未找到终点站: %1").arg(endName));
        return;
    }

    int priority = ui->SelectPriority->currentIndex();

    network.findRoute(startId, endId, priority);

    QString description = network.getDescription();

    // 如果是费用优先，添加提示
    if (priority == 1) {
        description = "费用是固定的，因此费用优先和时间优先结果相同。\n\n" + description;
    }

    ui->RouteDisplay->setText(description);

    metroMapWidget->highlightRoute(network.routeResult);
}

void MainWindow::on_ClearHighlight_clicked()
{
    metroMapWidget->clearHighlight();
}

void MainWindow::on_DeleteLine_clicked()
{
    if (m_currentLineId == -1) {
        QMessageBox::warning(this, "警告", "请先选择一条线路！");
        return;
    }

    // 获取当前选中的线路
    if (!network.lines.contains(m_currentLineId)) {
        QMessageBox::warning(this, "错误", "选中的线路不存在！");
        return;
    }

    MetroLine& line = network.lines[m_currentLineId];

    // 确认删除
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认删除",
                                 "确定要删除线路 '" + line.name + "' 吗？\n"
                                 "此操作将从所有站点和连接中移除该线路的属性。",
                                 QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    // 从所有站点中移除该线路
    for (auto it = network.stations.begin(); it != network.stations.end(); ++it) {
        Station& station = it.value();
        if (station.lineIds.contains(m_currentLineId)) {
            station.removeLine(m_currentLineId);
        }
    }

    // 从所有连接中移除该线路
    for (auto it = network.connections.begin(); it != network.connections.end(); ++it) {
        Connection& connection = it.value();
        if (connection.lineIds.contains(m_currentLineId)) {
            connection.removeLine(m_currentLineId);

            // 如果连接不再属于任何线路
            if (connection.lineIds.isEmpty()) {
            }
        }
    }

    // 删除线路
    if (network.removeLine(m_currentLineId)) {
        // 清除当前选中的线路
        m_currentLineId = -1;

        // 清空UI中的线路信息
        ui->LineName->clear();

        // 重新构建图结构
        network.buildGraph();

        if (metroMapWidget) {
            metroMapWidget->refreshMap();
        }

        QMessageBox::information(this, "成功", "线路已成功删除！");
    } else {
        QMessageBox::warning(this, "错误", "无法删除线路！");
    }
}
