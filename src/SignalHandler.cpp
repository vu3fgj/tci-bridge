#include "SignalHandler.h"

#ifdef _WIN32
#include <windows.h>

static SignalHandler *s_instance = nullptr;

static BOOL WINAPI consoleCtrlHandler(DWORD ctrlType) {
    if (!s_instance) return FALSE;
    switch (ctrlType) {
        case CTRL_C_EVENT:
            emit s_instance->interruptReceived();
            return TRUE;
        case CTRL_BREAK_EVENT:
        case CTRL_CLOSE_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_SHUTDOWN_EVENT:
            emit s_instance->terminateReceived();
            return TRUE;
        default:
            return FALSE;
    }
}

SignalHandler::SignalHandler(QObject *parent) : QObject(parent) {
    s_instance = this;
    SetConsoleCtrlHandler(consoleCtrlHandler, TRUE);
}

SignalHandler::~SignalHandler() {
    SetConsoleCtrlHandler(consoleCtrlHandler, FALSE);
    if (s_instance == this) {
        s_instance = nullptr;
    }
}

SignalHandler *SignalHandler::install(QObject *parent) {
    return new SignalHandler(parent);
}

#else

#include <sys/socket.h>
#include <unistd.h>
#include <csignal>
#include <QSocketNotifier>

int SignalHandler::sigintFd[2] = {0, 0};
int SignalHandler::sigtermFd[2] = {0, 0};

static void intSignalHandler(int) {
    char a = 1;
    ::write(SignalHandler::sigintFd[0], &a, sizeof(a));
}

static void termSignalHandler(int) {
    char a = 1;
    ::write(SignalHandler::sigtermFd[0], &a, sizeof(a));
}

SignalHandler::SignalHandler(QObject *parent) : QObject(parent) {
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigintFd) == 0) {
        snInt = new QSocketNotifier(sigintFd[1], QSocketNotifier::Read, this);
        connect(snInt, &QSocketNotifier::activated, this, [this]() {
            snInt->setEnabled(false);
            char tmp;
            ::read(sigintFd[1], &tmp, sizeof(tmp));
            emit interruptReceived();
            snInt->setEnabled(true);
        });
    }

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigtermFd) == 0) {
        snTerm = new QSocketNotifier(sigtermFd[1], QSocketNotifier::Read, this);
        connect(snTerm, &QSocketNotifier::activated, this, [this]() {
            snTerm->setEnabled(false);
            char tmp;
            ::read(sigtermFd[1], &tmp, sizeof(tmp));
            emit terminateReceived();
            snTerm->setEnabled(true);
        });
    }
}

SignalHandler::~SignalHandler() {
    if (sigintFd[0] > 0) { ::close(sigintFd[0]); ::close(sigintFd[1]); }
    if (sigtermFd[0] > 0) { ::close(sigtermFd[0]); ::close(sigtermFd[1]); }
}

SignalHandler *SignalHandler::install(QObject *parent) {
    auto *handler = new SignalHandler(parent);
    struct sigaction sa;
    sa.sa_handler = intSignalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, nullptr);

    sa.sa_handler = termSignalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGTERM, &sa, nullptr);
    return handler;
}

#endif
