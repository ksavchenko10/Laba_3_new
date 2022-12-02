#include "mainwindow.h"
#include <QSplitter>
#include <QListView>
#include <QTreeView>
#include <QTextEdit>
#include <QFileSystemModel>
#include <QItemSelectionModel>
#include <QVBoxLayout>
#include <QTableView>
#include <QHeaderView>
#include <QStatusBar>
#include <QDebug>
#include <QtWidgets/QWidget>
#include <QtSql>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QtCharts/QValueAxis>
#include <QPushButton>
#include <QPdfWriter>
#include <QSqlDatabase>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>


MainWindow::MainWindow(QWidget *parent)
    : //QWidget(parent)
      QMainWindow(parent)
{
    //Устанавливаем размер главного окна
    this->setGeometry(100, 100, 1500, 500);
    this->setStatusBar(new QStatusBar(this)); //устанавливаем статус бар внизу программы
    this->statusBar()->showMessage("Выберите файл БД:"); //устаналиваем сообщение в статус бар
    QString homePath = QDir::currentPath(); //текущая папка программы

    fileModel = new QFileSystemModel(this); //создаем эксемпляр класса QFileSystemModel для доступа к файлам
    fileModel->setFilter(QDir::NoDotAndDotDot | QDir::Files); //ставим фильтры:
    //NoDotAndDotDot - не выбирать переход на уровень вверх ..
    //AllDirs - все папки

    QSplitter *splitter = new QSplitter(parent); //создаем основной сплиттер
    tableView = new QTableView; //создаем таблицу
    tableView->setModel(fileModel); //устаналиваем модель работы с файлами в нашей таблице
    splitter->addWidget(tableView); //добавляем таблицу к сплиттеру
    setCentralWidget(splitter); //устаналиваем сплиттер как центральный виджет

    QSplitter *splitterRight = new QSplitter(parent); //создаем правый сплиттер
    splitterRight->setOrientation(Qt::Vertical); //размещение элементов в нем вертикальное, (все что будем добавлять к этому сплиттеру будет смещаться вниз)
    splitter->addWidget(splitterRight); //добавляем сплиттер к нашему основному сплиттеру, правая часть программы

    QSplitter *splitterEmptyVertical = new QSplitter(parent); //пустой сплиттер, чтобы сдвинуть элементы вверху
    splitterEmptyVertical->setMaximumHeight(10); //задаем максимальную высоту
    splitterRight->addWidget(splitterEmptyVertical); //добавляем сплитер к правому сплиттеру

    int rightopSplitterHeight = 30; //задает высоту панели
    QSplitter *splitterRightFirst = new QSplitter(parent); //создаем сплиттер с нашими элементами управления
    splitterRightFirst->setMaximumHeight(rightopSplitterHeight); //максимальная высота
    splitterRight->addWidget(splitterRightFirst); //доблавяем к нашему правому сплиттеру

    QLabel *caption = new QLabel("Выберите тип диаграммы: "); //текст
    caption->setMaximumWidth(200); //максимальная ширина
    caption->setAlignment(Qt::AlignRight|Qt::AlignCenter); //ориентация право по горизонтали и по вертикали по середине
    caption->setMaximumHeight(rightopSplitterHeight); //максимальная высота
    splitterRightFirst->addWidget(caption); //добавляем к сплиттеру панели

    qbox = new QComboBox(); //раскрывающийся список
    qbox->setMaximumWidth(100); //максимальная ширина
    qbox->setMaximumHeight(rightopSplitterHeight); //максимальная высота
    qbox->addItem("BarChart"); //добавляем элемент для выбора
    qbox->addItem("PieChart"); //добавляем элемент для выбора
    //выполняем соединения слота и сигнала
    //делаем вызов функции on_comboBoxChanged при изменении выбранного элемента в списке
    connect(qbox, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_comboBoxChanged()));
    splitterRightFirst->addWidget(qbox); //доблавяем к сплиттеру

    checkBox = new QCheckBox("Черно-белый график: "); //чек бокс для выбора цвета графика
    checkBox->setMaximumWidth(200); //максимальная ширина
    checkBox->setMaximumHeight(rightopSplitterHeight); //максимальная высота
    //делаем вызов функции on_checkBoxClicked при нажатии на checkbox
    connect(checkBox, SIGNAL(clicked()), this, SLOT(on_checkBoxClicked()));
    splitterRightFirst->addWidget(checkBox);//добавляем к правому сплиттеру

    QPushButton *print = new QPushButton("Печать"); //кнопка для создания pdf файла из графика
    print->setMaximumWidth(200); //максимальная ширина
    print->setMaximumHeight(rightopSplitterHeight); //максимальная высота
    //делаем вызов функции on_paintClicked при нажатии на кнопку
    connect(print, SIGNAL(clicked()), this, SLOT(on_paintClicked()));
    splitterRightFirst->addWidget(print); //добавляем к правому сплиттеру


    chartView = new QtCharts::QChartView(); //график
    chartView->resize(640, 480); //задаем размер
    chart = chartView->chart(); //объект для доступа к данным нашего графика
    splitterRight->addWidget(chartView); //добавляем к правому сплиттеру

     axisY = new QtCharts::QValueAxis(); //объект оси графика
     axisY->setMin(0); //минимальное значение оси
     axisY->setMax(200); //максимальное
     axisY->setTickCount(10); //шаг сетки (линий)

     chart->addAxis(axisY, Qt::AlignLeft);//доблавяем ось У к графику


        QItemSelectionModel *selectionModel = tableView->selectionModel(); //объект модели tableview
        tableView->setRootIndex(fileModel->setRootPath(homePath)); //устанавливаем для tableView текущую папку откуда запущена программа

        //Выполняем соединения слота и сигнала который вызывается когда осуществляется выбор элемента в TableView
        connect(selectionModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
                 this, SLOT(on_selectionChangedSlot(const QItemSelection &, const QItemSelection &)));
        //выбор осуществляется с помощью курсора
}

