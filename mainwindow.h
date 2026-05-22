#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QUdpSocket>
#include <QGridLayout>
#include <QGroupBox>

// создадим выровненную структуру с данными для отправки
#pragma pack(push, 1)
struct SensorPacket {
    // левый датчик
    uint8_t  l_motion;
    int8_t   l_dx;
    int8_t   l_dy;
    uint8_t  l_squal;
    uint16_t l_shutter;
    uint8_t  l_max_pix;
    // правый датчик
    uint8_t  r_motion;
    int8_t   r_dx;
    int8_t   r_dy;
    uint8_t  r_squal;
    uint16_t r_shutter;
    uint8_t  r_max_pix;
};
// возвращаем выравнивание
#pragma pack(pop)

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
    // устанавливаем все значения в ноль
    void setParametersToZero();

private:
    // структура с параметрами датчиков
    struct sensorParameters {
        QLabel *motionLabel;    // движение: 1/0
        QLabel *deltaXLabel;    // смещение по dx
        QLabel *deltaYLabel;    // смещение по dy
        QLabel *squalLabel;     // качество поврехности
        QLabel *shutterLabel;   // значение затвора
        QLabel *maxPixelLabel;  // максимальное значение пикселя в кадре

        // используем конструкторы для создания структуры
        // один текст для всех полей
        sensorParameters(const QString &text, QWidget *parent = nullptr) {
            motionLabel   = new QLabel(text, parent);
            deltaXLabel   = new QLabel(text, parent);
            deltaYLabel   = new QLabel(text, parent);
            squalLabel    = new QLabel(text, parent);
            shutterLabel  = new QLabel(text, parent);
            maxPixelLabel = new QLabel(text, parent);
        }

        // разный текст для каждого поля
        sensorParameters(const QString &m, const QString &dx, const QString &dy,
                         const QString &sq, const QString &sh, const QString &mp,
                         QWidget *parent = nullptr) {
            motionLabel   = new QLabel(m, parent);
            deltaXLabel   = new QLabel(dx, parent);
            deltaYLabel   = new QLabel(dy, parent);
            squalLabel    = new QLabel(sq, parent);
            shutterLabel  = new QLabel(sh, parent);
            maxPixelLabel = new QLabel(mp, parent);
        }
    };

    sensorParameters parametersName;
    sensorParameters leftParameters;
    sensorParameters rightParameters;

    // переменные для накапливания смещений
    int r_dx = 0, r_dy = 0, l_dx = 0, l_dy = 0;

    // окно для отображения сообщений из порта
    QTextEdit *trafficText;
    // статус-бар для состояний программы
    QStatusBar *statusBar;
    // кнопка для запуска приема данных
    QPushButton *startStopButton;
    // кнопка для сброса значений смещения
    QPushButton *clearOffsetButton;

    // боксы для размещения лейблов
    QGroupBox *parametersTitleBox;
    QGroupBox *leftParametersBox;
    QGroupBox *rightParametersBox;

    // создаем udp сокет для получения датаграмм
    QUdpSocket *udpSocket;
    // флаг прослушивания порта
    bool isListening;

    // создаем боксы с лейблами
    QGroupBox * createCustomBox(const sensorParameters &p, const char *s, int w, int h);

    // сетка для размещения компонентов
    void setMainLayout();

    // обновляем лейблы на основе данных из пакета
    void updateUiFromPacket(const SensorPacket *pkt);
};

#endif // MAINWINDOW_H
