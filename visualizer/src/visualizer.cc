// C++ Program to illustrate the system function
#include "visualizer.h"

DataVisualizer::DataVisualizer() {}
bool DataVisualizer::plotWave(const InfoPlot &p_info)
{
    std::string str = "python3 " + p_info.plotFile + " --output " + p_info.outputFile + " --input " + p_info.inputFile + " --fs " + p_info.fs;
    const char *command = str.c_str();
    int returnCode = system(command);
    if (returnCode == SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }
}
