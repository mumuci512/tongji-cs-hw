#pragma execution_character_set("utf-8")

#define _USE_MATH_DEFINES
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QSet>
#include <QToolBar>
#include <queue>
#include <QFontDatabase>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QGraphicsItem>
#include <QSqlDatabase>
#include <QSqlError>
#include <utility>
#include <vector>
#include <functional>
#include <QSqlQuery>
#include <QRandomGenerator>
#include <QFileInfo>
#include <QDir>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
// 全局常量：地球平均半径（km）、角度转弧度系数
const double EARTH_RADIUS = 6371.0;
const double DEG_TO_RAD = M_PI / 180.0;

const double POS_SCALE = 0.05;  // 经纬度转换坐标

const int DEFAULT_TRANSFER_TIME = 3;
const int DEFAULT_TRAVEL_TIME = 3;

class MetroMapWidget;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// 地铁站点类
class Station {
public:
    int id;
    QString name;
    QPointF position;

    QSet<int> lineIds;
    //QVector<QVector<int>> transferTimes; // 若为换乘站，则2个线路之间的换乘时间(分钟)
    int transferTime; // 换乘时间(分钟)，不是换乘站则为0

    QList<int> connectedStationIds; // 直接连接的站点ID
    QList<int> connectedConnectionIds; // 直接连接的边ID

    Station() : id(-1), transferTime(0) {}
    Station(int id, QString name, QPointF position, const QSet<int>& lineIds = QSet<int>());

    void addLine(int lineId); // 添加所属线路
    void removeLine(int lineId);

    void setTransferTime(int transferTime);

    bool isTransferStation();

    const QList<int>& getStations() const;

    const QList<int>& getConnections() const;

    void addStation(int stationId);
    void removeStation(int stationId);

    void addConnection(int connectionId);
    void removeConnection(int connectionId);
};

// 连接边类
class Connection {
public:
    int id;
    int stationId1;
    int stationId2;
    QSet<int> lineIds;
    int travelTime; // 通行时间(分钟)

    Connection() : id(-1), stationId1(-1), stationId2(-1), travelTime(0) {}
    Connection(int id, int station1, int station2, const QSet<int>& lineIds = QSet<int>(), int time = 0);

    void addLine(int lineId);
    void removeLine(int lineId);

    void setTravelTime(int travelTime);

    // 检查是否连接指定站点
    bool connectsStation(int stationId) const;

    // 获取连接的另一个站点
    int getOtherStation(int stationId) const;
};

// 地铁线路类
class MetroLine {
public:
    int id;
    QString name;
    QColor color;
    QSet<int> stationIds;
    QSet<int> connectionIds;

    MetroLine() : id(-1), color(Qt::black) {}
    MetroLine(int id, QString name, QColor color);

    void addStation(int stationId);
    void removeStation(int stationId);

    void addConnection(int connectionId);
    void removeConnection(int connectionId);

    // 检查线路是否包含站点或边
    bool containsStation(int stationId) const;
    bool containsConnection(int connectionId) const;

    // 检查线路的连通性
//    bool isConnected() const;
};


// 路径规划结果类
class RouteResult {
public:
    QList<int> stationPath; // 经过的站点ID
    QList<int> linePath; // 每段路径所属的线路ID
    QMap<int, int> lineTimes; // 每条线路上的时间(线路ID，时间)
    QList<QPair<int, int>> transferTimes; // 换乘点信息(站点ID，换乘时间)
    int totalTime; // 总时间(分钟)
    double distance;    // 距离(km)
    int cost;

    RouteResult();
    void clear();

    void addTransferPoint(int stationId, int transferTime);

    void calculateCost();
};

// 地铁网络类
class MetroNetwork {
public:
    QMap<int, Station> stations;
    QMap<int, MetroLine> lines;
    QMap<int, Connection> connections;
    int nextStationId;
    int nextLineId;
    int nextConnectionId;

    RouteResult& routeResult;

    // 图的邻接表存储整个地铁网络
    QMap<int, QList<std::pair<int, int>>> graph; // stationId->(neighborStationId, connectionId)
    void buildGraph() {
        graph.clear();

        // 遍历所有连接，构建图结构
        for (auto it = connections.begin(); it != connections.end(); ++it) {
            const Connection& conn = it.value();

            // 添加双向连接
            graph[conn.stationId1].append({conn.stationId2, conn.id});
            graph[conn.stationId2].append({conn.stationId1, conn.id});
        }
    }

    MetroNetwork();    

    // 站点操作
    int addStation(const QString& name, const QPointF& position, const QSet<int>& lineIds = QSet<int>());
    bool removeStation(int stationId);
    bool updateStation(int stationId, const QString& name, const QPointF& position);

    // 线路操作
    int addLine(const QString& name, const QColor& color = Qt::black);
    bool removeLine(int lineId);
    bool updateLine(int lineId, const QString& name, const QColor& color);

    // 连接操作
    int addConnection(int stationId1, int stationId2, int travelTime = 0);
    bool removeConnection(int connectionId);
    bool updateConnection(int connectionId, int travelTime);

    // 线路管理
    bool addStationToLine(int stationId, int lineId);
    bool removeStationFromLine(int stationId, int lineId);
    bool addConnectionToLine(int connectionId, int lineId);
    bool removeConnectionFromLine(int connectionId, int lineId);
    bool removeLineFromConnection(int connectionId, int lineId);

    // 路径规划
    void findRoute(int startId, int endId, int priority = 0) const;    // priority: 0时间优先, 1费用优先（费用固定，因此等于时间优先）, 2换乘优先
    void calculateDistance() const;
    QString getDescription() const;    // 获取详细描述

    bool isValidConnection(int stationId1, int stationId2, const QSet<int>& lineIds) const;
//    bool canAddStationToLine(int stationId, int lineId) const;
//    bool canRemoveStationFromLine(int stationId, int lineId) const;

    bool saveToFile() const;

    void initShanghaiMetroData();
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionZoomIn_clicked();

    void on_actionZoomOut_clicked();

    void on_actionResetZoom_clicked();

    void onStationSelected(Station* station);

    void on_MainWindow_iconSizeChanged(const QSize &iconSize);

    void on_AddStationLine_clicked();

    void on_StationLine_itemChanged(QListWidgetItem *item);

    void on_DeleteStationLine_clicked();

    void on_DeleteStation_clicked();

    void onConnectionSelected(Connection* Connection);

    void on_StationName_editingFinished();

    void on_StationX_editingFinished();

    void on_StationY_editingFinished();

    void on_TransferTime_editingFinished();

    void on_AddConnectionLine_clicked();

    void on_DeleteConnectionLine_clicked();

    void on_ConnectionLine_itemChanged(QListWidgetItem *item);

    void on_ConnectionPassTime_editingFinished();

    void on_DeleteConnection_clicked();

    void onLineSelected(MetroLine* line);

    void on_LineName_editingFinished();

    void onLineColorChanged(const QColor &color);

    void on_CommitAddLine_clicked();

    void on_CommitAddStation_clicked();

    void on_FindPath_clicked();

    void on_ClearHighlight_clicked();

    void on_DeleteLine_clicked();

private:
    Ui::MainWindow *ui;
    QScrollArea *pScroll;
    MetroNetwork network;
    MetroMapWidget *metroMapWidget;
    QGraphicsScene *scene;
    QGraphicsView *view;

    int m_currentStationId;
    int m_currentConnectionId;
    int m_currentLineId;

    QString m_originalStationName; // 原始的站点名称
    double m_originalStationX; // 原始的X坐标
    double m_originalStationY; // 原始的Y坐标

};

#endif // MAINWINDOW_H
