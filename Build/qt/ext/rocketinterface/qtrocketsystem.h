#ifndef QTROCKETSYSTEM_H
#define QTROCKETSYSTEM_H

#include <QObject>
#include <QString>
#include <QTime>
#include <Rocket/Core/SystemInterface.h>
#include <Rocket/Core/EventListener.h>

#include "qtrocketrenderinterface.h"

class RocketSystem : public QObject, public Rocket::Core::SystemInterface
{
    Q_OBJECT
public:
    typedef Rocket::Core::Context Context;
    RocketSystem();
    ~RocketSystem();
    bool initialize(int screen_w, int screen_h);
    void finalize();
    bool createContext(const int width, const int height);
    void resizeContext(const int width, const int height);
    void loadFonts(const QString &directory_path);
    void loadFont(const QString &file);
    Context *getContext() 
    {
        return context;
    }

    int context_width() { return context_w; }
    int context_height() { return context_h; }

    static RocketSystem & getInstance()
    {
        if( !instance )
        {
            instance = new RocketSystem;
        }
        return *instance;
    }

    static void removeInstance()
    {
        delete[] instance;
        instance = 0;
    }
    virtual float GetElapsedTime();

    virtual int TranslateString(Rocket::Core::String& translated, const Rocket::Core::String& input);

signals:
    void emitRocketEvent(Rocket::Core::Event &event);

private:

    class EventListener : public Rocket::Core::EventListener
    {
    public:
        EventListener() {}
        virtual void ProcessEvent(Rocket::Core::Event & event);
        virtual void OnDetach(Rocket::Core::Element *)
        {
            delete this;
        }
    };

    RocketRenderInterface renderInterface;
    Context *context;
    int context_w, context_h;
    EventListener *eventListener;
    QTime t;
    static RocketSystem * instance;

};

#endif // QTROCKETSYSTEM_H
