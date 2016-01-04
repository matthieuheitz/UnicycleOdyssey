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
  // Initialize random seed
  srand (time(NULL));

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
          smgr->createFlyStraightAnimator(ic::vector3df(0,0,0),
                                          ic::vector3df(0,0,-24),
                                          roadLength/10.0f/backgroundSpeed*1000*2,
                                          true
                                          );
  // Initialize the mesh node
  groundNode->setPosition(ic::vector3df(0,0,0));
  groundNode->setScale(ic::vector3df(roadWidth,1,roadLength));
  groundNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
  groundNode->setMaterialTexture(0, groundTex);
//  groundNode->getMaterial(0).getTextureMatrix(0).setTextureScale(roadLength, 1);
  groundNode->addAnimator(groundAnimator);
    
  // Loading a character mesh
  is::IAnimatedMesh *mesh_character = smgr->getMesh("data/character.x");
  // Creating node from mesh
  is::IAnimatedMeshSceneNode *node_character = smgr->addAnimatedMeshSceneNode(mesh_character);
  ic::vector3df scale(0.19,0.19,0.19 );
  node_character->setScale( scale );
  node_character->setRotation(ic::vector3df(0,180,0));
  node_character->setPosition(ic::vector3df(3,0.2,3.0));
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
  ic::vector3df scale_bike(1,1,1);
  node_bike->setScale( scale_bike );
  node_bike->setRotation(ic::vector3df(-90,90,0));
  node_bike->setPosition(ic::vector3df(3,0.2,3.35));
  node_bike->setMaterialFlag(video::EMF_LIGHTING, false);

  int state_left_arm = 0; // 0 for rest position, -1 for down, +1 for up
  int state_right_arm = 0;

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
                                          false
                                          );
  is::ISceneNodeAnimator * middleWallAnimator =
          smgr->createFlyStraightAnimator(ic::vector3df(3,1,24),
                                          ic::vector3df(3,1,0),
                                          roadLength/10.0f/backgroundSpeed*1000*2,
                                          false
                                          );
  is::ISceneNodeAnimator * rightWallAnimator =
          smgr->createFlyStraightAnimator(ic::vector3df(5,1,24),
                                          ic::vector3df(5,1,0),
                                          roadLength/10.0f/backgroundSpeed*1000*2,
                                          false
                                          );
  iv::ITexture * leftWallTex = driver->getTexture("data/Wall_left.png");
  iv::ITexture * middleWallTex = driver->getTexture("data/Wall_middle.png");
  iv::ITexture * rightWallTex = driver->getTexture("data/Wall_right.png");
  iv::ITexture * shapeUUTex = driver->getTexture("data/shapes/Shape_UU_t.png");
  iv::ITexture * shapeUDTex = driver->getTexture("data/shapes/Shape_UD_t.png");
  iv::ITexture * shapeDUTex = driver->getTexture("data/shapes/Shape_DU_t.png");
  iv::ITexture * shapeDDTex = driver->getTexture("data/shapes/Shape_DD_t.png");

  leftWallNode->addAnimator(leftWallAnimator);
  leftWallNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
  leftWallNode->setMaterialTexture(0, leftWallTex);
  leftWallNode->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF);

  leftWallNode->getMaterial(0).getTextureMatrix(0).setTextureScaleCenter(1,0.65);
  leftWallNode->getMaterial(0).getTextureMatrix(0).setTextureTranslate(0,0.15);

  middleWallNode->addAnimator(middleWallAnimator);
  middleWallNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
  middleWallNode->setMaterialTexture(0, shapeDDTex);
  middleWallNode->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF);

  middleWallNode->getMaterial(0).getTextureMatrix(0).setTextureScaleCenter(1,0.65);
  middleWallNode->getMaterial(0).getTextureMatrix(0).setTextureTranslate(0,0.15);

  rightWallNode->addAnimator(rightWallAnimator);
  rightWallNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
  rightWallNode->setMaterialTexture(0, rightWallTex);
  rightWallNode->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF);

  rightWallNode->getMaterial(0).getTextureMatrix(0).setTextureScaleCenter(1,0.65);
  rightWallNode->getMaterial(0).getTextureMatrix(0).setTextureTranslate(0,0.15);

  while(device->run())
  {
    driver->beginScene(true, true, iv::SColor(0,250,255,255));

    // Draw Axes
    drawAxes(driver);

    core::vector3df nodePosition = node_character->getPosition();
    core::vector3df nodeBikePosition = node_bike->getPosition();

    if(leftWallAnimator->hasFinished())
    {
        // Increase speed
        backgroundSpeed += 0.5;

        leftWallAnimator = smgr->createFlyStraightAnimator(
                ic::vector3df(1,1,24),
                ic::vector3df(1,1,0),
                roadLength/10.0f/backgroundSpeed*1000*2,
                false
                );
        middleWallAnimator = smgr->createFlyStraightAnimator(
                ic::vector3df(3,1,24),
                ic::vector3df(3,1,0),
                roadLength/10.0f/backgroundSpeed*1000*2,
                false
                );
        rightWallAnimator = smgr->createFlyStraightAnimator(
                ic::vector3df(5,1,24),
                ic::vector3df(5,1,0),
                roadLength/10.0f/backgroundSpeed*1000*2,
                false
                );
        leftWallNode->addAnimator(leftWallAnimator);
        middleWallNode->addAnimator(middleWallAnimator);
        rightWallNode->addAnimator(rightWallAnimator);

        // Randomly set a shape in a wall
        int wall = rand()%3;
        int shape = rand()%4;
        switch(shape)
        {
        case 0:
            switch(wall)
            {
            case 0:
                leftWallNode->setMaterialTexture(0, shapeUUTex);
                middleWallNode->setMaterialTexture(0, middleWallTex);
                rightWallNode->setMaterialTexture(0, rightWallTex);
                break;

            case 1:
                leftWallNode->setMaterialTexture(0, leftWallTex);
                middleWallNode->setMaterialTexture(0, shapeUUTex);
                rightWallNode->setMaterialTexture(0, rightWallTex);
                break;

            case 2:
                leftWallNode->setMaterialTexture(0, leftWallTex);
                middleWallNode->setMaterialTexture(0, middleWallTex);
                rightWallNode->setMaterialTexture(0, shapeUUTex);
                break;
            }
            break;

        case 1:
            switch(wall)
            {
            case 0:
                leftWallNode->setMaterialTexture(0, shapeDUTex);
                middleWallNode->setMaterialTexture(0, middleWallTex);
                rightWallNode->setMaterialTexture(0, rightWallTex);
                break;

            case 1:
                leftWallNode->setMaterialTexture(0, leftWallTex);
                middleWallNode->setMaterialTexture(0, shapeDUTex);
                rightWallNode->setMaterialTexture(0, rightWallTex);
                break;

            case 2:
                leftWallNode->setMaterialTexture(0, leftWallTex);
                middleWallNode->setMaterialTexture(0, middleWallTex);
                rightWallNode->setMaterialTexture(0, shapeDUTex);
                break;
            }
            break;

        case 2:
            switch(wall)
            {
            case 0:
                leftWallNode->setMaterialTexture(0, shapeUDTex);
                middleWallNode->setMaterialTexture(0, middleWallTex);
                rightWallNode->setMaterialTexture(0, rightWallTex);
                break;

            case 1:
                leftWallNode->setMaterialTexture(0, leftWallTex);
                middleWallNode->setMaterialTexture(0, shapeUDTex);
                rightWallNode->setMaterialTexture(0, rightWallTex);
                break;

            case 2:
                leftWallNode->setMaterialTexture(0, leftWallTex);
                middleWallNode->setMaterialTexture(0, middleWallTex);
                rightWallNode->setMaterialTexture(0, shapeUDTex);
                break;
            }
            break;

        case 3:
            switch(wall)
            {
            case 0:
                leftWallNode->setMaterialTexture(0, shapeDDTex);
                middleWallNode->setMaterialTexture(0, middleWallTex);
                rightWallNode->setMaterialTexture(0, rightWallTex);
                break;

            case 1:
                leftWallNode->setMaterialTexture(0, leftWallTex);
                middleWallNode->setMaterialTexture(0, shapeDDTex);
                rightWallNode->setMaterialTexture(0, rightWallTex);
                break;

            case 2:
                leftWallNode->setMaterialTexture(0, leftWallTex);
                middleWallNode->setMaterialTexture(0, middleWallTex);
                rightWallNode->setMaterialTexture(0, shapeDDTex);
                break;
            }
            break;

        }
    }

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
