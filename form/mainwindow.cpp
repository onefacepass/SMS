#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initTableWidgetInfo();
    initTableWidgetSchedule();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initTableWidgetInfo()
{
    int width = qApp->desktop()->availableGeometry().width() - 120;

    ui->tableWidgetInfo->setColumnCount(1);
    ui->tableWidgetInfo->setRowCount(7);
//    ui->tableWidgetInfo->setColumnWidth(0, width * 0.50);
//    ui->tableWidgetInfo->setColumnWidth(1, width * 0.06);
//    ui->tableWidgetInfo->setColumnWidth(2, width * 0.10);

    QStringList headText;
    headText << "学号" << "姓名" << "专业" << "性别" << "班级" << "联系方式" << "联系地址";
    ui->tableWidgetInfo->setVerticalHeaderLabels(headText);

    ui->tableWidgetInfo->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidgetInfo->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidgetInfo->setAlternatingRowColors(true);
    ui->tableWidgetInfo->horizontalHeader()->setVisible(false);

//    ui->tableWidgetInfo->verticalHeader()->setVisible(false);
    ui->tableWidgetInfo->horizontalHeader()->setStretchLastSection(true);

}

void MainWindow::initTableWidgetSchedule()
{
    int width = qApp->desktop()->availableGeometry().width() - 120;

    ui->tableWidgetSchedule->setColumnCount(7);
    ui->tableWidgetSchedule->setRowCount(7);

    QStringList headText;
    headText << "标题一" << "标题二" << "标题三";
    ui->tableWidgetSchedule->setHorizontalHeaderLabels(headText);

    ui->tableWidgetSchedule->verticalHeader()->setVisible(false);
}

void MainWindow::updateTableWidgetInfo(QVector<std::shared_ptr<QTableWidgetItem>> items)
{
    for (int i = 0; i < items.size(); ++i) {
        ui->tableWidgetInfo->setItem(i, 0, items[i].get());
    }


    ui->labMain->setText(QString("学号：" + items[0]->text() + "\t姓名：" + items[1]->text() + "\t专业：" + items[2]->text()));
}
