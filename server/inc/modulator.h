#pragma once
#pragma once
#include <iostream>
#include <vector>
#include <cmath>
#include <complex>
#include <gtest/gtest.h>

/// @brief The amplitude of carrier signal wave (value 1.0 is used to simplify equations)
constexpr double CARRIER_AMPLITUDE = 1.0;

/// @brief The default value of amplitude index (only changed when applied ASK)
constexpr double DEFAULT_AMPLITUDE_INDEX = 1.0;

/// @brief The default value of frequency index (only changed when applied FSK)
constexpr double DEFAULT_FREQUENCY_INDEX = 1.0;

/// @brief The default value of phase angle (only changed when applied PSK)
constexpr double DEFAULT_PHASE = -M_PI / 2;

/// @brief The sample rate key
constexpr const char *SAMPLE_RATE_KEY = "/fs";

/// @brief The amplitude index (ASK) key of bit 0
constexpr const char *ASK_ZERO_SIGN_KEY = "/modulation/ask/zeroSign";

/// @brief The amplitude index (ASK) key of bit 1
constexpr const char *ASK_ONE_SIGN_KEY = "/modulation/ask/oneSign";

/// @brief The phase index (PSK) key of bit 0
constexpr const char *PSK_ZERO_SIGN_KEY = "/modulation/psk/zeroSign";

/// @brief The phase index (PSK) key of bit 1
constexpr const char *PSK_ONE_SIGN_KEY = "/modulation/psk/oneSign";

/// @brief The frequency index (FSK) key of bit 0
constexpr const char *FSK_ZERO_SIGN_KEY = "/modulation/fsk/zeroSign";

/// @brief The frequency index (FSK) key of bit 1
constexpr const char *FSK_ONE_SIGN_KEY = "/modulation/fsk/oneSign";

/// @brief The sizeo of bits chunk when using 16QAM to modulate
constexpr unsigned int BIT_SIZE_16QAM = 4;

/// @brief The maximum distance to assign a signal point to each 16QAM point
constexpr double RADIUS_BOUND_16QAM = 0.2;

/// @brief The amplitude levels for 16QAM constellation diagram
constexpr double IQ_VALUES[] = {-0.75, -0.25, 0.25, 0.75};

class Modulator
{
public:
    /// @brief Default constructor, use to read modulation key in server database
    Modulator();

    /**
     * @brief Customize Constructor to receive carrier frequency and binary serires
     *
     * @param p_carrierFrequency - carrier wave frequency
     * @param p_binaryData - a binary data series
     */

    /**
     * @brief Customize Constructor to receive carrier frequency and binary serires
     *
     * @param p_carrierFrequency - carrier wave frequency
     * @param p_binaryData - a binary data series
     */
    Modulator(const double p_carrierFrequency, const std::string &p_binaryData);

    /**
     * @brief Modulate signal based on network type
     *
     * @param p_networkTypes - a network type
     *
     * @return a vector of real number (type double) representing modulated signal
     */
    std::vector<double> modulate(const std::string &p_networkTypes);

    /**
     * @brief Demodulate signal based on network type
     *
     * @param p_signal - a vector of real number (type double) representing modulated signal
     * @param p_networkTypes - a network type
     *
     * @return a set of real number (type double) representing modulated signal
     */
    std::string demodulate(const std::vector<double> &p_signal, const std::string &p_networkTypes);

    /**
     * @brief Set carrier wave frequency for server
     *
     * @param p_frequency - carrier wave frequency
     */
    void setFrequency(const double &p_frequency);

    /**
     * @brief Set binary data input for server
     *
     * @param p_binaryData - a binary data series
     */
    void setBinaryInput(const std::string &p_binaryData);

    /**
     * @brief Generate random binary data
     *
     * @param p_length - length of binary data
     *
     * @return a binary data series
     */
    std::string randomBinaryMessageGenerator(const int p_length);

private:
    /// @brief this define for unit test private attribute and function
    FRIEND_TEST(modulatorTestSuite, checkModulationPrivateMethod);

    /// @brief Carrier wave frequency
    double m_carrierFrequency;

    /// @brief The amount of samples transmitted in 1 second
    int m_sampleRate;

    /// @brief The amount of time to transmit 1 sample
    double m_sampleDuration;

    /// @brief The amount of bits transmitted in 1 second
    unsigned int m_bitRate;

    /// @brief The amount of bits transmitted in 1 second
    size_t m_samplesPerBit;

    /// @brief A binary data series
    std::string m_binaryInput;

    /// @brief key of bit 0 sign for ASK
    float m_askZeroSign;

    /// @brief key of bit 1 sign for ASK
    float m_askOneSign;

    /// @brief key of bit 0 sign for PSK
    float m_pskZeroSign;

    /// @brief key of bit 1 sign for PSK
    float m_pskOneSign;

    /// @brief key of bit 0 sign for FSK
    float m_fskZeroSign;

    /// @brief key of bit 1 sign for FSK
    float m_fskOneSign;

    /**
     * @brief Reading all modulation and sample rate values in server database
     */
    void readDatabase();

    /**
     * @brief Calculate signal value given amplitude, frequency, time and phase of the signal wave
     *
     * @param p_amplitudeIndex - the amplitude index in ASK modulation
     * @param p_frequencyIndex - the frequency index in FSK modulation
     * @param p_time - time variable
     * @param p_phase - the phase increment in PSK modulation
     *
     * @return value of signal at specific time
     */
    double getCarrierSignalValue(const double &p_amplitudeIndex, const double &p_frequencyIndex, const double &p_time, const double &p_phase);

    /**
     * @brief ASK Modulation
     *
     * @return a vector of real number (type double) representing modulated signal
     */
    std::vector<double> askModulation();

    /**
     * @brief PSK Modulation
     *
     * @return a vector of real number (type double) representing modulated signal
     */
    std::vector<double> pskModulation();

    /**
     * @brief FSK Modulation
     *
     * @return a vector of real number (type double) representing modulated signal
     */
    std::vector<double> fskModulation();

    /**
     * @brief 16 QAM Modulation
     *
     * @return a vector of real number (type double) representing modulated signal
     */
    std::vector<double> qam16Modulation();

    /**
     * @brief ASK Demodulation
     *
     * @param p_signal - a vector of real number (type double) representing modulated signal
     *
     * @return a binary data series representing message signal
     */
    std::string askDemodulation(const std::vector<double> &p_signal);

    /**
     * @brief PSK Demodulation
     *
     * @param p_signal - a vector of real number (type double) representing modulated signal
     *
     * @return a binary data series representing message signal
     */
    std::string pskDemodulation(const std::vector<double> &p_signal);

    /**
     * @brief FSK Demodulation
     *
     * @param p_signal - a vector of real number (type double) representing modulated signal
     *
     * @return a binary data series representing message signal
     */
    std::string fskDemodulation(const std::vector<double> &p_signal);

    /**
     * @brief 16 QAM Demodulation
     *
     * @param p_signal - a vector of real number (type double) representing modulated signal
     *
     * @return a binary data series representing message signal
     */
    std::string qam16Demodulation(const std::vector<double> &p_signal);
};