#include <TL-Engine.h>

using namespace tle;

// Struct to represent a 3D vector with x, y, and z components
struct Vector3 {
    float x, y, z;
};

// Struct to represent a bounding box with min and max values along x, y, and z axes
struct BoundingBox {
    float minX;
    float maxX;
    float minY;
    float maxY;
    float minZ;
    float maxZ;
};

// Struct to represent Enemy Cars
struct EnemyCars {

    // Models
    IModel* enemyCarModel;
    IModel* sphereModel;

    // Movement speeds
    const float carMovementSpeed = 30 * 0.5;
    float sphereMovementSpeed = 2.5;

    // Flags for car and sphere status
    bool carHitStatus = false;
    bool carSideHit = false;
    bool carMovementStatus = true;
    bool sphereMovementStatus = true;
};

// Calculate the dot product of two 3D vectors
float calculateDotProduct(Vector3 v, Vector3 w) {
    return (v.x * w.x + v.y * w.y + v.z * w.z);
}

// Calculate the modulus (magnitude) of a 3D vector
float calculateModulus(Vector3 v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

// Calculate the facing vector of a model by moving it forward and recording the change in position
void calculateFacingVector(IModel* model, Vector3& facingVector) {
    Vector3 initialPos = { model->GetX(), model->GetY(), model->GetZ() };
    model->MoveLocalZ(1.0f);
    Vector3 finalPos = { model->GetX(), model->GetY(), model->GetZ() };
    facingVector = { finalPos.x - initialPos.x, finalPos.y - initialPos.y, finalPos.z - initialPos.z };
    model->MoveLocalZ(-1.0f);
}

// Calculate the vector from an enemy car to the player's car
void calculateEnemyCarToJeepVector(IModel* model, IModel* player, Vector3& enemyCarToJeepVector) {

    enemyCarToJeepVector = { player->GetX() - model->GetX(),
                             player->GetY() - model->GetY(),
                             player->GetZ() - model->GetZ() };

}

// Check for collision between the player's car and an enemy car using bounding box and player's car radius
bool CheckCollision(IModel* playerCar, IModel* enemyCar, float playerCarRadius, const BoundingBox& box) {

    // Calculate the min and max bounds of the bounding box, considering the player's car radius
    float boxminX = (enemyCar->GetLocalX() + box.minX) - playerCarRadius;
    float boxmaxX = (enemyCar->GetLocalX() + box.maxX) + playerCarRadius;
    float boxminY = (enemyCar->GetLocalY() + box.minY) - playerCarRadius;
    float boxmaxY = (enemyCar->GetLocalY() + box.maxY) + playerCarRadius;
    float boxminZ = (enemyCar->GetLocalZ() + box.minZ) - playerCarRadius;
    float boxmaxZ = (enemyCar->GetLocalZ() + box.maxZ) + playerCarRadius;

    // Get the position of the player's car
    float playerX = playerCar->GetX();
    float playerY = playerCar->GetY();
    float playerZ = playerCar->GetZ();

    // Check for collision using bounding box and player's car position
    bool isCollision = (playerX > boxminX && playerX < boxmaxX&&
        playerY > boxminY && playerY < boxmaxY&&
        playerZ > boxminZ && playerZ < boxmaxZ);

    return isCollision;
}


void main() {

    // Constants, Variables, defining initial game state and parameters
    const float groundYPosition = 0.0f;
    const float skyYPosition = -960.0f;
    const float playerCarYPosition = 0.0f;
    const float wheelYPosition = 0.0f;
    const float enemySphereYPosition = 2.5f;
    const float cameraXPosition = 0.0f;
    const float cameraYPosition = 15.0f;
    const float cameraZPosition = -60.0f;
    const float cameraRotationX = 15.0f;
    const float backdropWidth = 305.0f;
    const float backdropHeight = 659.0f;
    const float perimeterRadius = 50.0f;
    const int numStaticEnemies = 4;
    const int numMovingEnemies = 4;
    const int noOfTrees = 160;

    const float cameraDefaultX = 0.0f;
    const float cameraDefaultY = 15.0f;
    const float cameraDefaultZ = -60.0f;
    const float cameraAttachedX = 0.0f;
    const float cameraAttachedY1 = 5.0f;
    const float cameraAttachedY2 = 2.0f;
    const float cameraAttachedZ = -15.0f;
    const int scoreX = 640;
    const int scoreY = 675;
    const int healthX = 640;
    const int healthY = 10;

    const float sideCollisionChecker = 3.5f;
    const int scoreIncreaseForSideCollision = 15;
    const int scoreIncreaseForFrontCollision = 10;
    const int resetCarTimeDefault = 0;

    const int carTimerXPosition = 10;
    const int carTimerYPositions[] = { 10, 50, 90, 130 };

    const float resetCarTimeThreshold1 = 3.0f;
    const float resetCarTimeThreshold2 = 15.0f;
    const float sphereMovementSpeedDefault = 2.5f;
    const float sphereMovementSpeedDecrease = 1.125f;

    const int gameOverTextX = 640;
    const int gameOverTextY = 320;
    const int scoreTextX = 640;
    const int scoreTextY = 360;
    const int restartTextX = 640;
    const int restartTextY = 675;

    const std::string defaultCarSkin = "white.png";
    const float defaultCarScaleFactor = 6.0f;
    const float sphereModelYPosition = 2.5f;
    const float defaultSphereMovementSpeed = 2.5f;
    const int defaultCarMovementStatus = true;
    const int defaultSphereMovementStatus = true;
    const int defaultCarHitStatus = false;
    const int defaultResetCarTime = 0;
    const float positionIncrement = 0.01f;

    I3DEngine* myEngine = New3DEngine(kTLX);
    myEngine->StartWindowed();

    myEngine->AddMediaFolder("C:\\ProgramData\\TL-Engine\\Media");

    IMesh* groundMesh = myEngine->LoadMesh("ground.x");
    IModel* groundModel = groundMesh->CreateModel();

    IMesh* skyMesh = myEngine->LoadMesh("skybox01.x");
    IModel* skyModel = skyMesh->CreateModel(0, skyYPosition, 0);

    IMesh* playerCarMesh = myEngine->LoadMesh("4x4jeep.x");
    IModel* playerCarModel = playerCarMesh->CreateModel();

    ISceneNode* frontLeftWheelNode = playerCarModel->GetNode(4);
    ISceneNode* frontRightWheelNode = playerCarModel->GetNode(5);
    ISceneNode* backLeftWheelNode = playerCarModel->GetNode(6);
    ISceneNode* backRightWheelNode = playerCarModel->GetNode(7);

    IMesh* enemyStaticCarMesh = myEngine->LoadMesh("audi.x");
    IMesh* enemyMovingCarMesh = myEngine->LoadMesh("estate.x");
    IMesh* ballMesh = myEngine->LoadMesh("ball.x");

    EnemyCars staticEnemies[numStaticEnemies];
    const float enemyStaticCarPositions[numStaticEnemies][3] = {
        { -20, groundYPosition, 20 },
        { 20, groundYPosition, 20 },
        { -20, groundYPosition, 0 },
        { 20, groundYPosition, 0 }
    };

    for (int i = 0; i < numStaticEnemies; ++i) {
        staticEnemies[i].enemyCarModel = enemyStaticCarMesh->CreateModel(enemyStaticCarPositions[i][0], enemyStaticCarPositions[i][1], enemyStaticCarPositions[i][2]);
        staticEnemies[i].sphereModel = ballMesh->CreateModel(0, enemySphereYPosition, 0);
        staticEnemies[i].sphereModel->AttachToParent(staticEnemies[i].enemyCarModel);
    }

    EnemyCars movingEnemies[numMovingEnemies];
    const float enemyMovingCarPositions[numMovingEnemies][3] = {
        { -30, groundYPosition, 15 },
        { 30, groundYPosition, -15 },
        { 30, groundYPosition, 30 },
        { -30, groundYPosition, -30 }
    };

    for (int i = 0; i < numMovingEnemies; ++i) {
        movingEnemies[i].enemyCarModel = enemyMovingCarMesh->CreateModel(enemyMovingCarPositions[i][0], enemyMovingCarPositions[i][1], enemyMovingCarPositions[i][2]);

        if (i == 0 || i == 3) {
            movingEnemies[i].enemyCarModel->RotateY(90);
        }
        else {
            movingEnemies[i].enemyCarModel->RotateY(-90);
        }
        movingEnemies[i].sphereModel = ballMesh->CreateModel(0, enemySphereYPosition, 0);
        movingEnemies[i].sphereModel->AttachToParent(movingEnemies[i].enemyCarModel);
    }

    ICamera* myCamera;
    myCamera = myEngine->CreateCamera(kManual);
    myCamera->SetPosition(cameraXPosition, cameraYPosition, cameraZPosition);
    myCamera->RotateLocalX(cameraRotationX);

    ISprite* backdrop = myEngine->CreateSprite("backdrop.jpg", backdropWidth, backdropHeight);
    IFont* myFont1 = myEngine->LoadFont("Comic Sans MS", 40);
    IFont* myFont2 = myEngine->LoadFont("Comic Sans MS", 30);

    IMesh* treeMesh = myEngine->LoadMesh("tree.x");
    IModel* perimeterTrees[noOfTrees];

    for (int i = 0; i < noOfTrees; i++) {
        float angle = (2 * 3.14 / noOfTrees) * i;
        float treeXPos = perimeterRadius * sin(angle);
        float treeYPos = perimeterRadius * cos(angle);
        perimeterTrees[i] = treeMesh->CreateModel(treeXPos, groundYPosition, treeYPos);
    }


    myEngine->Timer();

    const BoundingBox enemyMovingCar = { -1.05776, 1.05776, -2.86102e-006, 1.61014, -2.13928, 2.13928 };
    const BoundingBox enemyStaticCar = { -0.946118, 0.946118, -0.0065695, 1.50131, -1.97237, 1.97237 };

    int score = 0;
    int playerHealth = 100;

    float dotProduct;

    float prevPosX = 0;
    float prevPosY = 0;
    float prevPosZ = 0;

    const float playerCarRadius = 2;
    const float treeRadius = 1;

    const float maxForwardVelocity = 30.0f;
    const float maxBackwardVelocity = -30.0f;
    const float turningVelocity = 100;
    const float acceleration = 30.0f;
    const float deceleration = 30.0f;
    const float minVelocity = 0.0f;

    float forwardVelocity = 0.0f;
    float backwardVelocity = 0.0f;
    float rotationAngle = 0.0f;
    float currentWheelRotation = 0.0f;

    const float bounceFactor = 0.5f;
    const float scaleFactor = 0.6f;
    const float decelerationAfterBounce = 5;

    bool moveOppositeCar1 = false;
    bool moveOppositeCar2 = false;
    bool moveOppositeSphere = false;

    const float movingCarRange = 30.0f;
    const float sphereMovingMinRange = 2.5;
    const float sphereMovingMaxRange = 3;

    float resetCarTimes[numMovingEnemies] = { 0, 0, 0, 0 };

    bool allStaticCarsHit = false;
    bool allMovingCarsHit = false;

    bool win = false;

    const float maxWheelRotation = 30.0f;
    bool turningLeft = false;
    bool turningRight = false;

    const int gamePausedTextX = 640;
    const int gamePausedTextY = 320;
    const int healthTextX = 640;
    const int healthTextY = 10;

    enum GameState {
        GAME_PLAYING,
        GAME_PAUSED,
        GAME_OVER
    };

    GameState gameState = GAME_PLAYING;

    while (myEngine->IsRunning()) {

        myEngine->DrawScene();

        float frameTime = myEngine->Timer();

        if (myEngine->KeyHit(Key_Escape)) {
            myEngine->Stop();
        }

        prevPosX = playerCarModel->GetX();
        prevPosY = playerCarModel->GetY();
        prevPosZ = playerCarModel->GetZ();

        // Handling game controls and logic based on the current game state
        switch (gameState) {

        case GAME_PLAYING:

            if (myEngine->KeyHit(Key_1)) {
                myCamera->DetachFromParent();
                myCamera->SetPosition(cameraDefaultX, cameraDefaultY, cameraDefaultZ);
            }

            if (myEngine->KeyHit(Key_2)) {
                myCamera->AttachToParent(playerCarModel);
                myCamera->SetLocalPosition(cameraAttachedX, cameraAttachedY1, cameraAttachedZ);
            }

            if (myEngine->KeyHit(Key_3)) {
                myCamera->AttachToParent(playerCarModel);
                myCamera->SetLocalPosition(cameraAttachedX, cameraAttachedY2, cameraAttachedX);
            }

            myFont1->Draw("Score: " + std::to_string(score), scoreX, scoreY, kBlue, kCentre);
            myFont1->Draw("Health: " + std::to_string(playerHealth), healthX, healthY, kGreen, kCentre);

            if (myEngine->KeyHit(Key_P)) {
                gameState = GAME_PAUSED;
            }

            if (myEngine->KeyHeld(Key_W))
            {
                if (forwardVelocity < maxForwardVelocity)
                {
                    forwardVelocity += acceleration * frameTime;
                }

                if (myEngine->KeyHeld(Key_D) && !myEngine->KeyHeld(Key_A))
                {
                    playerCarModel->RotateY(turningVelocity * frameTime);
                    turningRight = true;
                    turningLeft = false;
                }
                else if (myEngine->KeyHeld(Key_A) && !myEngine->KeyHeld(Key_D))
                {
                    playerCarModel->RotateY(-turningVelocity * frameTime);
                    turningLeft = true;
                    turningRight = false;
                }
                else
                {
                    turningLeft = false;
                    turningRight = false;
                }
            }
            else
            {
                if (forwardVelocity > minVelocity)
                {
                    forwardVelocity -= deceleration * frameTime;
                }

            }

            if (myEngine->KeyHeld(Key_S))
            {
                if (backwardVelocity > maxBackwardVelocity)
                {
                    backwardVelocity -= acceleration * frameTime;
                }

                if (myEngine->KeyHeld(Key_Q) && !myEngine->KeyHeld(Key_A))
                {
                    playerCarModel->RotateY(-turningVelocity * frameTime);
                    turningRight = true;
                    turningLeft = false;
                }
                else if (myEngine->KeyHeld(Key_A) && !myEngine->KeyHeld(Key_D))
                {
                    playerCarModel->RotateY(turningVelocity * frameTime);
                    turningLeft = true;
                    turningRight = false;
                }
                else
                {
                    turningLeft = false;
                    turningRight = false;
                }
            }
            else
            {
                if (backwardVelocity < minVelocity)
                {
                    backwardVelocity += deceleration * frameTime;
                }

            }

            if (myEngine->KeyHeld(Key_D) && !myEngine->KeyHeld(Key_A))
            {
                turningRight = true;
                turningLeft = false;
            }
            else if (myEngine->KeyHeld(Key_A) && !myEngine->KeyHeld(Key_D))
            {
                turningLeft = true;
                turningRight = false;
            }
            else
            {
                turningLeft = false;
                turningRight = false;
            }

            rotationAngle = (forwardVelocity + backwardVelocity) * frameTime * turningVelocity;

            backLeftWheelNode->RotateLocalX(rotationAngle);
            backRightWheelNode->RotateLocalX(rotationAngle);
            frontLeftWheelNode->RotateLocalX(rotationAngle);
            frontRightWheelNode->RotateLocalX(rotationAngle);

            playerCarModel->MoveLocalZ((forwardVelocity + backwardVelocity) * frameTime);

            if (forwardVelocity < minVelocity) {
                forwardVelocity += decelerationAfterBounce * frameTime;
            }
            if (backwardVelocity > minVelocity) {
                backwardVelocity -= decelerationAfterBounce * frameTime;
            }


            if (turningLeft || turningRight)
            {
                if (currentWheelRotation > -maxWheelRotation && currentWheelRotation < maxWheelRotation)
                {
                    float rotationAmount = (turningLeft ? -maxWheelRotation : maxWheelRotation);

                    frontLeftWheelNode->RotateY(rotationAmount);
                    frontRightWheelNode->RotateY(rotationAmount);
                    currentWheelRotation = rotationAmount;
                }
            }
            else
            {
                frontLeftWheelNode->RotateY(-currentWheelRotation);
                frontRightWheelNode->RotateY(-currentWheelRotation);
                currentWheelRotation = minVelocity;
            }

            for (int i = 0; i < noOfTrees; i++) {
                Vector3 carToTreeVector = { playerCarModel->GetX() - perimeterTrees[i]->GetX(),
                                            playerCarModel->GetY() - perimeterTrees[i]->GetY(),
                                            playerCarModel->GetZ() - perimeterTrees[i]->GetZ() };

                float distance = calculateModulus(carToTreeVector);

                if (distance <= playerCarRadius + treeRadius) {

                    float relativeVelocity = forwardVelocity + backwardVelocity;

                    if (relativeVelocity > minVelocity) {
                        forwardVelocity = -relativeVelocity * bounceFactor;
                        backwardVelocity = minVelocity;
                    }

                    if (relativeVelocity < minVelocity) {
                        backwardVelocity = -relativeVelocity * bounceFactor;
                        forwardVelocity = minVelocity;
                    }

                    playerHealth -= 1;

                    playerCarModel->SetPosition(prevPosX, prevPosY, prevPosZ);
                }
            }

            myFont2->Draw("Car1 Timer: " + std::to_string(resetCarTimes[0]) + " seconds", carTimerXPosition, carTimerYPositions[0], kBlack, kLeft, kTop);
            myFont2->Draw("Car2 Timer: " + std::to_string(resetCarTimes[1]) + " seconds", carTimerXPosition, carTimerYPositions[1], kBlack, kLeft, kTop);
            myFont2->Draw("Car3 Timer: " + std::to_string(resetCarTimes[2]) + " seconds", carTimerXPosition, carTimerYPositions[2], kBlack, kLeft, kTop);
            myFont2->Draw("Car4 Timer: " + std::to_string(resetCarTimes[3]) + " seconds", carTimerXPosition, carTimerYPositions[3], kBlack, kLeft, kTop);

            for (int i = 0; i < numStaticEnemies; i++) {
                if (CheckCollision(playerCarModel, staticEnemies[i].enemyCarModel, playerCarRadius, enemyStaticCar)) {

                    Vector3 playerFacingVector;
                    calculateFacingVector(playerCarModel, playerFacingVector);

                    Vector3 enemyCarToJeepVector;
                    calculateEnemyCarToJeepVector(staticEnemies[i].enemyCarModel, playerCarModel, enemyCarToJeepVector);

                    dotProduct = calculateDotProduct(playerFacingVector, enemyCarToJeepVector);

                    if (staticEnemies[i].carHitStatus == false) {
                        if (dotProduct > -sideCollisionChecker) {
                            score += scoreIncreaseForFrontCollision;
                            staticEnemies[i].carSideHit = true;
                            float matrix[4][4];
                            staticEnemies[i].enemyCarModel->GetMatrix(&matrix[0][0]);
                            matrix[0][0] *= scaleFactor;
                            matrix[0][1] *= scaleFactor;
                            matrix[0][2] *= scaleFactor;
                            staticEnemies[i].enemyCarModel->SetMatrix(&matrix[0][0]);
                            staticEnemies[i].carHitStatus = true;
                        }
                        else if (dotProduct < -sideCollisionChecker) {
                            score += scoreIncreaseForSideCollision;
                            staticEnemies[i].carSideHit = false;
                            float matrix[4][4];
                            staticEnemies[i].enemyCarModel->GetMatrix(&matrix[0][0]);
                            matrix[2][0] *= scaleFactor;
                            matrix[2][1] *= scaleFactor;
                            matrix[2][2] *= scaleFactor;
                            staticEnemies[i].enemyCarModel->SetMatrix(&matrix[0][0]);
                            staticEnemies[i].carHitStatus = true;
                        }
                    }


                    float relativeVelocity = forwardVelocity + backwardVelocity;

                    if (relativeVelocity > minVelocity) {
                        forwardVelocity = -relativeVelocity * bounceFactor;
                        backwardVelocity = minVelocity;


                    }

                    if (relativeVelocity < minVelocity) {
                        backwardVelocity = -relativeVelocity * bounceFactor;
                        forwardVelocity = minVelocity;
                    }

                    playerCarModel->SetPosition(prevPosX, prevPosY, prevPosZ);
                    staticEnemies[i].sphereModel->SetSkin("red.png");
                }
            }

            for (int i = 0; i < numMovingEnemies; i++) {
                if (movingEnemies[i].carMovementStatus == true) {
                    if (i == 0 || i == 3) {
                        if (moveOppositeCar1 == false) {
                            if (movingEnemies[i].enemyCarModel->GetX() <= movingCarRange) {
                                movingEnemies[i].enemyCarModel->MoveX(movingEnemies[i].carMovementSpeed * frameTime);
                            }
                            else {
                                moveOppositeCar1 = true;
                            }
                        }
                        else {
                            if (movingEnemies[i].enemyCarModel->GetX() >= -movingCarRange) {
                                movingEnemies[i].enemyCarModel->MoveX(-movingEnemies[i].carMovementSpeed * frameTime);

                            }
                            else {
                                moveOppositeCar1 = false;
                            }
                        }
                    }
                    else {
                        if (moveOppositeCar2 == false) {
                            if (movingEnemies[i].enemyCarModel->GetX() <= movingCarRange) {
                                movingEnemies[i].enemyCarModel->MoveX(movingEnemies[i].carMovementSpeed * frameTime);

                            }
                            else {
                                moveOppositeCar2 = true;
                            }
                        }
                        else {
                            if (movingEnemies[i].enemyCarModel->GetX() >= -movingCarRange) {
                                movingEnemies[i].enemyCarModel->MoveX(-movingEnemies[i].carMovementSpeed * frameTime);

                            }
                            else {
                                moveOppositeCar2 = false;
                            }
                        }
                    }
                }

                if (CheckCollision(playerCarModel, movingEnemies[i].enemyCarModel, playerCarRadius, enemyMovingCar)) {

                    Vector3 playerFacingVector;
                    calculateFacingVector(playerCarModel, playerFacingVector);

                    Vector3 enemyCarToJeepVector;
                    calculateEnemyCarToJeepVector(movingEnemies[i].enemyCarModel, playerCarModel, enemyCarToJeepVector);

                    dotProduct = calculateDotProduct(playerFacingVector, enemyCarToJeepVector);

                    if (movingEnemies[i].carHitStatus == false) {
                        if (dotProduct < -sideCollisionChecker) {
                            score += scoreIncreaseForSideCollision;
                            movingEnemies[i].carHitStatus = true;
                            movingEnemies[i].carMovementStatus = false;
                            resetCarTimes[i] = resetCarTimeDefault;
                        }
                        else if (dotProduct > -sideCollisionChecker) {
                            score += scoreIncreaseForFrontCollision;
                            movingEnemies[i].carHitStatus = true;
                            movingEnemies[i].carMovementStatus = false;
                            resetCarTimes[i] = resetCarTimeDefault;
                        }
                    }

                    float relativeVelocity = forwardVelocity + backwardVelocity;

                    if (relativeVelocity > minVelocity) {
                        forwardVelocity = -relativeVelocity * bounceFactor;
                        backwardVelocity = minVelocity;


                    }

                    if (relativeVelocity < minVelocity) {
                        backwardVelocity = -relativeVelocity * bounceFactor;
                        forwardVelocity = minVelocity;
                    }

                    if (prevPosX < 0) {
                        prevPosX -= positionIncrement;
                    }
                    else {
                        prevPosX += positionIncrement;
                    }

                    if (prevPosZ < 0) {
                        prevPosZ -= positionIncrement;
                    }
                    else {
                        prevPosZ += positionIncrement;
                    }

                    playerCarModel->SetPosition(prevPosX, prevPosY, prevPosZ);


                    movingEnemies[i].sphereModel->SetSkin("red.png");
                }

                if (movingEnemies[i].sphereMovementStatus == true) {

                    if (moveOppositeSphere == false) {
                        if (movingEnemies[i].sphereModel->GetY() <= sphereMovingMaxRange) {
                            movingEnemies[i].sphereModel->MoveY(movingEnemies[i].sphereMovementSpeed * frameTime);
                        }
                        else {
                            moveOppositeSphere = true;
                        }
                    }
                    else {
                        if (movingEnemies[i].sphereModel->GetY() >= sphereMovingMinRange) {
                            movingEnemies[i].sphereModel->MoveY(-movingEnemies[i].sphereMovementSpeed * frameTime);
                        }
                        else {
                            moveOppositeSphere = false;
                        }
                    }
                }

                if (movingEnemies[i].carMovementStatus == false) {
                    resetCarTimes[i] += frameTime;
                    movingEnemies[i].sphereMovementSpeed -= sphereMovementSpeedDecrease * frameTime;

                    if (resetCarTimes[i] >= resetCarTimeThreshold1) {
                        movingEnemies[i].sphereMovementStatus = false;
                        movingEnemies[i].sphereMovementSpeed = sphereMovementSpeedDefault;
                    }

                    if (resetCarTimes[i] >= resetCarTimeThreshold2) {
                        movingEnemies[i].carMovementStatus = true;
                        movingEnemies[i].sphereMovementStatus = true;
                        movingEnemies[i].carHitStatus = false;
                        movingEnemies[i].sphereModel->SetSkin("white.png");

                        if (dotProduct < -sideCollisionChecker) {
                            score -= scoreIncreaseForSideCollision;
                        }
                        else if (dotProduct > -sideCollisionChecker) {
                            score -= scoreIncreaseForFrontCollision;
                        }
                    }
                }
            }

            if (movingEnemies[0].carMovementStatus == false &&
                movingEnemies[1].carMovementStatus == false &&
                movingEnemies[2].carMovementStatus == false &&
                movingEnemies[3].carMovementStatus == false) {

                allMovingCarsHit = true;
            }

            if (staticEnemies[0].carHitStatus == true &&
                staticEnemies[1].carHitStatus == true &&
                staticEnemies[2].carHitStatus == true &&
                staticEnemies[3].carHitStatus == true) {

                allStaticCarsHit = true;
            }

            // Checking win condition
            if ((allStaticCarsHit == true && allMovingCarsHit == true) || playerHealth < 1) {
                gameState = GAME_OVER;
            }

            break;

        case GAME_PAUSED:

            myFont1->Draw("Game Paused", gamePausedTextX, gamePausedTextY, kRed, kCentre);
            myFont1->Draw("Score: " + std::to_string(score), scoreTextX, scoreTextY, kBlue, kCentre);
            myFont1->Draw("Health: " + std::to_string(playerHealth), healthTextX, healthTextY, kGreen, kCentre);

            if (myEngine->KeyHit(Key_P)) {
                gameState = GAME_PLAYING;
            }

            break;

        case GAME_OVER:

            std::string outcome = (allStaticCarsHit == true && allMovingCarsHit == true) ? "You Win!" : "You Lose!";
            myFont1->Draw(outcome, gameOverTextX, gameOverTextY, kRed, kCentre);
            myFont1->Draw("Score = " + std::to_string(score), scoreTextX, scoreTextY, kRed, kCentre);
            myFont1->Draw("Tap R to Restart / Tap Esc to Quit", restartTextX, restartTextY, kBlue, kCentre);

            if (myEngine->KeyHit(Key_R)) {
                myCamera->DetachFromParent();
                myCamera->SetPosition(cameraDefaultX, cameraDefaultY, cameraDefaultZ);

                score = 0;
                playerHealth = 100;

                prevPosX = 0;
                prevPosY = 0;
                prevPosZ = 0;

                forwardVelocity = 0.0f;
                backwardVelocity = 0.0f;

                moveOppositeCar1 = false;
                moveOppositeCar2 = false;
                moveOppositeSphere = false;

                allStaticCarsHit = false;
                allMovingCarsHit = false;

                playerCarModel->SetPosition(0, 0, 0);
                playerCarModel->ResetOrientation();

                for (int i = 0; i < numStaticEnemies; ++i) {
                    staticEnemies[i].enemyCarModel->SetPosition(enemyStaticCarPositions[i][0], enemyStaticCarPositions[i][1], enemyStaticCarPositions[i][2]);
                    staticEnemies[i].sphereModel->SetSkin(defaultCarSkin);

                    if (staticEnemies[i].carSideHit == true && staticEnemies[i].carHitStatus == true) {
                        float matrix[4][4];
                        staticEnemies[i].enemyCarModel->GetMatrix(&matrix[0][0]);
                        matrix[0][0] /= scaleFactor;
                        matrix[0][1] /= scaleFactor;
                        matrix[0][2] /= scaleFactor;
                        staticEnemies[i].enemyCarModel->SetMatrix(&matrix[0][0]);
                    }

                    if (staticEnemies[i].carSideHit == false && staticEnemies[i].carHitStatus == true) {
                        float matrix[4][4];
                        staticEnemies[i].enemyCarModel->GetMatrix(&matrix[0][0]);
                        matrix[2][0] /= scaleFactor;
                        matrix[2][1] /= scaleFactor;
                        matrix[2][2] /= scaleFactor;
                        staticEnemies[i].enemyCarModel->SetMatrix(&matrix[0][0]);
                    }
                    staticEnemies[i].carHitStatus = false;

                    movingEnemies[i].enemyCarModel->SetPosition(enemyMovingCarPositions[i][0], enemyMovingCarPositions[i][1], enemyMovingCarPositions[i][2]);
                    movingEnemies[i].sphereModel->SetSkin(defaultCarSkin);
                    movingEnemies[i].sphereModel->SetLocalPosition(0, sphereModelYPosition, 0);
                    movingEnemies[i].sphereMovementSpeed = defaultSphereMovementSpeed;
                    movingEnemies[i].carHitStatus = false;
                    movingEnemies[i].carMovementStatus = true;
                    movingEnemies[i].sphereMovementStatus = true;

                    resetCarTimes[i] = 0;
                }


                gameState = GAME_PLAYING;
            }

            break;
        }
    }
    myEngine->Delete();
}
