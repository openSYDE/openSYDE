//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for IP Address configuration (implementation)

   Widget for IP Address configuration

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEIPADDRESSCONFIGURATIONWIDGET_HPP
#define C_SDNDEIPADDRESSCONFIGURATIONWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QLineEdit>
#include "stwtypes.hpp"
#include "C_OgePopUpDialog.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeIpAddressConfigurationWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeIpAddressConfigurationWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeIpAddressConfigurationWidget(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                const uint32_t ou32_NodeIndex, const uint32_t ou32_ComIf);
   ~C_SdNdeIpAddressConfigurationWidget(void) override;

   void InitStaticNames(void) const;

   void SlotTabKey(const QLineEdit * const opc_LineEdit);

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SdNdeIpAddressConfigurationWidget * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   const uint32_t mu32_NodeIndex;
   const uint32_t mu32_ComIf;

   void m_LoadData(void) const;
   void m_OkClicked(void);
   void m_CancelClicked(void);

   //Avoid call
   C_SdNdeIpAddressConfigurationWidget(const C_SdNdeIpAddressConfigurationWidget &);
   C_SdNdeIpAddressConfigurationWidget & operator =(const C_SdNdeIpAddressConfigurationWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
