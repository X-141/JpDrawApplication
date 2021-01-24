#ifndef DRAWLAYER_H
#define DRAWLAYER_H

#include <QPixmap>

class DrawLayer : public QPixmap {

//    Q_OBJECT

public:
    DrawLayer(QSize aSize, uint id);
    ~DrawLayer() = default;

    /**
     * @brief enable
     * Set the layer to be displayed in the draw area.
     */
    void enable();

    /**
     * @brief disable
     * Set the layer not to be displayed in the draw area.
     */
    void disable();

    uint getId() const;

    void setId(const uint aId);

    bool isEnabled() const;

    void setEnableStatus(bool status);

private:
    uint mId;
    // Possibly implement string id
    // but not necessary for internal representation.

    // Is the layer currently being shown?
    bool mEnabled;

    // In the future have some sort of way to know
    // what the "core" pixels are for removing pixels
    // based on mouse position.

};


#endif // DRAWLAYER_H
