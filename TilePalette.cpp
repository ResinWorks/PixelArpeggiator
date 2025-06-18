#include "ColorPanel.hpp"
#include "UIHelper.hpp"
#include <algorithm>
#include <sstream>
#include "TilePalette.hpp"


void TilePalette::selectPattern(int index) {
    if (index >= 0 && index < static_cast<int>(patterns.size())) {
        selectedIndex = index;
    }
}