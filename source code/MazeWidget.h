#ifndef MAZEWIDGET_H
#define MAZEWIDGET_H

#include <QWidget>
#include <QPoint>
#include <vector>

class MazeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MazeWidget(QWidget *parent = nullptr);

    // 随机生成迷宫
    void generateRandomMaze();
    // A* 求解
    void solveMaze();

    // 设置迷宫大小
    void setMazeSize(int rows, int cols);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    int m_rows;
    int m_cols;
    int m_cellSize;

    std::vector<std::vector<int>> m_maze;
    std::vector<QPoint> m_path;

    QPoint m_start;
    QPoint m_end;

    // 随机数帮助
    int randInt(int low, int high);

    // 内部函数
    void initMaze();
    void doAStar();

    int heuristic(const QPoint &a, const QPoint &b);
};

#endif // MAZEWIDGET_H
