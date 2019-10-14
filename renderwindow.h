#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include "camera.h"
#include "input.h"
#include "matrix4x4.h"
#include "texture.h"
#include "visualobject.h"
#include <QElapsedTimer>
#include <QOpenGLFunctions_4_1_Core>
#include <QTimer>
#include <QWindow>
#include <vector>

class QOpenGLContext;
class Shader;
class TriangleSurface;
class MainWindow;
class RollingStone;
class Collision;
typedef gsl::Vector3D vec3;
/// This inherits from QWindow to get access to the Qt functionality and
/// OpenGL surface.
/// We also inherit from QOpenGLFunctions, to get access to the OpenGL functions
/// This is the same as using glad and glw from general OpenGL tutorials
class RenderWindow : public QWindow, protected QOpenGLFunctions_4_1_Core {
    Q_OBJECT
public:
    RenderWindow(const QSurfaceFormat &format, MainWindow *mainWindow);
    ~RenderWindow() override;

    QOpenGLContext *context() { return mContext; }

    void exposeEvent(QExposeEvent *) override;
    void toggleWireframe();

    void checkForGLerrors();

    bool detectPlayer();
private slots:
    void render();

private:
    // Player variables
    RollingStone *pawn;
    float mPlayerSpeed{3.f};
    gsl::Vector3D mDesiredVelocity{0, 0, 0};
    void calculateKeyInputs();
    bool playerCaught{false};
    std::vector<gsl::Vector3D> mSurfacePoints;
    Collision *collisionSystem;

    TriangleSurface *mNPC;
    float mNPCSpeed{1.5};
    unsigned int curNode{1};
    gsl::Vector3D startLoc;
    std::vector<gsl::Vector3D> bezierPoints;
    float t{0};
    bool dir = true;

    void init();
    void setCameraSpeed(float value);

    QOpenGLContext *mContext{nullptr};
    bool mInitialized{false};

    Texture *mTexture[4]{nullptr};      //We can hold 4 textures
    Shader *mShaderProgram[4]{nullptr}; //We can hold 4 shaders

    void setupPlainShader(int shaderIndex);
    GLint mMatrixUniform0{-1};
    GLint vMatrixUniform0{-1};
    GLint pMatrixUniform0{-1};

    void setupTextureShader(int shaderIndex);
    GLint mMatrixUniform1{-1};
    GLint vMatrixUniform1{-1};
    GLint pMatrixUniform1{-1};
    GLint mTextureUniform{-1};

    std::vector<VisualObject *> mVisualObjects;

    Camera *mCurrentCamera{nullptr};

    bool mWireframe{false};

    Input mInput;
    float mCameraSpeed{0.05f};
    float mCameraRotateSpeed{0.1f};
    int mMouseXlast{0};
    int mMouseYlast{0};

    QTimer *mRenderTimer{nullptr}; //timer that drives the gameloop
    QElapsedTimer mTimeStart;      //time variable that reads the actual FPS

    float mAspectratio{1.f};

    MainWindow *mMainWindow{nullptr}; //points back to MainWindow to be able to put info in StatusBar

    class QOpenGLDebugLogger *mOpenGLDebugLogger{nullptr};

    void calculateFramerate();

    void startOpenGLDebugger();

    void handleInput();

    void consumeMovementInput(float deltaTime);

    void moveAlongLine(float deltaTime);

    void chasePlayer();

    vec3 barycentricCoordinates(const vec3 &pointA, const vec3 &pointB, const vec3 &pointC);

protected:
    //The QWindow that we inherit from has these functions to capture
    // mouse and keyboard. Uncomment to use
    //
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
};

#endif // RENDERWINDOW_H
