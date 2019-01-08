//-----------------------------------------------------------------------------
/*!
   \file
   \brief       A concrete node selector widget for C_SdBueNodeSelectorCheckBoxListWidget (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     07.05.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDBUENODESELECTORCHECKBOXITEMWIDGET_H
#define C_SDBUENODESELECTORCHECKBOXITEMWIDGET_H

/* -- Includes ------------------------------------------------------------- */

#include <QIcon>

#include "C_OgeWiWithToolTip.h"
#include "C_OgeChxDefaultSmall.h"
#include "C_OSCCanProtocol.h"

#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */

namespace Ui
{
class C_SdBueNodeSelectorCheckBoxItemWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdBueNodeSelectorCheckBoxItemWidget :
   public stw_opensyde_gui_elements::C_OgeWiWithToolTip
{
   Q_OBJECT

public:
   explicit C_SdBueNodeSelectorCheckBoxItemWidget(const stw_types::uint32 ou32_NodeIndex,
                                                  const stw_types::uint32 ou32_InterfaceIndex, const QString & orc_Name,
                                                  QWidget * const opc_Parent = NULL);
   ~C_SdBueNodeSelectorCheckBoxItemWidget();

   void InitStaticNames(void) const;
   void GetIndexes(stw_types::uint32 & oru32_Index, stw_types::uint32 & oru32_SubIndex) const;
   void SetChecked(const bool oq_Checked);
   bool IsChecked(void) const;
   void SetProtocol(const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_Protocol);
   void UpdateToolTip(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigNodeToggled(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                       const bool oq_Checked);
   void SigComImport(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex);

private:
   //Avoid call
   C_SdBueNodeSelectorCheckBoxItemWidget(const C_SdBueNodeSelectorCheckBoxItemWidget &);
   C_SdBueNodeSelectorCheckBoxItemWidget & operator =(const C_SdBueNodeSelectorCheckBoxItemWidget &);

   void m_NodeToggled(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                      const bool oq_Checked);
   void m_ImportClicked(void);
   void m_AdaptIcon(const bool oq_Checked);

   Ui::C_SdBueNodeSelectorCheckBoxItemWidget * mpc_Ui;

   stw_opensyde_gui_elements::C_OgeChxDefaultSmall * mpc_CheckBox;

   QIcon mc_IconActive;
   QIcon mc_IconInactive;
   stw_opensyde_core::C_OSCCanProtocol::E_Type me_ProtocolType;
};
}
#endif
