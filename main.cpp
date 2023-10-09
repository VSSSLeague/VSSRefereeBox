#include <QApplication>

#include <Armorial/Threaded/EntityManager/EntityManager.h>
#include <Armorial/Utils/ExitHandler/ExitHandler.h>

#include <src/constants/constants.h>
#include <src/referee/referee.h>
#include <src/gui/refereegui.h>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    Constants::loadFile(PROJECT_PATH + QString("/src/constants/constants.json"));
    // Create EntityManager
    Threaded::EntityManager* entityManager = new Threaded::EntityManager();

    // Register Referee metatypes
    qRegisterMetaType<VSSRef::Color>("VSSRef::Color");
    qRegisterMetaType<VSSRef::Foul>("VSSRef::Foul");
    qRegisterMetaType<VSSRef::Quadrant>("VSSRef::Quadrant");
    qRegisterMetaType<VSSRef::Half>("VSSRef::Half");

    // Create and show GUI
    RefereeGUI* refereeGUI = new RefereeGUI();
    refereeGUI->show();

    // Create Referee and register as Entity instance
    Referee* referee = new Referee("224.5.23.2", 10003);
    entityManager->addEntity(referee);

    // Setup ExitHandler
    Utils::ExitHandler::setApplication(&a);
    Utils::ExitHandler::setup();

    // Start entities
    entityManager->startEntities();

    // Make connections
    QObject::connect(refereeGUI, &RefereeGUI::sendManualFoul, referee, &Referee::sendCommandThroughNetwork);
    QObject::connect(referee, &Referee::sendFoul, refereeGUI, &RefereeGUI::takeFoul);
    QObject::connect(referee, &Referee::sendTimeStamp, refereeGUI, &RefereeGUI::takeTimeStamp);
    QObject::connect(refereeGUI, &RefereeGUI::addTime, referee->scoreboard(), &Scoreboard::addTime);
    QObject::connect(refereeGUI, &RefereeGUI::sendScores, referee->scoreboard(), &Scoreboard::updateScores);

    // Wait for closure
    bool ret = a.exec();

    // Stop entities and finish
    entityManager->disableEntities();
    delete entityManager;

    return ret;
}
