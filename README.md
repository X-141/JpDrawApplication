# JpDrawApplication
This is an application meant to help people like me dip their toes into machine learning and it's potential for learning new languages. In its current form,
JpDrawApplication is a functional demo piece showing specific models being used to predict what character is drawn by the user. 

It should also be stated that this application is a part of a summer independent study for my own personal education. Prior to this application, I had zero
knowledge in machine learning. But! That's all the more reason to try applying it in a cool application! It's personally how I learn best and make the mistakes
as early as I can.

## Current Model
The current model being used is K-Nearest Neighbor (kNN) Model. It is a surpervised model that is very easy to use and train. The only requirements to use the model
are images with cooresponding labeling for each.

The model is found in the resource folder with a .opknn extension. There should be an .txt file in the directory that allows the numeric labeling to be
tied to the images in the resource folder.

## Future Models?
Currently, kNN is being used as it's very straight forward model to use. It's not the most accurate/robust model, however, It's a very good teaching tool!
Future models will be implemented, likely as seperate branches.

## What Dataset is being used?
For this iteration the ETL Database is being used as it provides japanese character with cooresponding labeling for each. More specifically: ETL7LC and ETL7SC
is the target set as they contain hirigana characters for our uses. The dataset provides png images that must be sliced and csv files with character labeling for
each.

Below is the link to the dataset: http://etlcdb.db.aist.go.jp/

# How to build
### Requirements
1. Make
2. CMake
3. Git
4. GCC/G++
5. Qt5
6. OpenCV (Most recent version)

The application is built primarily using C++, Qt-gui framework, and the OpenCV library. These are the libraries I knew/wanted to learn so they are used
in this project.

## Getting started
These instructions will assume a Linux environment (This project was made on ubuntu based distro). Windows build is possible, however, it requires more in-depth
instructions that may be added later.

1. git clone https://github.com/X-141/JpDrawApplication.git
2. cd JpDrawApplication
3. mkdir Build ; cd Build
4. cmake ..
5. make -j 4

And you should be set! You may come across issues with CMake being unable to find packages like OpenCV and Qt.

## How to use
Once built and launched. A small window will appear with a black box (White box is also normal). This is the drawing area where you can play around with the
application!
Once you draw the image, you can select "Compare Layer" button. This button will process the image you have drawn and attempt to predict what character was
drawn. 
For instance it will extract your drawn image, rescale, and filtered to prepare it for the kNN model. Once fed, the model will output a numeric value that will
then be used to bring up the character the model predicted. Good Fun!

## Special Thanks
Personally want to thank Dr. Banerjee at WSU for providing help and insight into the world of Machine Learning!
