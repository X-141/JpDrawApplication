#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "DrawArea.hpp"

#include <QMouseEvent>
#include <QPixmap>

// Test
#include <QListWidget>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    drawArea(nullptr)
{
    ui->setupUi(this);
    ui->centralwidget->resize(750, 750);

    drawArea = new DrawArea(ui->centralwidget);
    drawArea->setObjectName("DrawArea");
    drawArea->setEnabled(true);
    drawArea->setCursor(QCursor(Qt::CrossCursor));
    drawArea->resizeDrawArea(QSize(700, 700));
    ui->gridLayout->addWidget(drawArea, 0,0,1,1);

    listwidget = new QListWidget(ui->centralwidget);
    listwidget->setObjectName("LayerListWidget");
    listwidget->setEnabled(true);
    ui->gridLayout->addWidget(listwidget,1,0,1,1);

    mEnableButton = new QPushButton(ui->centralwidget);
    mEnableButton->setObjectName("EnableLayerButton");
    mEnableButton->setText("Enable Layer");
    mEnableButton->setEnabled(true);
    ui->gridLayout->addWidget(mEnableButton, 2,0,1,1);

    mDisableButton = new QPushButton(ui->centralwidget);
    mDisableButton->setObjectName("DisableLayerButton");
    mDisableButton->setText("Disable Layer");
    mDisableButton->setEnabled(true);
    ui->gridLayout->addWidget(mDisableButton, 3,0,1,1);

    // Whenever, we add a layer to drawArea, we send a signal to
    // update
    QObject::connect(drawArea, SIGNAL(layerUpdateHandle(void)),
                     this, SLOT(updateLayerList(void)));

    QObject::connect(mEnableButton, SIGNAL(clicked(bool)),
                     this, SLOT(enableLayer(bool)));

    QObject::connect(mDisableButton, SIGNAL(clicked(bool)),
                     this, SLOT(disableLayer(bool)));

    this->adjustSize();
    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete drawArea;
    delete listwidget;
}

void
MainWindow::updateLayerList() {
    qInfo() << "We received an update!";
    static uint lastAddedLayerID;
    const auto& layer_vector = drawArea->getListOfLayers();
    for(const auto& layer: layer_vector) {
        if(lastAddedLayerID < layer.getId()) {
            QListWidgetItem* item = new QListWidgetItem(listwidget);
            if(layer.isEnabled())
                item->setForeground(Qt::blue);
            else
                item->setForeground(Qt::red);
            item->setText(QString::number(layer.getId()));
            lastAddedLayerID = layer.getId();
        }
    }
}

void
MainWindow::enableLayer(bool checked) {
    qInfo() << "Add layer back!\n";
    if(listwidget) {
        auto selected_item = listwidget->currentItem();
        auto& layer = drawArea->getLayerById(selected_item->text().toUInt());
        if(!layer.isEnabled()) {
            layer.setEnableStatus(true);
            drawArea->updateDrawArea();
            selected_item->setForeground(Qt::blue);
        }
    }
}

void
MainWindow::disableLayer(bool checked) {
    qInfo() << "Removing layer!\n";
    if(listwidget) {
        auto selected_item = listwidget->currentItem();
        auto& layer = drawArea->getLayerById(selected_item->text().toUInt());
        if(layer.isEnabled()) {
            layer.setEnableStatus(false);
            drawArea->updateDrawArea();
            selected_item->setForeground(Qt::red);
        }
    }
}
