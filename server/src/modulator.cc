#include "modulator.h"
#include "serverCommon.h"
#include <random>
#include <stdexcept>

Modulator::Modulator()
{
    readDatabase();
}

Modulator::Modulator(const double p_carrierFrequency, const std::string &p_binaryData) : Modulator()
{
    setFrequency(p_carrierFrequency);
    setBinaryInput(p_binaryData);
}

void Modulator::readDatabase()
{
    auto floatValue = InMemDatabase::getInstance().getValue(ASK_ZERO_SIGN_KEY);
    extractValue(floatValue, m_askZeroSign);
    floatValue = InMemDatabase::getInstance().getValue(ASK_ONE_SIGN_KEY);
    extractValue(floatValue, m_askOneSign);
    floatValue = InMemDatabase::getInstance().getValue(PSK_ZERO_SIGN_KEY);
    extractValue(floatValue, m_pskZeroSign);
    m_pskZeroSign = m_pskZeroSign * M_PI / 180;
    floatValue = InMemDatabase::getInstance().getValue(PSK_ONE_SIGN_KEY);
    extractValue(floatValue, m_pskOneSign);
    m_pskOneSign = m_pskOneSign * M_PI / 180;
    floatValue = InMemDatabase::getInstance().getValue(FSK_ZERO_SIGN_KEY);
    extractValue(floatValue, m_fskZeroSign);
    floatValue = InMemDatabase::getInstance().getValue(FSK_ONE_SIGN_KEY);
    extractValue(floatValue, m_fskOneSign);
    const char *sampleChar = "";
    auto intValue = InMemDatabase::getInstance().getValue(SAMPLE_RATE_KEY);
    extractValue<char const *>(intValue, sampleChar);
    m_sampleRate = std::stoi(std::string(sampleChar));
    m_sampleDuration = static_cast<double>(1.0 / m_sampleRate);
}

void Modulator::setFrequency(const double &p_frequency)
{
    m_carrierFrequency = p_frequency;
    m_bitRate = p_frequency;
    m_samplesPerBit = m_sampleRate / m_bitRate;
}

void Modulator::setBinaryInput(const std::string &p_binaryData)
{
    m_binaryInput = p_binaryData;
}

double Modulator::getCarrierSignalValue(const double &p_amplitudeIndex, const double &p_frequencyIndex, const double &p_time, const double &p_phase)
{
    double amplitude = p_amplitudeIndex * CARRIER_AMPLITUDE;
    double frequency = p_frequencyIndex * m_carrierFrequency;
    return amplitude * cos(2 * M_PI * frequency * p_time + p_phase);
}

std::vector<double> Modulator::askModulation()
{
    std::vector<double> signal;
    double time = 0.0; // Time variable
    
    for (size_t bitIdx = 0; bitIdx < m_binaryInput.size(); ++bitIdx)
    {
        double amplitudeIndex = (m_binaryInput[bitIdx] == '1') ? m_askOneSign : m_askZeroSign;
        for (size_t sampleIdx = 0; sampleIdx < m_samplesPerBit; ++sampleIdx)
        {
            signal.emplace_back(getCarrierSignalValue(amplitudeIndex, DEFAULT_FREQUENCY_INDEX, time, DEFAULT_PHASE));
            time += m_sampleDuration;
        }
    }
    return signal;
}

std::string Modulator::askDemodulation(const std::vector<double> &p_signal)
{
    std::string outputBinary;
    double threshold = m_askZeroSign;
    int totalBits = p_signal.size() / m_samplesPerBit;
    for (size_t bitIdx = 0; bitIdx < totalBits; ++bitIdx)
    {
        double accumulator = 0.0;
        for (size_t sampleIdx = 0; sampleIdx < m_samplesPerBit && (bitIdx * m_samplesPerBit + sampleIdx) < p_signal.size(); ++sampleIdx)
        {
            accumulator += std::abs(p_signal[bitIdx * m_samplesPerBit + sampleIdx]);
        }
        accumulator /= m_samplesPerBit;
        outputBinary += (accumulator > threshold) ? '1' : '0';
    }
    return outputBinary;
}

