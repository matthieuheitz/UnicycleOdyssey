#include <irrlicht.h>
#include <iostream>
#include <math.h>

#include "irrlichtDebug.hpp"

using namespace irr;

namespace ic = irr::core;
namespace is = irr::scene;
namespace iv = irr::video;
namespace ig = irr::gui;

// Prototypes
void drawAxes(irr::video::IVideoDriver * driver);
// Modify the active wall and shape
void changeWallAndShape(int wallNumber, int shapeNumber,
                   is::IMeshSceneNode* leftWallNode, iv::ITexture* leftWallTex,
                   is::IMeshSceneNode* middleWallNode, iv::ITexture* middleWallTex,
                   is::IMeshSceneNode* rightWallNode, iv::ITexture* rightWallTex,
                   iv::ITexture* shapeUUTex,  iv::ITexture* shapeUDTex,
                   iv::ITexture* shapeDUTex, iv::ITexture* shapeDDTex);

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
  device->setWindowCaption(L"Unicycle Odyssey");
  device->setResizable(false);

  iv::IVideoDriver  *driver = device->getVideoDriver();
  is::ISceneManager *smgr = device->getSceneManager();
  ig::IGUIEnvironment *gui = device->getGUIEnvironment();

  // Global variables
  // Speeds are in m/s
  // physical coordinates are in meters
  // Times are in seconds
  float backgroundSpeed = 4.0;
  float roadLength = 100;
  float roadWidth = 6;
  int armState = 3; // 0 for rest position, 1 for UU, 2 for UD, 3 for DU, 4 for DD
  
  int width = device->getVideoDriver()->getScreenSize().Width;
  int height = device->getVideoDriver()->getScreenSize().Height;

  // We want the character to be able to cross the road from one
  // end to the other in the interval of 2 walls
  float characterTransversalSpeed = roadWidth/(24/backgroundSpeed);
  float frameDeltaTime = 1/60.0f;

  // Current wall and shape chosen
  int wallNumber = 1;
  int shapeNumber = 3;

  // Collisions
  float validWindowLength = 0.4;

  // Initialize the camera
//  is::ICameraSceneNode *camera = smgr->addCameraSceneNodeFPS(0,50.0f,0.02f); // Camera de debug
  is::ICameraSceneNode *camera = smgr->addCameraSceneNode(); // Camera de jeu
  camera->setTarget(ic::vector3df(roadWidth/2.0, 1, 3));
  camera->setPosition(ic::vector3df(roadWidth/2.0, 1.5, 0));

  iv::ITexture *startScreenText;
  iv::ITexture *startButtonText;
  iv::ITexture *gameoverScreenText;

  startScreenText = driver->getTexture("data/startScreen_640x480.png");
  startButtonText = driver->getTexture("data/startButton.png");
  gameoverScreenText = driver->getTexture("data/gameoverScreen.png");

  ig::IGUIImage *imageStartScreen   = gui->addImage(ic::rect<s32>(0,0,  width, height));
  imageStartScreen->setUseAlphaChannel(true);
  imageStartScreen->setImage(startScreenText);
  imageStartScreen->setScaleImage(true);

  ig::IGUIButton *startButton = gui->addButton(ic::rect<s32>(width/2 - 50, height/2 - 50, width/2 + 50, height/2 + 50));
  startButton->setScaleImage(true);
  startButton->setImage(startButtonText);
  startButton->setUseAlphaChannel(true);
  startButton->setDrawBorder(false);

  iv::ITexture *digits[10];
  digits[0] = driver->getTexture("data/0.png");
  digits[1] = driver->getTexture("data/1.png");
  digits[2] = driver->getTexture("data/2.png");
  digits[3] = driver->getTexture("data/3.png");
  digits[4] = driver->getTexture("data/4.png");
  digits[5] = driver->getTexture("data/5.png");
  digits[6] = driver->getTexture("data/6.png");
  digits[7] = driver->getTexture("data/7.png");
  digits[8] = driver->getTexture("data/8.png");
  digits[9] = driver->getTexture("data/9.png");

  ig::IGUIImage *score_10000 = gui->addImage(ic::rect<s32>(10,10,  50,50)); score_10000->setScaleImage(true);
  ig::IGUIImage *score_1000  = gui->addImage(ic::rect<s32>(50,10,  90,50)); score_1000->setScaleImage(true);
  ig::IGUIImage *score_100   = gui->addImage(ic::rect<s32>(90,10,  130,50)); score_100->setScaleImage(true);
  ig::IGUIImage *score_10    = gui->addImage(ic::rect<s32>(130,10, 170,50)); score_10->setScaleImage(true);
  ig::IGUIImage *score_1     = gui->addImage(ic::rect<s32>(170,10, 210,50)); score_1->setScaleImage(true);

  int score = 0;

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




  // Initialize the ground mesh node
  groundNode->setPosition(ic::vector3df(0,0,0));
  groundNode->setScale(ic::vector3df(roadWidth,1,roadLength));
  groundNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
  groundNode->setMaterialTexture(0, groundTex);
