# A Logging System used for Radio 5G Internship project

## **1. What is Logging System in general?**

The logging system is **an extremely important part to monitor the behavior of the application, identify issues, and comprehend the system's performance**. In C++, we may construct a straightforward yet incredibly efficient logging system to record and capture different data and events that take place while a program is running.

Designing Consideration for a Logging System:

- Logging Levels
- Final Destinations
- Context and Timestamps
- Setup

## **2. Logging System concept in the scope of this project**

In this project, we need a simple logging system to **record of noteworthy occurrences, user actions, or system activity, logging makes auditing and compliance easier**, and from there, Logs may **track the problems, monitor program behavior, and locate performance bottlenecks**.

## **3. Logging system class usage**

For using the Logging , the dev/user will need to take a look at the logger.h. It provides various basic methods of a logging system and it should include the following functionalities to implement a logging system, such as:

- `enableLogFile()`
    - Usage: Set `m_saveLogToFile` to a new boolean value and p_flag to the boolean value that is assigned to `m_saveLogToFile`.    
    - Parameters: flag (`const bool`)
    - Return type: `void`
    - Possible exception types: none

- `setPriority()`: 
    - Usage: get the string represents the record of the given key. The string is formatted for
    output to the console.
    - Parameters: enum (`LogPriority`)
    - Return type: `static void`
    - Possible exception types: none
 
- `trace(const std::string, const Log::sourceInfo)`:
    - Usage: specific log priority level Trace.
    - Parameters: string (`std::string`), Log (`namespace`).
    - Return type: `void`
    - Possible exception types: none

- `debug(const std::string, const Log::sourceInfo)`:
    - Usage: specific log priority level Debug.
    - Parameters: string (`std::string`), Log (`namespace`).
    - Return type: `void`
    - Possible exception types: none

- `info(const std::string, const Log::sourceInfo)`:
    - Usage: specific log priority level Info
    - Parameters: string (`std::string`), Log (`namespace`).
    - Return type: `void`
    - Possible exception types: none

- `warning(const std::string, const Log::sourceInfo)`:
    - Usage: specific log priority level Warning
    - Parameters: string (`std::string`), Log (`namespace`).
    - Return type: `void`
    - Possible exception types: none

- `error(const std::string, const Log::sourceInfo)`:
    - Usage: specific log priority level Error
    - Parameters: string (`std::string`), Log (`namespace`).
    - Return type: `void`
    - Possible exception types: none

- `fatal(const std::string, const Log::sourceInfo)`:
    - Usage: specific log priority level Fatal
    - Parameters: string (`std::string`), Log (`namespace`).
    - Return type: `void`
    - Possible exception types: none

## **4. References**

- [_What is a Logging System | Definition from GeeksForGeeks_](https://www.geeksforgeeks.org/logging-system-in-cpp/)