void MainWindow::on_selectionChangedSlot(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected); //чтобы компилятор не ругался на не используемую переменную
    redraw(); //вызываем функцию попытки открыть базы данных и перерисовки графика
}

void MainWindow::on_checkBoxClicked()
{
    redraw(); //вызываем функцию попытки открыть базы данных и перерисовки графика
}

void MainWindow::on_comboBoxChanged()
{
   redraw(); //вызываем функцию попытки открыть базы данных и перерисовки графика
}

void MainWindow::on_paintClicked() //печать графика в pdf
{
    QPdfWriter writer("out.pdf"); // создаем экземпляр QPdfWriter и передаем в конструктор имя файла
    writer.setCreator("Someone");//Указываем создателя документа
    writer.setPageSize(QPagedPaintDevice::A4);//Устанавливаем размер страницы
    QPainter painter(&writer); //объект для создания графики
    chartView->render(&painter); //считываем график из chartView
    painter.end(); //завершаем создание
}

void MainWindow::redraw()
{
    QModelIndex index = tableView->selectionModel()->currentIndex(); //текущая выбранная строка в дереве файлов
    QModelIndex file = tableView->model()->index(index.row(),0,index.parent()); //доступ к объекту текущего выбранного файла из модели нашего дерева файлов
    QString homePath = QDir::currentPath(); //текущий путь к папке
    QString fileNameWithExtension = file.data().toString(); //имя файла с раширением
    QString filePath = homePath + "/" + fileNameWithExtension; //полный путь к файлу
    QString fileName = fileNameWithExtension.split(".").at(0); //имя файла

    bool extensionIsSqlLite = false; //флаг который будет определять расширение у файла sqlite или нет
    int extensionIndex = fileNameWithExtension.lastIndexOf("."); //ищем последнюю точку в имени файла
    if (extensionIndex != -1) //если не найдена, значит расширения нет
    {
        if (fileNameWithExtension.mid(extensionIndex+1) == "sqlite") //текст после точки это расширение, если равно sqllite значит расширение нужной базы данных
        {
            extensionIsSqlLite = true; //ставим флаг что это файл расширения sqllite
        }
    }

    if (!extensionIsSqlLite) //если файл не расширения sqlite
    {
        qDebug() << "File is not database sqllite"; //выводим информацию в консоль
        chart->removeAllSeries(); //удаляем все серии на графике
        return;
    }

    QSqlDatabase dbase = QSqlDatabase::addDatabase("QSQLITE"); //объект для работы с базой данных
    dbase.setDatabaseName(filePath); //устанавливаем имя базы данных

    if (!dbase.open()) //если база данных не открылась
    {
        qDebug() << "Open database ERROR"; //выводим информацию в консоль
        chart->removeAllSeries(); //удаляем все серии на графике
        return;
    }
    else
        {
            QSqlQuery a_query; //создаем экземпляр для sql запросов
            a_query.exec("SELECT * FROM "+fileName); //sql запрос к базе данных, выбираем все записи

            chart->removeAllSeries(); //удаляем все серии на графике
            int numRecord = 200; //сколько записей показыват на графике

            if (qbox->currentIndex() == 0) //если выбран Barchart
            {
                QtCharts::QBarSeries *series = new QtCharts::QBarSeries(); //серия данных для графика
                a_query.first(); //выбираем первую запись в базе данных
                int i = 0;
                float maxvalue = 0; //максимальное значение, нужно для задания  оси У
                float minvalue = 0;//минимальное значение, нужно для задания  оси У

                do //цикл по всем записям из запроса к базе данных
                {
                    float value = a_query.value("Value").toFloat(); //значение в записи из базы
                    if (maxvalue < value) // если максимальный меньше чем значение текущей записи
                    {
                        maxvalue = value; //устаналиваем новое максимальное
                    }

                    if (value < minvalue) //если минимальное больше чем значение текущей записи
                    {
                        minvalue = value; //устанавливаем новое минимальное
                    }

                    //для графика создаем пару: название для легенды из поля time и значением value
                    QtCharts::QBarSet *bar = new QtCharts::QBarSet(a_query.value("Time").toString()); //название для легенды этого значения на графике
                    *bar <<  value; //добавляем значение

                    if (checkBox->isChecked()) //если checkbox выбран, должно быть черно-белым
                    {
                        if (i % 2 == 0) //если четное
                        {
                          bar->setColor(Qt::black); //делаем эту линию на графике черной
                        }
                        else
                        {
                            bar->setColor(Qt::gray); //делаем эту линию на графике серой
                        }
                     }

                     series->append(bar); //доблавяем объект линии к серии графика

                     i++;
                  }
                  while(a_query.next() && i <= numRecord); //переходим к следующей записи и если не больше numRecord (слишком много записей не могут отобразиться)

                  axisY->setMax(maxvalue); //устанавливаем максимальное значаение оси У
                  axisY->setMin(minvalue);//устанавливаем минимальное значаение оси У

                  series->attachAxis(axisY); //доблавяем ось к серии  графика
                  chart->addSeries(series); //добавляем серию с данными к графику
             }
            else if (qbox->currentIndex() == 1) //если был выбран Piechart
            {
                QtCharts::QPieSeries *series = new QtCharts::QPieSeries();
                a_query.first();  //выбираем первую запись в базе данных
                int i = 0;
                do //цикл по всем записям из запроса к базе данных
                {
                    //доблавяем к серии пару значений time и value
                    series->append(a_query.value("Time").toString(), a_query.value("Value").toFloat());
                    i++;
                }
                while(a_query.next() && i < numRecord); //переходим к следующей записи и если не больше numRecord (слишком много записей не могут отобразиться)

                chart->addSeries(series); //добавляем серию к графику
             }

             dbase.close(); //закрываем базу данных
         }
}

MainWindow::~MainWindow()
{

}
