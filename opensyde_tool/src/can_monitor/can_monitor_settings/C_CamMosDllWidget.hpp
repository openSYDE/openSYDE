//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Generic part for handling CAN DLL configuration (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMOSDLLWIDGET_HPP
#define C_CAMMOSDLLWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeLeFilePathBase.hpp"
#include "C_OgeWiOnlyBackground.hpp"
#include "C_CamOgePubPathVariablesBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CamMosDllWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMosDllWidget :
   public stw::opensyde_gui_elements::C_OgeWiOnlyBackground
{
   Q_OBJECT

public:
   enum E_CanDllType
   {
      ePEAK = 0,
      eVECTOR = 1,
      eOTHER = 2
   };

   explicit C_CamMosDllWidget(QWidget * const opc_Parent = NULL);
   ~C_CamMosDllWidget(void) override;

   virtual void LoadUserSettings(void) const;
   void PrepareForExpanded(const bool oq_Expand) const;
   void OnCommunicationStarted(const bool oq_Online) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigCanDllConfigured(void);
   void SigHide(void);

protected:
   virtual void m_LoadConfig(void) const = 0;
   virtual QString m_GetCurrentDllPath(void) const = 0;
   virtual void m_SaveDllPath(const QString & orc_DllPath) const = 0;
   virtual void m_SaveDllType(const E_CanDllType oe_DllType) const = 0;
   virtual QString m_ResolvePlaceholderVariables(const QString & orc_Path) const = 0;
   virtual stw::opensyde_gui_elements::C_OgeLeFilePathBase * m_GetPlaceholderLineEdit(void) = 0;
   virtual stw::opensyde_gui_elements::C_CamOgePubPathVariablesBase * m_GetPathVariablesButton(void) = 0;

   void m_InitUi(void);
   void m_LoadSpecifiedConfig(const E_CanDllType oe_DllType, const QString & orc_DllPath) const;

private:
   //Avoid call
   C_CamMosDllWidget(const C_CamMosDllWidget &);
   C_CamMosDllWidget & operator =(const C_CamMosDllWidget &) &;

   Ui::C_CamMosDllWidget * mpc_Ui;
   stw::opensyde_gui_elements::C_OgeLeFilePathBase * mpc_LineEditCustomDllPath;
   stw::opensyde_gui_elements::C_CamOgePubPathVariablesBase * mpc_PushButtonVariables;

   void m_ConfigureDllClicked(void);
   void m_ConcreteDllClicked(void) const;
   void m_OtherDllClicked(void) const;
   void m_UpdateCanDllPath(void) const;
   void m_OnExpand(const bool oq_Expand) const;
   void m_OnBrowse(void);
   void m_OnDroppedDllPath(void);
   void m_SetCustomDllPath(const QString & orc_New);

   void m_InsertPathVar(const QString & orc_Variable);
   void m_OnCustomDllEdited(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
