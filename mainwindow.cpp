#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Initialize variables
    clicked = false;
    temporaryColor = Qt::yellow;
    bg_color = QColor(20,20,20);
    this->setCursor(Qt::CrossCursor);
    widthOfPen = 1;
    widthOfEraser = 20;
    drawingMode = Draw;
    setMouseTracking(true);
    ui->centralWidget->setMouseTracking(true);
    offset_x = 0;
    offset_y = 0;
    offset_start_x = 0;
    offset_start_y = 0;
    saved_work = true;
    previous_x = -1;
    previous_y = -1;
    previous2_x = -1;
    previous2_y = -1;
    counterForResizeEvent = 0;
    pixmap = nullptr;
    pixmapReset = false;
    showFullScreen();
    toogleFullScreen=true;
    this->createProject();

//    qDebug() << "PROSLA INICIJALIZACIJA MAINWINDOWA" << endl;
}

MainWindow::~MainWindow()
{
    delete pixmap;
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *e)
{
//    qDebug() << "Ulazim u paintEvent" << endl;
    QPainter painter(this);

    if(pixmap != nullptr)
    {
        painter.drawPixmap(this->rect(), *pixmap);
    }

    if(drawingMode == Erase)
    {
        painter.setPen(Qt::white);
        painter.setBrush(bg_color);
        painter.drawRect(mouse_x - widthOfEraser, mouse_y - widthOfEraser, 2 * widthOfEraser , 2* widthOfEraser);
    }

//    qDebug() << "paintEvent" << endl;

    QMainWindow::paintEvent(e);
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
//    qDebug() << "mouse move event! : " << e->x() << " , " << e->y() << endl;
    mouse_x = e->x();
    mouse_y = e->y();

    if(clicked){
//        globalPainter.begin(pixmap);
        if(drawingMode == Draw){
            if(previous_x >= 0 && previous_y >= 0){
                previous2_x = previous_x;
                previous2_y = previous_y;

                QPainter globalPainter(pixmap);
                globalPainter.setRenderHint(QPainter::Antialiasing, true);
                QPen pen;
                pen.setWidth(widthOfPen);
                pen.setColor(temporaryColor);
                pen.setCapStyle(Qt::PenCapStyle::RoundCap);
                pen.setJoinStyle(Qt::PenJoinStyle::RoundJoin);
                globalPainter.setPen(pen);
                globalPainter.drawLine(previous_x, previous_y, mouse_x, mouse_y);
                if(previous2_x >= 0 && previous2_y >= 0)
                    globalPainter.drawLine(previous2_x, previous2_y, mouse_x, mouse_y);

                this->ui->centralWidget->repaint();

                //Old stuff for moving
                Pixel p;
                p.x = e->x();
                p.y = e->y();
                p.width = widthOfPen;
                p.boja = temporaryColor;
                p.offset_x = 0;
                p.offset_y = 0;
                pixels.append(p);
                //--------------
            }
        }else if(drawingMode == Erase){
            QPainter globalPainter(pixmap);
            globalPainter.setPen(QPen(bg_color));
            globalPainter.setBrush(QBrush(bg_color));
            globalPainter.drawRect(mouse_x - widthOfEraser, mouse_y - widthOfEraser, widthOfEraser * 2, widthOfEraser * 2);
            globalPainter.setRenderHint(QPainter::Antialiasing, true);
            this->ui->centralWidget->repaint();

            //Old stuff --||--
            for(int i = 0; i < lines.count(); i++){
                for(int ii = 0; ii < lines.at(i).count(); ii++){
                    if(lines.at(i).at(ii).x + lines.at(i).at(ii).offset_x >= mouse_x - widthOfEraser  && lines.at(i).at(ii).x + lines.at(i).at(ii).offset_x<= mouse_x + widthOfEraser && lines.at(i).at(ii).y + lines.at(i).at(ii).offset_y>= mouse_y - widthOfEraser && lines.at(i).at(ii).y + lines.at(i).at(ii).offset_y<= mouse_y + widthOfEraser){
                        QVector<Pixel> vec2, vec3;
                        for(int a = 0; a < ii; a++){
                            vec2.append(lines.at(i).at(a));
                        }
                        for(int a = ii+1; a < lines.at(i).count(); a++){
                            vec3.append(lines.at(i).at(a));
                        }
                        lines.append(vec2);
                        lines.append(vec3);
                        lines.removeAt(i);
                    }
                }
            }
            //----------
        }else if(drawingMode == Move){
            if(lines.count() > 0){
                //Old stuff
                offset_x = mouse_x - offset_start_x ;
                offset_y = mouse_y - offset_start_y ;
                 //Alter all curent pixels by offset
                 for(int i = 0; i < lines.count(); i++){
                     for(int ii = 0; ii < lines.at(i).count(); ii++){
                         lines[i][ii].offset_x += offset_x;
                         lines[i][ii].offset_y += offset_y;
                     }
                 }
                 offset_start_x = mouse_x;
                 offset_start_y = mouse_y;
                 //----------------------
                 QPixmap p(pixmap->size());
                 QPainter painter(&p);
                 painter.setRenderHint(QPainter::Antialiasing, true);
                 painter.setBrush(bg_color);
                 painter.drawRect(pixmap->rect());
                 for(int i = 0; i < lines.count(); i++){
                     for(int ii = 0; ii < lines.at(i).count() -1; ii++){
                         QPen pen(QColor(lines.at(i).at(ii).boja));
                         pen.setWidth(lines.at(i).at(ii).width);
                         painter.setPen(pen);
                         painter.drawLine(lines.at(i).at(ii).x + lines.at(i).at(ii).offset_x, lines.at(i).at(ii).y + lines.at(i).at(ii).offset_y , lines.at(i).at(ii+1).x + lines.at(i).at(ii).offset_x, lines.at(i).at(ii+1).y + lines.at(i).at(ii).offset_y);
                     }
                 }
                 pixmap->swap(p);
                 pixmapReset = true;
                 //----------------------
                 this->ui->centralWidget->repaint();
            }
        }
    }else{
        if(drawingMode == Erase)
            this->ui->centralWidget->repaint();
    }

    previous_x = mouse_x;
    previous_y = mouse_y;

//    qDebug() << "mouseMoveEvent" << endl;

    QMainWindow::mouseMoveEvent(e);
}


