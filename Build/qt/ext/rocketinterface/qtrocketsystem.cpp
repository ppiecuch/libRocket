#include <Rocket/Core.h>
#ifdef ROCKET_FREETYPE
    #include <Rocket/Core/FreeType/FontProvider.h>
#endif
#include <Rocket/Controls.h>
#include <Rocket/Debugger.h>
#include <QDir>

#include "qtrocketsystem.h"
#include "qtrocketfileinterface.h"
#include "tools.h"

RocketSystem::RocketSystem() :
    renderInterface(),
    context( 0 ), context_w( 0 ), context_h( 0 ),
    eventListener( 0 )
{
    t.start();
}

RocketSystem::~RocketSystem()
{
    Rocket::Core::Shutdown();
}

float RocketSystem::GetElapsedTime()
{
    return t.elapsed();
}

int RocketSystem::TranslateString(Rocket::Core::String& translated, const Rocket::Core::String& input)
{
    translated = input;
    return 0;
}

bool RocketSystem::initialize(int screen_w, int screen_h)
{
    Rocket::Core::SetRenderInterface(&renderInterface);
    Rocket::Core::SetSystemInterface(this);
    Rocket::Core::SetFileInterface(new RocketFileInterface());
    Rocket::Core::Initialise();

    #if defined(ROCKET_FREETYPE) || defined(ROCKET_WITH_FREETYPE)
        Rocket::Core::FreeType::FontProvider::Initialise();
    #else
        Rocket::Core::FontDatabase::Initialise();
    #endif

    Rocket::Controls::Initialise();

    context_w = screen_w;
    context_h = screen_h;

    if (context_w && context_h)
        return createContext(context_w, context_h);
    else {
        context_w = 1024; context_h = 768;
        return createContext(1024, 768);
    }

    return createContext(context_w, context_h);
}

void RocketSystem::finalize()
{
#ifdef ROCKET_FREETYPE
    Rocket::Core::FreeType::FontProvider::Shutdown();
#endif
    Rocket::Core::Shutdown();
    Rocket::Core::SetRenderInterface(0);
    Rocket::Core::SetSystemInterface(0);
    Rocket::Core::SetFileInterface(0);
}

bool RocketSystem::createContext(const int width, const int height)
{
    if (context)
        context->RemoveReference();

    context = Rocket::Core::CreateContext("main", Rocket::Core::Vector2i(width, height));

    if (context == NULL) 
    {
        Rocket::Core::Shutdown();
        return false;
    }

	Rocket::Debugger::Initialise(context);
    Rocket::Debugger::SetContext(context);

    context_w = width;
    context_h = height;

    eventListener = new EventListener();

    context->AddEventListener("click", eventListener, true);

    return true;
}

void RocketSystem::resizeContext(const int width, const int height)
{
    if (context == NULL)
    {
        createContext(width, height);
        return;
    }

    context->SetDimensions(Rocket::Core::Vector2i(width, height));

    context_w = width;
    context_h = height;
}

void RocketSystem::loadFonts(const QString &directory_path)
{
    QDir directory(directory_path);
    QStringList name_filter_list;
    QStringList file_list;
    QString prefix;

    name_filter_list << "*.otf" << "*.ttf" << "*.fon" << "*.fnt" << "*.bmf";

    file_list = directory.entryList(name_filter_list);

    foreach(const QString &file_name, file_list) {
        loadFont(directory_path + file_name);
    }
}

void RocketSystem::loadFont(const QString &file)
{
    Rocket::Core::String
        r_string = file.toUtf8().data();

    #ifdef ROCKET_FREETYPE
        Rocket::Core::FreeType::FontProvider::LoadFontFace(r_string);
    #else
        Rocket::Core::FontDatabase::LoadFontFace(r_string);
    #endif
}

void RocketSystem::EventListener::ProcessEvent(Rocket::Core::Event &event)
{
    emit RocketSystem::getInstance().emitRocketEvent(event);
}


RocketSystem* RocketSystem::instance = 0;
