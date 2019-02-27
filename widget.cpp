
#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    resize(600, 560);
    setMouseTracking(true);
    pointerAngle = 0;

    // 抽奖按钮
    startButton = new QPushButton(this);
    startButton->move(250, 450);
    startButton->setFont(QFont("Timer", 20, QFont::Bold));
    startButton->setText(tr("抽奖"));
    startButton->resize(100, 50);

    // 结果标签
    resultLabel = new QLabel(tr("抽奖结果:"), this);
    resultLabel->move(223, 105);

    // 抽奖结果
    resultText = new QLineEdit(tr("未开始抽奖"), this);
    resultText->move(293, 103);
    resultText->setEnabled(false);
    resultText->setAlignment(Qt::AlignCenter);

    pointerTimer = new QTimer(this);

    setStyleSheet("QLabel{ font-size:15px; }\
                  QLineEdit{color:white; font-size:14px; background-color:#f57ab8;\
                  border:1px solid gray; border-radius:5px; }\
                  QPushButton{width:36px; height:16px; text-align:center; background-color:#da0000;\
                  color:white; border:1px solid gray; border-radius:3px; }"
                 );

    connect(pointerTimer, SIGNAL(timeout()), this, SLOT(pointerRotating()));
    connect(startButton, SIGNAL(clicked(bool)), this, SLOT(startClick()));
    connect(this, SIGNAL(sigResult(int)), this, SLOT(reResult(int)));
}

void Widget::gradientArc(QPainter *painter, int radius, int startAngle,
                         int angleLength, int arcHeight, QRgb color)
{
    QRadialGradient gradient(0, 0, radius);
    gradient.setColorAt(0, Qt::black);
    gradient.setColorAt(1.0, color);
    painter->setBrush(gradient);

    // << 1（左移1位）相当于radius*2 即：150*2=300
    //QRectF(-150, -150, 300, 300)
    QRectF rectf(-radius, -radius, radius << 1, radius << 1);
    QPainterPath path;
    path.arcTo(rectf, startAngle, angleLength);

    QPainterPath subPath;
    subPath.addEllipse(rectf.adjusted(arcHeight, arcHeight, -arcHeight, -arcHeight));
    //subPath.addEllipse(rect.adjusted(arcHeight,arcHeight,-arcHeight,-arcHeight));
    //path为扇形 subPath为椭圆
    path -= subPath;//

    // QFont font;
    // font.setFamily("Microsoft YaHei");
    // font.setPointSize(14);
    // painter->setPen(Qt::NoPen);
    // path.addText(path.pointAtPercent(0.5), font, QStringLiteral("一去丶二三里"));

    painter->setPen(Qt::NoPen);
    painter->drawPath(path);
}

