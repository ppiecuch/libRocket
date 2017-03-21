#ifndef QTROCKETSYSTEM_H
#define QTROCKETSYSTEM_H

#include <QString>
#include <QTime>
#include <Rocket/Core/SystemInterface.h>
#include <Rocket/Core/EventListener.h>
#include "qtRocketRenderInterface.h"

class RocketSystem : public Rocket::Core::SystemInterface
{
public:
    typedef Rocket::Core::Context Context;
    RocketSystem();
    ~RocketSystem();
    bool initialize();
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
