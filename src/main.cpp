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

  // Global variables
  // Speeds are in m/s
  // physical coordinates are in meters
  // Times are in seconds
  float backgroundSpeed = 4.0;
  float roadLength = 100;
  float roadWidth = 6;

  // We want the character to cross the road in 1 sec
  float characterTransversalSpeed = roadWidth/1.0;
  float frameDeltaTime = 1/60.0f;

  // Initialize the camera
  is::ICameraSceneNode *camera = smgr->addCameraSceneNodeFPS(0,50.0f,0.02f);
  camera->setTarget(ic::vector3df(roadWidth/2.0, 0, 3));
  camera->setPosition(ic::vector3df(roadWidth/2.0, 1.5, 0));

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
    
  // Loading a mesh
  is::IAnimatedMesh *mesh = smgr->getMesh("data/test_with_bones_and_skinweights_and_modif.x");

  // Creating node from mesh
  is::IAnimatedMeshSceneNode *node = smgr->addAnimatedMeshSceneNode(mesh);
  //ic::vector3df scale(0.005,0.005,0.005);
  //node->setScale( scale );

  // Create walls
  is::IMeshSceneNode * leftWallNode = smgr->addCubeSceneNode(1.0f, 0, -1, core::vector3df(1, 1, 5),
                                                        core::vector3df(0,0,0), core::vector3df(roadWidth/3.0, 2, 0.2));
  is::IMeshSceneNode * middleWallNode = smgr->addCubeSceneNode(1.0f, 0, -1, core::vector3df(3, 1, 5),
                                                        core::vector3df(0,0,0), core::vector3df(roadWidth/3.0, 2, 0.2));
  is::IMeshSceneNode * rightWallNode = smgr->addCubeSceneNode(1.0f, 0, -1, core::vector3df(5, 1, 5),
                                                        core::vector3df(0,0,0), core::vector3df(roadWidth/3.0, 2, 0.2));
  is::ISceneNodeAnimator * leftWallAnimator =
          smgr->createFlyStraightAnimator(ic::vector3df(1,1,24),
                                          ic::vector3df(1,1,0),
                                          roadLength/10.0f/backgroundSpeed*1000*2,
                                          true
                                          );
  is::ISceneNodeAnimator * middleWallAnimator =
          smgr->createFlyStraightAnimator(ic::vector3df(3,1,24),
                                          ic::vector3df(3,1,0),
                                          roadLength/10.0f/backgroundSpeed*1000*2,
                                          true
                                          );
  is::ISceneNodeAnimator * rightWallAnimator =
          smgr->createFlyStraightAnimator(ic::vector3df(5,1,24),
                                          ic::vector3df(5,1,0),
                                          roadLength/10.0f/backgroundSpeed*1000*2,
                                          true
                                          );
//  iv::ITexture * leftWallTex = driver->getTexture("data/Wall_left.png");
//  iv::ITexture * middleWallTex = driver->getTexture("data/Wall_middle.png");
//  iv::ITexture * rightWallTex = driver->getTexture("data/Wall_right.png");
  iv::ITexture * leftWallTex = driver->getTexture("data/shapes/Shape_UU_t.png");
  iv::ITexture * middleWallTex = driver->getTexture("data/shapes/Shape_UD_t.png");
  iv::ITexture * rightWallTex = driver->getTexture("data/shapes/Shape_DD_t.png");

  leftWallNode->addAnimator(leftWallAnimator);
  leftWallNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
  leftWallNode->setMaterialTexture(0, leftWallTex);
  leftWallNode->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF);

  middleWallNode->addAnimator(middleWallAnimator);
  middleWallNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
  middleWallNode->setMaterialTexture(0, middleWallTex);
  middleWallNode->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF);

  rightWallNode->addAnimator(rightWallAnimator);
  rightWallNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
  rightWallNode->setMaterialTexture(0, rightWallTex);
  rightWallNode->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF);

  while(device->run())
  {
    driver->beginScene(true, true, iv::SColor(0,250,255,255));

    // Draw Axes
    drawAxes(driver);

    core::vector3df nodePosition = node->getPosition();

    if(receiver.IsKeyDown(irr::KEY_KEY_Q))
        nodePosition.X -= characterTransversalSpeed * frameDeltaTime;
    else if(receiver.IsKeyDown(irr::KEY_KEY_D))
        nodePosition.X += characterTransversalSpeed * frameDeltaTime;

	node->setPosition(nodePosition);

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
