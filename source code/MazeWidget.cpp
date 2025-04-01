#include "MazeWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QRandomGenerator>
#include <queue>
#include <limits>
#include <algorithm>

// Node 结构体
struct Node {
    QPoint pt;
    int g;
    int h;
    QPoint parent;
    int f() const { return g + h; }
    bool operator<(const Node &other) const {
        return f() > other.f(); // 注意大顶堆or小顶堆
    }
};

MazeWidget::MazeWidget(QWidget *parent)
    : QWidget(parent)
{
    m_rows = 20;
    m_cols = 20;
    m_cellSize = 20;
    initMaze();
    // 默认起点(1,1)，终点(18,18)
    m_start = QPoint(1,1);
    m_end   = QPoint(m_cols-2, m_rows-2);
}

// 生成一个空白迷宫
void MazeWidget::initMaze()
{
    m_maze.resize(m_rows, std::vector<int>(m_cols, 0));
    // 外边界设为墙
    for(int i=0; i<m_rows; i++){
        m_maze[i][0] = 1;
        m_maze[i][m_cols-1] = 1;
    }
    for(int j=0; j<m_cols; j++){
        m_maze[0][j] = 1;
        m_maze[m_rows-1][j] = 1;
    }
    // 清空路径
    m_path.clear();
    update();
}

// 随机生成迷宫
void MazeWidget::generateRandomMaze()
{
    initMaze();
    // 设置墙的随机密度
    double wallDensity = 0.3; // 30% 几率生成墙
    for(int i=1; i<m_rows-1; i++){
        for(int j=1; j<m_cols-1; j++){
            if( (double)QRandomGenerator::global()->bounded(1.0) < wallDensity ) {
                m_maze[i][j] = 1; // 墙
            } else {
                m_maze[i][j] = 0; // 路
            }
        }
    }
    // 保证起点和终点不被墙挡住
    m_maze[m_start.y()][m_start.x()] = 0;
    m_maze[m_end.y()][m_end.x()] = 0;

    m_path.clear();
    update();
}

// A* 求解
void MazeWidget::solveMaze()
{
    doAStar();
    update();
}

// A* 核心
void MazeWidget::doAStar()
{
    // 如果起终点无效，直接清空
    if(m_maze[m_start.y()][m_start.x()] == 1 || m_maze[m_end.y()][m_end.x()] == 1) {
        m_path.clear();
        return;
    }

    // BFS/A* 通用结构
    std::vector<std::vector<int>> gScore(m_rows, std::vector<int>(m_cols, std::numeric_limits<int>::max()));
    std::vector<std::vector<bool>> visited(m_rows, std::vector<bool>(m_cols, false));
    std::vector<std::vector<QPoint>> cameFrom(m_rows, std::vector<QPoint>(m_cols, QPoint(-1,-1)));

    auto cmp = [](const Node &a, const Node &b){
        return a.f() > b.f();
    };
    std::priority_queue<Node, std::vector<Node>, decltype(cmp)> openSet(cmp);

    // 初始化
    gScore[m_start.y()][m_start.x()] = 0;
    Node startNode{m_start, 0, heuristic(m_start, m_end), QPoint(-1,-1)};
    openSet.push(startNode);

    // 四个方向
    std::vector<QPoint> dirs{{1,0},{-1,0},{0,1},{0,-1}};

    bool found = false;
    while(!openSet.empty()){
        Node current = openSet.top();
        openSet.pop();
        QPoint cpt = current.pt;

        if(visited[cpt.y()][cpt.x()]) continue;
        visited[cpt.y()][cpt.x()] = true;

        // 到达终点
        if(cpt == m_end){
            found = true;
            // 重建路径
            m_path.clear();
            QPoint p = cpt;
            while(p != QPoint(-1,-1)){
                m_path.push_back(p);
                p = cameFrom[p.y()][p.x()];
            }
            std::reverse(m_path.begin(), m_path.end());
            break;
        }

        // 扩展相邻
        for(auto &d : dirs){
            QPoint np = cpt + d;
            int nx = np.x(), ny = np.y();
            if(nx<0 || nx>=m_cols || ny<0 || ny>=m_rows) continue;
            if(m_maze[ny][nx] == 1) continue; // 墙
            if(visited[ny][nx]) continue;

            int tg = gScore[cpt.y()][cpt.x()] + 1;
            if(tg < gScore[ny][nx]){
                gScore[ny][nx] = tg;
                cameFrom[ny][nx] = cpt;
                Node nxt{np, tg, heuristic(np,m_end), cpt};
                openSet.push(nxt);
            }
        }
    }
    if(!found) {
        m_path.clear();
    }
}

