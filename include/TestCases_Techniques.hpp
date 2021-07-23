#ifndef TESTCASES_TECHNIQUES_HPP
#define TESTCASES_TECHNIQUES_HPP

#include "ImageProcessMethods.hpp"

#include "opencv2/core/mat.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"

#include <QDir>
#include <QString>
#include <QRegularExpression>

#include <vector>
#include <tuple>
#include <iostream>
#include <algorithm>
#include <map>
#include <chrono>

#include <gtest/gtest.h>

using logEntry = std::tuple<QString, char, int, int, bool, double>;

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

// Set a reasonable threshold for successful
// method. 90% accuracy given a method is good enough for
// a initial testing threshold.
// This can be set to a different value.
constexpr double ERROR_THRESHOLD = .90;

std::vector<std::pair<QString, cv::Mat>> LoadTestingImages() {
    QDir testingDir = QDir(QString("../testing"));
    QStringList imagesList = testingDir.entryList(QStringList() << "*.png" << "*.PNG", QDir::Files);
    
    //std::sort(imagesList.begin(), imagesList.end(), [](QString& a, QString& b){ return a < b;});

    std::vector<std::pair<QString, cv::Mat>> imageMats;
    imageMats.reserve(imagesList.size());

    for(const auto& image : imagesList) {
        //std::cout << image.toStdString() << std::endl;
        cv::Mat mat, greyMat;
        mat = cv::imread(testingDir.filePath(image).toStdString());
        cv::cvtColor(mat, greyMat, cv::COLOR_BGRA2GRAY);
        imageMats.emplace_back(std::make_pair<QString, cv::Mat>(QString(image), std::move(greyMat)));
    }

    return imageMats;
}

std::map<char, int> LoadDictionary() {
    QFile knnDictFile = QFile("../resource/kNNDictionary.txt");
    if(!knnDictFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return std::map<char, int>();

    std::map<char, int> dictionaryInfo;

    QTextStream in(&knnDictFile);

    QRegularExpression reg_txt("(?<character>\\w+),(?<number>\\d+)");

    while(!in.atEnd()) {
        QString line = in.readLine();
        auto match_txt = reg_txt.match(line);
        dictionaryInfo.insert(std::make_pair<char, int>(match_txt.captured("character").at(0).toLatin1(), 
        std::stoi(match_txt.captured("number").toStdString())));
    }

    return dictionaryInfo;
}

cv::Ptr<cv::ml::KNearest> LoadKNN() {
    return cv::ml::KNearest::load("../resource/kNN_ETL_Subset.opknn");
}

void LogTestData(const std::vector<logEntry>& aLogEntries, const char* aOutputFile, 
    float aTotalTests, float aTotalSuccess, float aTotalFails, double aTotalTime, double aAverageTime) {

    QFile logFile = QFile(aOutputFile);
    logFile.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate);

    if(logFile.isOpen()) {
        QTextStream stream(&logFile);

        double successRate = aTotalSuccess / aTotalTests;

        stream << "RESULTS [ TESTS : SUCCESS : FAILS ] -> " 
            << "[ " << aTotalTests << " : " << aTotalSuccess << " : " << aTotalFails << " ]\n";
        stream << "PERCENTAGE (SUCCESS/TESTS) -> " <<  successRate * 100.0 << "%\n";
        stream << "TIME (ms) [ TOTAL : AVERAGE ] -> " << "[ " << aTotalTime << " : " << aAverageTime << " ]\n";


        char character;
        int actualLabel;
        int calcLabel;
        double timeTaken;
        QString equal;
        QString pngName;
        for(const logEntry& entry : aLogEntries) {
            pngName = std::get<0>(entry);
            character = std::get<1>(entry);
            actualLabel = std::get<2>(entry);
            calcLabel = std::get<3>(entry);
            timeTaken = std::get<4>(entry);

            // Boolean value at index 4
            equal = "True";
            if(!std::get<4>(entry))
                equal = "False";

            stream << pngName << "," << character << "," << actualLabel << ","
                << calcLabel << "," << equal << "," << timeTaken << "\n"; 
        }

    } else
        std::cerr << "[ INFODATA ] unable to open file: " << aOutputFile << "\n";
}


