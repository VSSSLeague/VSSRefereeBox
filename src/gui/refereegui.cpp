#include "refereegui.h"
#include "ui_refereegui.h"

#include <QStyleFactory>
#include <QVariantAnimation>

#include <Armorial/Libs/nameof/include/nameof.hpp>

#include <src/constants/constants.h>

#include <spdlog/spdlog.h>


RefereeGUI::RefereeGUI(QWidget *parent) : QMainWindow(parent), ui(new Ui::RefereeGUI) {
    ui->setupUi(this);
    setupTeams();
    setupGoals();
    setupButtons();
    setDarkTheme();
}

RefereeGUI::~RefereeGUI() {
    delete ui;
}

void RefereeGUI::setDarkTheme() {
    this->setStyle(QStyleFactory::create("Fusion"));

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window,QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText,Qt::white);
    darkPalette.setColor(QPalette::Disabled,QPalette::WindowText,QColor(127,127,127));
    darkPalette.setColor(QPalette::Base,QColor(42,42,42));
    darkPalette.setColor(QPalette::AlternateBase,QColor(66,66,66));
    darkPalette.setColor(QPalette::ToolTipBase,Qt::white);
    darkPalette.setColor(QPalette::ToolTipText,Qt::white);
    darkPalette.setColor(QPalette::Text,Qt::white);
    darkPalette.setColor(QPalette::Disabled,QPalette::Text,QColor(127,127,127));
    darkPalette.setColor(QPalette::Dark,QColor(35,35,35));
    darkPalette.setColor(QPalette::Shadow,QColor(20,20,20));
    darkPalette.setColor(QPalette::Button,QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText,Qt::white);
    darkPalette.setColor(QPalette::Disabled,QPalette::ButtonText,QColor(127,127,127));
    darkPalette.setColor(QPalette::BrightText,Qt::red);
    darkPalette.setColor(QPalette::Link,QColor(42,130,218));
    darkPalette.setColor(QPalette::Highlight,QColor(42,130,218));
    darkPalette.setColor(QPalette::Disabled,QPalette::Highlight,QColor(80,80,80));
    darkPalette.setColor(QPalette::HighlightedText,Qt::white);
    darkPalette.setColor(QPalette::Disabled,QPalette::HighlightedText,QColor(127,127,127));

    this->setPalette(darkPalette);
}

void RefereeGUI::setupTeams() {
    QString leftName, rightName;

    leftName = Constants::blueIsLeftSide() ? Constants::blueTeamName() : Constants::yellowTeamName();
    rightName = Constants::blueIsLeftSide() ? Constants::yellowTeamName() : Constants::blueTeamName();

    // Set names
    ui->leftTeamName->setText(leftName);
    ui->leftTeamName->setStyleSheet(Constants::blueIsLeftSide() ? "color: #779FFF;" : "color: #FCEE44;");
    ui->rightTeamName->setText(rightName);
    ui->rightTeamName->setStyleSheet(Constants::blueIsLeftSide() ? "color: #FCEE44;" : "color: #779FFF;");

    // Remove spaces to take logo
    leftName.remove(" ");
    rightName.remove(" ");

    // Setup left logo
    QPixmap pixMap = QPixmap(":/teams/" + leftName.toLower() + ".png");
    if(!pixMap.isNull()) {
        ui->leftTeamLogo->setPixmap(pixMap);
    }
    else {
        spdlog::warn("[{}] Failed to setup left team logo, set as default.", NAMEOF_TYPE_RTTI(*this).data());
        ui->leftTeamLogo->setPixmap((Constants::blueIsLeftSide()) ? QPixmap(":/teams/defaultblue.png") : QPixmap(":/teams/defaultyellow.png"));
    }

    // Setup right logo
    pixMap = QPixmap(":/teams/" + rightName.toLower() + ".png");
    if(!pixMap.isNull()) {
        ui->rightTeamLogo->setPixmap(pixMap);
    }
    else {
        spdlog::warn("[{}] Failed to setup right team logo, set as default.", NAMEOF_TYPE_RTTI(*this).data());
        ui->rightTeamLogo->setPixmap((Constants::blueIsLeftSide()) ? QPixmap(":/teams/defaultyellow.png") : QPixmap(":/teams/defaultblue.png"));
    }

    // Set initial goals and call setupGoals
    _leftTeamGoals = 0;
    _rightTeamGoals = 0;
    setupGoals();

    // Set initial timeouts and vars per team
    _leftTeamVars = _rightTeamVars = Constants::varsPerTeam();
    _leftTeamTimeouts = _rightTeamTimeouts = Constants::timeoutsPerTeam();

    // Setup timeout
    _timeoutSet = false;
}

