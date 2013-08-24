#pragma once

#include "image.h"
#include <QObject>

namespace Spotinetta {

class ImageWatcher : public QObject
{
    Q_OBJECT
public:
    explicit ImageWatcher(const Session * session, QObject * parent);
    ~ImageWatcher();

    Image watched() const;
    void watch(const Image &object);

signals:
    void loaded();

protected:
    void customEvent(QEvent *);

private:
    void subscribe();
    void unsubscribe();

    Image m_watched;
};

}
