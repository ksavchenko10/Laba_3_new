#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QFileSystemModel>
#include <QTreeView>
#include <QTableView>
#include <QtCharts/QChartView>
#include <QComboBox>
#include <QCheckBox>
#include <QtCharts/QValueAxis>


class MainWindow : public QMainWindow
{
    Q_OBJECT
private slots:

    void on_selectionChangedSlot(const QItemSelection &selected, const QItemSelection &deselected); //метод при изменении выбора файла на дереве файлов
    void redraw(); //попытка открытия файла с базой и перерисовка графика
    void on_checkBoxClicked(); //изменение выбора в checbox
    void on_comboBoxChanged(); //изменение выбора в combobox
    void on_paintClicked(); //печать графика в pdf

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    QFileSystemModel *fileModel;
    //QFileSystemModel *dirModel;
    //QTreeView *treeView;
    QTableView *tableView;
    QtCharts::QChartView *chartView;
    QtCharts::QChart *chart;
    QComboBox *qbox;
    QCheckBox *checkBox;
    QtCharts::QValueAxis *axisY;

};

#endif // MAINWINDOW_H
