//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CAN bus (implementation)

   CAN bus

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QGraphicsView>

#include "C_GiLiCANBus.h"
#include "C_OgePopUpDialog.h"
#include "C_GiSyBaseWidget.h"
#include "C_GiSyLineWidget.h"
#include "gitypes.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_elements;
using namespace stw_types;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString mc_NAME_BUS = "BUS CAN";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     ors32_Index          Index of data element in system definition
   \param[in]     oru64_ID             Unique ID
   \param[in]     opc_TextElementName  Pointer to text element for showing bus name
   \param[in]     oq_DoErrorCheck      Optional flag to trigger error check directly in constructor
   \param[in]     opc_Points           Points for line
   \param[in,out] opc_Parent           Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiCANBus::C_GiLiCANBus(const stw_types::sint32 & ors32_Index, const uint64 & oru64_ID,
                           C_GiTextElementBus * const opc_TextElementName, const bool oq_DoErrorCheck,
                           const std::vector<QPointF> * const opc_Points, QGraphicsItem * const opc_Parent) :
   C_GiLiBus(ors32_Index, oru64_ID, opc_TextElementName, oq_DoErrorCheck, opc_Points, false, opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiCANBus::~C_GiLiCANBus()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the type of this itme

   \return  ID
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_GiLiCANBus::type() const
{
   return msn_GRAPHICS_ITEM_CANBUS;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Open style dialog
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiLiCANBus::OpenStyleDialog(void)
{
   bool q_Retval;

   QGraphicsView * const pc_View = this->scene()->views().at(0);

   QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(pc_View, pc_View);
   C_GiSyBaseWidget * pc_Dialog = new C_GiSyBaseWidget(*c_New, mc_NAME_BUS, false);
   C_GiSyLineWidget * pc_SettingsWidget = new C_GiSyLineWidget(C_GiSyLineWidget::E_Type::eCAN_BUS, *pc_Dialog);

   pc_SettingsWidget->SetLineColor(this->GetColor());
   pc_SettingsWidget->SetLineWidth(this->GetWidth());

   if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
   {
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
   //lint -e{429}  no memory leak because of the parent of pc_Dialog and pc_SettingsWidget and the Qt memory management
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Return bus type
*/
//----------------------------------------------------------------------------------------------------------------------
stw_opensyde_core::C_OSCSystemBus::E_Type C_GiLiCANBus::GetType() const
{
   return stw_opensyde_core::C_OSCSystemBus::E_Type::eCAN;
}
