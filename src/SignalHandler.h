// SignalHandler.h
#pragma once

#include <QObject>

class SignalHandler : public QObject {
    Q_OBJECT
public:
    explicit SignalHandler(QObject *parent = nullptr);
    ~SignalHandler() override;

    static void setupSignalHandlers();

signals:
    void quitRequested();

private:
#ifndef _WIN32
    static int sighupFd[2];
    static int sigtermFd[2];
    static int sigintFd[2];
    class QSocketNotifier *snInt = nullptr;
    class QSocketNotifier *snTerm = nullptr;
#endif
};
