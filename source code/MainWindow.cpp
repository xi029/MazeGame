#include "MainWindow.h"
#include "MazeWidget.h"

#include <QToolBar>
#include <QAction>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // --- 中心部件 ---
    // 用一个QWidget作为中央容器，里边用布局管理器放按钮+MazeWidget
    QWidget *central = new QWidget(this);
    QVBoxLayout *vLayout = new QVBoxLayout(central);

    // 迷宫绘制控件
    m_mazeWidget = new MazeWidget(this);
    m_mazeWidget->setFixedSize(500, 500); // 也可根据需求动态

    // 按钮
    m_btnGenerate = new QPushButton("随机生成", this);
    m_btnSolve = new QPushButton("求解", this);

    // 上面先放按钮，下面放迷宫
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(m_btnGenerate);
    hLayout->addWidget(m_btnSolve);
    hLayout->addStretch();
    vLayout->addLayout(hLayout);

    vLayout->addWidget(m_mazeWidget);

    setCentralWidget(central);

    // --- 连接信号槽 ---
    connect(m_btnGenerate, &QPushButton::clicked, this, &MainWindow::onGenerateMaze);
    connect(m_btnSolve, &QPushButton::clicked, this, &MainWindow::onSolveMaze);
}

MainWindow::~MainWindow()
{
}

void MainWindow::onGenerateMaze()
{
    // 调用 MazeWidget 里的随机生成函数
    m_mazeWidget->generateRandomMaze();
}

void MainWindow::onSolveMaze()
{
    // 调用 MazeWidget 里的 A* 求解
    m_mazeWidget->solveMaze();
}
