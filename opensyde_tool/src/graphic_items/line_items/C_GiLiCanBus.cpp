//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CAN bus (implementation)

   CAN bus

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QGraphicsView>

#include "C_GiLiCanBus.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_GiSyBaseWidget.hpp"
#include "C_GiSyLineWidget.hpp"
#include "gitypes.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in]     ors32_Index         Index of data element in system definition
   \param[in]     oru64_Id            Unique ID
   \param[in]     opc_TextElementName Pointer to text element for showing bus name
   \param[in]     oq_DoErrorCheck     Optional flag to trigger error check directly in constructor
   \param[in]     opc_Points          Points for line
   \param[in,out] opc_Parent          Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiCanBus::C_GiLiCanBus(const int32_t & ors32_Index, const uint64_t & oru64_Id,
                           C_GiTextElementBus * const opc_TextElementName, const bool oq_DoErrorCheck,
                           const std::vector<QPointF> * const opc_Points, QGraphicsItem * const opc_Parent) :
   C_GiLiBus(ors32_Index, oru64_Id, opc_TextElementName, oq_DoErrorCheck, opc_Points, false, opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiLiCanBus::~C_GiLiCanBus()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the type of this itme

   \return  ID
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_GiLiCanBus::type() const
{
   return ms32_GRAPHICS_ITEM_CANBUS;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Open style dialog

   \retval true   style dialog was accepted
   \retval false  style dialog was rejected
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiLiCanBus::OpenStyleDialog(void)
{
   bool q_Retval;

   QGraphicsView * const pc_View = this->scene()->views().at(0);

   const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(pc_View, pc_View);
   C_GiSyBaseWidget * const pc_Dialog = new C_GiSyBaseWidget(*c_New, "CAN Bus", false);
   C_GiSyLineWidget * const pc_SettingsWidget = new C_GiSyLineWidget(C_GiSyLineWidget::E_Type::eCAN_BUS, *pc_Dialog);

   pc_SettingsWidget->SetLineColor(this->GetColor());
   pc_SettingsWidget->SetLineWidth(this->GetWidth());

   if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
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
      c_New->deleteLater();
   }
   return q_Retval;
} //lint !e429  //no memory leak because of the parent of pc_Dialog and pc_SettingsWidget and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Return bus type

   \return
   Bus type (CAN or Ethernet)
*/
//----------------------------------------------------------------------------------------------------------------------
stw::opensyde_core::C_OscSystemBus::E_Type C_GiLiCanBus::GetType() const
{
   return stw::opensyde_core::C_OscSystemBus::E_Type::eCAN;
}