void Widget::pointerRotating() // 指针旋转事件
{
    if (isStoping) // 用这个变量控制而不是在 pointerAngle==resultAngle 是为了避免后期修改导致bug
    {
	pointerTimer->stop();
	pointerAngle = resultAngle;
	update();
	return ;
    }
    pointerAngle = pointerAngle + incAngle*0.1;

    if (keepRotating)
    {
	if (incAngle >= 51)
	    incAngle--;
    }
    else
    {
	if (incAngle >= 92)
	{
	    incAngle -= 2;
	}
	else if ((resultAngle > pointerAngle && resultAngle-pointerAngle < 20) || resultAngle+360-pointerAngle < 20)
	{
	    if (incAngle >= 12)
		incAngle--;
	}
	else if ((resultAngle > pointerAngle && resultAngle-pointerAngle < 30) || resultAngle+360-pointerAngle < 30)
	{
	    if (incAngle >= 22)
		incAngle -= 2;
	}
	else if ((resultAngle > pointerAngle && resultAngle-pointerAngle < 45) || resultAngle+360-pointerAngle < 45)
	{
	    if (incAngle >= 32)
		incAngle -= 2;
	}
	else if ((resultAngle > pointerAngle && resultAngle-pointerAngle < 65) || resultAngle+360-pointerAngle < 65)
	{
	    if (incAngle >= 42)
		incAngle -= 2;
	}
	else if ((resultAngle > pointerAngle && resultAngle-pointerAngle < 90) || resultAngle+360-pointerAngle < 90)
	{
	    if (incAngle >= 52)
		incAngle -= 2;
	}
	else if ((resultAngle > pointerAngle && resultAngle-pointerAngle < 120) || resultAngle+360-pointerAngle < 120)
	{
	    if (incAngle >= 62)
		incAngle -= 2;
	}
	else if ((resultAngle > pointerAngle && resultAngle-pointerAngle < 155) || resultAngle+360-pointerAngle < 155)
	{
	    if (incAngle >= 72)
		incAngle -= 2;
	}
	else if ((resultAngle > pointerAngle && resultAngle-pointerAngle < 200) || resultAngle+360-pointerAngle < 200)
	{
	    if (incAngle >= 82)
		incAngle -= 2;
	}
    }

    if(pointerAngle > 360) // 转了一圈
    {
	pointerAngle = 0;
	turnsNumber++;
	if(turnsNumber == 5) // 5圈之后开始停止
	{
	    qDebug() << "准备停止";
	    keepRotating = false;
	}
    }

    if (!keepRotating && incAngle < 51 && ((resultAngle -pointerAngle > 0 && resultAngle-pointerAngle <= incAngle*0.1) || resultAngle+360-pointerAngle <= incAngle*0.1) )
    {
	qDebug() << "停止，指针角度：" << pointerAngle << "  目标角度：" << resultAngle << "  旋转速度：" << incAngle;
	stopRotating();
    }

    update();
}

void Widget::startClick()
{
    /*
    click() ->> reResult() -> returnResult() ->> timer.updatePaint() -> paintEvent()
     */
    if(!pointerTimer->isActive()) // 静止状态
    {
	pointerTimer->start(30);
        int iRangd = getRand();
        emit sigResult(iRangd);
	turnsNumber = 0;
	incAngle = 360;
	keepRotating = true;
	isStoping = false;
	resultText->setStyleSheet("color:white;");
	resultText->setText(tr("正在抽奖..."));
    }
    else // 指针正在旋转，则立即停下
    {
	keepRotating = false;
    }
}

void Widget::reResult(int re) // 开始转后获得一个随即结果，传递到这里
{
    awardsMap = returnResult(re); // 根据结果设置角度和文字
    QMapIterator<int, QString> ii(awardsMap);
    if(awardsMap.isEmpty())
        return;
    if (ii.hasNext())
    {
        ii.next();
	resultAngle = ii.key();  // 结果角度
	resultString = ii.value(); // 结果文字
    }
}

QMap<int, QString> Widget::returnResult(int re) // 根据结果获取随机角度和文字
{

    int ire = 0;
    QMap<int, QString> map;
    switch(re)
    {
    case 0:
        //（rand（）%（b-a））+ a，-->[a,b)的随机数
	ire = (qrand() % 42 + 2);
        qDebug() << "二等奖";
        map.insert(ire, tr("二等奖"));
        break;
    case 1:
        qDebug() << "谢谢1";
	ire = (qrand() % 42 + 47);
        map.insert(ire, tr("谢谢参与"));
        break;
    case 2:
        qDebug() << "特等奖";
	ire = (qrand() % 42 + 92);
        map.insert(ire, tr("特等奖"));
        break;
    case 3:
        qDebug() << "三等奖";
	ire = (qrand() % 42 + 137);
        map.insert(ire, tr("三等奖"));
        break;
    case 4:
        qDebug() << "幸运2";
	ire = (qrand() % 42 + 182);
        map.insert(ire, tr("幸运奖"));
        break;
    case 5:
        qDebug() << "谢谢2";
	ire = (qrand() % 42 + 227);
        map.insert(ire, tr("谢谢参与"));
        break;
    case 6:
        qDebug() << "一等奖";
	ire = (qrand() % 42 + 272);
        map.insert(ire, tr("一等奖"));
        break;
    case 7:
        qDebug() << "幸运1";
	ire = (qrand() % 42 + 317);
        map.insert(ire, tr("幸运奖"));
        break;
    default:
        break;
    }
    return map;
}

