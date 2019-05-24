//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for handling CAN DLL configuration
 (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMOSDLLWIDGET_H
#define C_CAMMOSDLLWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeWiOnlyBackground.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CamMosDllWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMosDllWidget :
   public stw_opensyde_gui_elements::C_OgeWiOnlyBackground
{
   Q_OBJECT

public:
   explicit C_CamMosDllWidget(QWidget * const opc_Parent = NULL);
   ~C_CamMosDllWidget(void);

   void LoadUserSettings(void) const;
   void PrepareForExpanded(const bool oq_Expand) const;
   void OnCommunicationStarted(const bool oq_Online) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigCANDllConfigured(void);

private:
   //Avoid call
   C_CamMosDllWidget(const C_CamMosDllWidget &);
   C_CamMosDllWidget & operator =(const C_CamMosDllWidget &);

   Ui::C_CamMosDllWidget * mpc_Ui;

   void m_InitUi(void);
   void m_LoadConfig(void) const;

   void m_ConfigureDllClicked(void);
   void m_ConcreteDllClicked(void) const;
   void m_OtherDllClicked(void) const;
   void m_ShowCustomDllPath(const bool oq_Active) const;
   void m_UpdateCANDllPath(void) const;
   void m_OnExpand(const bool oq_Expand) const;
   void m_OnBrowse(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
