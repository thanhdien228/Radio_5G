#include "antenna.h"
#include <gtest/gtest.h>
#include "inMemDatabase.h"
#include <regex>
/// @brief Helper function to initialize DB for testing environment
void initDBForTestingEnvironment()
{
    std::string path = "/home/vagrant/RadioXFTInternshipSeason40/database/db";
    try
    {
        InMemDatabase::getInstance().init(path);
        std::cout << "Load successfully" << std::endl;
        return;
    }
    catch (DBException e)
    {
        std::cout << e.what() << std::endl;
        return;
    }
}
/// @brief Testing environment class for carrier to be able to work with Database
class AntennaTestingEnvironment : public ::testing::Environment
{
public:
    void SetUp() override
    {
        initDBForTestingEnvironment();
    }
    void TearDown() override
    {
        std::cout << "Testing environment is torn down successfully" << std::endl;
    }
    Antenna AntennaObject;
};

/// @brief Test antenna generate a right format binary and length of the input
TEST(AntennaTest, genBinaryTest)
{
    Antenna AntennaObject;
    const std::regex pattern("^[0-1]{1,}$");
    //Test the result is a binary
    EXPECT_EQ(std::regex_match(AntennaObject.randomBinaryMessageGenerator(5), pattern), true);
    //Test the length of the result
    EXPECT_EQ(AntennaObject.randomBinaryMessageGenerator(5).length(), 5);
    EXPECT_NE(AntennaObject.randomBinaryMessageGenerator(5).length(), 6);
}
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new AntennaTestingEnvironment);
    return RUN_ALL_TESTS();
}
