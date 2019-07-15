#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initTableWidgetInfo();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initTableWidgetInfo()
{
    int width = qApp->desktop()->availableGeometry().width() - 120;

    ui->tableWidgetInfo->setColumnCount(3);
    ui->tableWidgetInfo->setRowCount(1);
    ui->tableWidgetInfo->setColumnWidth(0, width * 0.06);
    ui->tableWidgetInfo->setColumnWidth(1, width * 0.06);
    ui->tableWidgetInfo->setColumnWidth(2, width * 0.10);

    QStringList headText;
    headText << "学号" << "姓名" << "专业";

    ui->tableWidgetInfo->setHorizontalHeaderLabels(headText);
    ui->tableWidgetInfo->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidgetInfo->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidgetInfo->setAlternatingRowColors(true);
    ui->tableWidgetInfo->verticalHeader()->setVisible(false);
    ui->tableWidgetInfo->horizontalHeader()->setStretchLastSection(true);

}


void MainWindow::updateTableWidgetInfo(QVector<std::shared_ptr<QTableWidgetItem>> items)
{
    for (int i = 0; i < items.size(); ++i) {
        ui->tableWidgetInfo->setItem(0, i, items[i].get());
    }
}
