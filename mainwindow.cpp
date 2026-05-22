#include "mainwindow.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QTextEdit>
#include <QTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    // создаем кнопки
    startStopButton(new QPushButton(tr("Start/Stop"))),
    clearOffsetButton(new QPushButton(tr("Clear offset values"))),

    // создаем окно для отображения информации
    trafficText(new QTextEdit()),

    // создадим три структуры с названиями параметров и их
    // значений для левого и правого датчиков
    parametersName(tr("Motion"), tr("Delta X"), tr("Delta Y"), tr("SQUAL"), tr("Shutter"), tr("Maximum Pixel")),
    leftParameters(tr("0")),
    rightParameters(tr("0"))
{
    // устанавливаем боксы с названиями параметров
    parametersTitleBox = new QGroupBox();
    parametersTitleBox = createCustomBox(parametersName, "Parameters", 250, 300);

    leftParametersBox = new QGroupBox();
    leftParametersBox = createCustomBox(leftParameters, "L", 145, 300);

    rightParametersBox = new QGroupBox();
    rightParametersBox = createCustomBox(rightParameters, "R", 145, 300);

    // устанавливаем сетку приложения
    setMainLayout();

    // фиксируем размер кнопок и текстового окна
    startStopButton->setFixedSize(250, 50);
    clearOffsetButton->setFixedSize(300, 50);
    trafficText->setFixedSize(700, 370);

    // создаем udp сокет
    udpSocket = new QUdpSocket(this);

    // соединяем слоты и сигналы
    connect(startStopButton, &QPushButton::clicked, this, &MainWindow::onStartStop);
    connect(clearOffsetButton, &QPushButton::clicked, this, &MainWindow::setParametersToZero);
    connect(udpSocket, &QUdpSocket::readyRead, this, &MainWindow::processPendingDatagrams);
}

MainWindow::~MainWindow()
{
}

// создаем кастомные боксы с параметрами датчиков
QGroupBox * MainWindow
    ::createCustomBox(const sensorParameters &p, const char *s, int w, int h)
{
    // создаем сетку, которую разместим в выделенном боксе
    QGridLayout *parametersLayout = new QGridLayout;
    parametersLayout->addWidget(p.motionLabel, 0, 0);
    parametersLayout->addWidget(p.deltaXLabel, 1, 0);
    parametersLayout->addWidget(p.deltaYLabel, 2, 0);
    parametersLayout->addWidget(p.squalLabel, 3, 0);
    parametersLayout->addWidget(p.shutterLabel, 4, 0);
    parametersLayout->addWidget(p.maxPixelLabel, 5, 0);
    QGroupBox *b = new QGroupBox(tr(s));

    b->setLayout(parametersLayout);
    b->setFixedSize(w, h);
    return b;
}

// размещаем компоненты в сетке
void MainWindow::setMainLayout()
{
    // создаем центральный виджет
    QWidget *central = new QWidget(this);

    // создаем сетку и назначаем ее центральному виджету
    QGridLayout *grid = new QGridLayout(central);

    // добавляем элементы в сетку
    grid->addWidget(parametersTitleBox, 1, 0);
    grid->addWidget(leftParametersBox, 1, 1, 1, 2);
    grid->addWidget(rightParametersBox, 1, 3, 1, 2);

    // добавим кнопки
    grid->addWidget(startStopButton, 0, 0, 1, 1);
    grid->addWidget(clearOffsetButton, 0, 1, 1, 4);
    // текстовый редактор
    grid->addWidget(trafficText, 0, 5, 0, 10);

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

            QString str = QTime::currentTime().toString();
            trafficText->append(QString("[%1] MESSAGE: Listening on UDP port 8888").arg(str));
        }
        else {
            trafficText->append("ERROR: Failed to bind to port 8888");
        }
    }
    else {
        udpSocket->close();
        isListening = false;
        startStopButton->setText("Start Listening");

        QString str = QTime::currentTime().toString();
        trafficText->append(QString("[%1] MESSAGE: Stopped listening").arg(str));
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

        // читаем датаграмму и сохраняем ее в datagram
        udpSocket->readDatagram(datagram.data(), datagram.size());

        // проверяем размер датаграммы
        if (datagram.size() != sizeof(SensorPacket)) {
            trafficText->append("ERROR: datagram size is not valid");
            continue;
        }

        // принимаем датаграмму и приводим ее к типу указанной структуры
        const SensorPacket *pkt = reinterpret_cast<const SensorPacket*>(datagram.constData());

        // читаем данные из структуры
        updateUiFromPacket(pkt);
    }
}

// обновляем лейблы на основе данных из пакета
void MainWindow::updateUiFromPacket(const SensorPacket *pkt)
{
    // левый датчик
    leftParameters.motionLabel->setText(QString::number(pkt->l_motion));
    leftParameters.deltaXLabel->setText(QString::number(l_dx));
    leftParameters.deltaYLabel->setText(QString::number(l_dy));

    // накапливаем смещения
    l_dx += pkt->l_dx;
    l_dy += pkt->l_dy;

    leftParameters.squalLabel->setText(QString::number(pkt->l_squal));
    leftParameters.shutterLabel->setText(QString::number(pkt->l_shutter));
    leftParameters.maxPixelLabel->setText(QString::number(pkt->l_max_pix));

    // правый датчик
    rightParameters.motionLabel->setText(QString::number(pkt->r_motion));
    rightParameters.deltaXLabel->setText(QString::number(r_dx));
    rightParameters.deltaYLabel->setText(QString::number(r_dy));

    // накапливаем смещения
    r_dx += pkt->r_dx;
    r_dy += pkt->r_dy;

    rightParameters.squalLabel->setText(QString::number(pkt->r_squal));
    rightParameters.shutterLabel->setText(QString::number(pkt->r_shutter));
    rightParameters.maxPixelLabel->setText(QString::number(pkt->r_max_pix));
}

void MainWindow::setParametersToZero()
{
    // левый датчик
    r_dx = 0;
    r_dy = 0;
    l_dx = 0;
    l_dy = 0;
}

