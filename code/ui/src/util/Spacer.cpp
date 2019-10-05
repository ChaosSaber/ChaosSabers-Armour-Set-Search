#include "util/Spacer.hpp"

util::HorizontalSpacer::HorizontalSpacer(QWidget* parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

util::VerticalSpacer::VerticalSpacer(QWidget* parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
}
