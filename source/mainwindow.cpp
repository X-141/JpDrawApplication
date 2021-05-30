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
#include <QLabel>

#include "Log.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , mUi(new Ui::MainWindow),
    mDrawArea(nullptr)
{
    mUi->setupUi(this);

    mDrawArea = new DrawArea(mUi->centralwidget);
    mDrawArea->setObjectName("DrawArea");
    mDrawArea->setEnabled(true);
    mDrawArea->setCursor(QCursor(Qt::CrossCursor));
    mDrawArea->resizeDrawArea(QSize(400, 400));
    mUi->gridLayout->addWidget(mDrawArea, 0,0,1,1);

    mPredictionArea = new QLabel(mUi->centralwidget);
    mPredictionArea->setObjectName("PredictionArea");
    mPredictionArea->setEnabled(true);
    mPredictionArea->setCursor(QCursor(Qt::BlankCursor));
    mPredictionArea->resize(QSize(400, 400));
    mUi->gridLayout->addWidget(mPredictionArea, 0, 1, 1, 1);

    mCompareButton = new QPushButton(mUi->centralwidget);
    mCompareButton->setObjectName("CompareLayerButton");
    mCompareButton->setText("Compare Layer");
    mCompareButton->setEnabled(true);
    mUi->gridLayout->addWidget(mCompareButton, 4, 0, 1, 1);

    mPenWidthSlider = new QSlider(Qt::Horizontal, mUi->centralwidget);
    mPenWidthSlider->setObjectName("PenWidthSlider");
    mPenWidthSlider->setEnabled(true);
    //mPenWidthSlider->setTickPosition(QSlider::TicksBelow);
    mPenWidthSlider->setMinimum(1);
    mPenWidthSlider->setMaximum(100);
    mUi->gridLayout->addWidget(mPenWidthSlider, 5, 0, 1, 1);

    QObject::connect(mCompareButton, SIGNAL(clicked(bool)),
                     this, SLOT(compareLayer(bool)));

    QObject::connect(mPenWidthSlider, SIGNAL(valueChanged(int)),
                     this, SLOT(changePenWidth(int)));

    this->adjustSize();
    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
}

MainWindow::~MainWindow()
{
    delete mUi;
    delete mDrawArea;
    delete mCompareButton;
    delete mPenWidthSlider;
}

void
MainWindow::compareLayer(bool checked) {
    LOG("Now comparing layers");
    int loadIndex = mDrawArea->compareLayer();
    QImage loadImage = mDrawArea->getComparisonImage(loadIndex);
    mPredictionArea->setPixmap(QPixmap::fromImage(loadImage));
}

void
MainWindow::changePenWidth(int value) {
    mDrawArea->setPenWidth(value);
    qInfo() << value;
}

void
MainWindow::keyPressEvent(QKeyEvent* event) {
    LOG("Handling key press event");
    switch (event->key()) {
        case Qt::Key_Control:
            LOG("Control key pressed!");
            mCtrlKey_modifier = true;
            break;
        case Qt::Key_Z:
            LOG("Z key pressed!");
            // Make sure user is pressing ctrl and that the draw area is
            // not null
            if (mCtrlKey_modifier && mDrawArea) {
                LOG("Performing undo operation.");
                mDrawArea->undoLayer();
            }
            break;
        default:
            break;
    };
}

void
MainWindow::keyReleaseEvent(QKeyEvent* event) {
    LOG("Handling key release event");
    if (event->key() == Qt::Key_Control)
        mCtrlKey_modifier = false;
}