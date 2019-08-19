//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Button with menu for selecting path variables
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMOGEPUBPATHVARIABLES_H
#define C_CAMOGEPUBPATHVARIABLES_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgePubToolTipBase.h"
#include "C_OgeMuSections.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamOgePubPathVariables :
      public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   explicit C_CamOgePubPathVariables(QWidget * const opc_Parent = NULL);
   ~C_CamOgePubPathVariables(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigVariableSelected(const QString & orc_Name);

private:
    stw_opensyde_gui_elements::C_OgeMuSections * mpc_Menu;

   void m_CanMonitorExeTriggered(void);
   void m_CanMonitorProjTriggered(void);
   void m_UserNameTriggered(void);
   void m_ComputerNameTriggered(void);

   //Avoid call
   C_CamOgePubPathVariables(const C_CamOgePubPathVariables &);
   C_CamOgePubPathVariables & operator =(const C_CamOgePubPathVariables &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
