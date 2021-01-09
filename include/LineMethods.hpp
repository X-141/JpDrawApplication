#ifndef LINEMETHODS_H
#define LINEMETHODS_H

#include <QVector>
#include <QPoint>

void
_calculateQPointsRise(const int c_y1, const int c_y2, const float m, const float b, QVector<QPoint>& vec_points);

void
_calculateQPointsRun(const int c_x1, const int c_x2, const float m, const float b, QVector<QPoint>& vec_points);

void
_calculateQPointsLinear(const int c_x1, const int c_x2, const float m, const float b, QVector<QPoint>& vec_points);

/**
 * @brief calculateQPoints
 * @param aStartPoint
 * @param aEndPoint
 * @param aEmptyVector
 * Calculate best fit points between aStartPoint and
 * aEndPoint.
 * As a note, the user should pass in a vector with adequate space
 * reserved for storing calculated points.
 */
void
calculateQPoints(QPoint aStartPoint, QPoint aEndPoint, QVector<QPoint>& aEmptyVector);

#endif // LINEMETHODS_H
