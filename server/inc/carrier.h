#pragma once
#include <string>
#include "serverCommon.h"

class Carrier
{
private:
	bool m_flagCarrier;
	std::string m_network;
	size_t m_frequency;

public:
	/**
	 * @brief Constructor of Carrier
	 */
	Carrier();

	/**
	 * @brief Destructor of Carrier
	 */
	~Carrier() = default;

	/**
	 * @brief function is called to set up network
	 *
	 * @param p_network - network that is set up to transmit and receive data
	 * @return false - The network has been set up and needs to be released, true - set up successfully
	 */
	bool setNetwork(const std::string &p_network);

	/**
	 * @brief function is called to get network
	 *
	 * @return network has been set up
	 */
	std::string getNetwork();

	/**
	 * @brief function is called to set frequency
	 *
	 * @param p_freq - frequency of carrier
	 */
	void setFrequency(const size_t &p_freq);

	/**
	 * @brief function is called to get frequency
	 *
	 * @return frequency has been set up
	 */
	size_t getFrequency();

	/**
	 * @brief function is called to remove old setting of carrier
	 */
	void releaseCarrier();

	/**
	 * @brief function is called to get status of carrier
	 *
	 * @return 0 - Carrier has not been set up, 1 - Carrier has been set up
	 */
	bool getCarrierStatus();

	/**
	 * @brief function is called to check supported carrier network
	 *
	 * @param p_netwwork - network client requested
	 * @return false - server isn't supported, true - server is supported
	 */
	bool checkSupportedCarrier(const std::string &p_network);

	/**
	 * @brief function is called to check supported carrier frequency
	 *
	 * @param p_freq - frequency client requested
	 * @return false - server isn't supported, true - server is supported
	 */
	bool checkSupportedFrequency(const ssize_t &p_freq);
};