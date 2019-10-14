#include "renderwindow.h"
#include "beziercurve.h"
#include "collision.h"
#include "gsl_math.h"
#include "lasmap.h"
#include "mainwindow.h"
#include "matrix4x4.h"
#include "rollingstone.h"
//#include "LASLoader.h"
#include "shader.h"
#include "tree.h"
#include "trianglesurface.h"
#include "xyz.h"
#include <QDebug>
#include <QKeyEvent>
#include <QOpenGLContext>
#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions>
#include <QStatusBar>
#include <QTimer>

RenderWindow::RenderWindow(const QSurfaceFormat &format, MainWindow *mainWindow)
    : mContext(nullptr), mInitialized(false), mMainWindow(mainWindow) {
    //This is sent to QWindow:
    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(format);
    //Make the OpenGL context
    mContext = new QOpenGLContext(this);
    //Give the context the wanted OpenGL format (v4.1 Core)
    mContext->setFormat(requestedFormat());
    if (!mContext->create()) {
        delete mContext;
        mContext = nullptr;
        qDebug() << "Context could not be made - quitting this application";
    }

    //Make the gameloop timer:
    mRenderTimer = new QTimer(this);
}

RenderWindow::~RenderWindow() {
}

/// Sets up the general OpenGL stuff and the buffers needed to render a triangle
void RenderWindow::init() {
    //Connect the gameloop timer to the render function:
    connect(mRenderTimer, SIGNAL(timeout()), this, SLOT(render()));

    //********************** General OpenGL stuff **********************

    //The OpenGL context has to be set.
    //The context belongs to the instanse of this class!
    if (!mContext->makeCurrent(this)) {
        qDebug() << "makeCurrent() failed";
        return;
    }

    //just to make sure we don't init several times
    //used in exposeEvent()
    if (!mInitialized)
        mInitialized = true;

    //must call this to use OpenGL functions
    initializeOpenGLFunctions();

    //Start the Qt OpenGL debugger
    //Really helpfull when doing OpenGL
    //Supported on most Windows machines
    //reverts to plain glGetError() on Mac and other unsupported PCs
    // - can be deleted
    startOpenGLDebugger();

    //general OpenGL stuff:
    glEnable(GL_DEPTH_TEST);              //enables depth sorting - must use GL_DEPTH_BUFFER_BIT in glClear
    glEnable(GL_CULL_FACE);               //draws only front side of models - usually what you want -
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f); //color used in glClear GL_COLOR_BUFFER_BIT

    //Compile shaders:
    //NB: hardcoded path to files! You have to change this if you change directories for the project.
    //Qt makes a build-folder besides the project folder. That is why we go down one directory
    // (out of the build-folder) and then up into the project folder.
    mShaderProgram[0] = new Shader("../VSIMOblig/plainvertex.vert", "../VSIMOblig/plainfragment.frag");
    qDebug() << "Plain shader program id: " << mShaderProgram[0]->getProgram();
    mShaderProgram[1] = new Shader("../VSIMOblig/texturevertex.vert", "../VSIMOblig/texturefragmet.frag");
    qDebug() << "Texture shader program id: " << mShaderProgram[1]->getProgram();

    setupPlainShader(0);
    setupTextureShader(1);
    collisionSystem = new Collision;

    //**********************  Texture stuff: **********************
    //    mTexture[0] = new Texture();
    //    mTexture[1] = new Texture("../VSIMOblig/Assets/hund.bmp");

    //    //Set the textures loaded to a texture unit
    //    glActiveTexture(GL_TEXTURE0);
    //    glBindTexture(GL_TEXTURE_2D, mTexture[0]->id());
    //    glActiveTexture(GL_TEXTURE1);
    //    glBindTexture(GL_TEXTURE_2D, mTexture[1]->id());
    //********************** Making the objects to be drawn **********************
    VisualObject *temp = new XYZ();
    mVisualObjects.push_back(temp);

    TriangleSurface *mSurface = new TriangleSurface("../VSIMOblig/Assets/triangles.txt");
    mSurface->move(vec3(-2, 3, -2));
    mSurface->rotate(vec3(0, 0, 10));
    mSurface->scale(5);
    mVisualObjects.push_back(mSurface);
    TriangleSurface *mSurface2 = new TriangleSurface("../VSIMOblig/Assets/triangles.txt");
    mSurface2->move(vec3(5, -4, -2));
    mSurface2->scale(5);
    mVisualObjects.push_back(mSurface2);

    pawn = new RollingStone;
    mVisualObjects.push_back(pawn);
    pawn->move(vec3(1.2, 4.7, 1));
    //    TriangleSurface *mSurface = new TriangleSurface("../VSIMOblig/Assets/triangles.txt");
    //    mSurface->move(vec3(-2, 3, -2));
    //    mSurface->rotate(vec3(0, 0, 30));
    //    mSurface->scale(5);
    //    mVisualObjects.push_back(mSurface);
    //    TriangleSurface *mSurface2 = new TriangleSurface("../VSIMOblig/Assets/triangles.txt");
    //    mSurface2->move(vec3(6.5, -4, -2));
    //    mSurface2->scale(5);
    //    mVisualObjects.push_back(mSurface2);

    //    pawn = new RollingStone;
    //    mVisualObjects.push_back(pawn);
    //    pawn->move(vec3(1.2, 5.5, 1));

    //    gsl::LASLoader *mTestMap = new gsl::LASLoader("../VSIMOblig/LASdata/33-1-497-327-20.las"); ////Get LASLoader to read correct constructor
    ////    mTestMap->readFile("../VSIMOblig/LASdata/33-1-497-327-20.las");
    ////    mTestMap->scale(0.001f);
    //    mVisualObjects.push_back(mTestMap);

    LasMap *mTestMap = new LasMap();
    //mTestMap->scale(10);
    mVisualObjects.push_back(mTestMap);
    //    mSurface->createSurface();
    //    mSurface->move(gsl::Vector3D(-3, 0, -3));
    //    mSurface->scale(gsl::Vector3D(3, 1, 3));
    //    mSurfacePoints = mSurface->getTrianglePoints(); // Getting this here since we never plan to change mSurface at any point.
    //    // Creating the player object
    //    std::string cylinder = "../VSIMOblig/Assets/Cylinder.txt";
    //    mVisualObjects.push_back(mSurface);
    //    mPlayer = new TriangleSurface();
    //    mPlayer->readFile(cylinder);
    //    mPlayer->move(gsl::Vector3D(2.5, 0, 0));
    //    mPlayer->scale(gsl::Vector3D(0.5, 0.5, 2));
    //    mPlayer->rotate(gsl::Vector3D(90, 0, 0));
    //    mVisualObjects.push_back(mPlayer);

    //    // Creating the NPC
    //    mNPC = new TriangleSurface();
    //    mNPC->readFile(cylinder);
    //    mNPC->scale(gsl::Vector3D(0.5, 0.5, 2));
    //    mNPC->rotate(gsl::Vector3D(90, 0, 0));
    //    mVisualObjects.push_back(mNPC);
    //    // Bezier Curve
    //    gsl::Vector3D point1{-2.5, 0, -2}, point2{1, 0, -0.5}, point3{-1.5, 0, 1}, point4{1.5, 0, 2.5};
    //    std::vector<gsl::Vector3D> controlPoints{point1, point2, point3, point4};
    //    BezierCurve *curve = new BezierCurve(controlPoints, 3);
    //    bezierPoints = curve->getPoints();
    //    mVisualObjects.push_back(curve);
    //    // Creating the popler trees
    //    Tree *mTree1 = new Tree();
    //    mTree1->move(gsl::Vector3D(1, 0, -.5));
    //    mVisualObjects.push_back(mTree1);
    //    Tree *mTree2 = new Tree();
    //    mTree2->move(gsl::Vector3D(-1.5, 0, 1));
    //    mVisualObjects.push_back(mTree2);

    //********************** Set up camera **********************
    mCurrentCamera = new Camera();
    mCurrentCamera->setPosition(gsl::Vector3D(-2.f, -7.5f, 10.f));
    mCurrentCamera->yaw(-1.6);
    mCurrentCamera->pitch(-30.6);
    for (VisualObject *object : mVisualObjects) {
        object->init();
    }
}

