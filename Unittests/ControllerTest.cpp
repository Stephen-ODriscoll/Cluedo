#include "stdafx.h"
#include "../Cluedo/Controller.h"

struct ControllerTest : testing::Test
{
    void SetUp()
    {
        Controller controller(Mode::SINGLE, 6);
    }
};