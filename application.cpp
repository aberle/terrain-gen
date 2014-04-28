//
//  Application Widget
//

#include <QtGui>
#include "application.h"
#include "terrainview.h"
#include "drawNoise.h"
#include "simplexnoise.h"

//
//  Constructor for Application widget
//
Application::Application()
{
   //  Set window title and location
   setWindowTitle(tr("Nicholas Aberle: Simplex Noise Terrain Generation"));
   move(0,0);

   //  Create new terrainView widget
   TerrainView* terrainView = new TerrainView;
   terrainView->setFixedSize(800,800);

   // Create new drawNoise widget popup
   DrawNoise* drawNoise = new DrawNoise;
   drawNoise->setFixedSize(NOISE_HEIGHT-32,NOISE_WIDTH-32);
   drawNoise->setWindowTitle("Noise Texture");
   drawNoise->move(1100,0);
   drawNoise->show();

   // Create text entry fields
   QValidator* validator = new QIntValidator(2, 256);
   QLineEdit* turb = new QLineEdit("105");
   turb->setFixedWidth(50);
   turb->setValidator(validator);

   QValidator* validator2 = new QDoubleValidator(1.0, 64.0, 1);
   QLineEdit* oct = new QLineEdit("1.0");
   oct->setFixedWidth(50);
   oct->setValidator(validator2);

   QLineEdit* pers = new QLineEdit("1.0");
   pers->setFixedWidth(50);
   pers->setValidator(validator2);

   QLineEdit* amp = new QLineEdit("1.0");
   amp->setFixedWidth(50);
   amp->setValidator(validator2);

   // Create shader button
   QPushButton *button = new QPushButton("Regenerate Terrain");

   //  Create slider and set range to 1-9
   QSlider* timeSlider = new QSlider(Qt::Horizontal);
   timeSlider->setRange(1,9);
   //  Eye candy - set tick interval for display
   timeSlider->setTickInterval(1);
   timeSlider->setTickPosition(QSlider::TicksBelow);

   //  Create slider and set range to 0-10
   QSlider* opacitySlider = new QSlider(Qt::Horizontal);
   opacitySlider->setRange(0,10);
   //  Eye candy - set tick interval for display
   opacitySlider->setTickInterval(1);
   opacitySlider->setTickPosition(QSlider::TicksBelow);
   opacitySlider->setValue(5);

   //  Create slider and set range to 0-30
   QSlider* densitySlider = new QSlider(Qt::Horizontal);
   densitySlider->setRange(1,30);
   //  Eye candy - set tick interval for display
   densitySlider->setTickInterval(5);
   densitySlider->setTickPosition(QSlider::TicksBelow);
   densitySlider->setValue(20);

   //  Create slider and set range to 0-100
   QSlider* waterSlider = new QSlider(Qt::Horizontal);
   waterSlider->setRange(1,100);
   //  Eye candy - set tick interval for display
   waterSlider->setTickInterval(5);
   waterSlider->setTickPosition(QSlider::TicksBelow);
   waterSlider->setValue(35);

   //  Create slider and set range to 0-50
   QSlider* blendSlider = new QSlider(Qt::Horizontal);
   blendSlider->setRange(1,50);
   //  Eye candy - set tick interval for display
   blendSlider->setTickInterval(5);
   blendSlider->setTickPosition(QSlider::TicksBelow);
   blendSlider->setValue(15);

   //  Create slider and set range to -30-160
   QSlider* sandLowSlider = new QSlider(Qt::Horizontal);
   sandLowSlider->setRange(-30,160);
   //  Eye candy - set tick interval for display
   sandLowSlider->setTickInterval(5);
   sandLowSlider->setTickPosition(QSlider::TicksBelow);
   sandLowSlider->setValue(-15);

   //  Create slider and set range to -30-160
   QSlider* sandHighSlider = new QSlider(Qt::Horizontal);
   sandHighSlider->setRange(-30,160);
   //  Eye candy - set tick interval for display
   sandHighSlider->setTickInterval(5);
   sandHighSlider->setTickPosition(QSlider::TicksBelow);
   sandHighSlider->setValue(30);

   //  Create slider and set range to -30-160
   QSlider* grassLowSlider = new QSlider(Qt::Horizontal);
   grassLowSlider->setRange(-30,160);
   //  Eye candy - set tick interval for display
   grassLowSlider->setTickInterval(5);
   grassLowSlider->setTickPosition(QSlider::TicksBelow);
   grassLowSlider->setValue(30);

   //  Create slider and set range to -30-160
   QSlider* grassHighSlider = new QSlider(Qt::Horizontal);
   grassHighSlider->setRange(-30,160);
   //  Eye candy - set tick interval for display
   grassHighSlider->setTickInterval(5);
   grassHighSlider->setTickPosition(QSlider::TicksBelow);
   grassHighSlider->setValue(45);

   //  Create slider and set range to -30-160
   QSlider* rockLowSlider = new QSlider(Qt::Horizontal);
   rockLowSlider->setRange(-30,160);
   //  Eye candy - set tick interval for display
   rockLowSlider->setTickInterval(5);
   rockLowSlider->setTickPosition(QSlider::TicksBelow);
   rockLowSlider->setValue(45);

   //  Create slider and set range to -30-160
   QSlider* rockHighSlider = new QSlider(Qt::Horizontal);
   rockHighSlider->setRange(-30,160);
   //  Eye candy - set tick interval for display
   rockHighSlider->setTickInterval(5);
   rockHighSlider->setTickPosition(QSlider::TicksBelow);
   rockHighSlider->setValue(70);

   //  Create slider and set range to -30-160
   QSlider* snowLowSlider = new QSlider(Qt::Horizontal);
   snowLowSlider->setRange(-30,160);
   //  Eye candy - set tick interval for display
   snowLowSlider->setTickInterval(5);
   snowLowSlider->setTickPosition(QSlider::TicksBelow);
   snowLowSlider->setValue(70);

   //  Create slider and set range to -30-160
   QSlider* snowHighSlider = new QSlider(Qt::Horizontal);
   snowHighSlider->setRange(-30,160);
   //  Eye candy - set tick interval for display
   snowHighSlider->setTickInterval(5);
   snowHighSlider->setTickPosition(QSlider::TicksBelow);
   snowHighSlider->setValue(150);

   // Big controls container
   QGroupBox* controls = new QGroupBox;
   QVBoxLayout* controlsLayout = new QVBoxLayout;

   // Container for environment controls
   QGroupBox* envControls = new QGroupBox("Envionment Controls");
   QVBoxLayout* envLayout = new QVBoxLayout;
   envLayout->addWidget(new QLabel("Time Rate"));
   envLayout->addWidget(timeSlider);
   envLayout->addWidget(new QLabel("Cloud Opacity"));
   envLayout->addWidget(opacitySlider);
   envLayout->addWidget(new QLabel("Cloud Density"));
   envLayout->addWidget(densitySlider);
   envLayout->addWidget(new QLabel("Water Height"));
   envLayout->addWidget(waterSlider);
   envControls->setLayout(envLayout);

   // Container for texturing controls
   QGroupBox* texControls = new QGroupBox("Texturing Controls");
   QVBoxLayout* texLayout = new QVBoxLayout;
   texLayout->addWidget(new QLabel("Blend Level"));
   texLayout->addWidget(blendSlider);
   texLayout->addWidget(new QLabel("Sand Low Bound"));
   texLayout->addWidget(sandLowSlider);
   texLayout->addWidget(new QLabel("Sand High Bound"));
   texLayout->addWidget(sandHighSlider);
   texLayout->addWidget(new QLabel("Grass Low Bound"));
   texLayout->addWidget(grassLowSlider);
   texLayout->addWidget(new QLabel("Grass High Bound"));
   texLayout->addWidget(grassHighSlider);
   texLayout->addWidget(new QLabel("Rock Low Bound"));
   texLayout->addWidget(rockLowSlider);
   texLayout->addWidget(new QLabel("Rock High Bound"));
   texLayout->addWidget(rockHighSlider);
   texLayout->addWidget(new QLabel("Snow Low Bound"));
   texLayout->addWidget(snowLowSlider);
   texLayout->addWidget(new QLabel("Snow High Bound"));
   texLayout->addWidget(snowHighSlider);
   texControls->setLayout(texLayout);

   // Container for noise controls
   QGroupBox* noiseControls = new QGroupBox("Noise Controls");
   QVBoxLayout* noiseLayout = new QVBoxLayout;
   noiseLayout->addWidget(new QLabel("Smoothing Passes (2-256)"));
   noiseLayout->addWidget(turb);
   noiseLayout->addWidget(new QLabel("Octaves (1.0-64.0)"));
   noiseLayout->addWidget(oct);
   noiseLayout->addWidget(new QLabel("Persistence (1.0-64.0)"));
   noiseLayout->addWidget(pers);
   noiseLayout->addWidget(new QLabel("Amplitude (1.0-64.0)"));
   noiseLayout->addWidget(amp);
   noiseLayout->addWidget(button);
   noiseControls->setLayout(noiseLayout);

   // Add the sub-containers to the big controls container
   controlsLayout->addWidget(envControls);
   controlsLayout->addWidget(texControls);
   controlsLayout->addWidget(noiseControls);
   controls->setLayout(controlsLayout);

   // Master layout -- view and controls
   QHBoxLayout* layout = new QHBoxLayout;
   layout->addWidget(terrainView);
   layout->addWidget(controls);
   setLayout(layout);

   // Set a timer with a 0 timeout to fake an 'idle' function
   QTimer* timer = new QTimer();
   timer->start(0);
   connect(timer,SIGNAL(timeout()),terrainView,SLOT(idle()));

   //  Connect textChanged signal of lineEdit to plug into setTurbulence function
   connect(turb, SIGNAL(textChanged(QString)), terrainView , SLOT(setTurbulence(QString)));
   //  Connect textChanged signal of lineEdit to plug into setOctaves function
   connect(oct, SIGNAL(textChanged(QString)), terrainView , SLOT(setOctaves(QString)));
   //  Connect textChanged signal of lineEdit to plug into setPersistence function
   connect(pers, SIGNAL(textChanged(QString)), terrainView , SLOT(setPersistence(QString)));
   //  Connect textChanged signal of lineEdit to plug into setAmplitude function
   connect(amp, SIGNAL(textChanged(QString)), terrainView , SLOT(setAmplitude(QString)));

   // Environment sliders
   connect(timeSlider, SIGNAL(valueChanged(int)) , terrainView , SLOT(setTime(int)));
   connect(opacitySlider, SIGNAL(valueChanged(int)) , terrainView , SLOT(setCloudOpacity(int)));
   connect(densitySlider, SIGNAL(valueChanged(int)) , terrainView , SLOT(setCloudDensity(int)));
   connect(waterSlider, SIGNAL(valueChanged(int)) , terrainView , SLOT(setWaterHeight(int)));

   // Texturing sliders
   connect(blendSlider, SIGNAL(valueChanged(int)) , terrainView , SLOT(setBlend(int)));
   connect(sandLowSlider, SIGNAL(valueChanged(int)) , terrainView , SLOT(setSandLowerBound(int)));
   connect(sandHighSlider, SIGNAL(valueChanged(int)) , terrainView , SLOT(setSandUpperBound(int)));
   connect(grassLowSlider, SIGNAL(valueChanged(int)) , terrainView , SLOT(setGrassLowerBound(int)));
   connect(grassHighSlider, SIGNAL(valueChanged(int)) , terrainView , SLOT(setGrassUpperBound(int)));
   connect(rockLowSlider, SIGNAL(valueChanged(int)) , terrainView , SLOT(setRockLowerBound(int)));
   connect(rockHighSlider, SIGNAL(valueChanged(int)) , terrainView , SLOT(setRockUpperBound(int)));
   connect(snowLowSlider, SIGNAL(valueChanged(int)) , terrainView , SLOT(setSnowLowerBound(int)));
   connect(snowHighSlider, SIGNAL(valueChanged(int)) , terrainView , SLOT(setSnowUpperBound(int)));
   
   //  Connect clicked() signal of push button to regenerate terrain (and redraw noise image)
   connect(button, SIGNAL(clicked()) , terrainView , SLOT(reGenTerrain()));
   connect(button, SIGNAL(clicked()) , drawNoise   , SLOT(reDraw()));
   
}