//  groundNode->getMaterial(0).getTextureMatrix(0).setTextureScale(roadLength, 1);
  groundNode->addAnimator(groundAnimator);

  // Load sky
  is::IMesh * skyMesh = loadIMeshFromOBJ(smgr, "data/sky.obj");
  iv::ITexture * skyText = driver->getTexture("data/sky.jpg");  

  // Create nodes for the sky
  is::IMeshSceneNode * skyNode = smgr->addMeshSceneNode(skyMesh);
  // Initialize the sky mesh node
  skyNode->setPosition(ic::vector3df(-45,-10,50));
  skyNode->setRotation(ic::vector3df(-90,0,0));
  skyNode->setScale(ic::vector3df(100,100,100));
  skyNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
  skyNode->setMaterialTexture(0, skyText);
//  skyNode->getMaterial(0).getTextureMatrix(0).setTextureScale(roadLength, 1);

  // Load grass
  is::IMesh * grassMesh = loadIMeshFromOBJ(smgr, "data/grass.obj");
  iv::ITexture * grassText = driver->getTexture("data/grass.jpg");  


  is::ISceneNodeAnimator * grassAnimator =
          smgr->createFlyStraightAnimator(ic::vector3df(-50,-0.1,0),
                                          ic::vector3df(-50,-0.1,-24),
                                          roadLength/10.0f/backgroundSpeed*1000*2,
                                          true
                                          );
  // Create nodes for the grass
  is::IMeshSceneNode * grassNode = smgr->addMeshSceneNode(grassMesh);
  // Initialize the grass mesh node
  grassNode->setPosition(ic::vector3df(-50,-0.1,0));
  grassNode->setRotation(ic::vector3df(0,0,0));
  grassNode->setScale(ic::vector3df(100,100,100));
  grassNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
  grassNode->setMaterialTexture(0, grassText);
  grassNode->addAnimator(grassAnimator);
