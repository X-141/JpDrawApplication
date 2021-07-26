#ifndef DRAWLAYER_H
#define DRAWLAYER_H

#include <QPixmap>

class DrawLayer : public QPixmap {

//    Q_OBJECT

public:
    DrawLayer(QSize aSize, uint id);
    ~DrawLayer() = default;

    /**
     * @brief Set the layer to be displayed in the draw area.
     */
    void enable();

    /**
     * @brief Set the layer not to be displayed in the draw area.
     */
    void disable();

    uint getId() const;

    void setId(const uint aId);

    bool isEnabled() const;

    void setEnableStatus(bool status);

private:
    // Numerical id for the layer.
    uint mId;

    // Is the layer currently being shown?
    bool mEnabled;

};


#endif // DRAWLAYER_H
