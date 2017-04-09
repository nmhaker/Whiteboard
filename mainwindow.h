#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QMouseEvent>
#include <QColorDialog>
#include <QDebug>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QDataStream>

namespace Ui {
class MainWindow;
}

struct Pixel{
    int x;
    int y;
    int width;
    QColor boja;
    int offset_x;
    int offset_y;
};

enum DrawingMode{
    Draw,
    Erase,
    Move
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *);
    void closeEvent(QCloseEvent *);
    void resizeEvent(QResizeEvent *);

private:
    Ui::MainWindow *ui;
    QVector<Pixel> pixels;
    QVector< QVector<Pixel> > lines;

    bool clicked;
    QColor bg_color;
    QColor temporaryColor;
    int widthOfPen;
    int widthOfEraser;
    int drawingMode;

    int mouse_x;
    int mouse_y;

    int offset_x;
    int offset_y;

    int offset_start_x;
    int offset_start_y;

    bool saved_work;

    QPixmap *pixmap;
    bool pixmapReset;

    int counterForResizeEvent;

    int previous_x;
    int previous_y;
    int previous2_x;
    int previous2_y;

    QString workingDirectory;
    QString projectName;
    void createProject();
    bool toogleFullScreen;

};

QDataStream &operator<<(QDataStream& out, const Pixel& p);

QDataStream &operator>>(QDataStream& in, Pixel& p);

#endif // MAINWINDOW_H