///Called each frame - doing the rendering
void RenderWindow::render() {
    //input
    handleInput();

    mCurrentCamera->update();

    mTimeStart.restart();        //restart FPS clock
    mContext->makeCurrent(this); //must be called every frame (every time mContext->swapBuffers is called)

    //to clear the screen for each redraw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (VisualObject *object : mVisualObjects) {
        if (object->getUseTextures()) {
            glUseProgram(mShaderProgram[1]->getProgram());
            glUniformMatrix4fv(vMatrixUniform1, 1, GL_TRUE, mCurrentCamera->mViewMatrix.constData());
            glUniformMatrix4fv(pMatrixUniform1, 1, GL_TRUE, mCurrentCamera->mProjectionMatrix.constData());
            glUniformMatrix4fv(mMatrixUniform1, 1, GL_TRUE, object->getModelMatrix().constData());
            glUniform1i(mTextureUniform, 1);
        } else {
            glUseProgram(mShaderProgram[0]->getProgram());
            glUniformMatrix4fv(vMatrixUniform0, 1, GL_TRUE, mCurrentCamera->mViewMatrix.constData());
            glUniformMatrix4fv(pMatrixUniform0, 1, GL_TRUE, mCurrentCamera->mProjectionMatrix.constData());
            glUniformMatrix4fv(mMatrixUniform0, 1, GL_TRUE, object->getModelMatrix().constData());
        }
        object->draw();
    }
    vec3 baryc;
    for (VisualObject *obj : mVisualObjects) {
        bool foundTriangle{false};
        if (TriangleSurface *triangle = dynamic_cast<TriangleSurface *>(obj)) {
            std::vector<vec3> triPoints = triangle->getTrianglePoints();
            for (size_t i = 0; i < triPoints.size(); i += 3) {
                baryc = collisionSystem->barycentricCoordinates(pawn->getPosition(), triPoints[i], triPoints[i + 1], triPoints[i + 2]);
                if (gsl::within(baryc.x) && gsl::within(baryc.y) && (baryc.x + baryc.y) <= 1) {
                    pawn->currentTriangle = std::vector<vec3>{triPoints[i], triPoints[i + 1], triPoints[i + 2]};
                    foundTriangle = true;
                    break;
                }
            }
            if (foundTriangle)
                break;
        }
        if (!foundTriangle) {
            pawn->currentTriangle.clear();
        }
    }
    pawn->update();
    //    if (!playerCaught)
    //    {
    //        consumeMovementInput(0.016f);
    //        if (detectPlayer())
    //        {
    //            chasePlayer();
    //        }
    //        else
    //            moveAlongLine(0.016f);
    //    }

    //Calculate framerate before
    // checkForGLerrors() because that takes a long time
    // and before swapBuffers(), else it will show the vsync time
    calculateFramerate();

    //using our expanded OpenGL debugger to check if everything is OK.
    checkForGLerrors();

    //Qt require us to call this swapBuffers() -function.
    // swapInterval is 1 by default which means that swapBuffers() will (hopefully) block
    // and wait for vsync.
    mContext->swapBuffers(this);
}