void MainWindow::mousePressEvent(QMouseEvent *e)
{
    if(drawingMode == Draw){
        QPen pen;
        pen.setColor(temporaryColor);
        pen.setWidth(widthOfPen);
        QPainter globalPainter(pixmap);
        globalPainter.setRenderHint(QPainter::Antialiasing, true);
        globalPainter.setPen(pen);
        globalPainter.drawPoint(e->x(), e->y());
//        globalPainter.end();


        previous_x = e->x();
        previous_y = e->y();

        Pixel p;
        p.x = e->x();
        p.y = e->y();
        p.width = widthOfPen;
        p.boja = temporaryColor;
        p.offset_x = 0;
        p.offset_y = 0;
        pixels.append(p);
    }

    saved_work = false;

    offset_start_x = mouse_x;
    offset_start_y = mouse_y;

    clicked = true;
    this->ui->centralWidget->repaint();

//    qDebug() << "mousePressEvent" << endl;

    QMainWindow::mousePressEvent(e);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{

    if(pixels.count() > 0)
    {
        lines.append(pixels);
        pixels.clear();
    }
    pixels.clear();
    clicked = false;

    if(drawingMode != Move){

    }
//    qDebug() << "mouseReleaseEvent" << endl;
    QMainWindow::mouseReleaseEvent(e);
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{

    if(e->key() == Qt::Key_Q){
        if(drawingMode == Draw)
            widthOfPen++;
        else if(drawingMode == Erase)
            widthOfEraser++;
        this->repaint();
    }else if(e->key() == Qt::Key_W){
        if(drawingMode == Draw){
            if(widthOfPen > 1)
                widthOfPen--;
        }else if(drawingMode == Erase)
            if(widthOfEraser > 1)
                widthOfEraser--;
        this->repaint();
    }else if(e->key() == Qt::Key_C){
        temporaryColor = QColorDialog::getColor();
    }else if(e->key() == Qt::Key_X){
        QMessageBox msgBox;
        msgBox.setText("Confirm deleting");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();
        if(ret == QMessageBox::Ok){
          QPainter globalPainter(pixmap);
          globalPainter.setRenderHint(QPainter::Antialiasing, true);
          globalPainter.setPen(bg_color);
          globalPainter.setBrush(bg_color);
          globalPainter.drawRect(pixmap->rect());
//          globalPainter.end();

          this->ui->centralWidget->repaint();
          lines.clear();
        }
    }else if(e->key() == Qt::Key_Escape){
        this->close();
    }else if(e->key() == Qt::Key_D){
        drawingMode = Draw;
    }else if(e->key() == Qt::Key_E){
        drawingMode = Erase;
        this->ui->centralWidget->repaint();
    }else if(e->key() == Qt::Key_Z){
        drawingMode = Move;
    }else if(e->key() == Qt::Key_S){
            //SERIALIZE DATA
            QFile file(projectName+".tzc");
            if(!file.open(QFile::WriteOnly))
                qDebug() << "ERROR NEUSPESNO OTVARANJE FAJLA ZA ISPIS PODATAKA" << endl;
            else{
                QDataStream out(&file);
                out << pixels << lines;
                file.flush();
                file.close();
            }
            //------------
            saved_work = true;
    }else if(e->key() == Qt::Key_S && e->modifiers() == Qt::CTRL){
        QPixmap pix(this->size());
        this->render(&pix);
        bool ok;
        QString nameOfFile = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                             tr("Unesite ime fajla"), QLineEdit::Normal,
                                             ".png", &ok);
        if (ok && !nameOfFile.isEmpty()){
            pix.save(workingDirectory+'/'+nameOfFile);
            saved_work = true;
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("You didnt enter filename!! Pls try again");
            msgBox.exec();
        }

    }else if(e->key() == Qt::Key_N){
        this->createProject();
    }else if(e->key() == Qt::Key_A){
        if(lines.count() > 0){
            lines.removeLast();
            QPixmap p(pixmap->size());
            QPainter painter(&p);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setBrush(bg_color);
            painter.drawRect(pixmap->rect());
            for(int i = 0; i < lines.count(); i++){
                for(int ii = 0; ii < lines.at(i).count() -1; ii++){
                    QPen pen(QColor(lines.at(i).at(ii).boja));
                    pen.setWidth(lines.at(i).at(ii).width);
                    painter.setPen(pen);
                    painter.drawLine(lines.at(i).at(ii).x + lines.at(i).at(ii).offset_x, lines.at(i).at(ii).y + lines.at(i).at(ii).offset_y , lines.at(i).at(ii+1).x + lines.at(i).at(ii).offset_x, lines.at(i).at(ii+1).y + lines.at(i).at(ii).offset_y);
                }
            }
            painter.end();
            pixmap->swap(p);
            pixmapReset = true;

            this->ui->centralWidget->repaint();
        }
    }else if(e->key() == Qt::Key_Space){
        if(toogleFullScreen){
            this->showNormal();
            toogleFullScreen=false;
        }else{
            this->showFullScreen();
            toogleFullScreen=true;
        }

    }else if(e->key() == Qt::Key_O){
        QFile file(QFileDialog::getOpenFileName(nullptr, "Otvorite projekat"));
        if(!file.open(QFile::ReadOnly))
            qDebug() << "Nije uspesno otvoren fajl" << endl;
        else{
            QDataStream in(&file);
            in >> pixels >> lines;
        }
    }
//    qDebug() << "keyPressEvent" << endl;
    QMainWindow::keyPressEvent(e);
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if(!saved_work){
        QMessageBox msgBox;
        msgBox.setText("You didnt saved your work! Proceed?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();
        if(ret == QMessageBox::Ok){
            e->accept();
        }else{
            e->ignore();
        }
    }else
        QMainWindow::closeEvent(e);
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    counterForResizeEvent++;
    if(counterForResizeEvent == 2){
        pixmap = new QPixmap(this->size());
        QPainter globalPainter(pixmap);
        globalPainter.setRenderHint(QPainter::Antialiasing, true);
        globalPainter.setBrush(QBrush(bg_color));
        globalPainter.drawRect(this->rect());
    //    globalPainter.end();

        this->ui->centralWidget->repaint();
    }
//    qDebug() << "resizeEvent" << endl;
    QMainWindow::resizeEvent(e);
}

void MainWindow::createProject()
{
    QDir::setCurrent(QFileDialog::getExistingDirectory(nullptr, "Direktorijum Projekta"));
    QString nameOfDir = QInputDialog::getText(nullptr, "Kreiranje Projekta", "Unesite ime projekta");
    QDir direc;
    direc.mkdir(nameOfDir);
    QDir::setCurrent(QDir::currentPath()+"/"+nameOfDir);
    projectName = nameOfDir;
    workingDirectory = QDir::currentPath();
}


QDataStream &operator<<(QDataStream& out, const Pixel& p){
    out << p.x << p.y << p.width << p.boja << p.offset_x << p.offset_y;
    return out;
}

QDataStream &operator>>(QDataStream& in, Pixel& p){
    in >> p.x >> p.y >> p.width >> p.boja >> p.offset_x >> p.offset_y;
    return in;
}
