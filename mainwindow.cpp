#include "mainwindow.h"
#include "ui_mainwindow.h"
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
#include <QtCharts/QChartGlobal>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QAbstractBarSeries>
#include <QtCharts/QPercentBarSeries>
#include <QtCharts/QStackedBarSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QLegend>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtCore/QTime>
#include <QtCharts/QBarCategoryAxis>


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
    dirModel =  new QFileSystemModel(this);
    dirModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    dirModel->setRootPath(homePath);

    fileModel = new QFileSystemModel(this);
    fileModel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    fileModel->setRootPath(homePath);

    //Показать как дерево, пользуясь готовым видом:
    treeView = new QTreeView(); //дерево файлов
    treeView->setModel(dirModel);
    treeView->expandAll();

    QSplitter *splitter = new QSplitter(parent); //создаем сплиттер
    tableView = new QTableView; //создаем таблицу
    tableView->setModel(fileModel); //добавляем наше дерево файлов в таблицу
    splitter->addWidget(tableView); //добавляем таблицу к сплиттеру
    setCentralWidget(splitter); //


    //1.Добавление диаграммы
        QChartView *chartView;
        QChart *chartBar =  themeWidget->createBarChart(5);
        chartView = new QChartView(chartBar);

        //splitter->addWidget(themeWidget);
        //splitter->addWidget(chartView);
        splitter->addWidget(chartView);
        setCentralWidget(splitter);

        QItemSelectionModel *selectionModel = treeView->selectionModel();
        QModelIndex rootIx = dirModel->index(0, 0, QModelIndex());//корневой элемент

        QModelIndex indexHomePath = dirModel->index(homePath);
        QFileInfo fileInfo = dirModel->fileInfo(indexHomePath);

        /* Рассмотрим способы обхода содержимого папок на диске.
         * Предлагается вариант решения, которы может быть применен для более сложных задач.
         * Итак, если требуется выполнить анализ содержимого папки, то необходимо организовать обход содержимого. Обход выполняем относительно модельного индекса.
         * Например:*/
        if (fileInfo.isDir()) {
            /*
             * Если fileInfo папка то заходим в нее, что бы просмотреть находящиеся в ней файлы.
             * Если нужно просмотреть все файлы, включая все вложенные папки, то нужно организовать рекурсивный обход.
            */
            QDir dir  = fileInfo.dir();

            if (dir.cd(fileInfo.fileName())) {
                /**
                 * Если зашли в папку, то пройдемся по контейнеру QFileInfoList ,полученного методом entryInfoList,
                 * */

                foreach (QFileInfo inf, dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Type)) {
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
        //Пример организации установки курсора в TreeView относит ельно модельного индекса
        QItemSelection toggleSelection;
        QModelIndex topLeft;
        topLeft = dirModel->index(homePath);
        dirModel->setRootPath(homePath);

        toggleSelection.select(topLeft, topLeft);
        selectionModel->select(toggleSelection, QItemSelectionModel::Toggle);
    }
}
