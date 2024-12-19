#pragma once
#include <cstdlib>
#include <iostream>

constexpr int SUCCESS = 0;
/**
 * @brief information of the command that will be run in system
 *
 * @param inputfile text file that hold the data
 * @param output png file of the picture
 * @param fs sample rate
 * @param plotFile name of python file that used to plot
 *
 */
struct InfoPlot
{
  std::string inputFile;
  std::string outputFile;
  std::string fs;
  std::string plotFile;
};

class DataVisualizer
{
public:
  DataVisualizer();
  /**
   * @brief  pass python command to system to plot wave using python
   *
   * @param p_info information hold the data to plot
   *
   * @returns true if successfully run system, false if unsuccessfully run system
   */

  bool plotWave(const InfoPlot &p_info);
};
