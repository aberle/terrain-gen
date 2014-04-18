//
//  Qt OpenGL Terrain Generation
//

#include <QApplication>
#include "application.h"

//
//  Main function
//
int main(int argc, char *argv[])
{
   //  Create the application
   QApplication app(argc,argv);
   //  Create and show application widget
   Application application;
   application.show();
   //  Main loop for application
   return app.exec();
}
