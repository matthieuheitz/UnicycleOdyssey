#include <irrlicht.h>

using namespace irr;

namespace ic = irr::core;
namespace is = irr::scene;
namespace iv = irr::video;

class MyEventReceiver : public IEventReceiver
{
public:
    bool OnEvent(const SEvent &event)
    {
        // If input is of keyboard type  (KEY_INPUT)
        // and a pressed key
        // and the key is ESCAPE
        if (event.EventType == EET_KEY_INPUT_EVENT &&
                event.KeyInput.PressedDown &&
                event.KeyInput.Key == KEY_ESCAPE)
            exit(0);
        if (event.EventType == irr::EET_KEY_INPUT_EVENT)
            KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;
        return false;
    }

    virtual bool IsKeyDown(EKEY_CODE keyCode) const
    {
        return KeyIsDown[keyCode];
    }
    MyEventReceiver()
    {
        for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
            KeyIsDown[i] = false;
    }
private:
    //store the state of each key
    bool KeyIsDown[KEY_KEY_CODES_COUNT];
};

int main()
{
    // Event Receiver
    MyEventReceiver receiver;
    // Initialization of the rendering system and window
    IrrlichtDevice *device = createDevice(iv::EDT_OPENGL,
                                          ic::dimension2d<u32>(640, 480),
                                          16, false, false, false, &receiver);

    iv::IVideoDriver  *driver = device->getVideoDriver();
    is::ISceneManager *smgr = device->getSceneManager();

    //  // Loading a mesh
    is::IAnimatedMesh *mesh = smgr->getMesh("data/test_with_bones_and_skinweights_and_modif.x");

    //  // Creating node from mesh
    is::IAnimatedMeshSceneNode *node = smgr->addAnimatedMeshSceneNode(mesh);
    //ic::vector3df scale(0.005,0.005,0.005);
    //node->setScale( scale );

    is::ICameraSceneNode *camera = smgr->addCameraSceneNodeFPS();
    //camera->setTarget(node->getPosition());
    //is::ICameraSceneNode *camera = smgr->addCameraSceneNode(node, ic::vector3df(0,0,0),ic::vector3df(0,0,5),-1, true);

    // In order to do framerate independent movement, we have to know
    // how long it was since the last frame
    u32 then = device->getTimer()->getTime();

    // This is the movemen speed in units per second.
    const f32 movementSpeed = 5.f;

    while(device->run())
    {
        const u32 now = device->getTimer()->getTime();
        const f32 frameDeltaTime = (f32)(now - then) / 1000.f; // Time in seconds
        int lastFPS = -1;
        then = now;
        driver->beginScene(true, true, iv::SColor(0,50,100,255));

        core::vector3df nodePosition = node->getPosition();

        if(receiver.IsKeyDown(irr::KEY_KEY_Q))
            nodePosition.X -= movementSpeed * frameDeltaTime;
        else if(receiver.IsKeyDown(irr::KEY_KEY_D))
            nodePosition.X += movementSpeed * frameDeltaTime;

        node->setPosition(nodePosition);

        // Draw the scene
        smgr->drawAll();
        driver->endScene();
        int fps = driver->getFPS();
        if (lastFPS != fps)
        {
            core::stringw tmp(L"Movement Example - Irrlicht Engine [");
            tmp += driver->getName();
            tmp += L"] fps: ";
            tmp += fps;

            device->setWindowCaption(tmp.c_str());
            lastFPS = fps;
        }
    }
    device->drop();

    return 0;
}
