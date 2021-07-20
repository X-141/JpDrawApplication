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

#include <gtest/gtest.h>

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

TEST(TechniqueTests, SingleConvertedImage) {
    auto images = LoadTestingImages();
    auto labelToChar = LoadDictionary();
    auto kNN = LoadKNN();

    // PNG name, char, true label, calculated label, Equal?
    std::vector<std::tuple<QString, char, int, int, bool>> testEntry;

    QRegularExpression reg_png("(?<number>\\d+)_(?<character>\\w+).png");

    int true_label = 0;
    int knn_label = 0;
    char character_label = '0';

    int total_chars = 0;
    int total_success = 0;
    int total_fails = 0;
    for(const auto& imageInfo : images) {
        //std::cout << imageInfo.first.toStdString() << std::endl;
        auto flatImage = ImageMethods::prepareMatrixForKNN(imageInfo.second);
        character_label = reg_png.match(imageInfo.first).captured("character")[0].toLatin1();
        true_label = labelToChar.find(character_label)->second;
        knn_label = ImageMethods::passThroughKNNModel(kNN, flatImage);

        testEntry.emplace_back(std::make_tuple(imageInfo.first, character_label, true_label, 
            knn_label, true_label == knn_label));
        
        std::cout << imageInfo.first.toStdString() << "\n" <<
        "\t" << character_label << "\n" <<
        "\t" << true_label << "\t" << knn_label;

        if(true_label == knn_label) {
            std::cout << "\t" << "TRUE" << std::endl;
            total_success++;
        }
        else {
            std::cout << "\t" << "FALSE" << std::endl;
            total_fails++;
        }
        ++total_chars;
    }
    std::cout << "TOTAL: " << total_chars << "\n\t" <<
    "TOTAL SUCCESS: " << total_success << "\n\t" <<
    "TOTAL FAILS: " << total_fails << std::endl;
}

#endif