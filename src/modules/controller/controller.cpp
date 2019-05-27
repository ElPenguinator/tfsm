#include "controller.h"

Controller::Controller():
    QObject() {}

void Controller::bindEvents(View &view, Model &model)
{

    _view = &view;
    _model = &model;

    // bind events for view
    _view->bindEvents();

    connect(_view, &View::importedFile, _model, &Model::importFile);
    connect(_view, &View::exportedFile, _model, &Model::exportFile);
    connect(_view, &View::checkingExperiment, _model, &Model::checkingExperiment);
    connect(_view, &View::checkingSequence, _model, &Model::checkingSequence);
    connect(_view, &View::checkingExperimentBenchmark, _model, &Model::checkingExperimentBenchmark);
    connect(_view, &View::checkingSequenceBenchmark, _model, &Model::checkingSequenceBenchmark);
    connect(_view, &View::generateSpecification, _model, &Model::generateSpecification);
    connect(_view, &View::generateMutation, _model, &Model::generateMutation);
    connect(_view, &View::machineTypeChanged, _model, &Model::changeMachineType);

    connect(_model, &Model::checkingExperimentResults, _view, &View::checkingExperimentResults);
    connect(_model, &Model::checkingSequenceResults, _view, &View::checkingSequenceResults);
    connect(_model, &Model::machineSVGGenerated, _view, &View::machineSVGGenerated);
}
