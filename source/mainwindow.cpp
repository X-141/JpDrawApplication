#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "DrawArea.hpp"
#include "ProcessLayer.hpp"

#include <QMouseEvent>
#include <QPixmap>

// Test
#include <QListWidget>
#include <QPushButton>
#include <QDebug>
#include <QSlider>

#include "Log.hpp"
#include "opencv2/core.hpp"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    drawArea(nullptr)
{
    ui->setupUi(this);
    ui->centralwidget->resize(1000, 1000);

    drawArea = new DrawArea(ui->centralwidget);
    drawArea->setObjectName("DrawArea");
    drawArea->setEnabled(true);
    drawArea->setCursor(QCursor(Qt::CrossCursor));
    drawArea->resizeDrawArea(QSize(800, 600));
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

    mCompareButton = new QPushButton(ui->centralwidget);
    mCompareButton->setObjectName("CompareLayerButton");
    mCompareButton->setText("Compare Layer");
    mCompareButton->setEnabled(true);
    ui->gridLayout->addWidget(mCompareButton, 4, 0, 1, 1);

    mPenWidthSlider = new QSlider(Qt::Horizontal, ui->centralwidget);
    mPenWidthSlider->setObjectName("PenWidthSlider");
    mPenWidthSlider->setEnabled(true);
    mPenWidthSlider->setTickPosition(QSlider::TicksBelow);
    ui->gridLayout->addWidget(mPenWidthSlider, 5, 0, 1, 1);

    // Whenever, we add a layer to drawArea, we send a signal to
    // update
    QObject::connect(drawArea, SIGNAL(layerUpdateHandle(void)),
                     this, SLOT(updateLayerList(void)));

    QObject::connect(mEnableButton, SIGNAL(clicked(bool)),
                     this, SLOT(enableLayer(bool)));

    QObject::connect(mDisableButton, SIGNAL(clicked(bool)),
                     this, SLOT(disableLayer(bool)));

    QObject::connect(mCompareButton, SIGNAL(clicked(bool)),
                     this, SLOT(compareLayer(bool)));

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
    //qInfo() << "We received an update!";
    LOG("We received an update!");
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
    //qInfo() << "Add layer back!\n";
    LOG("Add layer back!\n");
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
    //qInfo() << "Removing layer!\n";
    LOG("Removing Layer!\n");
    if(listwidget) {
        auto selected_item = listwidget->currentItem();

        if (!selected_item) {
            LOG("listwidget current item is NULL.\n");
            return;
        }

        auto& layer = drawArea->getLayerById(selected_item->text().toUInt());

        if(layer.isEnabled()) {
            layer.setEnableStatus(false);
            drawArea->updateDrawArea();
            selected_item->setForeground(Qt::red);
        }
    }
}

#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

void
MainWindow::compareLayer(bool checked) {
    LOG("Selected Compare layer button.\n");
    auto procLayer_SystemFile = ProcessLayer(DEBUG_targetFile);
    auto procLayer_HardLayer = ProcessLayer(drawArea->generateImage());
    //cv::imwrite("C:\\Users\\seanp\\source\\JpDrawApplication\\build\\testFILE.png", procLayer_SystemFile.mPngMatrix);

    // Reference https://stackoverflow.com/questions/9905093/how-to-check-whether-two-matrices-are-identical-in-opencv
    cv::Mat gray1, gray2;
    cvtColor(procLayer_HardLayer.mPngMatrix, gray1, cv::COLOR_BGRA2GRAY);
    cvtColor(procLayer_SystemFile.mPngMatrix, gray2, cv::COLOR_BGRA2GRAY);

    cv::imwrite("C:\\Users\\seanp\\source\\JpDrawApplication\\build\\grayscale_systemfile.png", gray2);
    cv::imwrite("C:\\Users\\seanp\\source\\JpDrawApplication\\build\\grayscale_hardlayer.png", gray1);

    cv::Mat diff;
    cv::compare(gray1, gray2, diff, cv::CMP_NE);
    int nz = cv::countNonZero(diff);
    LOG("DIFF: ");
    LOG(nz);
    LOG("\n");
}