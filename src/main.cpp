#include <irrlicht.h>
#include <iostream>
#include <math.h>

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
  float backgroundSpeed = 2.0;
  float roadLength = 100;
  float roadWidth = 5;

  float characterTransversalSpeed = 0.2f;
  float frameDeltaTime = 0.05f;

  // Initialize the camera
//  is::ICameraSceneNode *camera = smgr->addCameraSceneNodeFPS(0,50.0f,0.02f);
//  camera->setTarget(ic::vector3df(2.5,0,3));
//  camera->setPosition(ic::vector3df(2.5,2.0,0));
//  camera->setNearValue(0.1);

  is::ICameraSceneNode *camera = smgr->addCameraSceneNode(nullptr, ic::vector3df(0, 30, -40), ic::vector3df(0, 5, 0));
    camera->setTarget(ic::vector3df(2.5,2,3));
    camera->setPosition(ic::vector3df(2.5,2.2,0.3));
    camera->setNearValue(0.1);

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
    
  // Loading a character mesh
  is::IAnimatedMesh *mesh_character = smgr->getMesh("data/character.x");
  // Creating node from mesh
  is::IAnimatedMeshSceneNode *node_character = smgr->addAnimatedMeshSceneNode(mesh_character);
  ic::vector3df scale(0.02,0.02,0.02 );
  node_character->setScale( scale );
  node_character->setRotation(ic::vector3df(0,180,0));
  node_character->setPosition(ic::vector3df(2.5,2,0.5));
  node_character->setMaterialFlag(video::EMF_LIGHTING, false);

  /** Testing texture, to be changed **/
  //node_character->setMaterialTexture( 0, driver->getTexture("data/mountain.jpg") );
  //node_character->setMaterialType( video::EMT_SOLID );
  /** **/

  node_character->setFrameLoop(1, 1);
  node_character->setAnimationSpeed(15);

  // Loading a bike mesh
  is::IAnimatedMesh *mesh_bike = smgr->getMesh("data/bike.x");
  // Creating node from mesh
  is::IAnimatedMeshSceneNode *node_bike = smgr->addAnimatedMeshSceneNode(mesh_bike);
  ic::vector3df scale_bike(0.2,0.2,0.2 );
  node_bike->setScale( scale_bike );
  node_bike->setRotation(ic::vector3df(-90,90,0));
  node_bike->setPosition(ic::vector3df(2.5,2,0.55));
  node_bike->setMaterialFlag(video::EMF_LIGHTING, false);

  int state_left_arm = 0; // 0 for rest position, -1 for down, +1 for up
  int state_right_arm = 0;

  while(device->run())
  {
    driver->beginScene(true, true, iv::SColor(0,250,255,255));

    // Draw Axes
    drawAxes(driver);

    core::vector3df nodePosition = node_character->getPosition();
    core::vector3df nodeBikePosition = node_bike->getPosition();


    if(receiver.IsKeyDown(irr::KEY_KEY_P))
    {
        state_left_arm = 1;
           if(state_right_arm == 0)
           {
               node_character->setFrameLoop(80,80);
           }
           else if(state_right_arm == 1)
           {
               node_character->setFrameLoop(10,10);
           }
           else if(state_right_arm == -1)
           {
               node_character->setFrameLoop(40,40);
           }
    }

    if(receiver.IsKeyDown(irr::KEY_KEY_M))
    {
        state_left_arm = -1;
           if(state_right_arm == 0)
           {
               node_character->setFrameLoop(20,20);
           }
           else if(state_right_arm == 1)
           {
               node_character->setFrameLoop(90,90);
           }
           else if(state_right_arm == -1)
           {
               node_character->setFrameLoop(50,50);
           }
    }

    if(receiver.IsKeyDown(irr::KEY_KEY_I))
    {
        state_right_arm = 1;
           if(state_left_arm == 0)
           {
               node_character->setFrameLoop(70,70);
           }
           else if(state_left_arm == 1)
           {
               node_character->setFrameLoop(10,10);
           }
           else if(state_left_arm == -1)
           {
               node_character->setFrameLoop(90,90);
           }
    }

    if(receiver.IsKeyDown(irr::KEY_KEY_K))
    {
        state_right_arm = -1;
           if(state_left_arm == 0)
           {
               node_character->setFrameLoop(30,30);
           }
           else if(state_left_arm == 1)
           {
               node_character->setFrameLoop(40,40);
           }
           else if(state_left_arm == -1)
           {
               node_character->setFrameLoop(50,50);
           }
    }

    if(receiver.IsKeyDown(irr::KEY_KEY_Q))
    {
        nodePosition.X -= characterTransversalSpeed * frameDeltaTime;
        nodeBikePosition.X -= characterTransversalSpeed * frameDeltaTime;
    }
    else if(receiver.IsKeyDown(irr::KEY_KEY_D))
    {
        nodePosition.X += characterTransversalSpeed * frameDeltaTime;
        nodeBikePosition.X += characterTransversalSpeed * frameDeltaTime;
    }

    node_character->setPosition(nodePosition);
    node_bike->setPosition(nodeBikePosition);

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
