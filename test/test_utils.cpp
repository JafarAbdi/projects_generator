#include <gtest/gtest.h>

#include <utils.hpp>

TEST(UTILS, GetCurrentYear) { EXPECT_EQ(2021, getCurrentYear()); }

TEST(UTILS, UnderscoreToDash) {
  EXPECT_EQ("project-generator", underscoreToDash("project_generator"));
  EXPECT_EQ("project-generator", underscoreToDash("project-generator"));
  EXPECT_EQ("project-generator--", underscoreToDash("project_generator__"));
  EXPECT_EQ("--project-generator--", underscoreToDash("__project_generator__"));
  EXPECT_EQ("project", underscoreToDash("project"));
  EXPECT_EQ("--", underscoreToDash("__"));
}

TEST(UTILS, CamelCaseToSnakeCase) {
  EXPECT_EQ("project_generator", camelCaseToSnakeCase("ProjectGenerator"));
  EXPECT_EQ("project_generator", camelCaseToSnakeCase("projectGenerator"));
  EXPECT_EQ("projectgenerator", camelCaseToSnakeCase("projectgenerator"));
}

TEST(UTILS, capitalizeWordsWithWhitespace) {
  EXPECT_EQ("Project Generator", capitalizeWordsWithWhitespace("ProjectGenerator"));
  EXPECT_EQ("Project Generator", capitalizeWordsWithWhitespace("projectGenerator"));
  EXPECT_EQ("Projectgenerator", capitalizeWordsWithWhitespace("projectgenerator"));
  EXPECT_EQ("Project Generator", capitalizeWordsWithWhitespace("project-generator"));
  EXPECT_EQ("Project Generator", capitalizeWordsWithWhitespace("project_generator"));
  EXPECT_EQ("", capitalizeWordsWithWhitespace(""));
  EXPECT_EQ("P", capitalizeWordsWithWhitespace("p"));
  EXPECT_EQ("P", capitalizeWordsWithWhitespace("P"));
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
