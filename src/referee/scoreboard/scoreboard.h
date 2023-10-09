#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <QObject>
#include <QMutex>

#include <Armorial/Utils/Timer/Timer.h>

#include <proto/vssref_common.pb.h>

class Scoreboard : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Scoreboard class constructor.
     */
    Scoreboard();

    /*!
     * \brief Scoreboard
     */
    ~Scoreboard();

    /*!
     * \return The current game half.
     */
    VSSRef::Half getCurrentHalf();

    /*!
     * \return The current time stamp for the curent half.
     */
    float getCurrentTimestamp();

    /*!
     * \return The current time that is needed
     */
    float getCurrentHalfTime();

    /*!
     * \return True if the game was finished when the half transitioned and False otherwise.
     */
    bool isEndGame();

    bool isBetweenHalfs();

protected:
    friend class Referee;

    void updateTimestamp();
    void updateTimestampWithoutChanges();
    bool hasNextHalf();
    bool needsOvertime();

private:
    // Internal
    VSSRef::Half _currentHalf;
    Utils::Timer _halfTimer;
    float _halfTimeToCheck;
    float _remainingTime;
    bool _waitedBetweenHalfs;
    bool _noTimeHalf;
    bool _gameEnded;
    int _leftTeamScore;
    int _rightTeamScore;
    void processHalfTransition();

    QMutex _mutex;

signals:
    void interval();

public slots:
    void addTime(int seconds);
    void updateScores(int leftTeamScore, int rightTeamScore);
};

#endif // SCOREBOARD_H
