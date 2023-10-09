#ifndef REFEREEGUI_H
#define REFEREEGUI_H

#include <QMainWindow>
#include <QPushButton>
#include <QSignalMapper>

#include <Armorial/Utils/Timer/Timer.h>

#include <proto/vssref_command.pb.h>

QT_BEGIN_NAMESPACE
namespace Ui { class RefereeGUI; }
QT_END_NAMESPACE

class RefereeGUI : public QMainWindow
{
    Q_OBJECT
public:
    RefereeGUI(QWidget *parent = nullptr);
    ~RefereeGUI();

    // Dark theme
    void setDarkTheme();

    // Animations
    void animateWidget(QWidget *widget, QColor desiredColor, int animationTime);

    // Mapping buttons
    QList<QPushButton*> _buttons;
    QSignalMapper *_buttonsMapper;

    // Teams
    void setupTeams();
    void setupGoals();
    void setupButtons();

    // Timeouts
    int _leftTeamTimeouts;
    int _rightTeamTimeouts;
    bool _timeoutSet;
    float _timeoutTimestamp;
    Utils::Timer _timeoutTimer;

    // VARs
    int _leftTeamVars;
    int _rightTeamVars;

    // Goals
    int _leftTeamGoals;
    int _rightTeamGoals;
    std::pair<VSSRef::Color, bool> _happenedGoal;

private:
    Ui::RefereeGUI *ui;

signals:
    void sendManualFoul(const VSSRef::Foul& foul, const VSSRef::Color& foulTeamColor = VSSRef::Color::NONE, const VSSRef::Quadrant& foulQuadrant = VSSRef::Quadrant::NO_QUADRANT);
    void addTime(int seconds);
    void sendScores(int leftTeamScore, int rightTeamScore);

public slots:
    void takeFoul(VSSRef::Foul foul, VSSRef::Color foulColor, VSSRef::Quadrant foulQuadrant);
    void takeTimeStamp(float halftime, float timestamp, VSSRef::Half half, bool isEndGame, bool isBetweenHalfs);
    void addGoal(VSSRef::Color color, bool setFlag = false);
    void removeGoal(VSSRef::Color color);
    void processButton(QWidget *button);
};
#endif // REFEREEGUI_H
