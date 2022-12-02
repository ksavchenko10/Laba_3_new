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
    this->setStatusBar(new QStatusBar(this));
    this->statusBar()->showMessage("Выберите файл БД:");
    QString homePath = QDir::currentPath();

    // Определим  файловой системы:
    //dirModel =  new QFileSystemModel(this);
    //dirModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    //dirModel->setRootPath(homePath);

    fileModel = new QFileSystemModel(this);
    fileModel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    //fileModel->setRootPath(homePath);

    //Показать как дерево, пользуясь готовым видом:
    //treeView = new QTreeView(); //дерево файлов
    //treeView->setModel(dirModel);
    //treeView->expandAll();

    QSplitter *splitter = new QSplitter(parent); //создаем сплиттер
    tableView = new QTableView; //создаем таблицу
    tableView->setModel(fileModel); //добавляем наше дерево файлов в таблицу
    splitter->addWidget(tableView); //добавляем таблицу к сплиттеру
    setCentralWidget(splitter); //

    QSplitter *splitterRight = new QSplitter(parent); //содзаем правый сплиттер
    splitterRight->setOrientation(Qt::Vertical); //размещение элементов в нем вертикальное
    splitter->addWidget(splitterRight); //доблавяем сплиттер к нашему основному сплиттеру, правая часть программы

    QSplitter *splitterEmptyVertical = new QSplitter(parent); //пустой сплиттер, чтобы сдвинуть элементы вверху
    splitterEmptyVertical->setMaximumHeight(10); //задаем максимальную высоту
    splitterRight->addWidget(splitterEmptyVertical); //добавляем сплитер

    int rightopSplitterHeight = 30; //задает высоту панели
    QSplitter *splitterRightFirst = new QSplitter(parent); //создаем сплиттер с нашими элементами управления
    //splitterRightFirst->setOrientation(Qt::Horizontal); //ориентация размещения на нем горизонтальная
    splitterRightFirst->setMaximumHeight(rightopSplitterHeight); //максимальная высота
    splitterRight->addWidget(splitterRightFirst); //доблавяем к нашему правому сплиттеру

    QLabel *caption = new QLabel("Выберите тип диаграммы: "); //текст
    caption->setMaximumWidth(200); //максимальная ширина
    caption->setAlignment(Qt::AlignRight|Qt::AlignCenter); //ориентация право по горизонтали и по вертикали по середине
    caption->setMaximumHeight(rightopSplitterHeight); //максимальная высота
    splitterRightFirst->addWidget(caption); //добавляем к сплиттеру

    qbox = new QComboBox(); //раскрывающийся список
    qbox->setMaximumWidth(100); //максимальная ширина
    qbox->setMaximumHeight(rightopSplitterHeight); //максимальная высота
    qbox->addItem("BarChart"); //добавляем элемент для выбора
    qbox->addItem("PieChart"); //добавляем элемент для выбора
    //делаем вызов функции on_comboBoxChanged при изменении выбранного элемента в списке
    connect(qbox, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_comboBoxChanged()));
    splitterRightFirst->addWidget(qbox); //доблавяем к сплиттеру

    checkBox = new QCheckBox("Черно-белый график: "); //чек бокс для выбора цвета графика
    checkBox->setMaximumWidth(200); //максимальная ширина
    checkBox->setMaximumHeight(rightopSplitterHeight); //максимальная высота
    //делаем вызов функции on_checkBoxClicked при нажатии на checkbox
    connect(checkBox, SIGNAL(clicked()), this, SLOT(on_checkBoxClicked()));
    splitterRightFirst->addWidget(checkBox);//добавляем к сплиттеру

    QPushButton *print = new QPushButton("Печать"); //кнопка
    print->setMaximumWidth(200); //максимальная ширина
    print->setMaximumHeight(rightopSplitterHeight); //максимальная высота
    //делаем вызов функции on_paintClicked при нажатии на кнопку
    connect(print, SIGNAL(clicked()), this, SLOT(on_paintClicked()));
    splitterRightFirst->addWidget(print); //добавляем к сплиттеру


    chartView = new QtCharts::QChartView(); //график
    chartView->resize(640, 480); //задаем размер
    chart = chartView->chart();
    splitterRight->addWidget(chartView); //добавляем к сплиттеру

     axisY = new QtCharts::QValueAxis(); //ось
     axisY->setMin(0); //минимальное значение оси
     axisY->setMax(200); //максимальное
     axisY->setTickCount(10); //шаг

     chart->addAxis(axisY, Qt::AlignLeft);//доблавяем ось У к графику

 //1.Добавление диаграммы

        QItemSelectionModel *selectionModel = treeView->selectionModel();
        /*QModelIndex rootIx = dirModel->index(0, 0, QModelIndex());//корневой элемент

        QModelIndex indexHomePath = dirModel->index(homePath);
        QFileInfo fileInfo = dirModel->fileInfo(indexHomePath);
*/
        tableView->setRootIndex(fileModel->setRootPath(homePath)); //устанавливаем папку с файлами текущую программы

        /* Рассмотрим способы обхода содержимого папок на диске.
         * Предлагается вариант решения, который может быть применен для более сложных задач.
         * Итак, если требуется выполнить анализ содержимого папки, то необходимо организовать обход содержимого. Обход выполняем относительно модельного индекса.
         * Например:*/
        /*if (fileInfo.isDir()) {
            /*
             * Если fileInfo папка то заходим в нее, что бы просмотреть находящиеся в ней файлы.
             * Если нужно просмотреть все файлы, включая все вложенные папки, то нужно организовать рекурсивный обход.
            */
            /*QDir dir  = fileInfo.dir();

            if (dir.cd(fileInfo.fileName())) {
                /**
                 * Если зашли в папку, то пройдемся по контейнеру QFileInfoList ,полученного методом entryInfoList,
                 * */

               /* foreach (QFileInfo inf, dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Type)) {
                    qDebug() << inf.fileName() << "---" << inf.size();
                }

                dir.cdUp();//выходим из папки
            }
        }

        QDir dir = fileInfo.dir();

        foreach (QFileInfo inf, dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDir::Type)) {

            qDebug() << inf.fileName() << "---" << inf.size();
        }


        treeView->header()->resizeSection(0, 200);
        //Выполняем соединения слота и сигнала который вызывается когда осуществляется выбор элемента в TreeView
        connect(selectionModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
                this, SLOT(on_selectionChangedSlot(const QItemSelection &, const QItemSelection &)));
        //Пример организации установки курсора в TreeView относительно модельного индекса
        QItemSelection toggleSelection;
        QModelIndex topLeft;
        topLeft = dirModel->index(homePath);
        dirModel->setRootPath(homePath);

        toggleSelection.select(topLeft, topLeft);
        selectionModel->select(toggleSelection, QItemSelectionModel::Toggle);

        */
        //Выполняем соединения слота и сигнала который вызывается когда осуществляется выбор элемента в TableView
        connect(selectionModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
                 this, SLOT(on_selectionChangedSlot(const QItemSelection &, const QItemSelection &)));
        //выбор осуществляется с помощью курсора
}

void MainWindow::on_selectionChangedSlot(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);
    redraw(); //вызываем функции перерисовки графика
}

void MainWindow::on_checkBoxClicked()
{
    redraw(); //вызываем функции перерисовки графика
}

void MainWindow::on_comboBoxChanged()
{
   redraw(); //вызываем функции перерисовки графика
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
    QModelIndex i = tableView->model()->index(index.row(),0,index.parent());
    QString homePath = QDir::currentPath(); //текущий путь к папке
    QString fileNameWithExtension = file.data().toString(); //имя файла с раширением
    QString filePath = homePath + "/" + fileNameWithExtension; //путь к файлу
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
