#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QPoint;
class QPainter;
class QPixmap;

class DrawArea;
class QPushButton;
class QSlider;
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

//    /**
//    * @brief changePenWidth
//    * This slot will change the width of the
//    * pen for the DrawArea.
//    */
//    void changePenWidth(int);

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
//    QSlider* mPenWidthSlider;

    bool mCtrlKey_modifier;
};
#endif // MAINWINDOW_H
