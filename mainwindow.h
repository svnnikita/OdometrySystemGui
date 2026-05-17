#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QUdpSocket>
#include <QGridLayout>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // старт-стоп получения данных
    void onStartStop();
    void processPendingDatagrams();

private:
    // структура с параметрами датчиков
    struct sensorParameters {
        QLabel *motionLabel;    // движение: 1/0
        QLabel *deltaXLabel;    // смещение по dx
        QLabel *deltaYLabel;    // смещение по dy
        QLabel *squalLabel;     // качество поврехности
        QLabel *shutterLabel;   // значение затвора
        QLabel *maxPixelLabel;  // максимальное значение пикселя в кадре
    };

    QLabel *motionLabel;    // движение: 1/0
    QLabel *deltaXLabel;    // смещение по dx
    QLabel *deltaYLabel;    // смещение по dy
    QLabel *squalLabel;     // качество поврехности
    QLabel *shutterLabel;   // значение затвора
    QLabel *maxPixelLabel;  // максимальное значение пикселя в кадре

    QLabel *leftMotionValue;
    QLabel *leftDeltaXValue;
    QLabel *leftDdeltaYValue;
    QLabel *leftSqualValue;
    QLabel *leftShutterValue;
    QLabel *leftMaxPixelValue;

    QLabel *rightMotionValue;
    QLabel *rightDeltaXValue;
    QLabel *rightDdeltaYValue;
    QLabel *rightSqualValue;
    QLabel *rightShutterValue;
    QLabel *rightMaxPixelValue;

    // окно для отображения сообщений из порта
    QTextEdit *trafficText;
    // статус-бар для состояний программы
    QStatusBar *statusBar;
    // кнопка для запуска приема данных
    QPushButton *startStopButton;

    // сетка для размещения компонентов
    void setMainLayout();

    // создаем udp сокет для получения датаграмм
    QUdpSocket *udpSocket;
    // флаг прослушивания порта
    bool isListening;
};
#endif // MAINWINDOW_H
