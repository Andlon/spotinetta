#include <Spotinetta/imagewatcher.h>
#include <Spotinetta/session.h>

#include <QCoreApplication>

namespace Spotinetta {

namespace {
void SP_CALLCONV handleImageLoaded(sp_image * image, void * userdata);
}

ImageWatcher::ImageWatcher(const Session *session, QObject *parent)
    :   QObject(parent)
{
    connect(session, &Session::released, this, &ImageWatcher::onReleased);
}

ImageWatcher::~ImageWatcher()
{
    unsubscribe();
}

void ImageWatcher::onReleased()
{
    // Watching an invalid image clears any added callbacks
    // (which is essential before a session is released, thus being placed here
    watch(Image());
}

Image ImageWatcher::watched() const
{
    return m_watched;
}

void ImageWatcher::watch(const Image &object)
{
    if (object != watched())
    {
        unsubscribe();
        m_watched = object;
        subscribe();
    }
}

void ImageWatcher::subscribe()
{
    Image current = watched();
    if (current.isValid())
    {
        sp_image_add_load_callback(current.handle(), &handleImageLoaded,
                                   static_cast<void *>(this));
    }
}

void ImageWatcher::unsubscribe()
{
    Image current = watched();
    if (current.isValid())
    {
        sp_image_remove_load_callback(current.handle(), &handleImageLoaded,
                                      static_cast<void *>(this));
    }
}

void ImageWatcher::customEvent(QEvent * event)
{
    Q_ASSERT(event->type() == QEvent::User);

    // event->type() is QEvent::User
    emit loaded();
}

namespace {

void SP_CALLCONV handleImageLoaded(sp_image *, void * userdata)
{
    ImageWatcher * watcher = static_cast<ImageWatcher *>(userdata);
    QEvent event(QEvent::User);
    QCoreApplication::sendEvent(watcher, &event);
}

}

}
