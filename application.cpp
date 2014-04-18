//
//  Application Widget
//

#include <QtGui>
#include "application.h"
#include "terrainview.h"

//
//  Constructor for Application widget
//
Application::Application()
{
   //  Set window title
   setWindowTitle(tr("Nicholas Aberle: Simplex Noise Terrain Generation"));

   //  Create new terrainView widget
   TerrainView* terrainView = new TerrainView;

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

   // Create checkbox
   QCheckBox* check = new QCheckBox("Lights on");
   //check->toggle();

   // Create radio buttons
   QRadioButton* partyOn = new QRadioButton("Party on!");
   QRadioButton* partyOff = new QRadioButton("Party off :(");
   partyOff->toggle();

   // Create text entry fields
   QValidator *validator = new QIntValidator(0, 100);
   QLineEdit* num = new QLineEdit("15");
   num->setFixedWidth(50);
   num->setValidator(validator);
   QLineEdit* m2 = new QLineEdit("30");
   m2->setFixedWidth(50);

   // Create shader button
   QPushButton *button = new QPushButton("Toggle shaders!");

   //  Create slider and set range to 10-200
   QSlider* scaleslider = new QSlider(Qt::Horizontal);
   scaleslider->setRange(10,100);
   //  Eye candy - set tick interval for display
   scaleslider->setTickInterval(5);
   scaleslider->setTickPosition(QSlider::TicksBelow);


   //  Set layout of child widgets
   QVBoxLayout* layout = new QVBoxLayout;
   layout->addWidget(new QLabel("Theta angle"));
   layout->addWidget(hslider);
   layout->addWidget(new QLabel("\nPhi angle"));
   layout->addWidget(vslider);
   layout->addWidget(terrainView);
   layout->addWidget(scaleslider);
   layout->addWidget(label);
   //layout->addWidget(check);
   layout->addWidget(new QLabel("Enter a number!"));
   layout->addWidget(num);
   layout->addWidget(new QLabel("Now it is multiplied by 2!"));
   layout->addWidget(m2);
   layout->addWidget(partyOn);
   layout->addWidget(partyOff);
   layout->addWidget(button);
   setLayout(layout);

   // Set a timer with a 0 timeout to fake an 'idle' function
   QTimer* timer = new QTimer();
   timer->start(0);
   connect(timer,SIGNAL(timeout()),terrainView,SLOT(idle()));

   //  Connect valueChanged() signal of horizontal slider to setTheta slot of terrainView
   connect(hslider, SIGNAL(valueChanged(int)) , terrainView , SLOT(setTheta(int)));
   //  Connect valueChanged() signal of vertical slider to setTheta slot of terrainView
   connect(vslider, SIGNAL(valueChanged(int)) , terrainView , SLOT(setPhi(int)));
   //  Connect stateChanged() signal of checkbox to turn lights on/off
   connect(check, SIGNAL(stateChanged(int)) , terrainView , SLOT(toggleLights()));
   //  Connect message() signal of terrainView to setText slot of label
   connect(terrainView, SIGNAL(message(QString)) , label , SLOT(setText(QString)));
   //  Connect isChecked() signal of radio button to turn party mode on
   connect(partyOn, SIGNAL(toggled(bool)) , terrainView , SLOT(partyMode()));
   //  Connect textChanged signal of lineEdit to plug into multiply function
   connect(num, SIGNAL(textChanged(QString)), terrainView , SLOT(multiply(QString)));
   //  Connect message() signal of terrainView to setText of lineEdit to display result of multiply
   connect(terrainView, SIGNAL(multMessage(QString)) , m2 , SLOT(setText(QString)));
   //  Connect clicked() signal of push button to toggle shaders
   connect(button, SIGNAL(clicked()) , terrainView , SLOT(toggleShaders()));
   //  Connect valueChanged() signal of scale slider to setScale slot of terrainView
   connect(scaleslider, SIGNAL(valueChanged(int)) , terrainView , SLOT(setScale(int)));
}
