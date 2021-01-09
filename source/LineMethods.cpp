#include "LineMethods.hpp"

void
_calculateQPointsRise(const int c_y1, const int c_y2, const float m, const float b, QVector<QPoint>& vec_points) {
    float calcd_x;
    // Here we calculate the exact x coordinate position for
    // each y position from c_y1 to c_y2.
    for (int pos = std::min(c_y1, c_y2); pos <= std::max(c_y1, c_y2); ++pos) {
        calcd_x = (pos - b) / m; // x = ( y - b) / m;
        // Determine which pixel in the x position to color.
        if (calcd_x - static_cast<int>(calcd_x) <= static_cast<float>(.5))
            vec_points.push_back(QPoint(static_cast<int>(std::floor(calcd_x)), pos));
        else
            vec_points.push_back(QPoint(static_cast<int>(std::ceil(calcd_x)), pos));
    }
}

void
_calculateQPointsRun(const int c_x1, const int c_x2, const float m, const float b, QVector<QPoint>& vec_points) {
    float calcd_y;
    // Here we calculate the exact y coordinate position for
    // each x from c_x1 to c_x2.
    for (int pos = std::min(c_x1, c_x2); pos <= std::max(c_x1, c_x2); ++pos) {
        calcd_y = (m * pos) + b; // y = mx + b
        // Determine which pixel in the y position to color.
        if (calcd_y - static_cast<int>(calcd_y) < static_cast<float>(.5))
            vec_points.push_back(QPoint(pos, static_cast<int>(std::floor(calcd_y))));
        else
            vec_points.push_back(QPoint(pos, static_cast<int>(std::ceil(calcd_y))));
    }
}

void
_calculateQPointsLinear(const int c_x1, const int c_x2, const float m, const float b, QVector<QPoint>& vec_points) {
    // From whichever is the smallest x, create a linear line to big x.
    for (int pos = std::min(c_x1, c_x2); pos <= std::max(c_x1, c_x2); ++pos)
        vec_points.push_back({ pos, static_cast<int>((m * pos) + b) });
}

void
calculateQPoints(QPoint aStartPoint, QPoint aEndPoint, QVector<QPoint>& vec_points) {
    const int c_x1 = aStartPoint.x();
    const int c_y1 = aStartPoint.y();
    const int c_x2 = aEndPoint.x();
    const int c_y2 = aEndPoint.y();
    const float delta_x = static_cast<float>(c_x2 - c_x1);
    const float delta_y = static_cast<float>(c_y2 - c_y1);

    // Case where starting and ending positions are the same
    if(delta_x == 0 && delta_y == 0) {
        vec_points.push_back(aStartPoint);
        return;
    }

    // If vertical line, we just iterate over length and fill pixels
    if (delta_x == 0) {
        for (int pos = std::min(c_y1, c_y2); pos <= std::max(c_y1, c_y2); pos++)
//            vec_points.push_back({ c_x1, pos });
            vec_points.push_back(QPoint(c_x1,pos));
        return;
    }
    // If horizontal line, we do much the same
    else if (delta_y == 0) {
        for (int pos = std::min(c_x1, c_x2); pos <= std::max(c_x1, c_x2); pos++)
//            vec_points.push_back({ pos, c_y1 });
            vec_points.push_back(QPoint(pos, c_y1));
        return;
    }

    const float abs_x = std::abs(delta_x);
    const float abs_y = std::abs(delta_y);
    const float m = static_cast<float>(delta_y / delta_x); // rise / run
    const float b = c_y1 - (m * c_x1); // b = y - mx


    if (abs_y > abs_x) { // If rise is greater than run
        _calculateQPointsRise(c_y1, c_y2, m, b, vec_points);
    }
    else if (abs_y < abs_x) { // run is greater
        _calculateQPointsRun(c_x1, c_x2, m, b, vec_points);
    }
    else { // equal. This means its a linear function.
        // We do not need to determine the line heading.
        _calculateQPointsLinear(c_x1, c_x2, m, b, vec_points);
    }
}
