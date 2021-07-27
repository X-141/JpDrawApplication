#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "DrawArea.hpp"

#include <QMouseEvent>
#include <QPixmap>

// Test
#include <QListWidget>
#include <QPushButton>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , mUi(new Ui::MainWindow),
    mDrawArea(nullptr),
    mPredictionArea(nullptr),
    mCompareButton(nullptr),
    mCtrlKey_modifier(false)
{
    mUi->setupUi(this);

    mDrawArea = new DrawArea(mUi->centralwidget);
    mDrawArea->setObjectName("DrawArea");
    mDrawArea->setEnabled(true);
    mDrawArea->setCursor(QCursor(Qt::CrossCursor));
    mDrawArea->resizeDrawArea(QSize(384, 384));
    mDrawArea->setPenWidth(30);
    mUi->gridLayout->addWidget(mDrawArea, 0,0,1,1);

    mPredictionArea = new QLabel(mUi->centralwidget);
    mPredictionArea->setObjectName("PredictionArea");
    mPredictionArea->setEnabled(true);
    mPredictionArea->setCursor(QCursor(Qt::BlankCursor));
    mPredictionArea->resize(QSize(384, 384));
    mUi->gridLayout->addWidget(mPredictionArea, 0, 1, 1, 1);

    mCompareButton = new QPushButton(mUi->centralwidget);
    mCompareButton->setObjectName("CompareLayerButton");
    mCompareButton->setText("Compare Layer");
    mCompareButton->setEnabled(true);
    mUi->gridLayout->addWidget(mCompareButton, 4, 0, 1, 1);

    QObject::connect(mCompareButton, SIGNAL(clicked(bool)),
                     this, SLOT(compareLayer(bool)));

    this->adjustSize();
    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
}

MainWindow::~MainWindow()
{
    delete mUi;
    delete mDrawArea;
    delete mCompareButton;
}

void
MainWindow::compareLayer(bool) {
    //LOG("Now comparing layers");
    LOG(level::info, "MainWindow::CompareLayer()", "Now comparing layers");
    int loadIndex = mDrawArea->compareLayer();
    QImage loadImage = mDrawArea->getResourceCharacterImage(loadIndex);
    mPredictionArea->setPixmap(QPixmap::fromImage(loadImage));
}

void
MainWindow::keyPressEvent(QKeyEvent* event) {
    LOG(level::info, "MainWindow::KeyPressEvent()", "Handling key press event.");
    switch (event->key()) {
        case Qt::Key_Control:
            LOG(level::info, "MainWindow::KeyPressEvent()", "Control key pressed.");
            mCtrlKey_modifier = true;
            break;
        case Qt::Key_Z:
            LOG(level::info, "MainWindow::KeyPressEvent()",
                                          "Z key pressed.");
            // Make sure user is pressing ctrl and that the draw area is
            // not null
            if (mCtrlKey_modifier && mDrawArea) {
                LOG(level::info, "MainWindow::KeyPressEvent()", "Performing undo operation.");
                mDrawArea->undoLayer();
            }
            break;
        default:
            break;
    };
}

void
MainWindow::keyReleaseEvent(QKeyEvent* event) {
    LOG(level::info, "MainWindow::keyPressEvent()", "Handling key release event.");
    if (event->key() == Qt::Key_Control)
        mCtrlKey_modifier = false;
}