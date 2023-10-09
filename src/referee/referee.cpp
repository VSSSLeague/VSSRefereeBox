#include "referee.h"

#include <spdlog/spdlog.h>

Referee::Referee(QString refereeAddress, quint16 refereePort, QString networkInterface) : Base::UDP::Client(refereeAddress, refereePort, networkInterface) {
    _refereeAddress = refereeAddress;
    _refereePort = refereePort;
    _networkInterface = networkInterface;

    // Create Scoreboard
    _scoreboard = new Scoreboard();
}

Referee::~Referee() {

}

void Referee::initialization() {
    bool connected = connectToNetwork();
    if(connected) {
        spdlog::info("[{}] Connected to Referee at address '{}', port '{}' and interface '{}'.", clientName().toStdString(), _refereeAddress.toStdString(), _refereePort, _networkInterface.toStdString());
    }
    else {
        spdlog::warn("[{}] Could not connect to simulator at address '{}', port '{}' and interface '{}'.", clientName().toStdString(), _refereeAddress.toStdString(), _refereePort, _networkInterface.toStdString());
    }

    //
    QObject::connect(_scoreboard, &Scoreboard::interval, this, &Referee::processInterval);

    // Send halt as initial state
    sendCommandThroughNetwork(VSSRef::Foul::HALT, VSSRef::Color::NONE, VSSRef::Quadrant::NO_QUADRANT);
}

void Referee::loop() {
    /// TODO:
    /// Create checkers to analyze the whole field behaviors

    _mutex.lock();
    if(_scoreboard->isBetweenHalfs()) {
        _scoreboard->updateTimestamp();
    }
    else {
        if(_lastCommand != VSSRef::Foul::GAME_ON) {
            _scoreboard->updateTimestampWithoutChanges();
        }
        else {
            _scoreboard->updateTimestamp();
        }
    }
    _mutex.unlock();

    if(_scoreboard->isEndGame()) {
        emit sendCommandThroughNetwork(VSSRef::Foul::HALT, VSSRef::Color::NONE, VSSRef::Quadrant::NO_QUADRANT);
        this->stopEntity();
    }

    emit sendTimeStamp(_scoreboard->getCurrentHalfTime(), _scoreboard->getCurrentTimestamp(), _scoreboard->getCurrentHalf(), _scoreboard->isEndGame(), _scoreboard->isBetweenHalfs());
}

void Referee::finalization() {
    disconnectFromNetwork();
    spdlog::info("[{}] Disconnected from network.", clientName().toStdString());

    // Delete scoreboard here to maintain thread reference
    delete _scoreboard;
}

VSSRef::ref_to_team::VSSRef_Command Referee::mountNetworkCommandPacket(const VSSRef::Foul& foul, const VSSRef::Color& foulTeamColor, const VSSRef::Quadrant& foulQuadrant) {
    VSSRef::ref_to_team::VSSRef_Command command;

    command.set_foul(foul);
    command.set_foulquadrant(foulQuadrant);
    command.set_teamcolor(foulTeamColor);
    command.set_timestamp(_scoreboard->getCurrentTimestamp());
    command.set_gamehalf(_scoreboard->getCurrentHalf());

    return command;
}

void Referee::sendCommandThroughNetwork(const VSSRef::Foul& foul, const VSSRef::Color& foulTeamColor, const VSSRef::Quadrant& foulQuadrant) {
    // Generate and serialize packet to send through network
    std::string serializedCommand;
    mountNetworkCommandPacket(foul, foulTeamColor, foulQuadrant).SerializeToString(&serializedCommand);

    QNetworkDatagram datagram;
    datagram.setData(QByteArray(serializedCommand.c_str()));

    if(!sendDatagram(datagram)) {
        spdlog::error("[{}] Failed to write to socket.", clientName().toStdString());
    }

    _mutex.lock();
    _lastCommand = foul;
    emit sendFoul(foul, foulTeamColor, foulQuadrant);
    _mutex.unlock();

    spdlog::info("[{}] Send command '{}' for team '{}' at quadrant '{}'.", clientName().toStdString(), VSSRef::Foul_Name(foul), VSSRef::Color_Name(foulTeamColor), VSSRef::Quadrant_Name(foulQuadrant));
}

void Referee::processInterval() {
    sendCommandThroughNetwork(VSSRef::HALT, VSSRef::NONE, VSSRef::NO_QUADRANT);
}