void RefereeGUI::setupGoals() {
    // Setup goals
    char leftGoal[5], rightGoal[5];
    sprintf(leftGoal, "%02d", _leftTeamGoals);
    sprintf(rightGoal, "%02d", _rightTeamGoals);

    ui->leftTeamGoals->setText(QString("%1").arg(leftGoal));
    ui->rightTeamGoals->setText(QString("%1").arg(rightGoal));
}

void RefereeGUI::setupButtons() {
    // Setup buttons mapper
    _buttonsMapper = new QSignalMapper();

    // Adding buttons to list
    _buttons.push_back(ui->goalKick);
    _buttons.push_back(ui->freeball);
    _buttons.push_back(ui->penaltyKick);
    _buttons.push_back(ui->kickoff);
    _buttons.push_back(ui->startGame);
    _buttons.push_back(ui->stopGame);
    _buttons.push_back(ui->haltGame);

    ui->sendToDiscord->setVisible(false);

    ui->scoreboard->setTitle(Constants::gameType());

    // Connecting game control buttons
    connect(ui->startGame, &QPushButton::released, [this](){
        ui->startGame->setEnabled(false);
        ui->stopGame->setEnabled(true);
        ui->haltGame->setEnabled(true);
    });

    connect(ui->stopGame, &QPushButton::released, [this](){
        ui->stopGame->setEnabled(false);
        ui->startGame->setEnabled(true);
        ui->haltGame->setEnabled(true);
    });

    connect(ui->haltGame, &QPushButton::released, [this](){
        ui->startGame->setEnabled(false);
        ui->stopGame->setEnabled(true);
        ui->haltGame->setEnabled(false);
    });

    // Connecting time buttons to lambda
    connect(ui->addMinute, &QPushButton::released, [this](){
        emit addTime(60);
    });

    connect(ui->addSecond, &QPushButton::released, [this](){
        emit addTime(1);
    });

    // Connecting goal buttons to lambda
    connect(ui->addGoal, &QPushButton::released, [this](){
        VSSRef::Color calledColor = VSSRef::Color();
        VSSRef::Color_Parse(ui->forButtons->checkedButton()->whatsThis().toUpper().toStdString(), &calledColor);
        addGoal(calledColor);
        emit sendScores(_leftTeamGoals, _rightTeamGoals);
    });

    connect(ui->rmvGoal, &QPushButton::released, [this](){
        VSSRef::Color calledColor = VSSRef::Color();
        VSSRef::Color_Parse(ui->forButtons->checkedButton()->whatsThis().toUpper().toStdString(), &calledColor);
        removeGoal(calledColor);
        emit sendScores(_leftTeamGoals, _rightTeamGoals);
    });

    // Connect foul buttons to signal mapper
    for(int i = 0; i < _buttons.size(); i++) {
        QPushButton *button = _buttons.at(i);

        connect(button, SIGNAL(released()), _buttonsMapper, SLOT(map()), Qt::UniqueConnection);
        _buttonsMapper->setMapping(button, button);
        connect(_buttonsMapper, SIGNAL(mapped(QWidget *)), this, SLOT(processButton(QWidget *)), Qt::UniqueConnection);
    }

    QObject::connect(ui->var, &QPushButton::released, [this]() {
        emit sendManualFoul(VSSRef::Foul::STOP, VSSRef::Color::NONE, VSSRef::Quadrant::NO_QUADRANT);
        bool hasTeamBlue = ui->forBlue->isChecked();

        QString forBlue = QString("<font color=\"#0000CD\">%1</font>").arg(Constants::blueTeamName());
        QString forYellow = QString("<font color=\"#FCEE44\">%1</font>").arg(Constants::yellowTeamName());

        ui->statusColor->setText(QString("Game is stopped due to VAR for %2.").arg(hasTeamBlue ? forBlue : forYellow));
    });

    QObject::connect(ui->timeout, &QPushButton::released, [this]() {
        _timeoutSet = true;
        emit sendManualFoul(VSSRef::Foul::HALT, VSSRef::Color::NONE, VSSRef::Quadrant::NO_QUADRANT);
        bool hasTeamBlue = ui->forBlue->isChecked();
        QString forBlue = QString("<font color=\"#0000CD\">%1</font>").arg(Constants::blueTeamName());
        QString forYellow = QString("<font color=\"#FCEE44\">%1</font>").arg(Constants::yellowTeamName());

        ui->statusColor->setText(QString("Game is halted due to TIMEOUT for %2.").arg(hasTeamBlue ? forBlue : forYellow));
        _timeoutTimestamp = 0.0f;
        _timeoutTimer.start();
    });
}

