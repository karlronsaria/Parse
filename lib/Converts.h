#pragma once
#ifndef _CONVERTS_H_
#define _CONVERTS_H_

namespace Converts {
    // link:
    //   https://stackoverflow.com/questions/61047308/efficiently-convert-two-integers-x-and-y-into-the-float-x-y
    //   https://stackoverflow.com/users/1593077/einpoklum
    // retrieved: 2022_10_04
    inline float ToFloat(int x, int y) {
        float fy (y);
        if (y == 0)  { return float(x); }
        if (y >= 1e9) { return float(x + fy * 1e-10f); }
        if (y >= 1e8) { return float(x + fy * 1e-9f);  }
        if (y >= 1e7) { return float(x + fy * 1e-8f);  }
        if (y >= 1e6) { return float(x + fy * 1e-7f);  }
        if (y >= 1e5) { return float(x + fy * 1e-6f);  }
        if (y >= 1e4) { return float(x + fy * 1e-5f);  }
        if (y >= 1e3) { return float(x + fy * 1e-4f);  }
        if (y >= 1e2) { return float(x + fy * 1e-3f);  }
        if (y >= 1e1) { return float(x + fy * 1e-2f);  }
                        return float(x + fy * 1e-1f); 
    }
};

#endif