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
   drawNoise->setFixedSize(NOISE_HEIGHT,NOISE_WIDTH);
   drawNoise->setWindowTitle("Noise Texture");
   drawNoise->move(1100,0);
   drawNoise->show();

   //  Create slider and set range to 0-360 (degrees)
   QSlider* hslider = new QSlider(Qt::Horizontal);
   hslider->setRange(0,360);
   //  Eye candy - set tick interval for display
   hslider->setTickInterval(30);
   hslider->setTickPosition(QSlider::TicksBelow);

   //  Create slider and set range to 0-360 (degrees)
   QSlider* vslider = new QSlider(Qt::Horizontal);
   vslider->setRange(0,360);
   //  Eye candy - set tick interval for display
   vslider->setTickInterval(30);
   vslider->setTickPosition(QSlider::TicksBelow);

   //  Create label to show angle
   QLabel* label = new QLabel;

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

   //  Create slider and set range to 10-200
   QSlider* scaleslider = new QSlider(Qt::Horizontal);
   scaleslider->setRange(1,200);
   //  Eye candy - set tick interval for display
   scaleslider->setTickInterval(5);
   scaleslider->setTickPosition(QSlider::TicksBelow);

   //  Set layout of child widgets:
   // Big controls container
   QGroupBox* controls = new QGroupBox;
   QVBoxLayout* controlsLayout = new QVBoxLayout;

   // Container for view controls
   QGroupBox* viewControls = new QGroupBox("View Controls");
   QVBoxLayout* viewLayout = new QVBoxLayout;
   /*viewLayout->addWidget(new QLabel("Theta angle"));
   viewLayout->addWidget(hslider);
   viewLayout->addWidget(new QLabel("Phi angle"));
   viewLayout->addWidget(vslider);*/
   viewLayout->addWidget(new QLabel("Scale"));
   viewLayout->addWidget(scaleslider);
   viewLayout->addWidget(label);
   viewControls->setLayout(viewLayout);

   // Container for noise controls
   QGroupBox* noiseControls = new QGroupBox("Noise Controls");
   QVBoxLayout* noiseLayout = new QVBoxLayout;
   noiseLayout->addWidget(new QLabel("Turbulence Passes (2-256)"));
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
   controlsLayout->addWidget(viewControls);
   controlsLayout->addWidget(noiseControls);
   controls->setLayout(controlsLayout);

   // Master layout
   QHBoxLayout* layout = new QHBoxLayout;
   layout->addWidget(terrainView);
   layout->addWidget(controls);
   setLayout(layout);

   // Set a timer with a 0 timeout to fake an 'idle' function
   QTimer* timer = new QTimer();
   timer->start(0);
   connect(timer,SIGNAL(timeout()),terrainView,SLOT(idle()));

   //  Connect valueChanged() signal of horizontal slider to setTheta slot of terrainView
   connect(hslider, SIGNAL(valueChanged(int)) , terrainView , SLOT(setTheta(int)));
   //  Connect valueChanged() signal of vertical slider to setTheta slot of terrainView
   connect(vslider, SIGNAL(valueChanged(int)) , terrainView , SLOT(setPhi(int)));
   //  Connect valueChanged() signal of scale slider to setScale slot of terrainView
   connect(scaleslider, SIGNAL(valueChanged(int)) , terrainView , SLOT(setScale(int)));
   //  Connect message() signal of terrainView to setText slot of label
   connect(terrainView, SIGNAL(message(QString)) , label , SLOT(setText(QString)));

   //  Connect textChanged signal of lineEdit to plug into setTurbulence function
   connect(turb, SIGNAL(textChanged(QString)), terrainView , SLOT(setTurbulence(QString)));
   //  Connect textChanged signal of lineEdit to plug into setOctaves function
   connect(oct, SIGNAL(textChanged(QString)), terrainView , SLOT(setOctaves(QString)));
   //  Connect textChanged signal of lineEdit to plug into setPersistence function
   connect(pers, SIGNAL(textChanged(QString)), terrainView , SLOT(setPersistence(QString)));
   //  Connect textChanged signal of lineEdit to plug into setAmplitude function
   connect(amp, SIGNAL(textChanged(QString)), terrainView , SLOT(setAmplitude(QString)));
   
   //  Connect clicked() signal of push button to regenerate terrain (and redraw noise image)
   connect(button, SIGNAL(clicked()) , terrainView , SLOT(reGenTerrain()));
   connect(button, SIGNAL(clicked()) , drawNoise   , SLOT(reDraw()));
   
}