//  skyNode->getMaterial(0).getTextureMatrix(0).setTextureScale(roadLength, 1);
    
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

  node_character->setFrameLoop(50, 50);
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

  int state_left_arm = -1; // 0 for rest position, -1 for down, +1 for up
  int state_right_arm = -1;

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


  bool alreadyChecked = false;
  while(device->run())
  {
    driver->beginScene(true, true, iv::SColor(0,250,255,255));

    // Draw Axes
    drawAxes(driver);

    if(receiver.IsKeyDown(irr::KEY_RETURN))
    {
        startButton->setPressed(true);
    }

    if(startButton->isPressed() == false && startButton->isEnabled() == true)
    {
        gui->drawAll();
        driver->endScene();
    }
    else
    {
        if(startButton->isPressed() == true && startButton->isEnabled() == true)
        {
            startButton->setVisible(false);
            startButton->setEnabled(false);
            imageStartScreen->setVisible(false);
        }
        else
        {
            core::vector3df nodePosition = node_character->getPosition();
            core::vector3df nodeBikePosition = node_bike->getPosition();

            if(leftWallAnimator->hasFinished())
            {
                // Increase speed
                backgroundSpeed += 0.5;
                characterTransversalSpeed = roadWidth/(24/backgroundSpeed);

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
                grassAnimator = smgr->createFlyStraightAnimator(
                            ic::vector3df(-50,-0.1,0),
                            ic::vector3df(-50,-0.1,-24),
                            roadLength/10.0f/backgroundSpeed*1000*2,
                            true
                            );
                groundAnimator = smgr->createFlyStraightAnimator(
                            ic::vector3df(0,0,0),
                            ic::vector3df(0,0,-24),
                            roadLength/10.0f/backgroundSpeed*1000*2,
                            true
                            );
                leftWallNode->addAnimator(leftWallAnimator);
                middleWallNode->addAnimator(middleWallAnimator);
                rightWallNode->addAnimator(rightWallAnimator);
                grassNode->addAnimator(grassAnimator);
                groundNode->addAnimator(groundAnimator);

                // Randomly set a shape in a wall
                wallNumber = rand()%3;
                shapeNumber = rand()%4;
                changeWallAndShape(wallNumber, shapeNumber,
                                   leftWallNode, leftWallTex,
                                   middleWallNode, middleWallTex,
                                   rightWallNode, rightWallTex,
                                   shapeUUTex, shapeUDTex,
                                   shapeDUTex, shapeDDTex);
                alreadyChecked = false;
            }

            if(receiver.IsKeyDown(irr::KEY_KEY_P))
            {
                state_left_arm = 1;
                if(state_right_arm == 1)
                {
                    node_character->setFrameLoop(10,10);
                    armState = 0;
                }
                else if(state_right_arm == -1)
                {
                    node_character->setFrameLoop(40,40);
                    armState = 1;
                }
            }

            if(receiver.IsKeyDown(irr::KEY_KEY_M))
            {
                state_left_arm = -1;
                if(state_right_arm == 1)
                {
                    node_character->setFrameLoop(90,90);
                    armState = 2;
                }
                else if(state_right_arm == -1)
                {
                    node_character->setFrameLoop(50,50);
                    armState = 3;
                }
            }

            if(receiver.IsKeyDown(irr::KEY_KEY_I))
            {
                state_right_arm = 1;
                if(state_left_arm == 1)
                {
                    node_character->setFrameLoop(10,10);
                    armState = 0;
                }
                else if(state_left_arm == -1)
                {
                    node_character->setFrameLoop(90,90);
                    armState = 2;
                }
            }

            if(receiver.IsKeyDown(irr::KEY_KEY_K))
            {
                state_right_arm = -1;
                if(state_left_arm == 1)
                {
                    node_character->setFrameLoop(40,40);
                    armState = 1;
                }
                else if(state_left_arm == -1)
                {
                    node_character->setFrameLoop(50,50);
                    armState = 3;
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

            if(leftWallNode->getPosition().Z > 3.3 && leftWallNode->getPosition().Z < 4)
            {
                if(!alreadyChecked)
                {
                    // Check position
                    if(nodeBikePosition.X < 2*wallNumber + 1 - validWindowLength/2.0
                            || nodeBikePosition.X > 2*wallNumber + 1 + validWindowLength/2.0
                            || shapeNumber != armState)
                    {
                        ig::IGUIImage *imageGameoverScreen   = gui->addImage(ic::rect<s32>(0,0,  width, height));
                        imageGameoverScreen->setUseAlphaChannel(true);
                        imageGameoverScreen->setImage(gameoverScreenText);
                        imageGameoverScreen->setScaleImage(true);
                        gui->drawAll();
                        driver->endScene();
                    }
                    else
                    {
                        score++;
                    }
                    alreadyChecked = true;
                }
            }
            // Calcul du score :
            if (score == 50000) score = -1;
            // Mise à jour du score :
            score_10000->setImage(digits[(score / 10000) % 10]);
            score_1000->setImage(digits[(score / 1000) % 10]);
            score_100->setImage(digits[(score / 100) % 10]);
            score_10->setImage(digits[(score / 10) % 10]);
            score_1->setImage(digits[(score / 1) % 10]);
            // Draw the scene
            smgr->drawAll();
        }
        gui->drawAll();
        driver->endScene();
    }
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

void changeWallAndShape(int wallNumber, int shapeNumber,
                   is::IMeshSceneNode* leftWallNode, iv::ITexture* leftWallTex,
                   is::IMeshSceneNode* middleWallNode, iv::ITexture* middleWallTex,
                   is::IMeshSceneNode* rightWallNode, iv::ITexture* rightWallTex,
                   iv::ITexture* shapeUUTex,  iv::ITexture* shapeUDTex,
                   iv::ITexture* shapeDUTex, iv::ITexture* shapeDDTex)
{
    switch(shapeNumber)
    {
    case 0:
        switch(wallNumber)
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
        switch(wallNumber)
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
        switch(wallNumber)
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
        switch(wallNumber)
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
