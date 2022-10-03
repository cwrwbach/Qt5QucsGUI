#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QColor>
#include <QDebug>
#include <QtMath>
#include <QValueAxis>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>


#define MAX_X 200
#define MAX_POINTS 1000

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget),
    chart(new QChart),
    tip(0),
    count(0),
    isStopping(false)
{
ui->setupUi(this);
initUI();

initSlot();
updateData();
}

MainWidget::~MainWidget()
{
delete ui;
}

void MainWidget::wheelEvent(QWheelEvent *event)
{
if (event->delta() > 0) 
    {
    chart->zoom(1.1);
    } 
    else 
    {
    chart->zoom(10.0/11);
    }
QWidget::wheelEvent(event);
}

void MainWidget::initUI()
{
initChart();
}

//++++++++++++++++++++++++++++++++

void MainWidget::initChart()
{
chartView = new QChartView();
series_1 = new QSplineSeries; //series_1 = new QLineSeries; SPLine or Line ??? FIXME ???
chartView->chart()->addSeries(series_1);

QValueAxis *axisX = new QValueAxis;
QValueAxis *axisY = new QValueAxis;

axisX->setRange(3e6, 300e6); //MAX_X);
axisY->setRange(-90, 10);

QPen pen = series_1->pen();

pen.setColor("red");
series_1->setPen(pen);

chartView->chart()->setPlotAreaBackgroundBrush(QColor(0,64,0));
chartView->chart()->setPlotAreaBackgroundVisible(true);

chartView->chart()->addAxis(axisX, Qt::AlignBottom);
chartView->chart()->addAxis(axisY, Qt::AlignLeft);

series_1->attachAxis(axisX);
series_1->attachAxis(axisY);

axisX->setTickCount(11);
//axisX->setLabelFormat("%.2f");
axisY->setTickCount(11);

axisX->setTitleText("Frequency");
axisY->setTitleText("dBfs");

axisX->setGridLineVisible(true);
axisY->setGridLineVisible(true);

chartView->chart()->legend()->hide();

chartView->setRenderHint(QPainter::Antialiasing);

ui->mainHorLayout->addWidget(chartView);
}



//+++++++++++++++++++++++++++++++++++++++++++++++++++++




void MainWidget::initSlot()
{
connect(ui->buttonThree, SIGNAL(clicked(bool)), this, SLOT(buttonSlot()));
connect(series_1, SIGNAL(hovered(QPointF, bool)), this, SLOT(tipSlot(QPointF,bool)));
}

//---

void MainWidget::updateData()
{
int i;
int debug = 0;
float test[MAX_POINTS];
float freq[MAX_POINTS];
float s11[MAX_POINTS];
float s21[MAX_POINTS];

int num_points = 200;
float value;

QVector<QPointF> data;

//QFile in_file("qucsator-output.txt");
QFile in_file("bh50.sim");

if (in_file.open(QIODevice::ReadOnly)) 
    {
    QDataStream stream(&in_file);
    }
else 
    {
    printf(" NO file exists \n");
    exit -1;
    }

QTextStream stream(&in_file);
QString line;
char show[256];

//get frequency values
line = (QString) stream.readLine();
line = (QString) stream.readLine();
printf(" LL %d \n",__LINE__);

for(i=0;i<num_points;i++) 
    {
    line = (QString) stream.readLine();

    QByteArray ba = line.toLocal8Bit();
    const char *c_str2 = ba.data();

  //  printf("%d: str2: %s\n",debug,c_str2);

    value = strtof(c_str2,0);
    printf("value = %f\n ",value);


     freq[i] = value;

    }



printf(" debug = %d \n",debug);

//get S11 values
line = (QString) stream.readLine();
line = (QString) stream.readLine();
printf(" LL %d \n",__LINE__);

for(i=0;i<num_points;i++) 
    {
    line = (QString) stream.readLine();

    QByteArray ba = line.toLocal8Bit();
    const char *c_str2 = ba.data();

 //   printf("%d: str2: %s\n",debug,c_str2);

    value = strtof(c_str2,0);
    s11[i] = value;
 //   printf("value = %f\n ",value);

    }

//get S21 values
line = (QString) stream.readLine();
line = (QString) stream.readLine();
printf(" LL %d \n",__LINE__);

for(i=0;i<num_points;i++) 
    {
    line = (QString) stream.readLine();

    QByteArray ba = line.toLocal8Bit();
    const char *c_str2 = ba.data();



 //   printf("%d: str2: %s\n",debug,c_str2);

    value = strtof(c_str2,0);
   // printf("str-v = %f ",value);

    s21[i] = value;
   // printf("value: %f \n",value);

    }



in_file.close();





for(i = 0; i < MAX_X; ++i)
    {
    test [i] = s21[i] ; //i -100 ; 
   // printf("XXV: %f \n",test[i]);
    }



for(i = 0; i < MAX_X; ++i)
    {
   // data.append(QPointF(i,test[i])); 
data.append(QPointF(freq[i],test[i])); 
    }

series_1->replace(data);
}

//----

void MainWidget::buttonSlot()
{
    if (QObject::sender() == ui->buttonThree) {
        if (!isStopping) {
            timer->stop();
        } else {
            timer->start();
        }
        isStopping = !isStopping;
    }
}

void MainWidget::tipSlot(QPointF position, bool isHovering)
{
    if (tip == 0)
        tip = new Callout(chart);

    if (isHovering) {
        tip->setText(QString("X: %1 \nY: %2 ").arg(position.x()).arg(position.y()));
        tip->setAnchor(position);
        tip->setZValue(11);
        tip->updateGeometry();
        tip->show();
    } else {
        tip->hide();
    }
}

