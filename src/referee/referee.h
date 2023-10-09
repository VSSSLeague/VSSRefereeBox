#ifndef REFEREE_H
#define REFEREE_H

#include <Armorial/Base/Client/Client.h>
#include <Armorial/Threaded/Entity/Entity.h>

#include <proto/vssref_command.pb.h>

#include <src/referee/scoreboard/scoreboard.h>

class Referee : public Threaded::Entity, public Base::UDP::Client
{
    Q_OBJECT
public:
    /*!
     * \brief Referee class constructor.
     * \param refereeAddress, refereePort, networkInterface Referee network parameters.
     */
    Referee(QString refereeAddress, quint16 refereePort, QString networkInterface = "lo");

    /*!
     * \brief Referee class destructor.
     */
    ~Referee();

    Scoreboard* scoreboard() { return _scoreboard; }

protected:
    VSSRef::ref_to_team::VSSRef_Command mountNetworkCommandPacket(const VSSRef::Foul& foul, const VSSRef::Color& foulTeamColor, const VSSRef::Quadrant& foulQuadrant);

private:
    /*!
     * \brief Entity inheritated methods.
     */
    void initialization();
    void loop();
    void finalization();

    // Network internal
    QString _refereeAddress;
    quint16 _refereePort;
    QString _networkInterface;

    // Latest command
    VSSRef::Foul _lastCommand;

    // Internal
    QMutex _mutex;

    // Scoreboard
    Scoreboard *_scoreboard;

signals:
    void sendTimeStamp(float halftime, float timestamp, VSSRef::Half half, bool isEndGame, bool isBetweenHalfs);
    void sendFoul(const VSSRef::Foul& foul, const VSSRef::Color& foulTeamColor = VSSRef::Color::NONE, const VSSRef::Quadrant& foulQuadrant = VSSRef::Quadrant::NO_QUADRANT);

public slots:
    void sendCommandThroughNetwork(const VSSRef::Foul& foul, const VSSRef::Color& foulTeamColor = VSSRef::Color::NONE, const VSSRef::Quadrant& foulQuadrant = VSSRef::Quadrant::NO_QUADRANT);
    void processInterval();
};

#endif // REFEREE_H
