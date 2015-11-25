#include <irrlicht.h>

using namespace irr;

namespace ic = irr::core;
namespace is = irr::scene;
namespace iv = irr::video;

struct MyEventReceiver : IEventReceiver
{
  bool OnEvent(const SEvent &event)
  {
    // If input is of keyboard type  (KEY_INPUT)
    // and a pressed key
    // and the key is ESCAPE
    if (event.EventType == EET_KEY_INPUT_EVENT &&
        event.KeyInput.PressedDown &&
        event.KeyInput.Key == KEY_ESCAPE)
      exit(0);

    return false;
  }
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
//  is::IAnimatedMesh *mesh = smgr->getMesh("data/tris.md2");

//  // Creating node from mesh
//  is::IAnimatedMeshSceneNode *node = smgr->addAnimatedMeshSceneNode(mesh);


  is::ICameraSceneNode *camera = smgr->addCameraSceneNodeMaya();
//  camera->setTarget(node->getPosition());

  while(device->run())
  {
    driver->beginScene(true, true, iv::SColor(0,50,100,255));

    // Draw the scene
    smgr->drawAll();
    //
    driver->endScene();
  }
  device->drop();

  return 0;
}
