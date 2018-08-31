#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QLabel>
#include <QMenuBar>
#include <QMenu>
#include <QAction>

#include "tab/homepagewidget.h"
#include "tab/mutationwidget.h"
#include "tab/benchmarkwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QWidget *_central_widget;
    QVBoxLayout *_central_widget_layout;
    QTabWidget *_tabs_widget;
    QMenuBar *_menu_bar;
    QMenu *_file_menu,
        *_output_menu;
    QAction *_file_create_database_action,
        *_file_open_database_action,
        *_file_import_option_file_action,
        *_output_generate_graph_action,
        *_help_action;


    HomepageWidget *_homepage_widget;
    MutationWidget *_mutation_widget;
    BenchmarkWidget *_benchmark_widget;
    /*
    InsertionWidget *_insertion_widget;
    GenerationWidget *_generation_widget;
    ManagementWidget *_management_widget;
    ConfigFileWidget *_config_file_widget;
    */
    int _homepageWidgetPos;
    int _mutationWidgetPos;
    int _benchmarkWidgetPos;
    /*Un 4,
        _insertionWidgetPos,
        _generationWidgetPos,
        _managementWidgetPos,
        _configFileWidgetPos;
    */
    /**
     * @brief buildInterface creates all interface components and assembles them
     */
    void buildInterface();
    /**
     * @brief relaySignals link each used signals from QWidgets to InsertionWidget signals in order to simplify signal connections
     */
    void relaySignals();

public:
    MainWindow(QWidget *parent = 0);
    MutationWidget * getMutationWidget();
    BenchmarkWidget * getBenchmarkWidget();
};

#endif // MAINWINDOW_H
