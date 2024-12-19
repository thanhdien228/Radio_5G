#include "antenna.h"
#include <stdexcept>
#include <random>

Antenna::Antenna()
{
    g_serverLogger.enableLogFile(true);
}

void Antenna::visualizeData(std::optional<std::pair<std::string, std::string>> p_dataForDL)
{
    std::string plotFileKey;
    std::string binaryCommand;
    std::string frequencyCarrier;
    if (p_dataForDL.has_value())
    {
        plotFileKey = "/plotDL";
        binaryCommand = " --bin " + p_dataForDL.value().first;
        frequencyCarrier = " --fc " + p_dataForDL.value().second;
    }
    else
    {
        plotFileKey = "/plotUL";
        binaryCommand = "";
        frequencyCarrier = "";
    }
    std::map<std::string, std::string> dataKeys = {
        {"inputNoise", "/inputNoise"},
        {"inputFiltered", "/inputFiltered"},
        {"output", "/output"},
        {"fs", "/fs"},
        {"plotFile", plotFileKey}};
    std::map<std::string, std::optional<std::string>> dataValues;
    for (const auto &entry : dataKeys)
    {
        dataValues[entry.first] = getValue(entry.second);
    }

    if (!dataValues["inputNoise"].has_value() || !dataValues["inputFiltered"].has_value()  || !dataValues["output"].has_value() ||
        !dataValues["fs"].has_value() || !dataValues["plotFile"].has_value())
    {
        g_serverLogger.error("Error missing data for visualization.");
        return;
    }

    std::string str = "python3 " + dataValues["plotFile"].value() + " --output " + dataValues["output"].value() + " --inputNoise " \
    + dataValues["inputNoise"].value()+ " --inputFiltered " + dataValues["inputFiltered"].value() \
     + " --fs " + dataValues["fs"].value() +\
     binaryCommand + frequencyCarrier;
    const char *command = str.c_str();
    int SUCCESS = 0;
    int returnCode = system(command);
    if (returnCode == SUCCESS)
    {
        g_serverLogger.info("Calling system call to visualize data successfully");
    }
    else
    {
        g_serverLogger.error("Error when calling system call to visualize data.");
    }
}

std::optional<std::string> Antenna::getValue(const std::string &p_key)
{
    try
    {
        auto resVariant = InMemDatabase::getInstance().getValue(p_key);
        const char *value = "";
        extractValue(resVariant, value);
        return std::string(value);
    }
    catch (const DBException &e)
    {
        g_serverLogger.error(stringify("Error when loading database: ", e.what()));
        return std::nullopt;
    }
}

std::string Antenna::randomBinaryMessageGenerator(const int p_length)
{
    std::string binaryMessage;
    srand(time(0));
    for (int index = 0; index < p_length; index++)
    {
        binaryMessage += (rand() % 2 + '0');
    }
    std::cout << "Generated data: " << binaryMessage << std::endl;
    return binaryMessage;
}

void Antenna::addNoise(std::vector<double> &p_signal)
{
    std::default_random_engine generator(time(0));
    std::normal_distribution<double> distribution(0.0, NOISE_LEVEL);
    for (double &i : p_signal)
    {
        i += distribution(generator);
    }
}

void Antenna::filterNoise(std::vector<double> &p_signal)
{
    if (p_signal.empty())
        return;
    double prevValue = p_signal[0];
    double currentValue = 0.0;
    // Using Exponential Moving Average Filter
    for (size_t i = 1; i < p_signal.size(); ++i)
    {
        currentValue = p_signal[i];
        p_signal[i] = NOISE_FILTER_ALPHA * currentValue + (1 - NOISE_FILTER_ALPHA) * prevValue;
        prevValue = currentValue;
    }
}