QString Widget::showAwards(Widget::Awards award) // 根据结果返回字符串
{
    QString result = "";
    switch(award)
    {
    case Spe:
        result = "特等奖";
        break;
    case First:
        result = "一等奖";
        break;
    case Second:
        result = "二等奖";
        break;
    case Third:
        result = "三等奖";
        break;
    case Luck_Fir:
        result = "幸运奖";
        break;
    case Luck_Sec:
        result = "幸运奖";
        break;
    case ThanK_Fir:
        result = "谢谢参与";
        break;
    case Thank_Sec:
        result = "谢谢参与";
        break;
    default:
        break;
    }
    return result;
}

void Widget::stopRotating()
{
    //pointerTimer->stop();
    isStoping = true;
    //pointerAngle = resultAngle;
    resultText->setText(resultString);
    update();
}

int Widget::getRand() // 设置奖项概率
{
    int re = 10;
    QTime tim = QTime::currentTime();
    qsrand(tim.msec() + tim.second() * 1000);
    int rand = ((qrand() % 100) + 1);
    qDebug() << "rand:" << rand;

    if (rand <= 1)
	re = 2;
    else if (rand <= 4)
	re = 6;
    else if (rand <= 9)
	re = 0;
    else if (rand <= 20)
	re = 3;
    else if (rand <= 35)
	re = 4;
    else if (rand <= 50)
	re = 7;
    else if (rand <= 75)
	re = 5;
    else
	re = 1;

    return re;
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    int radius = 150;
    int arcHeight = 138;

    // 坐标中心
    painter.translate(width() >> 1, height() >> 1);

    // 旋转转盘而不旋转指针
    //painter.rotate(pointerAngle);

    /*
     * 参数二：半径
     * 参数三：开始的角度
     * 参数四：指扫取的角度-顺时针（360度 / 8 = 45度）
     * 参数五：圆环的高度
     * 参数六：填充色
    **/
    gradientArc(&painter, radius, 0, 45, arcHeight, qRgb(200, 200, 0));
    gradientArc(&painter, radius, 45, 45, arcHeight, qRgb(200, 0, 200));
    gradientArc(&painter, radius, 90, 45, arcHeight, qRgb(0, 200, 200));
    gradientArc(&painter, radius, 135, 45, arcHeight, qRgb(200, 0, 0));
    gradientArc(&painter, radius, 225, 45, arcHeight, qRgb(0, 200, 0));
    gradientArc(&painter, radius, 180, 45, arcHeight, qRgb(0, 0, 200));
    gradientArc(&painter, radius, 270, 45, arcHeight, qRgb(0, 0, 0));
    gradientArc(&painter, radius, 315, 45, arcHeight, qRgb(150, 150, 150));

    // 画文字
    painter.setPen(QPen(QColor(Qt::yellow), 2));
    painter.rotate(-35);
    painter.drawText(60, 30, tr("谢谢参与"));
    painter.rotate(40);
    painter.drawText(70, 30, tr("一等奖"));
    painter.rotate(95);
    painter.drawText(70, 30, tr("二等奖"));
    painter.rotate(135);
    painter.drawText(70, 30, tr("三等奖"));
    painter.rotate(180);
    painter.drawText(70, 30, tr("幸运奖"));
    painter.rotate(210);
    painter.drawText(70, 30, tr("幸运奖"));
    painter.rotate(245);
    painter.drawText(70, 30, tr("谢谢参与"));
    painter.rotate(40);
    painter.drawText(70, 30, tr("特等奖"));

    // 画指针红色部分
    QPainter painter2(this);
    painter2.setRenderHint(QPainter::Antialiasing, true);
    painter2.translate(width() >> 1, height() >> 1);
    painter2.rotate(pointerAngle);
    static const QPoint poit[4] = {QPoint(0, -18), QPoint(10, 0), QPoint(0, 60), QPoint(-10, 0)};
    painter2.setBrush(QColor(Qt::red));
    painter2.setPen(Qt::NoPen);
    painter2.drawPolygon(poit, 4);

    // 画指针黄色圆心
    painter2.setBrush(QColor(Qt::yellow));
    painter2.drawEllipse(-7, -7, 14, 14);
}
