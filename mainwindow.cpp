#include "mainwindow.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QTextEdit>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    // параметры поверхности под датчиками, надписи
    motionLabel(new QLabel(tr("Motion"))),
    deltaXLabel(new QLabel(tr("Delta X"))),
    deltaYLabel(new QLabel(tr("Delta Y"))),
    squalLabel(new QLabel(tr("SQUAL"))),
    shutterLabel(new QLabel(tr("Shutter"))),
    maxPixelLabel(new QLabel(tr("Maximum Pixel"))),

    // параметры с левого датчика
    leftMotionValue(new QLabel(tr("0"))),
    leftDeltaXValue(new QLabel(tr("0"))),
    leftDdeltaYValue(new QLabel(tr("0"))),
    leftSqualValue(new QLabel(tr("0"))),
    leftShutterValue(new QLabel(tr("0"))),
    leftMaxPixelValue(new QLabel(tr("0"))),

    // параметры с правого датчика
    rightMotionValue(new QLabel(tr("0"))),
    rightDeltaXValue(new QLabel(tr("0"))),
    rightDdeltaYValue(new QLabel(tr("0"))),
    rightSqualValue(new QLabel(tr("0"))),
    rightShutterValue(new QLabel(tr("0"))),
    rightMaxPixelValue(new QLabel(tr("0"))),

    // создаем кнопку
    startStopButton(new QPushButton(tr("Start/Stop"))),

    // создаем окно для отображения информации
    trafficText(new QTextEdit())
{    
    // установим шрифт побольше
    trafficText->setReadOnly(true);
    trafficText->setFont(QFont("Monospace", 14));

    // устанавливаем сетку приложения
    setMainLayout();

    udpSocket = new QUdpSocket(this);

    // соединяем слоты и сигналы
    connect(startStopButton, &QPushButton::clicked, this, &MainWindow::onStartStop);
    connect(udpSocket, &QUdpSocket::readyRead, this, &MainWindow::processPendingDatagrams);
}

MainWindow::~MainWindow()
{
}

// размещаем компоненты в сетке
void MainWindow::setMainLayout()
{
    // parametersLayout -- сетка с данными от датчиков
    // создаем сетку, которую разместим в выделенном боксе
    QGridLayout *parametersLayout = new QGridLayout;
    parametersLayout->addWidget(motionLabel, 0, 0);
    parametersLayout->addWidget(deltaXLabel, 1, 0);
    parametersLayout->addWidget(deltaYLabel, 2, 0);
    parametersLayout->addWidget(squalLabel, 3, 0);
    parametersLayout->addWidget(shutterLabel, 4, 0);
    parametersLayout->addWidget(maxPixelLabel, 5, 0);
    QGroupBox *parametersBox = new QGroupBox(tr("Parameters"));
    parametersBox->setLayout(parametersLayout);

    // leftParamsLayout -- данные от левого датчика
    // создаем сетку, которую разместим в выделенном боксе
    QGridLayout *leftParamsLayout = new QGridLayout;
    leftParamsLayout->addWidget(leftMotionValue, 0, 0);
    leftParamsLayout->addWidget(leftDeltaXValue, 1, 0);
    leftParamsLayout->addWidget(leftDdeltaYValue, 2, 0);
    leftParamsLayout->addWidget(leftSqualValue, 3, 0);
    leftParamsLayout->addWidget(leftShutterValue, 4, 0);
    leftParamsLayout->addWidget(leftMaxPixelValue, 5, 0);
    QGroupBox *leftParamsBox = new QGroupBox(tr("L"));
    leftParamsBox->setLayout(leftParamsLayout);

    // rightParamsLayout -- данные от правого датчика
    // создаем сетку, которую разместим в выделенном боксе
    QGridLayout *rightParamsLayout = new QGridLayout;
    rightParamsLayout->addWidget(rightMotionValue, 0, 0);
    rightParamsLayout->addWidget(rightDeltaXValue, 1, 0);
    rightParamsLayout->addWidget(rightDdeltaYValue, 2, 0);
    rightParamsLayout->addWidget(rightSqualValue, 3, 0);
    rightParamsLayout->addWidget(rightShutterValue, 4, 0);
    rightParamsLayout->addWidget(rightMaxPixelValue, 5, 0);
    QGroupBox *rightParamsBox = new QGroupBox(tr("R"));
    rightParamsBox->setLayout(rightParamsLayout);

    // создаем центральный виджет
    QWidget *central = new QWidget(this);

    // создаем сетку и назначаем ее центральному виджету
    QGridLayout *grid = new QGridLayout(central);

    // добавляем элементы в сетку
    grid->addWidget(parametersBox, 1, 0);
    grid->addWidget(leftParamsBox, 1, 1, 1, 2);
    grid->addWidget(rightParamsBox, 1, 3, 1, 2);
    // добавим кнопку
    grid->addWidget(startStopButton, 0, 0, 1, 3);
    // текстовый редактор
    grid->addWidget(trafficText, 1, 5, 1, 10);

    // устанавливаем центральный виджет
    setCentralWidget(central);
}

// обработка нажатия кнопки
void MainWindow::onStartStop()
{
    // первое нажатие кнопки
    if (!isListening) {
        // начинаем слушать
        // в bind указываем адрес и порт
        if (udpSocket->bind(QHostAddress::AnyIPv4, 8888)) {
            isListening = true;
            startStopButton->setText("Stop Listening");
            trafficText->append("MESSAGE: Listening on UDP port 8888");
        }
        else {
            trafficText->append("ERROR: Failed to bind to port 8888");
        }
    }
    else {
        udpSocket->close();
        isListening = false;
        startStopButton->setText("Start Listening");
        trafficText->append("MESSAGE: Stopped listening");
    }
}

// принимаем и обрабатываем датаграммы
void MainWindow::processPendingDatagrams()
{
    // если хотя бы одна датаграмма доступна для чтения
    while (udpSocket->hasPendingDatagrams()) {
        // создаем массив байтов для датаграммы
        QByteArray datagram;
        // изменяем размер массива на основе размера датаграммы
        datagram.resize(udpSocket->pendingDatagramSize());

        // отправитель и адрес отправителя
        QHostAddress sender;
        quint16 senderPort;
        // читаем датаграмму и сохраняем ее в datagram
        // адрес хоста и порт отправителя сохраняем в sender и senderPort соответственно
        udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        // преобразуем в строку и добавляем в текстовое поле
        QString line = QString::fromUtf8(datagram);
        // trimmed() возвращает строку без проблелов сначала и в конце
        line = line.trimmed();
        // отображаем строку в текстовом окне
        trafficText->append(QString("[%1:%2]\r\n%3").arg(sender.toString()).arg(senderPort).arg(line));
    }
}