std::vector<double> Modulator::pskModulation()
{
    std::vector<double> signal;
    double time = 0.0; // Time variable
    for (size_t bitIdx = 0; bitIdx < m_binaryInput.size(); ++bitIdx)
    {
        double phase = (m_binaryInput[bitIdx] == '1') ? m_pskOneSign : m_pskZeroSign;
        for (size_t sampleIdx = 0; sampleIdx < m_samplesPerBit; ++sampleIdx)
        {
            signal.emplace_back(getCarrierSignalValue(DEFAULT_AMPLITUDE_INDEX, DEFAULT_FREQUENCY_INDEX, time, DEFAULT_PHASE + phase));
            time += m_sampleDuration;
        }
    }
    return signal;
}

std::string Modulator::pskDemodulation(const std::vector<double> &p_signal)
{
    std::string outputBinary;
    int totalBits = p_signal.size() / m_samplesPerBit;
    for (size_t bitIdx = 0; bitIdx < totalBits; ++bitIdx)
    {
        // These variables accumulate the correlation between the received signal
        // and the reference signal for binary '0' and '1', respectively.
        double correlation0 = 0.0;
        double correlation1 = 0.0;
        for (size_t sampleIdx = 0; sampleIdx < m_samplesPerBit && (bitIdx * m_samplesPerBit + sampleIdx) < p_signal.size(); ++sampleIdx)
        {
            double time = static_cast<double>(bitIdx * m_samplesPerBit + sampleIdx) / m_sampleRate;
            double signalValue = p_signal[bitIdx * m_samplesPerBit + sampleIdx];
            correlation0 += getCarrierSignalValue(signalValue, DEFAULT_FREQUENCY_INDEX, time, DEFAULT_PHASE + m_pskZeroSign);
            correlation1 += getCarrierSignalValue(signalValue, DEFAULT_FREQUENCY_INDEX, time, DEFAULT_PHASE + m_pskOneSign);
        }
        // After calculating the correlations for a chunk, compares correlation0 and correlation1.
        // If correlation0 is greater, it means the signal is more similar to the reference signal
        // for '0', so the output bit is '0'. Otherwise, it's '1'.
        outputBinary += (correlation0 > correlation1) ? '0' : '1';
    }
    return outputBinary;
}

std::vector<double> Modulator::fskModulation()
{
    std::vector<double> signal;

    double time = 0.0; // Time variable
    for (size_t bitIdx = 0; bitIdx < m_binaryInput.size(); ++bitIdx)
    {
        double frequencyIndex = (m_binaryInput[bitIdx] == '1') ? m_fskOneSign : m_fskZeroSign;
        for (size_t sampleIdx = 0; sampleIdx < m_samplesPerBit; ++sampleIdx)
        {
            signal.emplace_back(getCarrierSignalValue(DEFAULT_AMPLITUDE_INDEX, frequencyIndex, time, DEFAULT_PHASE));
            time += m_sampleDuration;
        }
    }
    return signal;
}

std::string Modulator::fskDemodulation(const std::vector<double> &p_signal)
{
    std::string outputBinary;
    int totalBits = p_signal.size() / m_samplesPerBit;
    for (size_t bitIdx = 0; bitIdx < totalBits; ++bitIdx)
    {
        // These variables accumulate the correlation between the received signal
        // and the reference signal for binary '0' and '1', respectively.
        double correlation0 = 0.0;
        double correlation1 = 0.0;
        for (size_t sampleIdx = 0; sampleIdx < m_samplesPerBit && (bitIdx * m_samplesPerBit + sampleIdx) < p_signal.size(); ++sampleIdx)
        {
            double time = static_cast<double>(bitIdx * m_samplesPerBit + sampleIdx) / m_sampleRate;
            double signalValue = p_signal[bitIdx * m_samplesPerBit + sampleIdx];
            correlation0 += getCarrierSignalValue(signalValue, m_fskZeroSign, time, DEFAULT_PHASE);
            correlation1 += getCarrierSignalValue(signalValue, m_fskOneSign, time, DEFAULT_PHASE);
        }
        // After calculating the correlations for a chunk, compares correlation0 and correlation1.
        // If correlation1 is greater, it means the signal is more similar to the reference signal
        // for '1', so the output bit is '1'. Otherwise, it's '0'.
        outputBinary += (correlation1 > correlation0) ? '1' : '0';
    }

    return outputBinary;
}

