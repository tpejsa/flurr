#include <gtest/gtest.h>
#include <flurr.h>

#include <cstdio>
#include <thread>

using flurr::CurrentTime;
using flurr::TrimString;
using flurr::SplitString;
using flurr::FromString;
using flurr::ToString;
using flurr::ConfigFile;
using flurr::Status;
using flurr::FlurrCore;

class FlurrTest : public ::testing::Test {

protected:

  FlurrTest() {
    // You can do set-up work for each test here.
  }

  virtual ~FlurrTest() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }
};

// Test time utils
TEST_F(FlurrTest, FlurrTimeUtils) {
  double start_time = CurrentTime();
  constexpr long long kSleepTime = 1;
  std::this_thread::sleep_for(std::chrono::seconds(kSleepTime));
  EXPECT_TRUE(CurrentTime() - start_time > kSleepTime) << "Current time is off.";
}

// Test string utils
TEST_F(FlurrTest, FlurrStringUtils) {
  // Test string trimming
  EXPECT_TRUE(TrimString(" abc   ") == "abc");
  EXPECT_TRUE(TrimString("  abc") == "abc");
  EXPECT_TRUE(TrimString("abc ") == "abc");
  EXPECT_TRUE(TrimString("abc") == "abc");
  EXPECT_TRUE(TrimString("   ") == "");
  EXPECT_TRUE(TrimString(" ") == "");
  EXPECT_TRUE(TrimString("") == "");

  // Test string splitting
  auto&& tokens = SplitString("abc def ghi", ' ');
  EXPECT_TRUE(tokens.size() == 3 &&
    tokens[0] == "abc" &&
    tokens[1] == "def" &&
    tokens[2] == "ghi");
  tokens = SplitString(" abc def ghi  ", ' ');
  EXPECT_TRUE(tokens.size() == 5 &&
    tokens[0] == "" &&
    tokens[1] == "abc" &&
    tokens[2] == "def" &&
    tokens[3] == "ghi" &&
    tokens[4] == "");
  tokens = SplitString("abc", ' ');
  EXPECT_TRUE(tokens.size() == 1 && tokens[0] == "abc");
  tokens = SplitString("  abc=def  =ghi", '=');
  EXPECT_TRUE(tokens.size() == 3 &&
    tokens[0] == "  abc" &&
    tokens[1] == "def  " &&
    tokens[2] == "ghi");

  // Test string serialization/deserialization
  EXPECT_TRUE(ToString<std::string>("abc def") == "abc def");
  EXPECT_TRUE(ToString(123) == "123");
  EXPECT_TRUE(FromString<std::string>("abc def") == "abc def");
  EXPECT_TRUE(FromString<int>("123") == 123);
  EXPECT_TRUE(FromString<double>(ToString(123.321)) == 123.321);
  EXPECT_TRUE(FromString<bool>(ToString(true)) == true);
}

// Test configuration file reader
TEST_F(FlurrTest, FlurrConfigFile) {
  // Can we read the config file?
  ConfigFile cfg_file;
  EXPECT_TRUE(cfg_file.ReadFromFile("TestFlurr.cfg") == Status::kSuccess);

  // Verify config settings
  int option_int = 0;
  std::string option_str = "";
  float option_float = 0.f;
  bool option_bool = false;
  std::string resource_dir = "";
  bool log_enabled = true;
  std::string renderer_type = "";
  std::string texture_filtering = "";
  int anisotropy_level = 0;
  EXPECT_TRUE(cfg_file.ReadIntValue("", "optionInt", option_int) &&
    123 == option_int);
  EXPECT_TRUE(cfg_file.ReadStringValue("", "optionString", option_str) &&
    "abc" == option_str);
  EXPECT_TRUE(cfg_file.ReadFloatValue("", "optionFloat", option_float) &&
    12.35f == option_float);
  EXPECT_TRUE(cfg_file.ReadBoolValue("", "optionBool", option_bool) &&
    true == option_bool);
  EXPECT_TRUE(cfg_file.ReadStringValue("General", "resourceDirectory", resource_dir) &&
    "./resources" == resource_dir);
  EXPECT_TRUE(cfg_file.ReadBoolValue("General", "logEnabled", log_enabled) &&
    false == log_enabled);
  EXPECT_TRUE(cfg_file.ReadStringValue("Renderer", "rendererType", renderer_type) &&
    "OGL3" == renderer_type);
  EXPECT_TRUE(cfg_file.ReadStringValue("Renderer", "textureFiltering", texture_filtering) &&
    "Anisotropic" == texture_filtering);
  EXPECT_TRUE(cfg_file.ReadIntValue("Renderer", "anisotropyLevel", anisotropy_level) &&
    16 == anisotropy_level);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  std::getchar();
  return result;
}
