#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MazeWidget;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onGenerateMaze();
    void onSolveMaze();

private:
    MazeWidget *m_mazeWidget;
    QPushButton *m_btnGenerate;
    QPushButton *m_btnSolve;
};

#endif // MAINWINDOW_H
