#include "detector/detector.hpp"

Detector::Detector()
{

}

Detector::~Detector()
{
    OnDestroy();
}

void Detector::OnInit()
{
    LOG(INFO) << " - InferenceEngine: " << GetInferenceEngineVersion();
    std::string mode = "GPU";
    std::string dir_plugin = "/opt/intel/computer_vision_sdk_2018.2.299/deployment_tools/inference_engine/lib/ubuntu_16.04/intel64/";
	InferencePlugin plugin = PluginDispatcher({"", dir_plugin, ""}).getPluginByDevice(mode);

    std::string home = "/home/intel/workspace/Meepo/";
    std::string xml = home + "resource/model/head_fp16/head_fp16.xml";
    std::string bin = home + "resource/model/head_fp16/head_fp16.bin";
    CNNNetReader networkReader;
    networkReader.ReadNetwork(xml);
    networkReader.ReadWeights(bin);
    CNNNetwork network_ = networkReader.getNetwork();

    /** Taking information about all topology inputs **/
    InputsDataMap inputsInfo(network_.getInputsInfo());
	CHECK_EQ(inputsInfo.size(), 1) << "SSD network only has one input, but here are " << inputsInfo.size();
	batchSize_ = network_.getBatchSize();
	CHECK_EQ(batchSize_, 1) << "Only batch size = 1 support, but here are " << batchSize_;

    //std::string imageInputName_, imInfoInputName_;
    const auto& inputInfo_bitr = inputsInfo.begin();

	imageInputName_ = inputInfo_bitr->first;                                                                                                                                                                      
	/** Creating first input blob **/
	Precision inputPrecision = Precision::U8;
	inputInfo_bitr->second->setPrecision(inputPrecision);

	/*Prepare output blobs*/
	OutputsDataMap outputsInfo(network_.getOutputsInfo());
	CHECK_EQ(outputsInfo.size(), 1) << "SSD network only has one output, but here are " << outputsInfo.size();
	
    const auto& outputInfo_bitr = outputsInfo.begin();
	CHECK_EQ(outputInfo_bitr->second->creatorLayer.lock()->type, "DetectionOutput") << "Can't find a DetectionOutput layer in the topology";
	//std::string outputName_;
	DataPtr outputInfo;
	outputName_ = outputInfo_bitr->first;
	outputInfo = outputInfo_bitr->second;

	const SizeVector outputDims = outputInfo -> getTensorDesc().getDims();

    //LOG(INFO) << "outputDims: " << outputDims[0] << ", " << outputDims[1] << ", " << outputDims[2] << ", " << outputDims[3]; 
	maxProposalCount_ = outputDims[2];
	objectSize_ = outputDims[3];
	CHECK_EQ(objectSize_, 7) << "Output item should have 7 as a last dimension, but here are " << objectSize_;
	CHECK_EQ(outputDims.size(), 4) << "Incorrect output dimensions for SSD model, but here are " << outputDims.size();

	/** Set the precision of output data provided by the user, should be called before load of the network to the plugin **/
	outputInfo -> setPrecision(Precision::FP32);

	// Loading model to the plugin -
	LOG(INFO) << " - Loading model to the plugin";
	executable_network_ = plugin.LoadNetwork(network_, {});
	// Create infer request -
	infer_request_ = executable_network_.CreateInferRequest();

	/** get input blob **/
	Blob::Ptr imageInput = infer_request_.GetBlob(imageInputName_);
	N_ = imageInput->getTensorDesc().getDims()[0];
	C_ = imageInput->getTensorDesc().getDims()[1];
	H_ = imageInput->getTensorDesc().getDims()[2];
	W_ = imageInput->getTensorDesc().getDims()[3];
}

void Detector::OnDestroy()
{

}

void Detector::OnUpdate(cv::Mat& frame, std::vector<cv::Rect>& bbox)
{
	cv::Mat sample;
	cv::resize(frame, sample, cv::Size(W_, H_), 0, 0, cv::INTER_NEAREST); 
	std::vector<cv::Mat> inputChannels(3);
	cv::split(sample, inputChannels);  //将图像BGR进行拆分

	/** Creating input blob **/
	Blob::Ptr imageInput = infer_request_.GetBlob(imageInputName_);
	unsigned char* inputData = static_cast<unsigned char*>(imageInput->buffer());

	memcpy(inputData + 0 * H_ * W_, inputChannels[0].data, H_ * W_ * sizeof(unsigned char));
	memcpy(inputData + 1 * H_ * W_, inputChannels[1].data, H_ * W_ * sizeof(unsigned char));
	memcpy(inputData + 2 * H_ * W_, inputChannels[2].data, H_ * W_ * sizeof(unsigned char));
	
    infer_request_.Infer();
    const Blob::Ptr output_blob = infer_request_.GetBlob(outputName_);
    const float* detection = static_cast<PrecisionTrait<Precision::FP32>::value_type*>(output_blob->buffer());

    std::vector<std::vector<int> > boxes(batchSize_);
    std::vector<std::vector<int> > classes(batchSize_);

	//id, label, conf, xmin, ymin, xmax, ymax
	/* Each detection has image_id that denotes processed image */
	for (int curProposal = 0; curProposal < maxProposalCount_; curProposal++) {
		float image_id = detection[curProposal * objectSize_ + 0];
		float label = detection[curProposal * objectSize_ + 1];
		float confidence = detection[curProposal * objectSize_ + 2];
		/* CPU and GPU plugins have difference in DetectionOutput layer, so we need both checks */
		if (image_id < 0 || confidence == 0) {
			continue;
		}
		float xmin = detection[curProposal * objectSize_ + 3] * frame.cols;
		float ymin = detection[curProposal * objectSize_ + 4] * frame.rows;
		float xmax = detection[curProposal * objectSize_ + 5] * frame.cols;
		float ymax = detection[curProposal * objectSize_ + 6] * frame.rows;

		//LOG(INFO) << " - - [" << curProposal << "," << label << "] element, prob = " << confidence << "(" << xmin << "," << ymin << ")-(" << xmax << "," << ymax << ")" << " batch id : " << image_id;

		/*if (confidence > 0.3) {
			// Drawing only objects with >50% probability 
			classes[image_id].push_back(static_cast<int>(label));
			boxes[image_id].push_back(static_cast<int>(xmin));
			boxes[image_id].push_back(static_cast<int>(ymin));
			boxes[image_id].push_back(static_cast<int>(xmax - xmin));
			boxes[image_id].push_back(static_cast<int>(ymax - ymin));
		}*/
		if (confidence > 0.8 && label == 1) 
        {
            cv::Rect box(xmin, ymin, xmax - xmin, ymax - ymin);
            cv::Rect scale_roi = scaleRoi(box);
            bbox.emplace_back(scale_roi); 
        }
	}
}
