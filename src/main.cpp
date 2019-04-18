/******************************************************************************
 *                                                                            *
 * Copyright (C) 2019 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

/**
 * @file main.cpp
 * @authors: Jason Chevrie <jason.chevrie@iit.it>
 */

#include <string>

#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/cv/Cv.h>

using namespace yarp::os;
using namespace yarp::sig;


/****************************************************************/

class GraspScenarioSetupModule : public RFModule
{
    std::string moduleName;

    BufferedPort<ImageOf<PixelRgb> > inputPort;
    BufferedPort<ImageOf<PixelRgb> > outputPort;

    // Reference image loaded at start-up
    ImageOf<PixelRgb> refImage;
    // OpenCV Mat of the reference image rescaled to fit the input stream
    // its size can change at run time, but it should mostly be constant
    cv::Mat refImageMat;

    bool configure(ResourceFinder &rf) override
    {
        rf.setVerbose(rf.check("verbose"));

        moduleName = rf.check("name", Value("grasp-scenario-setup")).toString();

        std::string refFileName = rf.check("file", Value("")).toString();
        if(refFileName == "")
        {
            yError() << "Missing parameter --file for reference image file" << refFileName;
            return false;
        }

        std::string refFilePath = rf.findFile(refFileName);

        if(!file::read(refImage, refFilePath))
        {
            yError() << "Could not open reference image file" << refFileName;
            return false;
        }

        if(!inputPort.open("/" + moduleName + "/image:i"))
        {
            yError() << "Could not open port" << "/" + moduleName + "/image:i" ;
            return false;
        }

        if(!outputPort.open("/" + moduleName + "/image:o"))
        {
            yError() << "Could not open port" << "/" + moduleName + "/image:o" ;
            return false;
        }

        return true;
    }

    /****************************************************************/
    bool updateModule() override
    {
        if((inputPort.getInputCount() < 1) || (outputPort.getOutputCount() < 1))
        {
            yarp::os::Time::delay(0.1);
            return true;
        }

        ImageOf<PixelRgb> *inIm = inputPort.read();
        cv::Mat inMat = yarp::cv::toCvMat(*inIm);
        cv::Size imSize = inMat.size();

        // Rescale reference image Mat to adapt to input stream if necessary
        if(imSize!=refImageMat.size())
        {
            ImageOf<PixelRgb> imTmp;
            imTmp.copy(refImage);
            cv::Mat matTmp = yarp::cv::toCvMat(imTmp);
            cv::resize(matTmp, refImageMat, imSize);
        }

        cv::Mat mixIn[2] = {inMat, refImageMat};
        cv::Mat outMat(imSize, CV_8UC3, cvScalar(255,0,0));
        int from_to[] = {0,0, 1,1, 5,2};
        cv::mixChannels( mixIn, 2, &outMat, 1, from_to, 3);

        ImageOf<PixelRgb> &outImage = outputPort.prepare();
        outImage.copy(yarp::cv::fromCvMat<PixelRgb>(outMat));
        outputPort.write();

        return true;
    }

    /****************************************************************/
    double getPeriod() override
    {
        return 0.0;
    }

    /****************************************************************/
    bool interruptModule() override
    {
        inputPort.interrupt();
        outputPort.interrupt();

        return true;
    }

    /****************************************************************/
    bool close() override
    {
        inputPort.close();
        outputPort.close();

        return true;
    }

    /****************************************************************/
    bool respond(const Bottle& command, Bottle& reply) override
    {
        return true;
    }

public:
    GraspScenarioSetupModule():
        moduleName("grasp-scenario-setup"),
        inputPort(),
        outputPort(),
        refImage()
    {}
};

int main(int argc, char *argv[])
{
    Network yarp;
    if (!yarp.checkNetwork())
    {
        yError()<<"Unable to find Yarp server!";
        return EXIT_FAILURE;
    }

    ResourceFinder rf;
    rf.setDefaultContext("grasp-scenario-setup");
    rf.configure(argc, argv);

    GraspScenarioSetupModule module;
    return module.runModule(rf);
}
