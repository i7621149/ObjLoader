#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/NGLInit.h>
#include <iostream>
#include <ngl/ShaderLib.h>
#include <ngl/Random.h>

NGLScene::NGLScene(const std::string &_objFileName)
{
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  setTitle("Blank NGL");
  m_objFileName = _objFileName;
  m_spherePositions.empty();
}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}

void NGLScene::resizeGL(QResizeEvent *_event)
{
  m_width=_event->size().width()*devicePixelRatio();
  m_height=_event->size().height()*devicePixelRatio();
  m_project = ngl::perspective(45, float(m_width)/m_height, 0.5, 15);
}

void NGLScene::resizeGL(int _w , int _h)
{
  m_width=_w*devicePixelRatio();
  m_height=_h*devicePixelRatio();
  m_project = ngl::perspective(45, float(m_width)/m_height, 0.5, 15);
}


void NGLScene::initializeGL()
{
  // we need to initialise the NGL lib which will load all of the OpenGL functions, this must
  // be done once we have a valid GL context but before we call any GL commands. If we dont do
  // this everything will crash
  ngl::NGLInit::instance();

  ngl::ShaderLib *shader = ngl::ShaderLib::instance();

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);			   // White Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);

  m_mesh.reset( new ngl::Obj(m_objFileName, "textures/Smiley.png"));
  m_mesh->createVAO();

  m_view = ngl::lookAt(ngl::Vec3(0, 2, 10),
                       ngl::Vec3(0, 2, 0),
                       ngl::Vec3::up());

  m_rotation = 0;

  ngl::VAOPrimitives::instance()->createSphere("sphere", 0.5, 20);
  ngl::Random *rng = ngl::Random::instance();
  for(int i=0; i < 1000; i++){
    ngl::Vec3 pos = rng->getRandomPoint(15, 40, 15);
    if(pos[1] < 0) pos[1] *= -1;
    m_spherePositions.push_back(pos);
    m_sphereVelocities.push_back(0);
  }

  shader->createShaderProgram("TextureShader");

  shader->attachShader("TextureVertex", ngl::ShaderType::VERTEX);
  shader->attachShader("TextureFragment", ngl::ShaderType::FRAGMENT);
  shader->loadShaderSource("TextureVertex", "shaders/TextureVert.glsl");
  shader->loadShaderSource("TextureFragment", "shaders/TextureFrag.glsl");

  shader->compileShader("TextureVertex");
  shader->compileShader("TextureFragment");

  shader->attachShaderToProgram("TextureShader", "TextureVertex");
  shader->attachShaderToProgram("TextureShader", "TextureFragment");

  shader->linkProgramObject("TextureShader");

  shader->use("TextureShader");

  startTimer(10);
}



void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_width,m_height);

  m_transform.identity();
  m_transform.rotateY(m_rotation);
  loadMatricesToShader(0);
  m_mesh->draw();

  for(unsigned int i=0; i < m_spherePositions.size();i++){
    m_transform.identity();
    m_transform.translate(m_spherePositions[i].m_x, m_spherePositions[i].m_y, m_spherePositions[i].m_z);
    loadMatricesToShader(1);
    ngl::VAOPrimitives::instance()->draw("sphere");
  }

}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent (QMouseEvent * _event)
{

}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent ( QMouseEvent * _event)
{

}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent ( QMouseEvent * _event )
{

}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent(QWheelEvent *_event)
{

}
//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  default : break;
  }
  // finally update the GLWindow and re-draw

    update();
}

void NGLScene::loadMatricesToShader(int _mode)
{
  ngl::Mat4 MVP = m_transform * m_view * m_project;
  ngl::ShaderLib::instance()->setRegisteredUniform("MVP", MVP);
  ngl::ShaderLib::instance()->setRegisteredUniform("mode", _mode);
}

void NGLScene::timerEvent(QTimerEvent *)
{
  for(int i = 0; i < m_sphereVelocities.size(); i++){
    m_spherePositions[i].m_y += m_sphereVelocities[i];
    if(m_spherePositions[i].m_y <= 0){
      m_spherePositions[i].m_y = 0;
      m_sphereVelocities[i] *= -0.9;
    }
    else m_sphereVelocities[i] -= 0.01;
  }
  m_rotation += 1;
  update();
}
