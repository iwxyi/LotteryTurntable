#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPainter>
#include <QRadialGradient>
#include <QPainterPath>
#include <QTimer>
#include <QDebug>
#include <QMouseEvent>
#include <QPushButton>
#include <QTime>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QMapIterator>

#define MAX_CIRCLE  1800

class Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = 0);

    void gradientArc(QPainter *painter, int radius, int startAngle,
                     int angleLength, int arcHeight, QRgb color);

public slots:
    void pointerRotating();
    void startClick();
    void reResult(int re);

signals:
    void sigResult(int re);

private:
    int pointerAngle; // 指针旋转的角度
    int resultAngle;            // 结果所在的角度
    int incAngle;  // 增加的角度
    quint32 turnsNumber;          // 指针转了几圈
    QString resultString;         // 结果文字字符串
    QTimer *pointerTimer;      // 重绘界面时钟
    int m_i;
    QPushButton *startButton;   // 开始抽奖的按钮
    bool keepRotating;       // 是否保持长期旋转，或者正在停止
    bool isStoping;        // 用来避免最后一下跳一下
    enum Awards
    {
        Spe = 3,
        First = 7,     // 一等奖
        Second = 5,    // 二等奖
        Third = 2,     // 三等奖
        Luck_Fir = 6,  // 幸运奖1
        Luck_Sec = 1,  // 幸运奖2
        ThanK_Fir = 4, // 谢谢惠顾1
        Thank_Sec = 0, // 谢谢惠顾2
    };
    Awards m_award;
    QLabel *resultLabel;          // 抽奖结果的标签
    QLineEdit *resultText;       // 抽奖结果的显示
    QMap<int, QString> awardsMap; // 每个整数对应的文字
    QMap<int, QString> returnResult(int re);
    static QString showAwards(Awards award);

    void stopInit();
    void stopRotating();
    int getRand();

protected:
    void paintEvent(QPaintEvent *);
};


#endif // MAINWINDOW_H