void RenderWindow::setupPlainShader(int shaderIndex) {
    mMatrixUniform0 = glGetUniformLocation(mShaderProgram[shaderIndex]->getProgram(), "mMatrix");
    vMatrixUniform0 = glGetUniformLocation(mShaderProgram[shaderIndex]->getProgram(), "vMatrix");
    pMatrixUniform0 = glGetUniformLocation(mShaderProgram[shaderIndex]->getProgram(), "pMatrix");
}

void RenderWindow::setupTextureShader(int shaderIndex) {
    mMatrixUniform1 = glGetUniformLocation(mShaderProgram[shaderIndex]->getProgram(), "mMatrix");
    vMatrixUniform1 = glGetUniformLocation(mShaderProgram[shaderIndex]->getProgram(), "vMatrix");
    pMatrixUniform1 = glGetUniformLocation(mShaderProgram[shaderIndex]->getProgram(), "pMatrix");
    mTextureUniform = glGetUniformLocation(mShaderProgram[shaderIndex]->getProgram(), "textureSampler");
}
void RenderWindow::moveAlongLine(float deltaTime) {
    if (curNode == bezierPoints.size() - 1) // If you reach the end of the line, repeat in negative direction
        dir = false;
    if (t >= 1) {
        t = 0;                               // Reset t, you're starting a new path
        startLoc = bezierPoints.at(curNode); // New start point is last node
        if (dir)
            curNode++; // move on to the next node
        else
            curNode--;
    }
    if (curNode == 0)
        dir = true;
    t += deltaTime * mNPCSpeed; // t decides how far along the line the pawn should be
    vec3 pointOnLine = gsl::lerp3D(t, startLoc, bezierPoints.at(curNode));

    mNPC->move(pointOnLine);
}
bool RenderWindow::detectPlayer() {
    //    if ((mNPC->getPosition() - mPlayer->getPosition()).length() <= 2.5f) {
    //        mNPC->setUseTextures(true);
    //        startLoc = bezierPoints.at(0);
    //        t = 0;
    //        curNode = 1;
    //        return true;
    //    } else {
    //        mNPC->setUseTextures(false);
    //        return false;
    //    }
    return false;
}
void RenderWindow::chasePlayer() {
    //    gsl::Vector3D distanceToPlayer = mPlayer->getPosition() - mNPC->getPosition();
    //    if (distanceToPlayer.length() <= 1.0f) // each of the cylinders have radius 0.5. This means they will collide at 0.5+0.5 = 1.0f
    //    {
    //        playerCaught = true;
    //        return;
    //    }
    //    gsl::Vector3D moveVector = distanceToPlayer.normalized() * 0.016 * mNPCSpeed;
    //    mNPC->move(mNPC->getPosition() + moveVector);
}

