#ifndef combine_h__
#define combine_h__

#include "util/util.hpp"
#include "util/singleton.h" 
class Combine :public Singleton<Combine>
{
public:
    Combine();
    ~Combine();
    void OnInit();
    void OnDestroy();
    void OnUpdate(cv::Mat& frame, std::vector<cv::Rect>& bbox);
};
#endif//combine_h__
