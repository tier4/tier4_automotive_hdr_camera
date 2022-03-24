#include <gtest/gtest.h>

#include <t4cam_tools.hpp>

TEST(TestSetDigitalGain, over)
{
  C1 c1_a = C1(PORT_A_CAM);
  EXPECT_EQ(0, c1_a.setDigitalGain(30));

  EXPECT_EQ(-1, c1_a.setDigitalGain(300));
  EXPECT_EQ(-1, c1_a.setDigitalGain(-1));
}
