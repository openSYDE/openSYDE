//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Ethernet bus (implementation)

   Ethernet bus

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.08.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QGraphicsView>

#include "C_GiLiEthernetBus.h"
#include "C_OgePopUpDialog.h"
#include "C_GiSyBaseWidget.h"
#include "C_GiSyLineWidget.h"
#include "gitypes.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
using namespace stw_types;

/* -- Module Global Constants ---------------------------------------------- */
const QString mc_NAME_ETHERNET = "BUS ETHERNET";

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     ors32_Index          Index of data element in system definition
   \param[in]     oru64_ID             Unique ID
   \param[in]     opc_TextElementName  Pointer to text element for showing bus name
   \param[in]     opc_Points           Points for line
   \param[in,out] opc_Parent           Optional pointer to parent

   \created     17.08.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_GiLiEthernetBus::C_GiLiEthernetBus(const stw_types::sint32 & ors32_Index, const uint64 & oru64_ID,
                                     C_GiTextElementBus * const opc_TextElementName,
                                     const std::vector<QPointF> * const opc_Points, QGraphicsItem * const opc_Parent) :
   C_GiLiBus(ors32_Index, oru64_ID, opc_TextElementName, opc_Points, true, opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     18.08.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_GiLiEthernetBus::~C_GiLiEthernetBus()
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the type of this itme

   \return  ID

   \created     01.09.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
sintn C_GiLiEthernetBus::type() const
{
   return msn_GRAPHICS_ITEM_ETHERNETBUS;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Open style dialog

   \created     23.08.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_GiLiEthernetBus::OpenStyleDialog(void)
{
   bool q_Retval;
   QGraphicsView * const pc_View = this->scene()->views().at(0);

   QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(pc_View, pc_View);
   C_GiSyBaseWidget * pc_Dialog = new C_GiSyBaseWidget(*c_New, mc_NAME_ETHERNET, false);
   C_GiSyLineWidget * pc_SettingsWidget = new C_GiSyLineWidget(C_GiSyLineWidget::E_Type::eETHERNET_BUS, *pc_Dialog);

   pc_SettingsWidget->SetLineColor(this->GetColor());
   pc_SettingsWidget->SetLineWidth(this->GetWidth());
   pc_SettingsWidget->SetInnerLineColor(this->GetMiddleLineColor());

   if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
   {
      this->SetMiddleLineColor(pc_SettingsWidget->GetInnerLineColor());
      this->ApplyStyle(pc_SettingsWidget->GetLineColor(), pc_SettingsWidget->GetLineWidth());
      q_Retval = true;
   }
   else
   {
      q_Retval = false;
   }
   if (c_New != NULL)
   {
      c_New->HideOverlay();
   }
   //lint -e{429}  no memory leak because of the parent of pc_Dialog and the Qt memory management
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Return bus type

   \created     20.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
stw_opensyde_core::C_OSCSystemBus::E_Type C_GiLiEthernetBus::GetType() const
{
   return stw_opensyde_core::C_OSCSystemBus::E_Type::eETHERNET;
}
