//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Generic part of add variables button
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMOGEPUBPATHVARIABLESBASE_HPP
#define C_CAMOGEPUBPATHVARIABLESBASE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeMuSections.hpp"
#include "C_OgePubToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamOgePubPathVariablesBase :
   public C_OgePubToolTipBase
{
   Q_OBJECT

public:
   C_CamOgePubPathVariablesBase(QWidget * const opc_Parent = NULL);
   ~C_CamOgePubPathVariablesBase(void) override;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigVariableSelected(const QString & orc_Name);

protected:
   void m_AddEntry(const QString & orc_MenuText, const QString & orc_LineEditText);
   void m_AddHeading(const QString & orc_Text);
   void m_SetMenuMinWidth(const int32_t os32_Width);

private:
   stw::opensyde_gui_elements::C_OgeMuSections * mpc_Menu;
   QMap<const QAction *, QString> mc_Actions;

   //Avoid call
   C_CamOgePubPathVariablesBase(const C_CamOgePubPathVariablesBase &);
   C_CamOgePubPathVariablesBase & operator =(const C_CamOgePubPathVariablesBase &) &;

   void m_ActionTriggered(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
