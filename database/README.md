# A Simple Database Library used for Radio 5G Internship project

## **1. What is Database (DB) in general?**

A database is **information that's set up for easy access, management and updating.** Computer databases typically store aggregations of data records or files that contain information such as sales transactions, customer data, financials and product information.

Databases are **used for storing, maintaining and accessing any sort of data.** They collect information on people, places or things. This information is gathered in one place so it can be
observed and analyzed. Databases can be thought of as an organized collection of information.

Databases are classified into **different types**:

- **Relational database**
- **NoSQL database**
- Distributed database
- Object-oriented database
- Graph database
- Cloud database
- Open source database
- Data warehouse

## **2. DB concept in the scope of this project**

In this project, we need a simple database to **load data stored in text (.txt) files into Random Access Memory (RAM)**, and from there, we can **retrieve, modify data and write it back** to the corresponding file where data are originally stored.

## **3. In-memory database (InMemDatabase) class usage**

For using the database, the dev/user will need to take a look at the InMemDatabase.h. It provides various methods/APIs that allow manipulating the data stored in the memory, such as:

- `getInstance()`
    - Usage: the function that allows retrieving database singleton object. Should be used before invoking any other methods.
    - Example: if you want to invoke the init function, it should be: `InMemDatabase::getInstance().init(path);`
    - Parameters: none
    - Return type: `InMemDatabase&`

- `init(std::string)`: 
    - Usage: clear the current memory in the database and load the new data from the given path.
    - Parameters: path (`std::string`)
    - Return type: `void`
    - Possible exception types:`DBException`

- `get(std::string)`: 
    - Usage: get the string represents the record of the given key. The string is formatted for
    output to the console.
    - Parameters: key (`std::string`)
    - Return type: `std::string`
    - Possible exception types:`DBException`
 
- `modify(const std::string &, const std::string &, std::string, const bool)`:
    - Usage: write the new value on the record with given `key`. If `isForce` is `true`, also save that change on the corresponding lines.
    - Parameters: key (`std::string`), type (`std::string`), value (`std::string`), isForce (`bool`, is `false` by default)
    - Return type: `std::string`
    - Possible exception types: `DBException`

- `save()`:
    - Usage: save all the changes on the memory to their corresponding files.
    - Parameters: none
    - Return type: none
    - Possible exception types: `DBException`

- `getAll()`:
    - Usage: retrieve all the records loaded on the database.
    - Parameters: none
    - Return type: `std::vector<std::string>`
    - Possible exception types: none

- `getValue(const std::string &)`:
    - Usage: retrieve the real value of the record. 
    - Parameters: key (`const std::string &`)
    - Return type: `std::variant`
    - Possible exception types: `DBException`

- `template <typename T> extractValue(std::variant, T&)`:
    - Usage: helper function that extract the right type from the variant return by `getValue(const std::string &)`.
    - Parameters: var (`std::variant`), valueStore(`T`)
    - Return type: `void`
    - Possible exception types: none

## **4. Simple command line interface (CLI) to interact with the DB (for demo)**

There are 9 valid command lines:

- clear - clear the screen
- help - show help
- db get all - get all data
- db get \<key\> - get data by key
- db write [ -f ] \<key\> \<type\> \<value\> - modify data by key
- exit - exit the program (Can't use Ctrl + V to exit program)


## **5. References**

- [_What is a database (DB) | Definition from TechTarget_](https://www.techtarget.com/searchdatamanagement/definition/database)
- [_What is a Database | Coursera_](https://www.coursera.org/articles/what-is-database?utm_source=gg&utm_medium=sem&utm_campaign=b2c_apac_career-academy_coursera_ftcof_professional-certificates_arte_aug-24_dr_geo-set-2-multi-audience_pmax_gads_lg-all&utm_content=b2c&campaignid=21573875733&adgroupid=&device=c&keyword=&matchtype=&network=x&devicemodel=&adpostion=&creativeid=&hide_mobile_promo&gad_source=1&gclid=EAIaIQobChMIxrLK3cDGiQMVw6tmAh2DwwYgEAAYASAAEgISgfD_BwE)