TEST(TechniqueTests, ROITranslocation) {
    std::vector<std::pair<QString, cv::Mat>> images = LoadTestingImages();
    std::map<char, int> labelToChar = LoadDictionary();
    cv::Ptr<cv::ml::KNearest>  kNN = LoadKNN();

    // PNG name, char, true label, calculated label, Equal?
    std::vector<logEntry> testEntries;

    QRegularExpression regexprPNG("(?<number>\\d+)_(?<character>\\w+).png");

    int trueLabel = 0;
    int kNNLabel = 0;
    char characterLabel = '0';

    double totalTests = 0;
    double totalSuccess = 0;
    double totalFails = 0;

    double totalTime = 0;
    double timeTaken = 0;
    auto startTime = high_resolution_clock::now();
    auto endTime = high_resolution_clock::now();

    for(const auto& imageInfo : images) {

        characterLabel = regexprPNG.match(imageInfo.first).captured("character")[0].toLatin1();
        trueLabel = labelToChar.find(characterLabel)->second;
        
        startTime = high_resolution_clock::now();
        auto preparedImage = TechniqueMethods::ROITranslocation(imageInfo.second, false);
        kNNLabel = ImageMethods::passThroughKNNModel(kNN, preparedImage);
        endTime = high_resolution_clock::now();
        duration<double, std::milli> db_time = endTime - startTime;
        timeTaken = db_time.count();

        testEntries.emplace_back(std::make_tuple(imageInfo.first, characterLabel, trueLabel, 
            kNNLabel, trueLabel == kNNLabel, timeTaken));

        totalTime += timeTaken;

        if(trueLabel == kNNLabel)
            ++totalSuccess;
        else
            ++totalFails;

        ++totalTests;
    }

    
    double averageTime = totalTime / totalTests;
    
    double successRate = totalSuccess / totalTests;

    ASSERT_TRUE( successRate >= ERROR_THRESHOLD);

    std::cerr << "[ INFODATA ] RESULTS [ TESTS : SUCCESS : FAILS ] -> " 
        << "[ " << totalTests << " : " << totalSuccess << " : " << totalFails << " ]\n";
    std::cerr << "[ INFODATA ] PERCENTAGE (SUCCESS/TESTS) -> " <<  successRate * 100.0 << "%\n";
    std::cerr << "[ INFODATA ] TIME (ms) [ TOTAL : AVERAGE ] -> " << "[ " << totalTime << " : " << averageTime << " ]\n";

    LogTestData(testEntries, "../ROITranslocation_Data.csv", totalTests, totalSuccess, totalFails, totalTime, averageTime);
}

TEST(TechniqueTests, ROIRescaling) {
    std::vector<std::pair<QString, cv::Mat>> images = LoadTestingImages();
    std::map<char, int> labelToChar = LoadDictionary();
    cv::Ptr<cv::ml::KNearest>  kNN = LoadKNN();

    // PNG name, char, true label, calculated label, Equal?
    std::vector<logEntry> testEntries;
    QRegularExpression reg_png("(?<number>\\d+)_(?<character>\\w+).png");

    int trueLabel = 0;
    int kNNLabel = 0;
    char characterLabel = '0';

    double totalTests = 0;
    double totalSuccess = 0;
    double totalFails = 0;

    double totalTime = 0;
    double timeTaken = 0;
    auto startTime = high_resolution_clock::now();
    auto endTime = high_resolution_clock::now();

    for(const auto& imageInfo : images) {
        
        characterLabel = reg_png.match(imageInfo.first).captured("character")[0].toLatin1();
        trueLabel = labelToChar.find(characterLabel)->second;

        startTime = high_resolution_clock::now();
        auto translocatedImage = TechniqueMethods::ROIRescaling(imageInfo.second, false);
        kNNLabel = ImageMethods::passThroughKNNModel(kNN, translocatedImage);
        endTime = high_resolution_clock::now();
        duration<double, std::milli> db_time = endTime - startTime;
        timeTaken = db_time.count();

        testEntries.emplace_back(std::make_tuple(imageInfo.first, characterLabel, trueLabel, 
            kNNLabel, trueLabel == kNNLabel, timeTaken));

        totalTime += timeTaken;

        if(trueLabel == kNNLabel)
            ++totalSuccess;
        else
            ++totalFails;

        ++totalTests;
    }

    double averageTime = totalTime / totalTests;

    double successRate = totalSuccess / totalTests;

    ASSERT_TRUE( successRate >= ERROR_THRESHOLD);

    // testing::internal::CaptureStdout();

    std::cerr << "[ INFODATA ] RESULTS [ TESTS : SUCCESS : FAILS ] -> " 
        << "[ " << totalTests << " : " << totalSuccess << " : " << totalFails << " ]\n";
    std::cerr << "[ INFODATA ] PERCENTAGE (SUCCESS/TESTS) -> " <<  successRate * 100.0 << "%\n";
    std::cerr << "[ INFODATA ] TIME (ms) [ TOTAL : AVERAGE ] -> " << "[ " << totalTime << " : " << averageTime << " ]\n";


    LogTestData(testEntries, "../ROIRescaling_Data.csv", totalTests, totalSuccess, totalFails, totalTime, averageTime);
}

#endif