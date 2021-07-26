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
    * @briefCompares the currently drawn layer
    * to a layer loaded from system memory.
    */
    void compareLayer(bool);

    /**
    * @brief Capture key combinations:
    * ctrl-z : undo
    */
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    // Main GUI window pointer.
    Ui::MainWindow *mUi;

    // Pointer to region maintaining the drawing
    // space.
    DrawArea* mDrawArea;

    // Widget to place predicted character
    // according to the model.
    QLabel* mPredictionArea;

    // Button to initiate passing current
    // drawn image to the model.
    QPushButton* mCompareButton;

    // indicate that the ctrl key has been pressed.
    bool mCtrlKey_modifier;

};
#endif // MAINWINDOW_H
