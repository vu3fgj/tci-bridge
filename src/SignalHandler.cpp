#include "SignalHandler.h"

#ifndef _WIN32
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <QSocketNotifier>

int SignalHandler::sighupFd[2];
int SignalHandler::sigtermFd[2];
int SignalHandler::sigintFd[2];

// ... (keep the original Linux implementation inside this block) ...

#else

// Clean Windows fallback: Stub it out so MSVC compiles smoothly
SignalHandler::SignalHandler(QObject *parent) : QObject(parent) {}
SignalHandler::~SignalHandler() {}
void SignalHandler::setupSignalHandlers() {}

#endif
