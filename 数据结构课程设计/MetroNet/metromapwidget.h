#pragma execution_character_set("utf-8")
#ifndef METRO_MAP_WIDGET_H
#define METRO_MAP_WIDGET_H

#include <QApplication>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QMap>
#include <QSet>
#include <QColor>
#include <QQueue>
#include <QKeyEvent>
#include <QTimer>
#include <QDebug>

#include <QGraphicsTextItem>
#include <QGraphicsSceneMouseEvent>
#include <QVBoxLayout>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QColorDialog>
#include <cmath>

const int CANCEL_DOUBLECLICK_TIME = 5000;

class Station;
class Connection;
class MetroLine;
class MetroNetwork;
class RouteResult;
class SelectableLineLabel;

// 地铁站点图形项
class StationGraphicsItem : public QGraphicsEllipseItem {
public:
    StationGraphicsItem(Station* station, QGraphicsItem* parent = nullptr);

    Station* getStation() const { return m_station; }
    void updateAppearance();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private:
    Station* m_station;
};

// 地铁连接线图形项
class ConnectionGraphicsItem : public QGraphicsLineItem {
public:
    ConnectionGraphicsItem(Connection* connection, int lineId, QGraphicsItem* parent = nullptr);

    Connection* getConnection() const { return m_connection; }
    int getLineId() const { return m_lineId; }
    void updateAppearance();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
    Connection* m_connection;
    int m_lineId; // 这个图形项代表的特定线路，因为若同一条边属于多个线路，则需要多个图形项显示
};

// 地铁地图控件
class MetroMapWidget : public QWidget {
    Q_OBJECT

public:
    MetroNetwork* m_network;
    QMap<int, SelectableLineLabel*> m_lineLabels;
    QGraphicsView* m_view;

    explicit MetroMapWidget(MetroNetwork* network, QWidget* parent = nullptr);
    ~MetroMapWidget();

    void refreshMap();

    // 缩放控制
    void zoomIn();
    void zoomOut();
    void resetZoom();

    // 高亮显示路径
    void highlightRoute(const RouteResult& route);
    void clearHighlight();

    StationGraphicsItem* findStationItem(int stationId) const;
    ConnectionGraphicsItem* findConnectionItem(int connectionId) const;

    void updateTextScale();
    void optimizeLabelPositions();

    // 获取网络实例
    MetroNetwork* getNetwork() const { return m_network; }

    // 双击相关函数
    void stopDoubleClickTimer() { m_doubleClickTimer->stop(); }
    bool isWaitingForSecondClick() const { return m_waitingForSecondClick; }
    void setWaitingForSecondClick(bool waiting) { m_waitingForSecondClick = waiting; }
    int getFirstDoubleClickStationId() const { return m_firstDoubleClickStationId; }
    void setFirstDoubleClickStationId(int stationId) { m_firstDoubleClickStationId = stationId; }
    void startDoubleClickTimer() { m_doubleClickTimer->start(); }
    void clearAllHighlights()
    {
        for (auto it = m_stationItems.begin(); it != m_stationItems.end(); ++it) {
            it.value()->updateAppearance();
        }
    }
    void cancelDoubleClick() {
        m_doubleClickTimer->stop();
        clearAllHighlights();
        m_waitingForSecondClick = false;
        m_firstDoubleClickStationId = -1;
    }

    void createLineLabels();
    void addLineLabel(MetroLine* line);

    void addStationItem(Station* station);
    void addConnectionItem(Connection* connection);

    void centerOnItem(QGraphicsItem* item);
    void centerOnItem(SelectableLineLabel* label);

signals:
    void stationSelected(Station* station);
    void connectionSelected(Connection* connection);
    void lineSelected(MetroLine* line);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void setupScene();
    void onLineLabelClicked(MetroLine* line);

private:
    QGraphicsScene* m_scene;

    QMap<int, StationGraphicsItem*> m_stationItems;
    QMap<int, ConnectionGraphicsItem*> m_connectionItems;

    double m_scaleFactor;

    QMap<int, QGraphicsTextItem*> m_labelItems;

    int m_firstDoubleClickStationId; // 第一次双击的站点ID
    QTimer* m_doubleClickTimer;      // 双击计时器
    bool m_waitingForSecondClick;    // 是否等待第二次点击

    void createStationItems();
    void createConnectionItems();
    bool checkLabelOverlap(SelectableLineLabel* label, const QPointF& position, SelectableLineLabel* existingLabel);
};

#endif // METRO_MAP_WIDGET_H