std::complex<double> mapToQAM16Constellation(const int (&bits)[BIT_SIZE_16QAM])
{

    int sizeIQ = sizeof(IQ_VALUES) / sizeof(IQ_VALUES[0]);

    // Mapping for I and Q components

    double iValues[sizeIQ];

    double qValues[sizeIQ];

    std::copy(IQ_VALUES, IQ_VALUES + sizeIQ, iValues);

    std::copy(IQ_VALUES, IQ_VALUES + sizeIQ, qValues);

    // Map 2-bit pair to I and Q values
    double I = iValues[bits[0] * 2 + bits[1]]; // First two bits map to I
    double Q = qValues[bits[2] * 2 + bits[3]]; // Last two bits map to Q

    // Return the complex floatValue (I + jQ)
    return std::complex<double>(I, Q);
}

std::vector<std::complex<double>> mapBitsToSymbols16QAM(const std::string &p_binaryData)
{
    std::vector<std::complex<double>> symbols;

    if (p_binaryData.size() % BIT_SIZE_16QAM != 0)
    {
        throw std::invalid_argument("Binary data length must be a multiple of 4 for 16-QAM.");
    }

    // Process each group of 4 bits

    for (size_t bitIdx = 0; bitIdx < p_binaryData.size(); bitIdx += BIT_SIZE_16QAM)
    {
        // Convert from string to int by minus '0'
        int bits[BIT_SIZE_16QAM] = {p_binaryData[bitIdx] - '0', p_binaryData[bitIdx + 1] - '0',
                                    p_binaryData[bitIdx + 2] - '0', p_binaryData[bitIdx + 3] - '0'};

        symbols.emplace_back(mapToQAM16Constellation(bits));
    }

    return symbols;
}

std::vector<double> Modulator::qam16Modulation()
{
    std::vector<std::complex<double>> symbols = mapBitsToSymbols16QAM(m_binaryInput);

    std::vector<double> signal;

    double time = 0.0; // Time variable
    for (const auto &symbol : symbols)
    {
        // Generate the signal for the symbol
        for (int sampleIdx = 0; sampleIdx < m_samplesPerBit; ++sampleIdx)
        {
            // In-phase (I) and Quadrature (Q) components of the symbol
            // Cosine wave for the I component
            double inPhase = getCarrierSignalValue(symbol.real(), DEFAULT_FREQUENCY_INDEX, time, DEFAULT_PHASE);
            // Sine wave for the Q component
            double quadrature = getCarrierSignalValue(symbol.imag(), DEFAULT_FREQUENCY_INDEX, time, DEFAULT_PHASE - M_PI / 2);

            signal.emplace_back(inPhase + quadrature);

            time += m_sampleDuration;
        }
    }
    return signal;
}

// Normalize p_symbol from range (-4, 4) into mapping values (-3, -1, 1, 3)
double normalizeIQ(double p_symbol)
{
    p_symbol = static_cast<int>(floor(p_symbol * 2)) * 2 + 1;
    // Scale IQ_VALUES into {-0.75, -0.25, 0.25, 0.75}
    p_symbol /= 4.0;
    return p_symbol;
}