//This function is called from Qt when window is exposed (shown)
//and when it is resized
//exposeEvent is a overridden function from QWindow that we inherit from
void RenderWindow::exposeEvent(QExposeEvent *) {
    if (!mInitialized)
        init();

    //This is just to support modern screens with "double" pixels
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, static_cast<GLint>(width() * retinaScale), static_cast<GLint>(height() * retinaScale));

    //If the window actually is exposed to the screen we start the main loop
    //isExposed() is a function in QWindow
    if (isExposed()) {
        //This timer runs the actual MainLoop
        //16 means 16ms = 60 Frames pr second (should be 16.6666666 to be exact..)
        mRenderTimer->start(16);
        mTimeStart.start();
    }
    mAspectratio = static_cast<float>(width()) / height();
    //    qDebug() << mAspectratio;
    mCurrentCamera->mProjectionMatrix.perspective(45.f, mAspectratio, 1.f, 100.f);
    //    qDebug() << mCamera.mProjectionMatrix;
}

//Simple way to turn on/off wireframe mode
//Not totally accurate, but draws the objects with
//lines instead of filled polygons
void RenderWindow::toggleWireframe() {
    mWireframe = !mWireframe;
    if (mWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //turn on wireframe mode
        glDisable(GL_CULL_FACE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //turn off wireframe mode
        glEnable(GL_CULL_FACE);
    }
}

//The way this is set up is that we start the clock before doing the draw call,
//and check the time right after it is finished (done in the render function)
//This will approximate what framerate we COULD have.
//The actual frame rate on your monitor is limited by the vsync and is probably 60Hz
void RenderWindow::calculateFramerate() {
    long long nsecElapsed = mTimeStart.nsecsElapsed();
    static int frameCount{0}; //counting actual frames for a quick "timer" for the statusbar

    if (mMainWindow) //if no mainWindow, something is really wrong...
    {
        ++frameCount;
        if (frameCount > 30) //once pr 30 frames = update the message twice pr second (on a 60Hz monitor)
        {
            //showing some statistics in status bar
            mMainWindow->statusBar()->showMessage(" Time pr FrameDraw: " +
                                                  QString::number(nsecElapsed / 1000000., 'g', 4) + " ms  |  " +
                                                  "FPS (approximated): " + QString::number(1E9 / nsecElapsed, 'g', 7));
            frameCount = 0; //reset to show a new message in 60 frames
        }
    }
}

/// Uses QOpenGLDebugLogger if this is present
/// Reverts to glGetError() if not
void RenderWindow::checkForGLerrors() {
    if (mOpenGLDebugLogger) {
        const QList<QOpenGLDebugMessage> messages = mOpenGLDebugLogger->loggedMessages();
        for (const QOpenGLDebugMessage &message : messages)
            qDebug() << message;
    } else {
        GLenum err = GL_NO_ERROR;
        while ((err = glGetError()) != GL_NO_ERROR) {
            qDebug() << "glGetError returns " << err;
        }
    }
}

/// Tries to start the extended OpenGL debugger that comes with Qt
void RenderWindow::startOpenGLDebugger() {
    QOpenGLContext *temp = this->context();
    if (temp) {
        QSurfaceFormat format = temp->format();
        if (!format.testOption(QSurfaceFormat::DebugContext))
            qDebug() << "This system can not use QOpenGLDebugLogger, so we revert to glGetError()";

        if (temp->hasExtension(QByteArrayLiteral("GL_KHR_debug"))) {
            qDebug() << "System can log OpenGL errors!";
            mOpenGLDebugLogger = new QOpenGLDebugLogger(this);
            if (mOpenGLDebugLogger->initialize()) // initializes in the current context
                qDebug() << "Started OpenGL debug logger!";
        }

        if (mOpenGLDebugLogger)
            mOpenGLDebugLogger->disableMessages(QOpenGLDebugMessage::APISource, QOpenGLDebugMessage::OtherType, QOpenGLDebugMessage::NotificationSeverity);
    }
}

void RenderWindow::setCameraSpeed(float value) {
    mCameraSpeed += value;

    //Keep within min and max values
    if (mCameraSpeed < 0.01f)
        mCameraSpeed = 0.01f;
    if (mCameraSpeed > 0.3f)
        mCameraSpeed = 0.3f;
}

void RenderWindow::handleInput() {
    //Camera
    mCurrentCamera->setSpeed(0.f); //cancel last frame movement
    if (mInput.RMB) {
        if (mInput.W)
            mCurrentCamera->setSpeed(-mCameraSpeed);
        if (mInput.S)
            mCurrentCamera->setSpeed(mCameraSpeed);
        if (mInput.D)
            mCurrentCamera->moveRight(mCameraSpeed);
        if (mInput.A)
            mCurrentCamera->moveRight(-mCameraSpeed);
        if (mInput.Q)
            mCurrentCamera->updateHeigth(mCameraSpeed);
        if (mInput.E)
            mCurrentCamera->updateHeigth(-mCameraSpeed);
    }
}
/**
 * @brief consumeMovementInput Calculates desired velocity and applies it to the pawn.
 * Resets the given vector to zero after using it, but saves the old vector to a private member for recovery if needed.
 * @param deltaTime Time in seconds since the last tick
 */
void RenderWindow::consumeMovementInput(float deltaTime) {
    calculateKeyInputs(); // Get the actual desired movement from key inputs
    int triCheck{0};
    gsl::Vector3D counterForce;
    for (unsigned int i{0}; i < mSurfacePoints.size(); i += 3) {
        // this is relatively simple since we don't index the triangles, just get every three vertices' XYZ and assume they're part of the same triangle.
        gsl::Vector3D baryc = barycentricCoordinates(mSurfacePoints[i], mSurfacePoints[i + 1], mSurfacePoints[i + 2]);
        if (baryc.x < 0 || baryc.y < 0 || baryc.z < 0) {
            triCheck++;
        }
        qDebug() << baryc;
    }
    if (triCheck >= mSurfacePoints.size() / 3) {
        counterForce = mDesiredVelocity.normalized();
    }
    if (mDesiredVelocity.length() != 0) {
        gsl::Vector3D moveVector = mDesiredVelocity.normalized() * deltaTime * mPlayerSpeed;
        //        mPlayer->move(mPlayer->getPosition() + moveVector - counterForce);
        mDesiredVelocity = gsl::Vector3D();
    }
}
/**
 * @brief barycentricCoordinates computes the barycentric coordinates u,v,w for a point with respect to a triangle
 * @param pointA Point A of the triangle
 * @param pointB Point B of the triangle
 * @param pointC Point C of the triangle
 * @return barycentric coordinates
 */
vec3 RenderWindow::barycentricCoordinates(const vec3 &pointA, const vec3 &pointB, const vec3 &pointC) {
    vec3 v0 = pointB - pointA;
    vec3 v1 = pointC - pointA;
    vec3 v2 = pointA;

    float v0Sqr = gsl::Vector3D::dot(v0, v0);
    float v0Dotv1 = gsl::Vector3D::dot(v0, v1);
    float v1Sqr = gsl::Vector3D::dot(v1, v1);
    float v2Dotv0 = gsl::Vector3D::dot(v2, v0);
    float v2Dotv1 = gsl::Vector3D::dot(v2, v1);
    float denom = v0Sqr * v1Sqr - v0Dotv1 * v0Dotv1;
    vec3 baryc;
    baryc.setX((v1Sqr * v2Dotv0 - v0Dotv1 * v2Dotv1) / denom);
    baryc.setY((v0Sqr * v2Dotv1 - v0Dotv1 * v2Dotv0) / denom);
    baryc.setZ(1.0f - baryc.getX() - baryc.getY());
    //    qDebug() << baryc;
    return baryc;
}
void RenderWindow::calculateKeyInputs() {

    if (mInput.UP) {
        mDesiredVelocity -= gsl::Vector3D(0, 0, -1); // Forward (W) and Back (S) are reversed because in OpenGL -Z is forward.
    }
    if (mInput.DOWN)
        mDesiredVelocity += gsl::Vector3D(0, 0, -1);
    if (mInput.LEFT)
        mDesiredVelocity -= gsl::Vector3D(1, 0, 0);
    if (mInput.RIGHT)
        mDesiredVelocity += gsl::Vector3D(1, 0, 0);
}
void RenderWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) //Shuts down whole program
    {
        mMainWindow->close();
    }

    //    You get the keyboard input like this
    if (event->key() == Qt::Key_W) {
        mInput.W = true;
    }
    if (event->key() == Qt::Key_S) {
        mInput.S = true;
    }
    if (event->key() == Qt::Key_D) {
        mInput.D = true;
    }
    if (event->key() == Qt::Key_A) {
        mInput.A = true;
    }
    if (event->key() == Qt::Key_Q) {
        mInput.Q = true;
    }
    if (event->key() == Qt::Key_E) {
        mInput.E = true;
    }
    if (event->key() == Qt::Key_Z) {
    }
    if (event->key() == Qt::Key_X) {
    }
    if (event->key() == Qt::Key_Up) {
        mInput.UP = true;
    }
    if (event->key() == Qt::Key_Down) {
        mInput.DOWN = true;
    }
    if (event->key() == Qt::Key_Left) {
        mInput.LEFT = true;
    }
    if (event->key() == Qt::Key_Right) {
        mInput.RIGHT = true;
    }
    if (event->key() == Qt::Key_U) {
    }
    if (event->key() == Qt::Key_O) {
    }
}

