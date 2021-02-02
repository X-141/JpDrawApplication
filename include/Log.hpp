#ifndef LOG_HPP
#define LOG_HPP

#include <QtGlobal>
#include <QDebug>

/**
* This is a very basic logging system.
* Currently only supports printing to console.
*/

//#define DEBUG

#ifdef DEBUG
	#define LOG(x) qInfo() << x
#else
	#define LOG(x)
#endif


#endif // !LOG_HPP


