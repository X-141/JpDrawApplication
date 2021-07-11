#ifndef LOG_HPP
#define LOG_HPP

#include <QDebug>
#include <QDateTime>
#include <QString>
#include <QFile>

/**
* This is a very basic logging system.
* Currently only supports printing to console
* and writing to file.
*/

// https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
enum level {
    standard = 1,
    warning = 2,
    error = 4,
    info = 8
};

class Logger {
public:

    static Logger& getInstance()
    {
        static Logger instance;
        return instance;
    }

//    static void setLevel(int aLevel) {
//        mLevel = aLevel;
//    }

    static void logData(const int aLevel, const QString& aMethodLocation, const QString& aInfo) {
        QString output = "[ " + getTime() + " ] " + aMethodLocation + ": " + aInfo;

        if (mLevel | aLevel) {
            qInfo() << output;
            if(mFile.isOpen()) {
                QTextStream logFile(&mFile);
                logFile << output << "\n";
            }
        }
    }

    Logger(Logger const&) = delete;
    void operator=(Logger const&) = delete;

private:
    Logger() {
        open();
        mLevel = level::standard | level::warning | level::error;
    }

    ~Logger() {
        close();
    }

    static void close() {
        mFile.close();
    }

    static void open() {
        mFile.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate);
    }

    static QString getTime() {
        return std::move(QDateTime::currentDateTime().toString("hh:mm:ss"));
    }

private:
    static inline const char* mOutputFileName = "../Log.txt";
    static inline QFile mFile = QFile(mOutputFileName);
    static inline int mLevel;
};

const auto LOG = Logger::getInstance().logData;

#endif // !LOG_HPP


