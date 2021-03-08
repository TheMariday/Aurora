#include <math.h>
#include "lib/color.h"
#include "lib/effect.h"
#include "lib/effect_runner.h"
#include "lib/noise.h"

class MyEffect : public Effect
{
public:
    MyEffect()
        : cycle(0) {}

    float cycle;

    virtual void beginFrame(const FrameInfo& f)
    {
        const float speed = 10.0;
        cycle = fmodf(cycle + f.timeDelta * speed, 2 * M_PI);
    }

    virtual void shader(Vec3& rgb, const PixelInfo& p) const
    {
        float distance = len(p.point);
        float wave = sinf(3.0 * distance - cycle) + noise3(p.point);
        hsv2rgb(rgb, 0.2, 0.3, wave);

        //rgb[0] = 255;
        //rgb[1] = 255;
        //rgb[2] = 255;
    }
};

int main(int argc, char** argv)
{
    printf("herro!\n");
    EffectRunner r;

    MyEffect e;
    r.addEffect(&e);

    // Defaults, overridable with command line options
    r.setMaxFrameRate(100);
  
    int success = r.setLayout("../../../data/layout/singleEye.json");

    return r.main();
}