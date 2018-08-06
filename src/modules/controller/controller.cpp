#include "controller.h"

Controller::Controller():
    QObject() {}

void Controller::bindEvents(View &view, Model &model)
{

    _view = &view;
    _model = &model;

    // bind events for view
    _view->bindEvents();

    /* qobject_cast is a stupe. It doesn't appears in final code. It's ugly and absolutely not a good practice
    it's due to _view is of View abstract class that implements filesParsed method
    and ViewGUI that defines this method.
    but when call connect, we must set the address of the filesParsed method and we have to dynamicly use
    address of VuewGUI and ViewSH.
    SOLVE THIS HUGE PROBLEM !!! */

    connect(_view, &View::importedFile, _model, &Model::importFile);
    connect(_view, &View::exportedFile, _model, &Model::exportFile);
    connect(_view, &View::checkingExperiment, _model, &Model::checkingExperiment);
    connect(_view, &View::checkingSequence, _model, &Model::checkingSequence);

    connect(_model, &Model::checkingExperimentResults, _view, &View::checkingExperimentResults);
    connect(_model, &Model::checkingSequenceResults, _view, &View::checkingSequenceResults);

//    /* bind events between model and view */

//    connect(vGUI, &ViewGUI::filesParsed, _model, &Model::parseFiles);
//    connect(vGUI, &ViewGUI::dirParsed, _model, &Model::parseFolder);
//    connect(vGUI, &ViewGUI::configParsed, _model, &Model::parseConfig);
//    connect(vGUI, &ViewGUI::databaseCreated, _model, &Model::createDatabase);
//    connect(vGUI, &ViewGUI::databaseOpened, _model, &Model::openDatabase);
//    connect(vGUI, &ViewGUI::previewImageResized, _model, &Model::resizePreviewImage, Qt::BlockingQueuedConnection);
//    connect(vGUI, &ViewGUI::experimentsSelected, _model, &Model::getExperimentInformations);
//    connect(vGUI, &ViewGUI::experimentCampaignCleared, _model, &Model::clearExperimentCampaign);
//    connect(vGUI, &ViewGUI::experimentCampaignInserted, _model, &Model::insertExperimentCampaign);
//    connect(_model, &Model::setPreviewImage, vGUI, &ViewGUI::setPreviewImage);
//    connect(_model, &Model::setExperiments, vGUI, &ViewGUI::setExperiments);
//    connect(_model, &Model::selectedParsedExperiment, vGUI, &ViewGUI::setFiles);
//    connect(_model, &Model::setFindSelectorValues, vGUI, &ViewGUI::findSelectorValuesSetted);
//    connect(_model, &Model::setOrdinateSelectionValues, vGUI, &ViewGUI::ordinateSelectionValuesSetted);
//    connect(_model, &Model::configCheckResult, vGUI, &ViewGUI::configCheckResult);
//    connect(vGUI, &ViewGUI::insertionTabViewed, _model, &Model::onInsertionTabViewed);
//    connect(vGUI, &ViewGUI::insertionWidgetParserOptionSelected, _model, &Model::onInsertionWidgetParserOptionSelected);
//    connect(_model, &Model::setInsertionWidgetParserOptions, vGUI, &ViewGUI::setInsertionWidgetParserOptions);
//    // -------
//    connect(vGUI, &ViewGUI::generationTabViewed, _model, &Model::updateFindDatabaseWidgetContent);
//    connect(vGUI, &ViewGUI::metricValueChanged, _model, &Model::changeMetricValue);
//    connect(vGUI, &ViewGUI::configChecked, _model, &Model::checkConfig);
//    // generation widget
//    connect(vGUI, &ViewGUI::generationGraphTypeChanged, _model, &Model::onGenerationGraphTypeChange);
//    connect(vGUI, &ViewGUI::generationGraphFileGenerated, _model, &Model::onGenerationGraphGenerateFile);
//    connect(vGUI, &ViewGUI::generationSelectAbscissaMetric, _model, &Model::onGenerationSelectAbscissaMetric);
//    connect(_model, &Model::setGenerationAbscissaSelectorValues, vGUI, &ViewGUI::setGenerationAbscissaSelectorValues);
//    // management widget
//    connect(vGUI, &ViewGUI::managementTabViewed, _model, &Model::onManagementTabViewed);
//    connect(_model, &Model::managementExperimentCampaignsLoaded, vGUI, &ViewGUI::onManagementExperimentCampaignsLoaded);
//    connect(_model, &Model::managementExperimentCampaignLoaded, vGUI, &ViewGUI::onManagementExperimentCampaignLoaded);
//    connect(_model, &Model::managementMetricsLoaded, vGUI, &ViewGUI::onManagementMetricsLoaded);
//    connect(_model, &Model::managementModifyExperimentName, vGUI, &ViewGUI::managementModifyExperimentName);
//    connect(vGUI, &ViewGUI::managementExperimentCampaignSelected, _model, &Model::onManagementExperimentCampaignSelected);
//    connect(vGUI, &ViewGUI::managementExperimentsSelected, _model, &Model::onManagementExperimentsSelected);
//    connect(vGUI, &ViewGUI::managementUpdateCurrentValues, _model, &Model::onManagementUpdateCurrentValues);
//    // config file widget
//    connect(vGUI, &ViewGUI::configFileParsedFile, _model, &Model::parseConfigFile);
//    connect(vGUI, &ViewGUI::configFileSave, _model, &Model::saveConfigFile);
//    connect(_model, &Model::setConfigWidgetConfigFilePatterns, vGUI, &ViewGUI::setConfigFilePatterns);


//    /* events emitted only once (initialize application) */
//    connect(this, &Controller::initialiseEvent, _model, &Model::onFindSelectorValuesSet);
//    connect(this, &Controller::initialiseEvent, _model, &Model::onOrdinateSelectionValuesSet);
//    connect(this, &Controller::initialiseEvent, _model, &Model::onManagementMetricTableSetted);

//    /* initialize aapplication */
//    emit initialiseEvent();

//    /* disconnect single use events */
//    disconnect(this, &Controller::initialiseEvent, 0, 0);
}
