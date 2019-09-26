#ifndef DAUNTLESS_ASS_UI_ARMOURSETVIEW_HPP
#define DAUNTLESS_ASS_UI_ARMOURSETVIEW_HPP

#include "Dictionary.hpp"
#include "Options.hpp"
#include "gear/ArmourSet.hpp"
#include "gear/Armoury.hpp"
#include <QLabel>
#include <QWidget>

namespace Ui
{
class ArmourSetView;
class ElidingLabel : public QLabel
{
  public:
    QSize sizeHint() const override;
};
} // namespace Ui

class ArmourSetView : public QWidget
{
    Q_OBJECT

  public:
    explicit ArmourSetView(const Dictionary& dict, Options& options,
                           const Gear::ArmourSet& set, const Gear::Armoury& armoury,
                           int scrollBarWidth, Gear::SkillList& wantedSkills, QWidget* parent = 0);
    ~ArmourSetView();

    int getGearViewWidth() const;
    void setGearViewWidth(int width);

    int getCellViewWidth() const;
    void setCellViewWidth(int width);

    void resizeEvent(QResizeEvent* event) override;
    void showContextMenu(const QPoint& pos);

  private:
    void setElidedSkill(QLabel* label, const QString& text);
    void addGear(const Gear::Gear& gear);
    void addCell(const std::pair<Gear::Cell, int>& cell);
    void addSkill(const Gear::Skill& skill);
    void exportTextToClipBoard(const std::string& text) const;
    void exportTextToFile(const std::string& text) const;

    const Dictionary& dict;
    Options& options;
    Ui::ArmourSetView* ui;
    std::unordered_map<QLabel*, QString> uniqueSkillLabels;
    int scrollBarWidth;
    bool constructing = true;
    const Gear::ArmourSet& armourSet;
    const Gear::Armoury& armoury;
};

#endif // !DAUNTLESS_ASS_UI_ARMOURSETVIEW_HPP