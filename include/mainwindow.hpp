#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QPointer>

#include "Log.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QPoint;
class QPainter;
class QPixmap;

class DrawArea;
class QPushButton;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:

    /**
    * @brief compareLayer
    * Compares the currently drawn layer
    * to a layer loaded from system memory.
    */
    void compareLayer(bool);

    /**
    * @brief keyPressEvent
    * Capture key combinations:
    * ctrl-z : undo
    */
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    Ui::MainWindow *mUi;
    DrawArea* mDrawArea;
    QLabel* mPredictionArea;
    QPushButton* mCompareButton;

    bool mCtrlKey_modifier;

};
#endif // MAINWINDOW_H
