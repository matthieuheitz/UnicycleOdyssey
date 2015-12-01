#include <irrlicht.h>
#include <iostream>

#include "irrlichtDebug.hpp"

using namespace irr;

namespace ic = irr::core;
namespace is = irr::scene;
namespace iv = irr::video;

// Prototypes
void drawAxes(irr::video::IVideoDriver * driver);

// Event managing class
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

  // Global variables
  float backgroundSpeed = 2.0;
  float roadLength = 100;
  float roadWidth = 5;

  // Initialize the camera
  is::ICameraSceneNode *camera = smgr->addCameraSceneNodeFPS(0,50.0f,0.02f);
  camera->setTarget(ic::vector3df(2.5,0,3));
  camera->setPosition(ic::vector3df(2.5,2.0,0));

  // Load the ground
  is::IMesh * groundMesh = loadIMeshFromOBJ(smgr, "data/ground.obj");
  iv::ITexture * groundTex = driver->getTexture("data/Bois.png");

  // Create nodes for the ground
  is::IMeshSceneNode * groundNode = smgr->addMeshSceneNode(groundMesh);
  is::ISceneNodeAnimator * groundAnimator =
          smgr->createFlyStraightAnimator(ic::vector3df(0,0,-1),
                                          ic::vector3df(0,0,-11),
                                          roadLength/10.0f/backgroundSpeed*1000,
                                          true
                                          );
  // Initialize the mesh node
  groundNode->setPosition(ic::vector3df(0,0,0));
  groundNode->setScale(ic::vector3df(roadWidth,1,roadLength));
  groundNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
  groundNode->setMaterialTexture(0, groundTex);
  groundNode->getMaterial(0).getTextureMatrix(0).setTextureScale(roadLength, 1);
  groundNode->addAnimator(groundAnimator);


  while(device->run())
  {
    driver->beginScene(true, true, iv::SColor(0,250,255,255));

    // Draw Axes
    drawAxes(driver);


    // Draw the scene
    smgr->drawAll();

    driver->endScene();
  }
  device->drop();

  return 0;
}

void drawAxes(video::IVideoDriver *driver)
{
    iv::SMaterial lineMaterial;
    lineMaterial.Thickness = 2;
    lineMaterial.setFlag(irr::video::EMF_LIGHTING, false);
    driver->setMaterial(lineMaterial);
    driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);
    driver->draw3DLine(ic::vector3df(0,0,0),ic::vector3df(1,0,0),iv::SColor(0,255,0,0));
    driver->draw3DLine(ic::vector3df(0,0,0),ic::vector3df(0,1,0),iv::SColor(0,0,255,0));
    driver->draw3DLine(ic::vector3df(0,0,0),ic::vector3df(0,0,1),iv::SColor(0,0,0,255));
}
