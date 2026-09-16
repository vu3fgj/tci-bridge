#pragma once

#include <QObject>

class SignalHandler : public QObject {
    Q_OBJECT
public:
    explicit SignalHandler(QObject *parent = nullptr);
    ~SignalHandler() override;

    static SignalHandler *install(QObject *parent = nullptr);

signals:
    void interruptReceived();
    void terminateReceived();

private:
#ifndef _WIN32
    static int sigintFd[2];
    static int sigtermFd[2];
    class QSocketNotifier *snInt = nullptr;
    class QSocketNotifier *snTerm = nullptr;
#endif
};
