#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>


namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void updateTableWidgetInfo(QVector<std::shared_ptr<QTableWidgetItem>> items);

private:
    Ui::MainWindow *ui;

    void initTableWidgetInfo();
    void initTableWidgetSchedule();

};

#endif // MAINWINDOW_H