// 启发函数: 曼哈顿
int MazeWidget::heuristic(const QPoint &a, const QPoint &b)
{
    return abs(a.x() - b.x()) + abs(a.y() - b.y());
}

// 鼠标点击事件
void MazeWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        // 计算点击位置对应的单元格
        int mx = event->pos().x() / m_cellSize;
        int my = event->pos().y() / m_cellSize;
        if(mx<0||mx>=m_cols||my<0||my>=m_rows) return;

        // 示例：左键点击如果按住 Ctrl，则设为终点；否则设为起点
        if(event->modifiers() & Qt::ControlModifier) {
            m_end = QPoint(mx,my);
            if(m_maze[my][mx] == 1) {
                m_maze[my][mx] = 0; // 确保终点不是墙
            }
        } else {
            m_start = QPoint(mx,my);
            if(m_maze[my][mx] == 1) {
                m_maze[my][mx] = 0; // 确保起点不是墙
            }
        }
        m_path.clear();
        update();
    }
    else if(event->button() == Qt::RightButton) {
        // 右键点击：画墙/清除墙
        int mx = event->pos().x() / m_cellSize;
        int my = event->pos().y() / m_cellSize;
        if(mx<0||mx>=m_cols||my<0||my>=m_rows) return;
        // 切换墙与路
        m_maze[my][mx] = (m_maze[my][mx] == 1) ? 0 : 1;
        // 避免覆盖起点终点
        if(QPoint(mx,my) == m_start) m_maze[my][mx] = 0;
        if(QPoint(mx,my) == m_end)   m_maze[my][mx] = 0;
        m_path.clear();
        update();
    }
}

// 绘制
void MazeWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter p(this);
    // 绘制背景
    p.fillRect(rect(), Qt::white);

    // 画迷宫格子
    for(int i=0; i<m_rows; i++){
        for(int j=0; j<m_cols; j++){
            QRect cellRect(j*m_cellSize, i*m_cellSize, m_cellSize, m_cellSize);
            if(m_maze[i][j] == 1){
                p.fillRect(cellRect, Qt::black);
            }
            p.drawRect(cellRect);
        }
    }
    // 画起点、终点
    p.setBrush(Qt::green);
    QRect sRect(m_start.x()*m_cellSize, m_start.y()*m_cellSize, m_cellSize, m_cellSize);
    p.drawRect(sRect);

    p.setBrush(Qt::red);
    QRect eRect(m_end.x()*m_cellSize, m_end.y()*m_cellSize, m_cellSize, m_cellSize);
    p.drawRect(eRect);

    // 画路径
    if(!m_path.empty()){
        QPen pen(Qt::blue, 2);
        p.setPen(pen);
        for(size_t i=1; i<m_path.size(); i++){
            QPoint p1 = m_path[i-1];
            QPoint p2 = m_path[i];
            QPoint c1(p1.x()*m_cellSize + m_cellSize/2, p1.y()*m_cellSize + m_cellSize/2);
            QPoint c2(p2.x()*m_cellSize + m_cellSize/2, p2.y()*m_cellSize + m_cellSize/2);
            p.drawLine(c1, c2);
        }
    }
}
