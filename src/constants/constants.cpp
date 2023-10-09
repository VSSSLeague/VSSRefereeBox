#include "constants.h"
#include "constants.h"

#include <QNetworkInterface>
#include <QRegularExpression>

Utils::ParameterHandler Constants::_parameterHandler = Utils::ParameterHandler();

void Constants::loadFile(QString filePath) {
    // Load constants file
    bool couldLoad = _parameterHandler.loadFile(filePath);

    // Check if could load
    if(couldLoad) {
        spdlog::info("[Constants] Readed constants file in path '{}'.", filePath.toStdString());
    }
    else {
        spdlog::error("[Constants] Failed to read constants file in path '{}'.", filePath.toStdString());
        exit(-1);
    }
}

QString Constants::refereeNetworkAddress() {
    QString address = _parameterHandler["Network"].getAsMap()["Referee"].getAsMap()["refereeNetworkAddress"].toString();
    bool matchNetworkAddress = QRegularExpression("[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}").match(address).hasMatch();

    if(!matchNetworkAddress) {
        spdlog::error("[Constants] Failed to read a valid address in '{}'.", __FUNCTION__);
        exit(-1);
    }

    return address;
}

quint16 Constants::refereeNetworkPort() {
    bool converted;
    quint16 port = _parameterHandler["Network"].getAsMap()["Referee"].getAsMap()["refereeNetworkPort"].toInt(&converted);

    if(!converted) {
        spdlog::error("[Constants] Failed to read a valid integer in '{}'.", __FUNCTION__);
        exit(-1);
    }

    return port;
}

QString Constants::refereeNetworkInterface() {
    QString interface = _parameterHandler["Network"].getAsMap()["Referee"].getAsMap()["refereeNetworkInterface"].toString();
    bool networkInterfaceExists = QNetworkInterface::interfaceFromName(interface).isValid();

    if(!networkInterfaceExists) {
        spdlog::error("[Constants] Failed to read a valid network interface in '{}'.", __FUNCTION__);
    }

    return interface;
}

QString Constants::gameType() {
    QString gameType = _parameterHandler["Referee"].getAsMap()["Game"].getAsMap()["gameType"].toString();

    if(!(gameType.toLower().contains("group") || gameType.toLower().contains("double elimination"))) {
        spdlog::error("[Constants] Failed to read a valid network interface in '{}'.", __FUNCTION__);
    }

    return gameType;
}

float Constants::halfTime() {
    QString halfTime = _parameterHandler["Referee"].getAsMap()["Game"].getAsMap()["halfTime"].toString();
    bool converted = false;
    float halfTimeFloat = halfTime.toFloat(&converted);

    if(!converted) {
        spdlog::error("[Constants] Failed to read a valid half time in '{}'.", __FUNCTION__);
    }

    return halfTimeFloat;
}

float Constants::betweenHalfTime() {
    QString betweenHalfTime = _parameterHandler["Referee"].getAsMap()["Game"].getAsMap()["betweenHalfTime"].toString();
    bool converted = false;
    float betweenHalfTimeFloat = betweenHalfTime.toFloat(&converted);

    if(!converted) {
        spdlog::error("[Constants] Failed to read a valid between half time in '{}'.", __FUNCTION__);
    }

    return betweenHalfTimeFloat;
}

int Constants::varsPerTeam() {
    QString varsPerTeam = _parameterHandler["Referee"].getAsMap()["Game"].getAsMap()["betweenHalfTime"].toString();
    bool converted = false;
    int varsPerTeamInt = varsPerTeam.toInt(&converted);

    if(!converted) {
        spdlog::error("[Constants] Failed to read a valid vars per team in '{}'.", __FUNCTION__);
    }

    return varsPerTeamInt;
}

int Constants::timeoutsPerTeam() {
    QString timeoutsPerTeam = _parameterHandler["Referee"].getAsMap()["Game"].getAsMap()["timeoutsPerTeam"].toString();
    bool converted = false;
    int timeoutsPerTeamInt = timeoutsPerTeam.toInt(&converted);

    if(!converted) {
        spdlog::error("[Constants] Failed to read a valid timeouts per team in '{}'.", __FUNCTION__);
    }

    return timeoutsPerTeamInt;
}

float Constants::timeoutLength() {
    QString timeoutLength = _parameterHandler["Referee"].getAsMap()["Game"].getAsMap()["timeoutLength"].toString();
    bool converted = false;
    float timeoutLengthFloat = timeoutLength.toFloat(&converted);

    if(!converted) {
        spdlog::error("[Constants] Failed to read a valid timeout length in '{}'.", __FUNCTION__);
    }

    return timeoutLengthFloat;
}

float Constants::overtimeHalfTime() {
    QString overtimeHalfTime = _parameterHandler["Referee"].getAsMap()["Game"].getAsMap()["overtimeHalfTime"].toString();
    bool converted = false;
    float overtimeHalfTimeFloat = overtimeHalfTime.toFloat(&converted);

    if(!converted) {
        spdlog::error("[Constants] Failed to read a valid overtime half time in '{}'.", __FUNCTION__);
    }

    return overtimeHalfTimeFloat;
}

QString Constants::blueTeamName() {
    QString blueTeamName = _parameterHandler["Teams"].getAsMap()["blueTeamName"].toString();

    return blueTeamName;
}

QString Constants::yellowTeamName() {
    QString yellowTeamName = _parameterHandler["Teams"].getAsMap()["yellowTeamName"].toString();

    return yellowTeamName;
}

bool Constants::blueIsLeftSide() {
    bool blueIsLeftSide = _parameterHandler["Teams"].getAsMap()["timeoutLength"].toBool();

    return blueIsLeftSide;
}
