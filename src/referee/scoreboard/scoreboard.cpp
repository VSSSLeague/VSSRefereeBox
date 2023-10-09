#include "scoreboard.h"

#include <src/constants/constants.h>

Scoreboard::Scoreboard() {
    _currentHalf = VSSRef::Half::FIRST_HALF;
    _halfTimer.start();
    _halfTimeToCheck = Constants::halfTime();
    _remainingTime = _halfTimeToCheck;

    _waitedBetweenHalfs = false;
    _noTimeHalf = false;
    _gameEnded = false;
    _leftTeamScore = 0;
    _rightTeamScore = 0;
}

Scoreboard::~Scoreboard() {

}

VSSRef::Half Scoreboard::getCurrentHalf() {
    return _currentHalf;
}

float Scoreboard::getCurrentTimestamp() {
    _mutex.lock();
    float remainingTime = _remainingTime;
    _mutex.unlock();

    return _halfTimeToCheck - remainingTime;
}


float Scoreboard::getCurrentHalfTime() {
    return _halfTimeToCheck;
}

bool Scoreboard::isEndGame() {
    return _gameEnded;
}

bool Scoreboard::isBetweenHalfs() {
    return _waitedBetweenHalfs;
}

void Scoreboard::updateTimestampWithoutChanges() {
    _halfTimer.start();
}

bool Scoreboard::hasNextHalf() {
    if(_currentHalf == VSSRef::Half::SECOND_HALF) {
        // If game is not of double elimination type
        if(!Constants::gameType().toLower().contains("double elimination")) {
            return false;
        }
        else {
            return needsOvertime();
        }
    }

    if(_currentHalf == VSSRef::Half::OVERTIME_SECOND_HALF) {
        return needsOvertime();
    }

    return true;
}

bool Scoreboard::needsOvertime() {
    _mutex.lock();
    bool needsOvertime = (_leftTeamScore == _rightTeamScore);
    _mutex.unlock();

    return needsOvertime;
}

void Scoreboard::updateTimestamp() {
    if(_noTimeHalf) return ;

    _mutex.lock();
    _remainingTime -= _halfTimer.getSeconds();
    _mutex.unlock();
    _halfTimer.start();

    _mutex.lock();
    float remainingTime = _remainingTime;
    _mutex.unlock();

    if(remainingTime < 0.0f) {
        if(_waitedBetweenHalfs) {
            processHalfTransition();
            _waitedBetweenHalfs = false;
        }
        else {
            if(!hasNextHalf()) {
                processHalfTransition();
                _waitedBetweenHalfs = true;
            }
            else {
                _halfTimeToCheck = Constants::betweenHalfTime();
                _mutex.lock();
                _remainingTime = _halfTimeToCheck;
                _mutex.unlock();
                _waitedBetweenHalfs = true;
                emit interval();
            }
        }
    }
}

void Scoreboard::processHalfTransition() {
    if(_currentHalf == VSSRef::Half::SECOND_HALF) {
        // If game is not of double elimination type
        if(!Constants::gameType().toLower().contains("double elimination")) {
            _gameEnded = true;
            return ;
        }

        if(!needsOvertime()) {
            _gameEnded = true;
            return ;
        }
    }

    if(_currentHalf == VSSRef::Half::OVERTIME_SECOND_HALF) {
        if(!needsOvertime()) {
            _gameEnded = true;
            return ;
        }
    }

    _currentHalf = VSSRef::Half(_currentHalf + 1);

    // If new half is 'normal'
    if(_currentHalf == VSSRef::FIRST_HALF || _currentHalf == VSSRef::SECOND_HALF) {
        _mutex.lock();
        _remainingTime = Constants::halfTime();
        _mutex.unlock();
    }
    else if(_currentHalf == VSSRef::OVERTIME_FIRST_HALF || _currentHalf == VSSRef::OVERTIME_SECOND_HALF) {
        _mutex.lock();
        _remainingTime = Constants::overtimeHalfTime();
        _mutex.unlock();
    }
    else {
        // SHOOTOUT / NO_HALF
        _noTimeHalf = true;
    }
}

void Scoreboard::addTime(int seconds) {
    _mutex.lock();
    _remainingTime = std::min(_halfTimeToCheck, _remainingTime + seconds);
    _mutex.unlock();
}

void Scoreboard::updateScores(int leftTeamScore, int rightTeamScore) {
    _mutex.lock();
    _leftTeamScore = leftTeamScore;
    _rightTeamScore = rightTeamScore;
    _mutex.unlock();
}
