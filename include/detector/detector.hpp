#ifndef detector_h__
#define detector_h__
#include "util/util.hpp"
#include "util/singleton.h" 
#include "inference_engine/samples/common/format_reader/format_reader_ptr.h"
#include "inference_engine/include/inference_engine.hpp"
#include "inference_engine/samples/extension/ext_list.hpp"

#include "inference_engine/samples/common/samples/common.hpp"
//#include "inference_engine/samples/common/samples/slog.hpp"
//#include "inference_engine/samples/common/samples/args_helper.hpp"

using namespace InferenceEngine;
class Detector : public Singleton<Detector>
{
public:
    Detector();
    ~Detector();
    void OnInit();
    void OnDestroy();
    void OnUpdate(cv::Mat& frame, std::vector<cv::Rect>& bbox);

private:
    std::string imageInputName_, imInfoInputName_;
	std::string outputName_;
	int maxProposalCount_, objectSize_;
    size_t batchSize_;
	ExecutableNetwork executable_network_;
	InferRequest infer_request_;
	int N_, C_, H_, W_;

};

#endif//detector_h__