void RefereeGUI::takeTimeStamp(float halftime, float timestamp, VSSRef::Half half, bool isEndGame, bool isBetweenHalfs) {
    // If is penalty shootouts, set infinite time
    if(half == VSSRef::Half::PENALTY_SHOOTOUTS) {
        ui->halfTime->setText(QString(VSSRef::Half_Name(half).c_str()));
        ui->gameTime->setText("∞");
        return ;
    }

    // If is end game
    if(isEndGame) {
        ui->halfTime->setText(QString("Game ended"));
        ui->gameTime->setText("END");
        ui->manualReferee->setEnabled(false);
        ui->sendToDiscord->setVisible(true);
        return ;
    }

    // Check if timeout is set
    if(_timeoutSet) {
        halftime = Constants::timeoutLength();
        timestamp = _timeoutTimestamp;

        _timeoutTimestamp += _timeoutTimer.getSeconds();
        _timeoutTimer.start();

        if(_timeoutTimestamp >= halftime) {
            _timeoutTimestamp = halftime;

            // Disable timeout
            _timeoutSet = false;
            emit sendManualFoul(VSSRef::Foul::HALT, VSSRef::Color::NONE, VSSRef::NO_QUADRANT);
        }
    }

    int min = (halftime - timestamp) / 60.0;
    int sec = (halftime - timestamp) - (min * 60.0);

    // Seting gameTime
    char minStr[5], secStr[5];
    sprintf(minStr, "%02d", min);
    sprintf(secStr, "%02d", sec);

    if(isBetweenHalfs) {
        ui->halfTime->setText("Interval");
        ui->gameTime->setText(QString("%1:%2").arg(minStr).arg(secStr));
        return ;
    }

    // Setting gameHalf
    ui->gameTime->setText(QString("%1:%2").arg(minStr).arg(secStr));
    ui->halfTime->setText(QString(VSSRef::Half_Name(half).c_str()));
}

void RefereeGUI::addGoal(VSSRef::Color color, bool setFlag) {
    if(color == VSSRef::Color::BLUE) {
        if(Constants::blueIsLeftSide()) {
            _leftTeamGoals++;
        }
        else{
            _rightTeamGoals++;
        }
    }
    else if(color == VSSRef::Color::YELLOW) {
        if(!Constants::blueIsLeftSide()) {
            _leftTeamGoals++;
        }
        else{
            _rightTeamGoals++;
        }
    }

    if(setFlag) _happenedGoal = std::make_pair(color, true);

    setupGoals();
}

void RefereeGUI::removeGoal(VSSRef::Color color) {
    if(color == VSSRef::Color::BLUE) {
        if(Constants::blueIsLeftSide()) {
            _leftTeamGoals = std::max(0, _leftTeamGoals - 1);
        }
        else{
            _rightTeamGoals = std::max(0, _rightTeamGoals - 1);
        }
    }
    else if(color == VSSRef::Color::YELLOW) {
        if(!Constants::blueIsLeftSide()) {
            _leftTeamGoals = std::max(0, _leftTeamGoals - 1);
        }
        else{
            _rightTeamGoals = std::max(0, _rightTeamGoals - 1);
        }
    }

    setupGoals();
}

void RefereeGUI::processButton(QWidget *button) {
    QPushButton *castedButton = static_cast<QPushButton*>(button);

    // Parsing foul
    VSSRef::Foul calledFoul = VSSRef::Foul();
    VSSRef::Foul_Parse(castedButton->whatsThis().toUpper().toStdString(), &calledFoul);

    // Parsing color
    VSSRef::Color calledColor = VSSRef::Color();
    VSSRef::Color_Parse(ui->forButtons->checkedButton()->whatsThis().toUpper().toStdString(), &calledColor);

    // Parsing quadrant
    VSSRef::Quadrant calledQuadrant = VSSRef::Quadrant();
    VSSRef::Quadrant_Parse(ui->quadrantButtons->checkedButton()->whatsThis().toUpper().toStdString(), &calledQuadrant);

    // Updating game state buttons
    ui->startGame->setEnabled(false);
    ui->stopGame->setEnabled(true);
    ui->haltGame->setEnabled(true);

    // Emit manual foul
    emit sendManualFoul(calledFoul, (calledFoul == VSSRef::Foul::FREE_BALL) ? VSSRef::Color::NONE : calledColor, (calledFoul != VSSRef::Foul::FREE_BALL) ? VSSRef::Quadrant::NO_QUADRANT : calledQuadrant);
}

