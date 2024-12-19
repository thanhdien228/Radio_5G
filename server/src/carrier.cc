#include "carrier.h"

Carrier::Carrier()
{
    m_network = "";
    m_flagCarrier = false;
    m_frequency = 0;
}

bool Carrier::setNetwork(const std::string &p_network)
{
    if (!m_flagCarrier)
    {
        m_network = p_network;
        m_flagCarrier = true;
        return true;
    }
    else
    {
        return false;
    }
}

std::string Carrier::getNetwork()
{
    return m_network;
}

void Carrier::setFrequency(const size_t &p_freq)
{
    m_frequency = p_freq;
}

size_t Carrier::getFrequency()
{
    return m_frequency;
}

void Carrier::releaseCarrier()
{
    m_network = "";
    m_flagCarrier = false;
    m_frequency = 0;
}

bool Carrier::getCarrierStatus()
{
    return m_flagCarrier;
}

bool Carrier::checkSupportedCarrier(const std::string &p_network)
{
    std::string key = "/supportedCarriers";
    const char *supportedCarriers = "";
    auto resVariant = InMemDatabase::getInstance().getValue(key);
    extractValue<char const *>(resVariant, supportedCarriers);
    std::string strSupportedCarriers(supportedCarriers);
    if (strSupportedCarriers.find(p_network) != std::string::npos)
        return true;
    else
        return false;
}

bool Carrier::checkSupportedFrequency(const ssize_t &p_freq)
{
    std::string keyLowFreq = "/antenna/supportedLowFreq";
    std::string keyHighFreq = "/antenna/supportedHighFreq";
    int supportedLowFreq;
    int supportedHighFreq;
    auto varSupportedLowFreq = InMemDatabase::getInstance().getValue(keyLowFreq);
    auto varSupportedHighFreq = InMemDatabase::getInstance().getValue(keyHighFreq);
    extractValue<int>(varSupportedLowFreq, supportedLowFreq);
    extractValue<int>(varSupportedHighFreq, supportedHighFreq);
    if ((p_freq >= supportedLowFreq) && (p_freq <= supportedHighFreq))
        return true;
    else
        return false;
}