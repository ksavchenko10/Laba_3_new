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
    MainWindow(QWidget *parent = 0); //конструктор
    ~MainWindow();
private:
    QFileSystemModel *fileModel; //объект для работы с файловой системой
    QTableView *tableView; //таблица с деревом файлов
    QtCharts::QChartView *chartView; //объект график
    QtCharts::QChart *chart; //объект данные графика
    QComboBox *qbox; //раскрывающийся список
    QCheckBox *checkBox; //checkbox выбора черное/бело
    QtCharts::QValueAxis *axisY; //объект для задания оси графика

};

#endif // MAINWINDOW_H
