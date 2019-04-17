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

using namespace yarp::os;
using namespace yarp::sig;


/****************************************************************/

class GraspScenarioSetupModule : public RFModule
{
    std::string moduleName;

    BufferedPort<ImageOf<PixelRgba> > inputPort;
    BufferedPort<ImageOf<PixelRgba> > outputPort;

    ImageOf<PixelRgba> refImage;

    bool configure(ResourceFinder &rf) override
    {
        moduleName = rf.check("name", Value("grasp-scenario-setup")).toString();

        std::string refFileName = rf.check("file", Value("")).toString();

        if(!file::read(refImage, refFileName))
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
        return false;
    }

    /****************************************************************/
    double getPeriod() override
    {
        return 1.0;
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
