

#include "Simulator.hpp"



int main()
{
    IY::Simulator* simulator = new IY::Simulator(1200, 800, "Clouds");
    
    IY::Viewer3D* viewer3D = new IY::Viewer3D(1, 1200, 800);
    // IY::Viewer2D* viewer2D_1 = new IY::Viewer2D(2, 100, 100);
    // IY::Viewer2D* viewer2D_2 = new IY::Viewer2D(3, 100, 100);

    simulator->setViewer(viewer3D);
    // simulator->setViewer(viewer2D_1);
    // simulator->setViewer(viewer2D_2);
    
    simulator->setTap("Volume");
    
    simulator->addSliderFUI("Volume X", &viewer3D->volumeScaleX, 0.1f, 50.f, 0.01f);
    simulator->addSliderFUI("Volume Y", &viewer3D->volumeScaleY, 0.1f, 50.f, 0.01f);
    simulator->addSliderFUI("Volume Z", &viewer3D->volumeScaleZ, 0.1f, 50.f, 0.01f);

    simulator->addSliderFUI("r1", &viewer3D->r1, 0.1f, 10.f, 0.01f);
    simulator->addSliderFUI("r2", &viewer3D->r2, 0.1f, 10.f, 0.01f);
    
    simulator->setTap("Noise");

    simulator->addSliderIUI("Octaves", &viewer3D->octave, 1, 16, 1);
    simulator->addSliderFUI("Frequency", &viewer3D->frequency, 0.1f, 10.f, 0.1f);
    simulator->addSliderFUI("H", &viewer3D->H, -1.f, 0.f, 0.001f);
    
    simulator->setTap("Sampling");

    simulator->addSliderFUI("Step Size", &viewer3D->rayStep, 0.001f, 0.5f, 0.001f);
    simulator->addSliderIUI("Max Step", &viewer3D->maxStep, 64, 500, 1);
    simulator->addSliderFUI("Light Step Size", &viewer3D->lightStep, 0.001f, 0.5f, 0.001f);
    simulator->addSliderIUI("Max Light Step", &viewer3D->maxLightStep, 3, 10, 1);

    simulator->setTap("Clouds");

    simulator->addSliderFUI("coverage", &viewer3D->coverage, 0.001f, 1.f, 0.001f);

    simulator->setTap("Lighting");

    simulator->addSliderFUI("Position X", &viewer3D->light.position.x, -10.f, 10.f, 0.1f);
    simulator->addSliderFUI("Position Y", &viewer3D->light.position.y, -10.f, 10.f, 0.1f);
    simulator->addSliderFUI("Position Z", &viewer3D->light.position.z, -10.f, 10.f, 0.1f);

    simulator->addSliderFUI("Color r", &viewer3D->light.color.r, 0.f, 1.f, 0.01f);
    simulator->addSliderFUI("Color g", &viewer3D->light.color.g, 0.f, 1.f, 0.01f);
    simulator->addSliderFUI("Color b", &viewer3D->light.color.b, 0.f, 1.f, 0.01f);

    simulator->addSliderFUI("absorption", &viewer3D->absorption, 0.001f, 10.f, 0.001f);
    simulator->addSliderFUI("lightAbsorptionToSun", &viewer3D->lightAbsorptionToSun, 0.001f, 10.f, 0.001f);
    simulator->addSliderFUI("g", &viewer3D->g, 0.f, 1.f, 0.01f);


    simulator->run();
}
