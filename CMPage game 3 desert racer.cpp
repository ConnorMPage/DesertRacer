// CMPage game 3 desert racer.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
using namespace std;
using namespace tle;
//game states
enum primaryGamestates {PreGame,Countdown, Playing, Paused, Over};
enum secondaryGameStates {Start, stage1Complete, stage2Complete, stage3Complete, stage4Complete, stage5Complete, EndOfRace};
enum collisionAxis {xAxis,zAxis, None};
enum boostStates {Active, NotInUse, OverHeated};

//structure declaration
struct checkpointStruct//contains all details for checkpoints 
{
	IModel* model;
	const float insideWidth = 13.72f;
	const float insideLength = 1.0f;
	const float distanceToStrut = 8.3f;
	const float strutRadius = 0.25f;

};

struct wallSectionStruct//contains all the compentants for awall section
{
	IModel* isle1;
	IModel* wall1;
	IModel* MidIsle;
	IModel* wall2;
	IModel* isle2;
	const float wallWidth = 3.75f;
	const float wallLength = 37.88f;

};
struct TankStructure//contains all details for the tank structure
{
	IModel* model;
	const float tankRadius = 3.0f;
};
struct Vector3
{
	float x;
	float y;
	float z;
};
struct Vector2D//this structer creates x,z 
{
	float x;
	float z;
};
struct TextVector// creates x and y for text output
{
	float x;
	float y;
};
//-----------------------------------------------------------------
//function declaration
Vector2D getFacingVector(IModel* model);
Vector2D multiplyByScalar(Vector2D vector, float scalar);
Vector2D vectorAdd(Vector2D vectorA, Vector2D vectorB);
void moveByVector(IModel* model, Vector2D movement, const float kDeltaTime);
bool sphereToBox(Vector2D spherePos, float sphereRadius, Vector2D boxPos, float boxLength, float boxWidth);
Vector2D GetPos(IModel* model);
Vector2D GetStrutPos(IModel* model, float distanceToStrut);
collisionAxis getCollisionAxis(Vector2D spherePos, float sphereRadius, Vector2D boxPos, float boxLength, float boxWidth);
bool sphereToSphere(Vector2D sphereAPos, float sphereARadius, Vector2D sphereBPos, float sphereBRadius);
bool pointToBox(Vector2D pointPos, Vector2D boxPos,float boxWidth, float boxLength);
Vector2D GetRotateStrutPos(IModel* model, float distanceToStrut);
void readFile(vector<float>& modelx, vector<float>& modely, vector<float>& modelz, ifstream& infile);


