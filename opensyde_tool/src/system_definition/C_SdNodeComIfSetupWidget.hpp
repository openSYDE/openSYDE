//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node Com Interface Setup (header)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNODECOMIFSETUPWIDGET_H
#define C_SDNODECOMIFSETUPWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QRadioButton>

#include "stwtypes.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_OscNodeComInterfaceSettings.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNodeComIfSetupWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SdNodeComIfSetupWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNodeComIfSetupWidget(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                     const uint32_t & oru32_NodeIndex, const uint32_t & oru32_BusIndex,
                                     const int32_t & ors32_SpecialInterface);
   ~C_SdNodeComIfSetupWidget(void) override;

   void InitStaticNames(void) const;
   uint8_t GetSelectedInterface(void) const;
   bool GetInteractionPossible(void) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   //Avoid call
   C_SdNodeComIfSetupWidget(const C_SdNodeComIfSetupWidget &);
   C_SdNodeComIfSetupWidget & operator =(const C_SdNodeComIfSetupWidget &) &;

   Ui::C_SdNodeComIfSetupWidget * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   const uint32_t mu32_NodeIndex;
   const uint32_t mu32_BusIndex;
   const int32_t ms32_SpecialInterface;
   bool mq_InteractionPossible;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_InitFromData(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