std::string mapSymbolToBits16QAM(double &p_symbol)
{
    std::string bits;
    // For comparing double to double
    double epsilon = 1e-6;
    std::string bitArray[] = {"00", "01", "10", "11"};
    int sizeIQ = sizeof(IQ_VALUES) / sizeof(IQ_VALUES[0]);

    p_symbol = normalizeIQ(p_symbol);

    for (int bitIdx = 0; bitIdx < sizeIQ; ++bitIdx)
    {
        if (std::fabs(IQ_VALUES[bitIdx] - p_symbol) < epsilon)
        {
            bits = bitArray[bitIdx];
        }
    }
    if (bits.empty())
    {
        g_serverLogger.error("Invalid symbol of 16 QAM!");
    }
    return bits;
}

std::string Modulator::qam16Demodulation(const std::vector<double> &p_signal)
{
    std::string demodulatedBits;
    int totalBits = p_signal.size() / m_samplesPerBit;
    for (size_t bitIdx = 0; bitIdx < totalBits; ++bitIdx)
    {
        double iAvg = 0.0, qAvg = 0.0;

        // Accumulate the in-phase and quadrature components
        for (size_t sampleIdx = 0; sampleIdx < m_samplesPerBit; ++sampleIdx)
        {
            double time = (bitIdx * m_samplesPerBit + sampleIdx) * m_sampleDuration;
            double signalValue = p_signal[bitIdx * m_samplesPerBit + sampleIdx];
            iAvg += getCarrierSignalValue(signalValue, DEFAULT_FREQUENCY_INDEX, time, DEFAULT_PHASE);
            qAvg += getCarrierSignalValue(signalValue, DEFAULT_FREQUENCY_INDEX, time, DEFAULT_PHASE - M_PI / 2);
        }

        // Normalize I and Q by the number of samples per symbol
        iAvg /= m_samplesPerBit;
        qAvg /= m_samplesPerBit;

        // Map I and Q to the closest constellation points
        // During signal generation
        // The in-phase component I is scaled by cos(2*pi*f_c*t),
        // and the quadrature component Q is scaled by sin(2*pi*f_c*t).
        // These scaling factors lead to a factor of 1/2
        // when you're averaging the received signal to recover I and Q.
        // When compute the average over the symbol duration
        // (which is essentially performing an integration over time)
        // we get a floatValue that is half the magnitude of the original transmitted values for both I and Q.
        // hence, multiply by 2.
        double iIndex = iAvg * 2;
        double qIndex = qAvg * 2;
        double distance = pow(iIndex - normalizeIQ(iIndex), 2) + pow(qIndex - normalizeIQ(qIndex), 2);
        if (distance > pow(RADIUS_BOUND_16QAM, 2))
        {
            g_serverLogger.error(stringify("Signal in 16QAM symbol out of bounds! Distance: ", distance));
            return "";
        }

        // Map indices to bits
        demodulatedBits += mapSymbolToBits16QAM(iIndex);
        demodulatedBits += mapSymbolToBits16QAM(qIndex);
    }
    return demodulatedBits;
}

std::string Modulator::randomBinaryMessageGenerator(const int p_length)
{
    std::string binaryMessage;
    srand(time(0));
    for (int index = 0; index < p_length; index++)
    {
        binaryMessage += (rand() % 2 + '0');
    }
    return binaryMessage;
}

std::vector<double> Modulator::modulate(const std::string &p_networkTypes)
{
    if (p_networkTypes == "2G")
    {
        return askModulation();
    }
    if (p_networkTypes == "3G")
    {
        return pskModulation();
    }
    if (p_networkTypes == "4G")
    {
        return fskModulation();
    }
    if (p_networkTypes == "5G")
    {
        return qam16Modulation();
    }
    return {};
}

std::string Modulator::demodulate(const std::vector<double> &p_signal, const std::string &p_networkTypes)
{
    if (p_networkTypes == "2G")
    {
        return askDemodulation(p_signal);
    }
    if (p_networkTypes == "3G")
    {
        return pskDemodulation(p_signal);
    }
    if (p_networkTypes == "4G")
    {
        return fskDemodulation(p_signal);
    }
    if (p_networkTypes == "5G")
    {
        return qam16Demodulation(p_signal);
    }
    return "";
}