//--------------------------------------------------------------------
void main()
{
	
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine( kTLX );
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder( "C:\\ProgramData\\TL-Engine\\Media" );
	myEngine->AddMediaFolder(".\\media");


	
	/**** Set up your scene here ****/
	const int startingIndex = 0;//this is the starting index for every list/loop
	//-----------------------------------------------------------
	//array declaration:
	//constants for array declaration
	const int minusOne = 1;
	const int maxCheckpoints = 5;
	const int firstRotatedCP = 1;
	const int secRotatedCP = 4;
	const int maxrotated = 1;
	const int wallArraySize = 10;
	const int maxTanks = 10;

	//variables for arrays
	//these variables have 1 subtracted as arrays start at 0 not 1
	int tankAmount = maxTanks - minusOne;
	int checkpointAmount = maxCheckpoints - minusOne;
	int wallAmount = wallArraySize - minusOne;

	//arrays
	checkpointStruct checkpoints[maxCheckpoints];//contains the checkpoints
	wallSectionStruct wallSection[wallArraySize];//contains the wall sections
	TankStructure waterTanks[maxTanks];//contains the water tanks


	//iMesh declaration:
	IMesh* groundMesh = myEngine->LoadMesh("ground.x");
	IMesh* skyBoxMesh = myEngine->LoadMesh("Skybox 07.x");
	IMesh* checkPointMesh = myEngine->LoadMesh("Checkpoint.x");
	IMesh* wallMesh = myEngine->LoadMesh("Wall.x");
	IMesh* isleMesh = myEngine->LoadMesh("IsleStraight.x");
	IMesh* hoverCarMesh = myEngine->LoadMesh("race2.x");
	IMesh* waterTankMesh = myEngine->LoadMesh("TankSmall1.x");
	
	//constants for skybox
	const Vector3 skyBoxInitalPosition{ 0,-960,0 };
	//create layout
	IModel* ground = groundMesh->CreateModel();//this creates the ground
	IModel* skyBox = skyBoxMesh->CreateModel(skyBoxInitalPosition.x, skyBoxInitalPosition.y, skyBoxInitalPosition.z);//this creates the skybox

	//constants for checkpoint positions
	const Vector3 checkpoint2InitialPosition{ 10, 0, 120 };
	const Vector3 checkpoint3InitialPosition{ 25, 0, 56 };
	const Vector3 checkpoint4InitialPosition{ 32.5, 0, -10 };
	const Vector3 checkpoint5InitialPosition{ 15, 0, -120 };
	const float checkpointRotation = 90.0f;

	//checkpoints
	checkpoints[0].model = checkPointMesh->CreateModel();
	//starting/finish line
	checkpoints[1].model = checkPointMesh->CreateModel(checkpoint2InitialPosition.x, checkpoint2InitialPosition.y, checkpoint2InitialPosition.z);
	checkpoints[1].model->RotateY(checkpointRotation);
	//rotated checkpoint
	checkpoints[2].model = checkPointMesh->CreateModel(checkpoint3InitialPosition.x, checkpoint3InitialPosition.y, checkpoint3InitialPosition.z);
	//checkpoint 3
	checkpoints[3].model = checkPointMesh->CreateModel(checkpoint4InitialPosition.x, checkpoint4InitialPosition.y, checkpoint4InitialPosition.z);
	//checkpoint 4
	checkpoints[4].model = checkPointMesh->CreateModel(checkpoint5InitialPosition.x, checkpoint5InitialPosition.y, checkpoint5InitialPosition.z);
	checkpoints[4].model->RotateY(checkpointRotation);
	//checkpoint 5 is rotated
	 

	//---------------------------------------------------------------------
	// 	   Wall Sections - 10 in total
	//wall section 1 
	//constants for position
	const Vector3 wallSection1Isle1Pos{ -10, 0, 40 };
	const Vector3 wallSection1Wall1Pos{ -10, 0, 48 };
	const Vector3 wallSection1MidIslePos{ -10, 0, 56 };
	const Vector3 wallSection1Wall2Pos{ -10, 0, 64 };
	const Vector3 wallSection1Isle2Pos{ -10, 0, 72 };

	//create models
	wallSection[0].isle1 = isleMesh->CreateModel(wallSection1Isle1Pos.x, wallSection1Isle1Pos.y, wallSection1Isle1Pos.z);
	wallSection[0].wall1 = wallMesh->CreateModel(wallSection1Wall1Pos.x, wallSection1Wall1Pos.y, wallSection1Wall1Pos.z);
	wallSection[0].MidIsle = isleMesh->CreateModel(wallSection1MidIslePos.x, wallSection1MidIslePos.y, wallSection1MidIslePos.z);
	wallSection[0].wall2 = wallMesh->CreateModel(wallSection1Wall2Pos.x, wallSection1Wall1Pos.y, wallSection1Wall1Pos.z);
	wallSection[0].isle2 = isleMesh->CreateModel(wallSection1Isle2Pos.x, wallSection1Isle2Pos.y, wallSection1Isle2Pos.z);


	//wall section 2
	//constants for position
	const Vector3 wallSection2Isle1Pos{ 10, 0, 40 };
	const Vector3 wallSection2Wall1Pos{ 10, 0, 48 };
	const Vector3 wallSection2MidIslePos{ 10, 0, 56 };
	const Vector3 wallSection2Wall2Pos{ 10, 0, 64 };
	const Vector3 wallSection2Isle2Pos{ 10, 0, 72 };

	//create models
	wallSection[1].isle1 = isleMesh->CreateModel(wallSection2Isle1Pos.x, wallSection2Isle1Pos.y, wallSection2Isle1Pos.z);
	wallSection[1].wall1 = wallMesh->CreateModel(wallSection2Wall1Pos.x, wallSection2Wall1Pos.y, wallSection2Wall1Pos.z);
	wallSection[1].MidIsle = isleMesh->CreateModel(wallSection2MidIslePos.x, wallSection2MidIslePos.y, wallSection2MidIslePos.z);
	wallSection[1].wall2 = wallMesh->CreateModel(wallSection2Wall2Pos.x, wallSection2Wall2Pos.y, wallSection2Wall2Pos.z);
	wallSection[1].isle2 = isleMesh->CreateModel(wallSection2Isle2Pos.x, wallSection2Isle2Pos.y, wallSection2Isle2Pos.z);

	//wall section 3
	//constants for position
	const Vector3 wallSection3Isle1Pos{ 40, 0, 72 };
	const Vector3 wallSection3Wall1Pos{ 40, 0, 80 };
	const Vector3 wallSection3MidIslePos{ 40, 0, 88 };
	const Vector3 wallSection3Wall2Pos{ 40, 0, 96 };
	const Vector3 wallSection3Isle2Pos{ 40, 0, 104 };

	//create models

	wallSection[2].isle1 = isleMesh->CreateModel(wallSection3Isle1Pos.x, wallSection3Isle1Pos.y, wallSection3Isle1Pos.z);
	wallSection[2].wall1 = wallMesh->CreateModel(wallSection3Wall1Pos.x, wallSection3Wall1Pos.y, wallSection3Wall1Pos.z);
	wallSection[2].MidIsle = isleMesh->CreateModel(wallSection3MidIslePos.x, wallSection3MidIslePos.y, wallSection3MidIslePos.z);
	wallSection[2].wall2 = wallMesh->CreateModel(wallSection3Wall2Pos.x, wallSection3Wall2Pos.y, wallSection3Wall2Pos.z);
	wallSection[2].isle2 = isleMesh->CreateModel(wallSection3Isle2Pos.x, wallSection3Isle2Pos.y, wallSection3Isle2Pos.z);

	//wall section 4
	//constants for position
	const Vector3 wallSection4Isle1Pos{ 40, 0, 16 };
	const Vector3 wallSection4Wall1Pos{ 40, 0, 24 };
	const Vector3 wallSection4MidIslePos{ 40, 0, 32 };
	const Vector3 wallSection4Wall2Pos{ 40, 0, 40 };
	const Vector3 wallSection4Isle2Pos{ 40, 0, 48 };

	//create models

	wallSection[3].isle1 = isleMesh->CreateModel(wallSection4Isle1Pos.x, wallSection4Isle1Pos.y, wallSection4Isle1Pos.z);
	wallSection[3].wall1 = wallMesh->CreateModel(wallSection4Wall1Pos.x, wallSection4Wall1Pos.y, wallSection4Wall1Pos.z);
	wallSection[3].MidIsle = isleMesh->CreateModel(wallSection4MidIslePos.x, wallSection4MidIslePos.y, wallSection4MidIslePos.z);
	wallSection[3].wall2 = wallMesh->CreateModel(wallSection4Wall2Pos.x, wallSection4Wall2Pos.y, wallSection4Wall2Pos.z);
	wallSection[3].isle2 = isleMesh->CreateModel(wallSection4Isle2Pos.x, wallSection4Isle2Pos.y, wallSection4Isle2Pos.z);

	//wall section 5
	//constants for position
	const Vector3 wallSection5Isle1Pos{ 50, 0, -40 };
	const Vector3 wallSection5Wall1Pos{ 50, 0, -32 };
	const Vector3 wallSection5MidIslePos{ 50, 0, -24 };
	const Vector3 wallSection5Wall2Pos{ 50, 0, -16 };
	const Vector3 wallSection5Isle2Pos{ 50, 0, -8 };

	//create models

	wallSection[4].isle1 = isleMesh->CreateModel(wallSection5Isle1Pos.x, wallSection5Isle1Pos.y, wallSection5Isle1Pos.z);
	wallSection[4].wall1 = wallMesh->CreateModel(wallSection5Wall1Pos.x, wallSection5Wall1Pos.y, wallSection5Wall1Pos.z);
	wallSection[4].MidIsle = isleMesh->CreateModel(wallSection5MidIslePos.x, wallSection5MidIslePos.y, wallSection5MidIslePos.z);
	wallSection[4].wall2 = wallMesh->CreateModel(wallSection5Wall2Pos.x, wallSection5Wall2Pos.y, wallSection5Wall2Pos.z);
	wallSection[4].isle2 = isleMesh->CreateModel(wallSection5Isle2Pos.x, wallSection5Isle2Pos.y, wallSection5Isle2Pos.z);

	//wall section 6
	//constants for position
	const Vector3 wallSection6Isle1Pos{ 50, 0, -60 };
	const Vector3 wallSection6Wall1Pos{ 50, 0, -68 };
	const Vector3 wallSection6MidIslePos{ 50, 0, -76 };
	const Vector3 wallSection6Wall2Pos{ 50, 0, -84 };
	const Vector3 wallSection6Isle2Pos{ 50, 0, -92 };

	//create models

	wallSection[5].isle1 = isleMesh->CreateModel(wallSection6Isle1Pos.x, wallSection6Isle1Pos.y, wallSection6Isle1Pos.z);
	wallSection[5].wall1 = wallMesh->CreateModel(wallSection6Wall1Pos.x, wallSection6Wall1Pos.y, wallSection6Wall1Pos.z);
	wallSection[5].MidIsle = isleMesh->CreateModel(wallSection6MidIslePos.x, wallSection6MidIslePos.y, wallSection6MidIslePos.z);
	wallSection[5].wall2 = wallMesh->CreateModel(wallSection6Wall2Pos.x, wallSection6Wall2Pos.y, wallSection6Wall2Pos.z);
	wallSection[5].isle2 = isleMesh->CreateModel(wallSection6Isle2Pos.x, wallSection6Isle2Pos.y, wallSection6Isle2Pos.z);

	//wall section 7
	//constants for position
	const Vector3 wallSection7Isle1Pos{ 15, 0, -47 };
	const Vector3 wallSection7Wall1Pos{ 15, 0, -39 };
	const Vector3 wallSection7MidIslePos{ 15, 0, -31 };
	const Vector3 wallSection7Wall2Pos{ 15, 0, -23 };
	const Vector3 wallSection7Isle2Pos{ 15, 0, -15 };

	//create models
	wallSection[6].isle1 = isleMesh->CreateModel(wallSection7Isle1Pos.x, wallSection7Isle1Pos.y, wallSection7Isle1Pos.z);
	wallSection[6].wall1 = wallMesh->CreateModel(wallSection7Wall1Pos.x, wallSection7Wall1Pos.y, wallSection7Wall1Pos.z);
	wallSection[6].MidIsle = isleMesh->CreateModel(wallSection7MidIslePos.x, wallSection7MidIslePos.y, wallSection7MidIslePos.z);
	wallSection[6].wall2 = wallMesh->CreateModel(wallSection7Wall2Pos.x, wallSection7Wall2Pos.y, wallSection7Wall2Pos.z);
	wallSection[6].isle2 = isleMesh->CreateModel(wallSection7Isle2Pos.x, wallSection7Isle2Pos.y, wallSection7Isle2Pos.z);

	//wall section 8
	//constants for position
	const Vector3 wallSection8Isle1Pos{ 15, 0, -67 };
	const Vector3 wallSection8Wall1Pos{ 15, 0, -75 };
	const Vector3 wallSection8MidIslePos{ 15, 0, -83 };
	const Vector3 wallSection8Wall2Pos{ 15, 0, -91 };
	const Vector3 wallSection8Isle2Pos{ 15, 0, -99 };

	//create models
	wallSection[7].isle1 = isleMesh->CreateModel(wallSection8Isle1Pos.x, wallSection8Isle1Pos.y, wallSection8Isle1Pos.z);
	wallSection[7].wall1 = wallMesh->CreateModel(wallSection8Wall1Pos.x, wallSection8Wall1Pos.y, wallSection8Wall1Pos.z);
	wallSection[7].MidIsle = isleMesh->CreateModel(wallSection8MidIslePos.x, wallSection8MidIslePos.y, wallSection8MidIslePos.z);
	wallSection[7].wall2 = wallMesh->CreateModel(wallSection8Wall2Pos.x, wallSection8Wall2Pos.y, wallSection8Wall2Pos.z);
	wallSection[7].isle2 = isleMesh->CreateModel(wallSection8Isle2Pos.x, wallSection8Isle2Pos.y, wallSection8Isle2Pos.z);

	//wall section 9
	//constants for position
	const Vector3 wallSection9Isle1Pos{ -15, 0, -47 };
	const Vector3 wallSection9Wall1Pos{ -15, 0, -39 };
	const Vector3 wallSection9MidIslePos{ -15, 0, -31 };
	const Vector3 wallSection9Wall2Pos{ -15, 0, -23 };
	const Vector3 wallSection9Isle2Pos{ -15, 0, -15 };

	//create models
	wallSection[8].isle1 = isleMesh->CreateModel(wallSection9Isle1Pos.x, wallSection9Isle1Pos.y,wallSection9Isle1Pos.z);
	wallSection[8].wall1 = wallMesh->CreateModel(wallSection9Wall1Pos.x, wallSection9Wall1Pos.y, wallSection9Wall1Pos.z);
	wallSection[8].MidIsle = isleMesh->CreateModel(wallSection9MidIslePos.x, wallSection9MidIslePos.y,wallSection9MidIslePos.z);
	wallSection[8].wall2 = wallMesh->CreateModel(wallSection9Wall2Pos.x, wallSection9Wall2Pos.y, wallSection9Wall2Pos.z);
	wallSection[8].isle2 = isleMesh->CreateModel(wallSection9Isle2Pos.x, wallSection9Isle2Pos.y, wallSection9Isle2Pos.z);

	//wall section 10
	//constants for position
	const Vector3 wallSection10Isle1Pos{ -15, 0, -67 };
	const Vector3 wallSection10Wall1Pos{ -15, 0, -75 };
	const Vector3 wallSection10MidIslePos{ -15, 0, -83 };
	const Vector3 wallSection10Wall2Pos{ -15, 0, -91 };
	const Vector3 wallSection10Isle2Pos{ -15, 0, -99 };

	//create models
	wallSection[9].isle1 = isleMesh->CreateModel(wallSection10Isle1Pos.x, wallSection10Isle1Pos.y, wallSection10Isle1Pos.z);
	wallSection[9].wall1 = wallMesh->CreateModel(wallSection10Wall1Pos.x, wallSection10Wall1Pos.y, wallSection10Wall1Pos.z);
	wallSection[9].MidIsle = isleMesh->CreateModel(wallSection10MidIslePos.x, wallSection10MidIslePos.y, wallSection10MidIslePos.z);
	wallSection[9].wall2 = wallMesh->CreateModel(wallSection10Wall2Pos.x, wallSection10Wall2Pos.y, wallSection10Wall2Pos.z);
	wallSection[9].isle2 = isleMesh->CreateModel(wallSection10Isle2Pos.x, wallSection10Isle2Pos.y, wallSection10Isle2Pos.z);

	//-----------------------------------------------------------------------------
	//water tanks - 10 water tanks in total
	// 	   water tank constants
	const Vector3 waterTank1InitialPos{ 0, 0, 135 };
	const Vector3 waterTank2InitialPos{ -15, 0, 110 };
	const Vector3 waterTank3InitialPos{ 40, 0, 110 };
	const Vector3 waterTank4InitialPos{ 20, 0, 135 };
	const Vector3 waterTank5InitialPos{ 15, 0, -135 };
	const Vector3 waterTank6InitialPos{ 0, 0, -135 };
	const Vector3 waterTank7InitialPos{ 40, 0, -120 };
	const Vector3 waterTank8InitialPos{ -20, 0, -115 };
	const Vector3 waterTank9InitialPos{ 45, 0, 5 };
	const Vector3 waterTank10InitialPos{ 32.5, -2, -57 };
	const float rotatedWaterTank = -70.0f;

	//corner 1
	waterTanks[0].model = waterTankMesh->CreateModel(waterTank1InitialPos.x, waterTank1InitialPos.y, waterTank1InitialPos.z);
	waterTanks[1].model = waterTankMesh->CreateModel(waterTank2InitialPos.x, waterTank2InitialPos.y, waterTank2InitialPos.z);
	waterTanks[2].model = waterTankMesh->CreateModel(waterTank3InitialPos.x, waterTank3InitialPos.y, waterTank3InitialPos.z);
	waterTanks[3].model = waterTankMesh->CreateModel(waterTank4InitialPos.x, waterTank4InitialPos.y, waterTank4InitialPos.z);

	//coner 2
	waterTanks[4].model = waterTankMesh->CreateModel(waterTank5InitialPos.x, waterTank5InitialPos.y, waterTank5InitialPos.z);
	waterTanks[5].model = waterTankMesh->CreateModel(waterTank6InitialPos.x, waterTank6InitialPos.y, waterTank6InitialPos.z);
	waterTanks[6].model = waterTankMesh->CreateModel(waterTank7InitialPos.x, waterTank7InitialPos.y, waterTank7InitialPos.z);
	waterTanks[7].model = waterTankMesh->CreateModel(waterTank8InitialPos.x, waterTank8InitialPos.y, waterTank8InitialPos.z);

	//Along straights
	waterTanks[8].model = waterTankMesh->CreateModel(waterTank9InitialPos.x, waterTank9InitialPos.y, waterTank9InitialPos.z);
	
	
	//Rotated water tank 
	waterTanks[9].model = waterTankMesh->CreateModel(waterTank10InitialPos.x, waterTank10InitialPos.y, waterTank10InitialPos.z);
	waterTanks[9].model->RotateLocalX(rotatedWaterTank);


	//creating hover car
	const Vector3 hoverCarInitialPos{ 0,0,-20 };
	IModel* hoverCar = hoverCarMesh->CreateModel(hoverCarInitialPos.x, hoverCarInitialPos.y, hoverCarInitialPos.z);
	const float hoverCarRadius = (4);
	const int startingHealth = 100;
	int hoverCarHealth = startingHealth;
	float mouseRotateY;
	float mouseRotateX;

	//creating the camera
	ICamera* gameCamera = myEngine->CreateCamera(kManual);
	
	gameCamera->AttachToParent(hoverCar);//attaches the camera to the hover car
	const Vector3 thirdPersonCam{ 0,5,-20 };
	const Vector3 firstPersonCam = { 0.0f, 5.0f, 2.158f };
	gameCamera->SetLocalPosition(thirdPersonCam.x, thirdPersonCam.y, thirdPersonCam.z);
	
	int mouseMoveX;//saves the amount the user moves the mouse along x
	int mouseMoveY;//save the value the user moves along the y


	//-----------------------------------------------
	//constant keycode declaration
	//Hover Car controls
	const EKeyCode kMoveForwardKey = EKeyCode::Key_W;
	const EKeyCode kMoveBackwardKey = EKeyCode::Key_S;
	const EKeyCode kLeftTurnKey = EKeyCode::Key_A;
	const EKeyCode kRightTurnKey = EKeyCode::Key_D;
	const EKeyCode kBoostButton = EKeyCode::Key_Space;

	//Game controls - eg pause & exit 
	const EKeyCode escapeKey = EKeyCode::Key_Escape;
	const EKeyCode pauseButton = EKeyCode::Key_P;
	const EKeyCode startButton = EKeyCode::Key_Space;
	
	//Camera Controls
	const EKeyCode resetCam = EKeyCode::Key_1;
	const EKeyCode CamToFirst = EKeyCode::Key_2;
	
	const EKeyCode KMoveCamForward = EKeyCode::Key_Up;
	const EKeyCode KMoveCamBackward = EKeyCode::Key_Down;
	const EKeyCode KMoveCamLeft = EKeyCode::Key_Left;
	const EKeyCode KMoveCamRight = EKeyCode::Key_Right;
	
	
	//speed declaration
	const float kRoationSpeed = 50.0f;
	const float kMovementSpeed = 25.0f;
	const float kReverseSpeed = -12.5f;
	const float coefficientOfDrag = 0.99979f;
	const float kMouseRotation = 5.0f;
	const float kCamMovementSpeed = 10.0f;
	const float kBoostSpeed = kMovementSpeed / 2;// the boost speed is half of regular speed so when both are used together theres a 50% increase
	const float maxSpeedWithoutBoost = 25.0f;
	Vector2D movementThisFrame = { 0.0f,0.0f };
	//max boost speed is 150% of max without boost
	const float maxSpeedWithBoost = 37.5f;

	const float toMetres = 0.3f;
	const float toKiloMultiplier = 60.0f;
	const float divideByThousand = 1000.0f;
	const float toMiles = 0.62137f;
	//text declaraation
	IFont* gameFont = myEngine->LoadFont("Comic Sans MS", 36);
	IFont* subFont = myEngine->LoadFont("Comic Sans MS", 20);
	const ISprite* backdrop = myEngine->CreateSprite("ui_backdrop.jpg", 300, 660);

	

	//constants strings for output messages
	const string spaceToStart = "Press Space To Start";
	const string countDown[3] = {"           3", "           2", "           1"};
	const string start = "           Go";
	const string completed1text = "Stage 1 Complete";
	const string completed2text = "Stage 2 Complete";
	const string completed3text = "Stage 3 Complete";
	const string completed4text = "Stage 4 Complete";
	const string completed5text = "Stage 5 Complete";
	const string Pausedtext = "         Paused";
	const string EndOfRacetext = "         Finished";
	const string healthMessageText = "Health: ";
	const string speedMessage = "Speed: ";
	const string tLPerSec = "Units/Seconds";
	const string metrePerSec = "Metres/Seconds";
	const string kiloPerHour = "Kilometres/Hour";
	const string milesPerHour = "Miles/Hour";
	const string spaceToBoost = "Press space to use Boost";
	const string boostWarn = "Warning boost overheat";
	const string clearStream = "";

	//stringstream declarations
	stringstream stateMessage;
	stringstream healthMessage;
	stringstream healthAmount;
	stringstream speedText;
	stringstream boostMessage;

	//const for text output
	const TextVector boostOutput{ 520, 700 };
	const TextVector speedMSOuput{ 770, 665 };
	const TextVector MSOuput{ 870, 665 };
	const TextVector speedkmOuput{ 770, 680 };
	const TextVector kmOuput{ 870, 680 };
	const TextVector speedmphOuput{ 770, 695 };
	const TextVector mphOuput{ 870, 695 };
	const int precisionLevel = 4;
	const TextVector stateMsgOutput{ 500,660 };
	const TextVector healthMsg{ 300,680 };
	const TextVector healthAmountOutput{ 360, 680 };

	primaryGamestates mainState = PreGame;
	secondaryGameStates subState;
	boostStates boostState = NotInUse;

	//other variables
	const int countLastInt = 3;
	float speedTLUnit;
	float speedMetres;
	float speedKilo;
	float speedMiles;
	const int damage = 5;
	float frameTime;
	const float TIME_LIMIT = 1;
	float timeLimit = TIME_LIMIT;
	bool onStart = true;
	const int maxRotation = 45;
	float rotateCamY = startingIndex;
	float rotateCamx = startingIndex;
	int countdownindex = startingIndex;
	float drag = coefficientOfDrag;
	myEngine->StartMouseCapture();


	//boost variable declaration
	const float boostCooldown = 5;
	const float maxActiveBoost = 3;
	const float warningCount = 1;
	bool boostJustActive = false;
	const float outOfTime = 0;
	const float noBoostHealth = 30.0f;
	

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		
		
		// Draw the scene
		myEngine->DrawScene();
		float kDeltaTime = myEngine->Timer();

		speedTLUnit = sqrt(movementThisFrame.z*movementThisFrame.z);
		//calculates the speed in tl units
		speedMetres = speedTLUnit * toMetres;
		//calculates the speed to metres per second
		speedKilo = (speedMetres * toKiloMultiplier * toKiloMultiplier) / divideByThousand;
		//calculates the speed in kilometres per hour 
		speedMiles = speedKilo * toMiles;
		//calculates the speed in miles per hour 
		Vector2D oldPosition = GetPos(hoverCar);
		//gets the location of hover car at the start of the frame
		
		if (mainState == PreGame)//this will be the starting state of the game
		{
			
			if (myEngine->KeyHit(startButton))// space bar
			{
				mainState = Countdown;//changes to countdown state
				
			}
			
			stateMessage << spaceToStart;//views message space to start
		
		}
		else if (mainState == Countdown)//countdown state
		{

			timeLimit -= kDeltaTime;//deducts from countdown each fram
			if (timeLimit <= outOfTime)//when the time limit is less that zero
			{

				
				
				countdownindex++;//index for number output is increased by one
				timeLimit = TIME_LIMIT;
			}
			if (countdownindex == countLastInt)//if the last number is ouputted
			{
				mainState = Playing;//changes to the playing state
				subState = Start;//the race starts
			}
			else
			{
				stateMessage << countDown[countdownindex];//outputs countdown
				
			}
			
		
		}
		else if (mainState == Playing)//playing state
		{
			//-----------------------------------------------------
			//Hover Car movement
			Vector2D facingVector = getFacingVector(hoverCar);
			//declares a 2d vector to get the facing vector

			if (myEngine->KeyHeld(kLeftTurnKey))//A key
			{
				hoverCar->RotateLocalY(-kRoationSpeed * kDeltaTime);//rotate left
			}
			if (myEngine->KeyHeld(kRightTurnKey))//D key
			{
				hoverCar->RotateLocalY(kRoationSpeed * kDeltaTime);//rotate right
			}
			if (myEngine->KeyHeld(kMoveForwardKey))//W key
			{
				movementThisFrame = vectorAdd(movementThisFrame, multiplyByScalar(facingVector, kMovementSpeed * kDeltaTime));//adds to the movement vector by using scalar 
				//Limit speed
				if (boostState != Active)//checks that the boost isnt active
				{
					if (sqrt(movementThisFrame.z * movementThisFrame.z) > maxSpeedWithoutBoost )//makes the speed always positive and then checks against speed limit
					{
						movementThisFrame = multiplyByScalar(movementThisFrame, drag);//makes the drag higher than 1 so that it stays beneath the limit
					}
				}				
			}
			if (myEngine->KeyHeld(kMoveBackwardKey))//S key
			{
				movementThisFrame = vectorAdd(movementThisFrame, multiplyByScalar(facingVector, kReverseSpeed * kDeltaTime));//increases the movment z value by reverse speed
				if (sqrt(movementThisFrame.z * movementThisFrame.z) > maxSpeedWithoutBoost)//makes the speed always positive and then checks against speed limit
				{
					movementThisFrame = multiplyByScalar(movementThisFrame, drag);//makes the drag higher than 1 so that it stays beneath the limit
				}
			}
			Vector2D newPosition;//creates a vector for new position
			//new position is then set as the current position + the movement
			newPosition.x = oldPosition.x + movementThisFrame.x;
			newPosition.z = oldPosition.z + movementThisFrame.z;

			for (int index = startingIndex; index <= wallAmount; index++)//loops through wall array - 10 walls total
			{								
				if (sphereToBox(GetPos(hoverCar), hoverCarRadius, GetPos(wallSection[index].MidIsle), wallSection[index].wallLength, wallSection[index].wallWidth))//checks if wihin bounding box
				{
					hoverCarHealth -= damage;//car takes 5 damage
					collisionAxis collision = getCollisionAxis(oldPosition, hoverCarRadius, GetPos(wallSection[index].MidIsle), wallSection[index].wallLength, wallSection[index].wallWidth);
					//checks collision axis

					if (collision == zAxis)//if z
					{
						movementThisFrame.z = -movementThisFrame.z;	//reverses z	
					}						
					else if (collision == xAxis)//if x
					{
						//reverses both z and x
						movementThisFrame.x = -movementThisFrame.x;
						movementThisFrame.z = -movementThisFrame.z;
					}
					else
					{
						//reverses both z and x
						movementThisFrame.z = -movementThisFrame.z;
						movementThisFrame.x = -movementThisFrame.x;
					}															
				}																						
			 }
			for (int index = startingIndex; index <= checkpointAmount; index++)//loops through all the checkpoints
			{
				//creates a 2d vector for each strut
				Vector2D leftStrutPos;
				Vector2D rightStrutPos;
				
				if (index == firstRotatedCP || index == secRotatedCP)//checks if the checkpoints are rotated by 90 degrees
				{
					leftStrutPos = GetRotateStrutPos(checkpoints[index].model, -checkpoints[index].distanceToStrut);//gets strut locations
					rightStrutPos = GetRotateStrutPos(checkpoints[index].model, checkpoints[index].distanceToStrut);					
				}
				else
				{
					leftStrutPos = GetStrutPos(checkpoints[index].model, -checkpoints[index].distanceToStrut);
					rightStrutPos = GetStrutPos(checkpoints[index].model, checkpoints[index].distanceToStrut);;//gets strut locations
				}
			
				
				if (sphereToSphere(GetPos(hoverCar), hoverCarRadius, leftStrutPos, checkpoints[index].strutRadius ) ||
					sphereToSphere(GetPos(hoverCar), hoverCarRadius, rightStrutPos, checkpoints[index].strutRadius))//sphere to spere collision
				{
					movementThisFrame.x = -movementThisFrame.x;//makes movment negative
					movementThisFrame.z = -movementThisFrame.z;
					hoverCarHealth-=damage;//reduces health by 5
				}
			}
			for (int index = startingIndex; index <= tankAmount; index++)//loops through tanks
			{
				if (sphereToSphere(GetPos(hoverCar), hoverCarRadius, GetPos(waterTanks[index].model), waterTanks[index].tankRadius))//sphere to sphere collision
				{
					movementThisFrame.x = -movementThisFrame.x;//makes movement negative
					movementThisFrame.z = -movementThisFrame.z;
					hoverCarHealth-=damage;//reduces health by 5
				}
			}

			//-------------------------------------------------------------
			//boost system

			if (myEngine->KeyHit(kBoostButton))//Space bar
			{
				timeLimit = maxActiveBoost;//the initial keyhit sets the timer to 3
			}
			if (myEngine->KeyHeld(kBoostButton))//space key
			{
				if (boostState != OverHeated && hoverCarHealth > noBoostHealth)//if its already over heated 
				{
					boostState = Active;//sets boost state to active
					
				}
				
			}
			else//let go off key
			{
				if (boostState != OverHeated)//if its not overheated it goes back to be not in use
				{
					boostState = NotInUse;
				}				
				
				
			}

			if (boostState == Active)//boost active state
			{
				timeLimit -= kDeltaTime;//time limit has 1 deducted each frame
				if (timeLimit <= outOfTime)//if time limit is equal to 0
				{
				
					boostState = OverHeated;//boost state set to over heated
					timeLimit = boostCooldown;//sets the time limit to 5
					
				}
				else if (timeLimit <= warningCount)
				{
					boostMessage << boostWarn;
					subFont->Draw(boostMessage.str(), boostOutput.x, boostOutput.y);
					boostMessage.str(clearStream);
				}
				movementThisFrame = vectorAdd(movementThisFrame, multiplyByScalar(facingVector, kBoostSpeed * kDeltaTime));
				if (sqrt(movementThisFrame.z * movementThisFrame.z) > maxSpeedWithBoost || sqrt(movementThisFrame.x * movementThisFrame.x) > maxSpeedWithBoost)
				{
					movementThisFrame = multiplyByScalar(movementThisFrame, drag);
				}
			}
			else if (boostState == OverHeated)
			{
				movementThisFrame = multiplyByScalar(movementThisFrame, drag);//this will double the current drag force
			
				timeLimit -= kDeltaTime;
				if (timeLimit <= outOfTime)
				{
					boostState = NotInUse;
				}
			}
			else if (boostState = NotInUse)
			{
				boostMessage << spaceToBoost;
				subFont->Draw(boostMessage.str(), boostOutput.x, boostOutput.y);
				boostMessage.str(clearStream);
			}
			//-----------------------------------------------------------------------------------------------------
			
		
			moveByVector(hoverCar, movementThisFrame, kDeltaTime);
			movementThisFrame = multiplyByScalar(movementThisFrame, drag);
			
			//pause 
			if (myEngine->KeyHit(pauseButton))//if P key is hit
			{
				mainState = Paused;//changes the game state to paused
				//everything will stop moving
			}


			//-------------------------------------------------------------------    
			//Camera Movement
			if (myEngine->KeyHit(resetCam))//if 1 key is hit
			{
				gameCamera->SetLocalPosition(thirdPersonCam.x, thirdPersonCam.y, thirdPersonCam.z);//resets to the starting positon
				gameCamera->ResetOrientation();//this resets the orientation of the camera
				rotateCamY = startingIndex;//this sets the float to 0
				rotateCamx = startingIndex;//this sets the float to 0
			}
			if (myEngine->KeyHit(CamToFirst))//2 key
			{
				gameCamera->SetLocalPosition(firstPersonCam.x,firstPersonCam.y,firstPersonCam.z);//sets cam to first person
			}
			if (myEngine->KeyHeld(KMoveCamForward))//up arrow key
			{
				gameCamera->MoveLocalZ(kCamMovementSpeed* kDeltaTime);//move across z
			}
			if (myEngine->KeyHeld(KMoveCamBackward))//down arrow key
			{
				gameCamera->MoveLocalZ(-kCamMovementSpeed* kDeltaTime);//negative move acrosss z
			}
			if (myEngine->KeyHeld(KMoveCamLeft))//left arrow key
			{
				gameCamera->MoveLocalX(-kCamMovementSpeed* kDeltaTime);//negative move across x
			}
			if (myEngine->KeyHeld(KMoveCamRight))//right arrow key
			{
				gameCamera->MoveLocalX(kCamMovementSpeed* kDeltaTime);//negative move across x
			}
			
			
			// camera roation 
			//rotating along the y axis
			if (rotateCamx < -maxRotation )//checks if at max down rotation
			{
				mouseMoveX = myEngine->GetMouseMovementX() * kRoationSpeed * kDeltaTime;//gets mouse movement 
				if (mouseMoveX > startingIndex)
				{
					rotateCamx += mouseMoveX;//when roated rotate cam increases
					gameCamera->RotateY(mouseMoveX);
				}

		
			}
			else if (rotateCamx > maxRotation)
			{
				
				mouseMoveX = myEngine->GetMouseMovementX() * kRoationSpeed * kDeltaTime;//gets mouse movement
				if (mouseMoveX < startingIndex)
				{
					rotateCamx += mouseMoveX;//when roated rotate cam increases
					gameCamera->RotateY(mouseMoveX);
				}
			}
			else
			{
				mouseMoveX = myEngine->GetMouseMovementX();//gets mouse movement
				gameCamera->RotateY(mouseMoveX* (kMouseRotation* kDeltaTime));
				rotateCamx += (mouseMoveX * (kMouseRotation * kDeltaTime));
			}

			//rotating along the x axis
			if (rotateCamY < -maxRotation)
			{
				mouseMoveY = myEngine->GetMouseMovementY() * kRoationSpeed * kDeltaTime;//gets mouse movement
				if (mouseMoveY > startingIndex)
				{
					rotateCamY += mouseMoveY;//when roated rotate cam increases
					gameCamera->RotateX(mouseMoveY);
				}
			}
			else if (rotateCamY > maxRotation)
			{
				mouseMoveY = myEngine->GetMouseMovementY() * kRoationSpeed * kDeltaTime;//gets mouse movement
				if (mouseMoveY < startingIndex)
				{
					rotateCamY += mouseMoveY;//when roated rotate cam increases
					gameCamera->RotateX(mouseMoveY);
				}
			}
			else
			{
				mouseMoveY = myEngine->GetMouseMovementY();//gets mouse movement
				gameCamera->RotateLocalX(mouseMoveY * (kMouseRotation * kDeltaTime));
				rotateCamY += (mouseMoveY * (kMouseRotation * kDeltaTime));
			}
			//--------------------------------------------------------------------------
			//checkpoint collision and substate changes
			if (subState == Start)
			{
				stateMessage << start;
				if (pointToBox(GetPos(hoverCar),GetPos(checkpoints[0].model),checkpoints[0].insideWidth, checkpoints[0].insideLength))//point to box collision
				{
					subState = stage1Complete;//substate changes to stage1complete
				}
			}


			else if (subState == stage1Complete)
			{
				stateMessage << completed1text;
				if (pointToBox(GetPos(hoverCar), GetPos(checkpoints[1].model), checkpoints[1].insideWidth, checkpoints[1].insideLength))//point to box collision
				{
					subState = stage2Complete;//substate changes to stage2complete
				}
			}



			else if (subState == stage2Complete)
			{
				stateMessage << completed2text;
				if (pointToBox(GetPos(hoverCar), GetPos(checkpoints[2].model), checkpoints[2].insideWidth, checkpoints[2].insideLength))//point to box collision
				{
					subState = stage3Complete;//substate changes to stage2complete
				}
			}



			else if (subState == stage3Complete)
			{
				stateMessage << completed3text;
				if (pointToBox(GetPos(hoverCar), GetPos(checkpoints[3].model), checkpoints[3].insideWidth, checkpoints[3].insideLength))//point to box collision
				{
					subState = stage4Complete;//substate changes to stage2complete
				}
			}
			else if (subState == stage4Complete)
			{
				stateMessage << completed4text;
				if (pointToBox(GetPos(hoverCar), GetPos(checkpoints[4].model), checkpoints[4].insideWidth, checkpoints[4].insideLength))//point to box collision
				{
					subState = stage5Complete;//substate changes to stage5complete
				}
			}



			else if (subState == stage5Complete)
			{
				stateMessage << completed5text;
				if (pointToBox(GetPos(hoverCar), GetPos(checkpoints[0].model), checkpoints[0].insideWidth, checkpoints[0].insideLength))
				{
					subState = EndOfRace;//changes substate
				}
			}



			else if (subState == EndOfRace)
			{
				mainState = Over;//sets the game state to over
			}
			//-------------------------------------------------------------------------

			if (hoverCarHealth <= startingIndex) 
			{
				mainState = Over;
			}
			//-----------------------------------------------
			// 
			//outputs all of the text
			speedText << speedMessage <<setprecision(precisionLevel)<< speedMetres;
			subFont->Draw(speedText.str(), speedMSOuput.x, speedMSOuput.y);
			speedText.str(clearStream);
			speedText << metrePerSec;
			subFont->Draw(speedText.str(), MSOuput.x, MSOuput.y);
			speedText.str(clearStream);
			//metre per second
			speedText << speedMessage << setprecision(precisionLevel) << speedKilo;
			subFont->Draw(speedText.str(), speedkmOuput.x, speedkmOuput.y);
			speedText.str(clearStream);
			speedText << kiloPerHour;
			subFont->Draw(speedText.str(), kmOuput.x, kmOuput.y);
			speedText.str(clearStream);
			speedText << speedMessage << setprecision(precisionLevel) << speedMiles;
			subFont->Draw(speedText.str(), speedmphOuput.x, speedmphOuput.y);
			speedText.str(clearStream);
			speedText << milesPerHour;
			subFont->Draw(speedText.str(), mphOuput.x, mphOuput.y);
			speedText.str(clearStream);
		}
		else if (mainState == Paused)//if paused
		{
		    stateMessage << Pausedtext;// chnages the text to paused
			if (myEngine->KeyHit(pauseButton))//if P key is hit
			{
				mainState = Playing;//changes the gamestate to playing
				//everything will be able to move again
			}
		}
		else if (mainState == Over)//over state
		{
		stateMessage << EndOfRacetext;

		}
		//exit - this needed to be outside all gamestates so that it can work from any state
		if (myEngine->KeyHit(escapeKey))
		{
			myEngine->Stop();//stops the engine
			myEngine->StopMouseCapture();
		}
		//outputs more text
		gameFont->Draw(stateMessage.str(), stateMsgOutput.x, stateMsgOutput.y);
		stateMessage.str(clearStream);
		healthMessage.str(healthMessageText);
		subFont->Draw(healthMessage.str(), healthMsg.x, healthMsg.y);
		healthMessage.str(clearStream);
		healthAmount << hoverCarHealth;
		subFont->Draw(healthAmount.str(), healthAmountOutput.x, healthAmountOutput.y);
		healthAmount.str(clearStream);
	
		
		


		/**** Update your scene each frame here ****/

	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
Vector2D getFacingVector(IModel* model)
{
	//matrix values
	const int matrixnum4 = 4;
	const int matrixnum0 = 0;
	const int matrixnum2 = 2;


	float matrix[matrixnum4][matrixnum4];
	//gets desired matrix
	model->GetMatrix(&matrix[matrixnum0][matrixnum0]);
	Vector2D result;
	result.x = matrix[matrixnum2][matrixnum0];
	//sets the results x and z
	result.z = matrix[matrixnum2][matrixnum2];
	return result;//return result
}
Vector2D multiplyByScalar(Vector2D vector, float scalar)
{
	Vector2D result;
	result.x = vector.x * scalar;
	//mulitplys the vector by the scalar
	result.z = vector.z * scalar;
	return result;
}
Vector2D vectorAdd(Vector2D vectorA, Vector2D vectorB)
{
	Vector2D result;
	result.x = vectorA.x + vectorB.x;
	//adds the vectors together
	result.z = vectorA.z + vectorB.z;
	return result;
}
void moveByVector(IModel* model, Vector2D movement, const float kDeltaTime)
{
	const float yMovement = 0.0f;
	model->Move(movement.x * kDeltaTime, yMovement ,movement.z * kDeltaTime);
	//moves by the movment vector and delta time
}
Vector2D GetPos(IModel* model)
{
	Vector2D result;
	result.x = model->GetLocalX();
	//gets the position in a 2d vector
	result.z = model->GetLocalZ();
	return result;
}
Vector2D GetStrutPos(IModel* model, float distanceToStrut)
{
	Vector2D result;
	result.x = model->GetLocalX() + distanceToStrut;
	//gets the location of the struts
	result.z = model->GetLocalZ();
	return result;//returns result

}
Vector2D GetRotateStrutPos(IModel* model, float distanceToStrut)
{
	Vector2D result;
	result.x = model->GetLocalX() ;
	//gets the rotated strut positions
	result.z = model->GetLocalZ() + distanceToStrut;
	return result;

}
bool sphereToBox(Vector2D spherePos, float sphereRadius, Vector2D boxPos, float boxLength, float boxWidth)
{
	const float divide2 = 2.0f;
	float length = sphereRadius + (boxLength / divide2);//gets length
	float width = sphereRadius + (boxWidth / divide2);//gets width
	
	return (spherePos.x > boxPos.x - width && spherePos.x < boxPos.x + width && spherePos.z > boxPos.z - length && spherePos.z < boxPos.z + length);//returns if within box
}

collisionAxis getCollisionAxis(Vector2D spherePos, float sphereRadius, Vector2D boxPos, float boxLength, float boxWidth)
{
	const float divide2 = 2.0f;
	float length = sphereRadius + (boxLength / divide2);
	float width = sphereRadius + (boxWidth / divide2);
	if (spherePos.x > boxPos.x - width && spherePos.x < boxPos.x + width)//if coming from x axis
	{
		return collisionAxis::xAxis;
		
	}
	else if (spherePos.z > boxPos.z - length && spherePos.z < boxPos.z + length)//if coming from z axis
	{
		return collisionAxis::zAxis;
	}	
	return collisionAxis::None;
}

bool sphereToSphere(Vector2D sphereAPos, float sphereARadius, Vector2D sphereBPos, float sphereBRadius)
{
	
	int combinedRadius = sphereARadius + sphereBRadius;//creates a combined radius
	return sphereAPos.x > sphereBPos.x - combinedRadius && sphereAPos.x < sphereBPos.x + combinedRadius && sphereAPos.z > sphereBPos.z - combinedRadius && sphereAPos.z < sphereBPos.z + combinedRadius;
	//checks for overlap
}
bool pointToBox(Vector2D pointPos, Vector2D boxPos,float boxWidth, float boxLength)
{
	const float divide2 = 2.0f;
	float width = boxWidth / divide2;
	float length = boxLength / divide2;
	return pointPos.x > boxPos.x - width && pointPos.x < boxPos.x + width && pointPos.z > boxPos.z - length && pointPos.z < boxPos.z + length;
	//checks if point enters box
}
