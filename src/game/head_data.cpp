#include "game/head_data.hpp"

HeadData::HeadData(size_t id, const cv::Rect& bbox)
    :bbox_(bbox)
{
    id_ = id;
	int cent_x = bbox.x + (bbox.width >> 1);
	int cent_y = bbox.y + (bbox.height >> 1);
    trace_.emplace_back(cv::Point(cent_x, cent_y));
}
