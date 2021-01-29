#include "DrawLayer.hpp"

DrawLayer::DrawLayer(QSize aSize, uint aId)
    : QPixmap(aSize),
    mId(aId), mEnabled(false)
{}

void
DrawLayer::enable() { mEnabled = true; }

void
DrawLayer::disable() { mEnabled = false; }

uint
DrawLayer::getId() const { return mId; }

void
DrawLayer::setId(const uint aId) { mId = aId; }

bool
DrawLayer::isEnabled() const { return mEnabled; }

void
DrawLayer::setEnableStatus(bool status) { mEnabled = status; }