void RefereeGUI::animateWidget(QWidget *widget, QColor desiredColor, int animationTime) {
    // Animate widget
    QVariantAnimation *statusAnimation = new QVariantAnimation(this);
    statusAnimation->setStartValue(widget->palette().background().color());
    statusAnimation->setEndValue(desiredColor);
    statusAnimation->setDuration(animationTime);

    connect(statusAnimation, &QVariantAnimation::valueChanged, [widget](const QVariant &value){
        QColor colorValue = QColor(value.value<QColor>());
        QString r = QString(std::to_string(colorValue.red()).c_str());
        QString g = QString(std::to_string(colorValue.green()).c_str());
        QString b = QString(std::to_string(colorValue.blue()).c_str());
        QString a = QString(std::to_string(colorValue.alpha()).c_str());
        QString rgb = QString("rgb(%1, %2, %3)").arg(r).arg(g).arg(b);
        widget->setStyleSheet("border-radius: 10px; background-color: " + rgb + "; color: white;");
    });

    statusAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void RefereeGUI::takeFoul(VSSRef::Foul foul, VSSRef::Color foulColor, VSSRef::Quadrant foulQuadrant) {
    // Take color to animate statusBoard and set its text
    QColor desiredColor;
    if(foul == VSSRef::Foul::GAME_ON) {
        desiredColor = QColor(53, 53, 53, 255);
        ui->statusColor->setText("Game is Running");

        // Enabling associated buttons
        ui->startGame->setEnabled(false);
        ui->stopGame->setEnabled(true);
        ui->haltGame->setEnabled(true);
    }
    else if(foul == VSSRef::Foul::STOP) {
        desiredColor = QColor(255, 112, 0, 255);
        ui->statusColor->setText("Game is Stopped");

        // Enabling associated buttons
        ui->startGame->setEnabled(true);
        ui->stopGame->setEnabled(false);
        ui->haltGame->setEnabled(true);
    }
    else if(foul == VSSRef::Foul::HALT) {
        desiredColor = QColor(238, 0, 34, 255);
        ui->statusColor->setText("Game is Halted");

        // Enabling associated buttons
        ui->startGame->setEnabled(false);
        ui->stopGame->setEnabled(true);
        ui->haltGame->setEnabled(false);
    }
    else {
        desiredColor = QColor(238, 0, 34, 255);

        if(foul == VSSRef::Foul::KICKOFF && _happenedGoal.second) {
            QString forBlue = QString("<font color=\"#0000CD\">%1</font>").arg(Constants::blueTeamName());
            QString forYellow = QString("<font color=\"#FCEE44\">%1</font>").arg(Constants::yellowTeamName());

            ui->statusColor->setText(QString("GOAL for %2").arg(foulColor == VSSRef::Color::BLUE ? forYellow : forBlue));

            _happenedGoal.second = false;
        }
        else {
            if(foulColor != VSSRef::Color::NONE) {
                QString forBlue = QString("<font color=\"#0000CD\">%1</font>").arg(Constants::blueTeamName());
                QString forYellow = QString("<font color=\"#FCEE44\">%1</font>").arg(Constants::yellowTeamName());

                ui->statusColor->setText(QString("%1 for %2").arg(VSSRef::Foul_Name(foul).c_str()).arg(foulColor == VSSRef::Color::BLUE ? forBlue : forYellow));
            }
            else {
                ui->statusColor->setText(QString("%1 occurred at %2").arg(VSSRef::Foul_Name(foul).c_str()).arg(VSSRef::Quadrant_Name(foulQuadrant).c_str()));
            }
        }
    }

    // Animate statusboard
    animateWidget(ui->statusColor, desiredColor, 500);

    // Animate timer
    // Taking desired color for timer
    if(foul == VSSRef::Foul::GAME_ON){
        desiredColor = QColor(48, 74, 48, 255);
    }
    else {
        desiredColor = QColor(238, 0, 34, 255);
    }

    animateWidget(ui->gameTime, desiredColor, 200);
}
