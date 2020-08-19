#ifndef SURRY_HEADER_FILE
#define SURRY_HEADER_FILE
    #include "glbl.h"
    #include "rays.h"
    #include "canvas.h"
    #include "colours.h"

    typedef struct SR_3DScene {
        // TODO
    } SR_3DScene;

    typedef struct SR_3DCamera {
        float x; float pitch;
        float y; float yaw;
        float z; float roll;
        SR_Canvas *canvas;
        SR_3DScene *scene;
    } SR_3DCamera;
#endif