#include "view.h"

View::View()
{
    _main_window = new MainWindow;
}

void View::show()
{
    _main_window->show();
}

void View::importFile(QMap<QString, QTableWidget *> map, QLineEdit * edit)
{
    QString fileName(QFileDialog::getOpenFileName(
                         _main_window,
                         "Select machine file to import"
                         ));
    if (!fileName.isNull())
        emit importedFile(fileName, map, edit);
}

void View::exportFile()
{
    QString fileName(QFileDialog::getSaveFileName(_main_window, "Select the file to export."));
    if (!fileName.isNull())
        emit exportedFile(fileName);
}

void View::bindEvents()
{
    MutationWidget *mutationWidget = _main_window->getMutationWidget();
    connect(mutationWidget, &MutationWidget::importFile, this, &View::importFile);
    connect(mutationWidget, &MutationWidget::exportFile, this, &View::exportFile);
    connect(mutationWidget, &MutationWidget::checkingExperiment, this, &View::checkingExperiment);
    connect(mutationWidget, &MutationWidget::checkingSequence, this, &View::checkingSequence);
    connect(mutationWidget, &MutationWidget::generateSpecification, this, &View::generateSpecification);
    connect(mutationWidget, &MutationWidget::generateMutation, this, &View::generateMutation);
    connect(mutationWidget, &MutationWidget::machineTypeChanged, this, &View::machineTypeChanged);
    connect(this, &View::checkingExperimentResults, mutationWidget, &MutationWidget::checkingExperimentResults);
    connect(this, &View::checkingSequenceResults, mutationWidget, &MutationWidget::checkingSequenceResults);
    connect(this, &View::machineSVGGenerated, mutationWidget, &MutationWidget::machineSVGGenerated);
}
