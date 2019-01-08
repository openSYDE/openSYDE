//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Node Com Interface Setup (header)

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     07.06.2017  STW/S.Singer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNODECOMIFSETUPWIDGET_H
#define C_SDNODECOMIFSETUPWIDGET_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>
#include <QRadioButton>

#include "stwtypes.h"
#include "C_OgePopUpDialog.h"
#include "C_OSCNodeComInterfaceSettings.h"

/* -- Namespace ------------------------------------------------------------ */
namespace Ui
{
class C_SdNodeComIfSetupWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class C_SdNodeComIfSetupWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNodeComIfSetupWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                     const stw_types::uint32 & oru32_NodeIndex,
                                     const stw_types::uint32 & oru32_BusIndex,
                                     const stw_types::sint32 & ors32_SpecialInterface);
   virtual ~C_SdNodeComIfSetupWidget(void);

   void InitStaticNames(void) const;
   stw_types::uint32 GetSelectedInterface(void) const;
   stw_types::uint8 GetNodeId(void) const;
   bool GetInteractionPossible(void) const;
   void GetComDataPoolConfiguration(bool & orq_ComProtocolL2, bool & orq_ComProtocolECeS,
                                    bool & orq_ComProtocolECoS) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   //Avoid call
   C_SdNodeComIfSetupWidget(const C_SdNodeComIfSetupWidget &);
   C_SdNodeComIfSetupWidget & operator =(const C_SdNodeComIfSetupWidget &);

   Ui::C_SdNodeComIfSetupWidget * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   const stw_types::uint32 mu32_NodeIndex;
   const stw_types::uint32 mu32_BusIndex;
   const stw_types::sint32 ms32_SpecialInterface;
   std::vector<stw_types::uint32> mc_UsedNodeIds;
   bool mq_InteractionPossible;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_InitFromData(void);
   void m_OnNodeIdChange(const stw_types::sint32 & ors32_Value);
   bool m_CheckDatapoolNumber(void) const;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
