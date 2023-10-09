#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Armorial/Common/Enums/Enums.h>
#include <Armorial/Utils/ParameterHandler/ParameterHandler.h>

class Constants
{
public:
    /*!
     * \brief Load a json file in the given file path, extracting its components.
     * \param fileName The given file path.
     */
    static void loadFile(QString filePath);

    /// Network
    // Referee
    /*!
     * \return A QString object containing the referee network address.
     */
    static QString refereeNetworkAddress();

    /*!
     * \return A quint16 object containing the referee network port.
     */
    static quint16 refereeNetworkPort();

    /*!
     * \return A QString object containing the referee network interface.
     */
    static QString refereeNetworkInterface();

    static QString gameType();
    static float halfTime();
    static float betweenHalfTime();
    static int varsPerTeam();
    static int timeoutsPerTeam();
    static float timeoutLength();
    static float overtimeHalfTime();

    static QString blueTeamName();
    static QString yellowTeamName();
    static bool blueIsLeftSide();


private:
    static Utils::ParameterHandler _parameterHandler;
};

#endif // CONSTANTS_H
