#include <calib.h>
#include <iostream>

int main(int argc, char *argv[])
{
    if(argc<2) return 0 ;
    std::shared_ptr<OneshotCalib> calib = OneshotCalib::create(
        argv[1],
        CameraType::CameraSensorType::Pinhole,
        CameraType::CameraNumType::Monocular,
        20
    );
    calib->run();

    return 0;
}