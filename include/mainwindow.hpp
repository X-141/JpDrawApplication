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
class QListWidget;

class DrawArea;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    /**
     * @brief updateLayerList
     * Update the list of layers currently in the draw area.
     */
    void updateLayerList();

    /**
     * @brief enableLayer
     * Handles adding a layer back to
     * the draw area.
     */
    void enableLayer(bool);

    /**
     * @brief disableLayer
     * Handles removing a layer from
     * the draw area.
     */
    void disableLayer(bool);

private:
    Ui::MainWindow *ui;
    DrawArea* drawArea;
    QListWidget* listwidget;

    QPushButton* mEnableButton;
    QPushButton* mDisableButton;

};
#endif // MAINWINDOW_H