void RenderWindow::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_W) {
        mInput.W = false;
    }
    if (event->key() == Qt::Key_S) {
        mInput.S = false;
    }
    if (event->key() == Qt::Key_D) {
        mInput.D = false;
    }
    if (event->key() == Qt::Key_A) {
        mInput.A = false;
    }
    if (event->key() == Qt::Key_Q) {
        mInput.Q = false;
    }
    if (event->key() == Qt::Key_E) {
        mInput.E = false;
    }
    if (event->key() == Qt::Key_Z) {
    }
    if (event->key() == Qt::Key_X) {
    }
    if (event->key() == Qt::Key_Up) {
        mInput.UP = false;
    }
    if (event->key() == Qt::Key_Down) {
        mInput.DOWN = false;
    }
    if (event->key() == Qt::Key_Left) {
        mInput.LEFT = false;
    }
    if (event->key() == Qt::Key_Right) {
        mInput.RIGHT = false;
    }
    if (event->key() == Qt::Key_U) {
    }
    if (event->key() == Qt::Key_O) {
    }
}

void RenderWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton)
        mInput.RMB = true;
    if (event->button() == Qt::LeftButton)
        mInput.LMB = true;
    if (event->button() == Qt::MiddleButton)
        mInput.MMB = true;
}

void RenderWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton)
        mInput.RMB = false;
    if (event->button() == Qt::LeftButton)
        mInput.LMB = false;
    if (event->button() == Qt::MiddleButton)
        mInput.MMB = false;
}

void RenderWindow::wheelEvent(QWheelEvent *event) {
    QPoint numDegrees = event->angleDelta() / 8;

    //if RMB, change the speed of the camera
    if (mInput.RMB) {
        if (numDegrees.y() < 1)
            setCameraSpeed(0.001f);
        if (numDegrees.y() > 1)
            setCameraSpeed(-0.001f);
    }
    event->accept();
}

void RenderWindow::mouseMoveEvent(QMouseEvent *event) {
    if (mInput.RMB) {
        //Using mMouseXYlast as deltaXY so we don't need extra variables
        mMouseXlast = event->pos().x() - mMouseXlast;
        mMouseYlast = event->pos().y() - mMouseYlast;

        if (mMouseXlast != 0)
            mCurrentCamera->yaw(mCameraRotateSpeed * mMouseXlast);
        if (mMouseYlast != 0)
            mCurrentCamera->pitch(mCameraRotateSpeed * mMouseYlast);
    }
    mMouseXlast = event->pos().x();
    mMouseYlast = event->pos().y();
}
