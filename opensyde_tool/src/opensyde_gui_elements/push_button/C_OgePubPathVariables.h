//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Button with menu for selecting path variables
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBPATHVARIABLES_H
#define C_OGEPUBPATHVARIABLES_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgePubToolTipBase.h"
#include "C_OgeMuSections.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubPathVariables :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   explicit C_OgePubPathVariables(QWidget * const opc_Parent = NULL);
   ~C_OgePubPathVariables(void);

   void AddDatablockSection(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigVariableSelected(const QString & orc_Name);

private:
   stw_opensyde_gui_elements::C_OgeMuSections * mpc_Menu;

   void m_OpenSYDEExeTriggered(void);
   void m_OpenSYDEProjTriggered(void);
   void m_DataBlockProjTriggered(void);
   void m_UserNameTriggered(void);
   void m_ComputerNameTriggered(void);

   //Avoid call
   C_OgePubPathVariables(const C_OgePubPathVariables &);
   C_OgePubPathVariables & operator =(const C_OgePubPathVariables &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
