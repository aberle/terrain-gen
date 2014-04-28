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
   QLineEdit* turb = new QLineEdit("64");
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

   //  Create slider and set range to 1-10
   QSlider* timeSlider = new QSlider(Qt::Horizontal);
   timeSlider->setRange(1,10);
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

   //  Create slider and set range to 0-10
   QSlider* densitySlider = new QSlider(Qt::Horizontal);
   densitySlider->setRange(1,30);
   //  Eye candy - set tick interval for display
   densitySlider->setTickInterval(5);
   densitySlider->setTickPosition(QSlider::TicksBelow);
   densitySlider->setValue(10);

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
   envControls->setLayout(envLayout);

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
   
   //  Connect clicked() signal of push button to regenerate terrain (and redraw noise image)
   connect(button, SIGNAL(clicked()) , terrainView , SLOT(reGenTerrain()));
   connect(button, SIGNAL(clicked()) , drawNoise   , SLOT(reDraw()));
   
}
