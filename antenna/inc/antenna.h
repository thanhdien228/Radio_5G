#pragma once
#include <cstdlib>
#include <iostream>
#include "serverCommon.h"
#include <format>
#include <optional>
#include <utility>

/// @brief The deviation of Gaussian noise, use in noise normal distribution
constexpr double NOISE_LEVEL = 0.07;

/// @brief The index use to smoothing out the noise and maintaining the integrity of the original signal
constexpr double NOISE_FILTER_ALPHA = 0.7;

class Antenna
{
public:
    /**
     * @brief constructor to enable log file
     *
     * @returns None
     */
    Antenna();

    /**
     * @brief if the parameter is a binary data, this will plot wave after modulator,
     *  if the parameter is empty, this plot original wave using FFT
     *
     * @param p_dataForDL optional parameter, contain a string of binary data if plot modulated wave
     * @returns None
     */
    void visualizeData(std::optional<std::pair<std::string, std::string>> p_dataForDL = std::nullopt);

    /**
     * @brief generate the random binary data which simulate the data which we receive
     *
     * @param p_length length of the binary data
     * @returns value of length
     */
    std::string randomBinaryMessageGenerator(const int p_length);
    
    /**
     * @brief Add Gaussian noise to the signal
     *
     * @param p_signal - a vector of real number (type double) representing modulated signal
     */
    void addNoise(std::vector<double> &p_signal);

    /**
     * @brief Filter by smoothing out the noise
     *
     * @param p_signal - a vector of real number (type double) representing modulated signal
     */
    void filterNoise(std::vector<double> &p_signal);

private:
    /**
     * @brief using key to get value from database
     *
     * @param p_key key of the data that need read
     * @returns value of key
     */
    std::optional<std::string> getValue(const std::string &p_key);
};
