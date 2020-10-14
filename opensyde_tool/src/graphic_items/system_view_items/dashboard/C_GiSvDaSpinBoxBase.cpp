//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for system view dashboard spin box item (implementation)

   Class for system view dashboard spin box item

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QGraphicsView>

#include <cmath>
#include <limits>
#include "gitypes.h"
#include "stwtypes.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_OSCUtils.h"
#include "C_PuiSvHandler.h"
#include "C_GiSvDaSpinBoxBase.h"
#include "C_PuiSvDbSpinBox.h"
#include "C_OgePopUpDialog.h"
#include "C_SyvDaPeBase.h"
#include "C_SyvDaPeSpinBox.h"
#include "C_SdNdeDpContentUtil.h"
#include "C_OSCNodeDataPoolListElement.h"
#include "C_PuiSdHandler.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]      oru32_ViewIndex        Index of system view
   \param[in]      oru32_DashboardIndex   Index of dashboard in system view
   \param[in]      ors32_DataIndex        Index of data element in dashboard in system view
   \param[in]      oru64_ID               Unique ID
   \param[in,out]  opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSvDaSpinBoxBase::C_GiSvDaSpinBoxBase(const uint32 & oru32_ViewIndex, const uint32 & oru32_DashboardIndex,
                                         const sint32 & ors32_DataIndex, const uint64 & oru64_ID,
                                         QGraphicsItem * const opc_Parent) :
   C_GiSvDaRectBaseGroup(oru32_ViewIndex, oru32_DashboardIndex, ors32_DataIndex, C_PuiSvDbDataElement::eSPIN_BOX, 1,
                         oru64_ID, 57.6, 20.0, 200.0, 60.0, false, false, opc_Parent),
   mq_Connected(false)
{
   QFont c_Font;

   this->mpc_SpinBoxWidget = new C_OgeWiDashboardSpinBoxGroup();
   //Default font
   c_Font = this->mpc_SpinBoxWidget->font();
   c_Font.setFamily("Segoe UI");
   this->mpc_SpinBoxWidget->setFont(c_Font);
   this->mpc_Widget->SetWidget(this->mpc_SpinBoxWidget);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSvDaSpinBoxBase::~C_GiSvDaSpinBoxBase(void)
{
   //lint -e{1540}  no memory leak because of the parent of mpc_SpinBoxWidget and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the type of this item

   \return  ID
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_GiSvDaSpinBoxBase::type(void) const
{
   return msn_GRAPHICS_ITEM_DB_SPIN_BOX;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adjust font to current size
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaSpinBoxBase::ReInitializeSize(void)
{
   if (this->mpc_SpinBoxWidget != NULL)
   {
      this->mpc_SpinBoxWidget->AdjustFontToSize();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data from system view dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaSpinBoxBase::LoadData(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbSpinBox * const pc_Box = pc_Dashboard->GetSpinBox(static_cast<uint32>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         this->LoadSvBasicData(*pc_Box);
         this->m_UpdateStaticValues();
         if (this->mpc_SpinBoxWidget != NULL)
         {
            this->mpc_SpinBoxWidget->SetDesignType(pc_Box->e_Type);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update data in system view dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaSpinBoxBase::UpdateData(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbSpinBox * const pc_Box = pc_Dashboard->GetSpinBox(static_cast<uint32>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         C_PuiSvDbSpinBox c_Box = *pc_Box;
         this->UpdateSvBasicData(c_Box);
         if (this->mpc_SpinBoxWidget != NULL)
         {
            const QVariant c_QValue = this->mpc_SpinBoxWidget->GetValue();
            tgl_assert(C_SdNdeDpContentUtil::h_SimpleConvertFromVariant(c_QValue, c_Box.c_Value) == C_NO_ERR);
         }
         tgl_assert(C_PuiSvHandler::h_GetInstance()->SetDashboardWidget(this->mu32_ViewIndex,
                                                                        this->mu32_DashboardIndex,
                                                                        static_cast<uint32>(this->ms32_Index),
                                                                        &c_Box, this->me_Type) == C_NO_ERR);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete data in system view dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaSpinBoxBase::DeleteData(void)
{
   if (this->ms32_Index >= 0)
   {
      tgl_assert(C_PuiSvHandler::h_GetInstance()->DeleteDashboardWidget(this->mu32_ViewIndex,
                                                                        this->mu32_DashboardIndex,
                                                                        static_cast<uint32>(this->ms32_Index),
                                                                        this->me_Type) ==
                 C_NO_ERR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Information about the start or stop of a connection

   \param[in]  oq_Active   Flag if connection is active or not active now
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaSpinBoxBase::ConnectionActiveChanged(const bool oq_Active)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->GetSvDashboard();

   if (oq_Active == true)
   {
      this->m_UpdateStaticValues();
   }

   C_GiSvDaRectBaseGroup::ConnectionActiveChanged(oq_Active);
   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbSpinBox * const pc_Box = pc_Dashboard->GetSpinBox(static_cast<uint32>(this->ms32_Index));
      if ((pc_Box != NULL) && (pc_Box->e_ElementWriteMode == C_PuiSvDbToggle::eWM_ON_CHANGE))
      {
         if (oq_Active == true)
         {
            if (this->mq_Connected == false)
            {
               this->mq_Connected = true;
               connect(this->mpc_SpinBoxWidget, &C_OgeWiDashboardSpinBoxGroup::SigValueChanged, this,
                       &C_GiSvDaSpinBoxBase::SendCurrentValue);
            }
         }
         else
         {
            this->mq_Connected = false;
            disconnect(this->mpc_SpinBoxWidget, &C_OgeWiDashboardSpinBoxGroup::SigValueChanged, this,
                       &C_GiSvDaSpinBoxBase::SendCurrentValue);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sends the current set value of the element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaSpinBoxBase::SendCurrentValue(void)
{
   if (this->mpc_SpinBoxWidget != NULL)
   {
      // Prepare the value
      QString c_Value = this->mpc_SpinBoxWidget->GetText();
      C_PuiSvDbDataElementScaling c_Scaling;
      bool q_Success = false;

      // Prepare the string
      tgl_assert(this->GetDataPoolElementScaling(0, c_Scaling) == C_NO_ERR);
      c_Value = c_Value.remove(c_Scaling.c_Unit);
      c_Value = c_Value.trimmed();

      this->mf64_WriteValue = c_Value.toDouble(&q_Success);

      if (q_Success == true)
      {
         // Send the value
         C_GiSvDaRectBaseGroup::SendCurrentValue();
      }
      else
      {
         //Attempt replace of ','
         QString c_Tmp = c_Value;
         c_Tmp.replace(',', '.');
         this->mf64_WriteValue = c_Tmp.toDouble(&q_Success);
         if (q_Success == true)
         {
            // Send the value
            C_GiSvDaRectBaseGroup::SendCurrentValue();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call properties for widgets

   \return true (configurable properties called)
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaSpinBoxBase::CallProperties(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbSpinBox * const pc_Box = pc_Dashboard->GetSpinBox(static_cast<uint32>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         C_PuiSvDbNodeDataPoolListElementId c_ElementId;
         C_PuiSvDbDataElementScaling c_Scaling;
         QGraphicsView * const pc_View = this->scene()->views().at(0);
         QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(pc_View, pc_View);
         C_SyvDaPeBase * pc_Dialog;
         C_SyvDaPeSpinBox * pc_PropertiesWidget;

         if (pc_Box->c_DataPoolElementsConfig.size() > 0)
         {
            c_ElementId = pc_Box->c_DataPoolElementsConfig[0].c_ElementId;
            c_Scaling = pc_Box->c_DataPoolElementsConfig[0].c_ElementScaling;
         }
         else
         {
            c_ElementId = C_PuiSvDbNodeDataPoolListElementId(0, 0, 0, 0,
                                                             C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT,
                                                             false, 0UL, false);
            c_Scaling = C_PuiSvDbDataElementScaling();
         }

         pc_Dialog = new C_SyvDaPeBase(*c_New, this->mu32_ViewIndex, this->mu32_DashboardIndex, "Spin Box",
                                       c_ElementId, c_Scaling, false, this->mq_DarkMode);
         pc_PropertiesWidget = new C_SyvDaPeSpinBox(*pc_Dialog, this->mq_DarkMode);

         pc_Dialog->SetWidget(pc_PropertiesWidget);
         pc_Dialog->SetTheme(pc_Box->e_DisplayStyle);

         //Resize
         c_New->SetSize(QSize(800, 800));

         pc_PropertiesWidget->SetType(pc_Box->e_Type);
         pc_PropertiesWidget->SetShowUnit(pc_Box->q_ShowUnit);
         pc_Dialog->SetWriteMode(pc_Box->e_ElementWriteMode);

         if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
         {
            C_PuiSvDbSpinBox c_Box = *pc_Box;
            C_PuiSvDbNodeDataElementConfig c_Tmp;
            //Save
            c_Box.e_DisplayStyle = pc_Dialog->GetTheme();
            c_Box.e_Type = pc_PropertiesWidget->GetType();
            c_Box.q_ShowUnit = pc_PropertiesWidget->GetShowUnit();

            c_Tmp.c_ElementId = pc_Dialog->GetDataElementId();
            c_Tmp.c_ElementScaling = pc_Dialog->GetScalingInformation();
            c_Box.c_DataPoolElementsConfig.clear();
            if (c_Tmp.c_ElementId.GetIsValid())
            {
               c_Box.c_DataPoolElementsConfig.push_back(c_Tmp);
            }
            c_Box.e_ElementWriteMode = pc_Dialog->GetWriteMode();

            //Apply
            this->me_WriteMode = pc_Dialog->GetWriteMode();
            this->SetDisplayStyle(c_Box.e_DisplayStyle, this->mq_DarkMode);
            if (this->mpc_SpinBoxWidget != NULL)
            {
               this->mpc_SpinBoxWidget->SetDesignType(c_Box.e_Type);
               this->mpc_SpinBoxWidget->SetShowUnit(c_Box.q_ShowUnit);
            }
            this->ClearDataPoolElements();
            if (c_Tmp.c_ElementId.GetIsValid())
            {
               this->RegisterDataPoolElement(pc_Dialog->GetDataElementId(), pc_Dialog->GetScalingInformation());
            }

            tgl_assert(C_PuiSvHandler::h_GetInstance()->CheckAndHandleNewElement(c_Tmp.c_ElementId) == C_NO_ERR);
            tgl_assert(C_PuiSvHandler::h_GetInstance()->SetDashboardWidget(this->mu32_ViewIndex,
                                                                           this->mu32_DashboardIndex,
                                                                           static_cast<uint32>(this->ms32_Index),
                                                                           &c_Box, this->me_Type) == C_NO_ERR);

            this->m_UpdateStaticValues();
         }
         Q_EMIT this->SigTriggerUpdateTransmissionConfiguration();
         if (c_New != NULL)
         {
            c_New->HideOverlay();
         }
         //lint -e{429}  no memory leak because of the parent of pc_Dialog and the Qt memory management
      }
   }
   return true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update type

   Warning: Only use for preview

   \param[in]  oe_Type        Type
   \param[in]  oq_ShowUnit    Show unit
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaSpinBoxBase::UpdateTypePe(const C_PuiSvDbSpinBox::E_Type oe_Type, const bool oq_ShowUnit)
{
   if (this->mpc_SpinBoxWidget != NULL)
   {
      this->mpc_SpinBoxWidget->SetDesignType(oe_Type);
      this->mpc_SpinBoxWidget->SetShowUnit(oq_ShowUnit);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set value

   Warning: Only use for preview

   \param[in]  os32_Value  New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaSpinBoxBase::SetValuePe(const sint32 os32_Value)
{
   if (this->mpc_SpinBoxWidget != NULL)
   {
      C_OSCNodeDataPoolContent c_Min;
      C_OSCNodeDataPoolContent c_Max;
      C_OSCNodeDataPoolContent c_Value;
      c_Min.SetType(C_OSCNodeDataPoolContent::eSINT32);
      c_Min.SetArray(false);
      c_Max.SetType(C_OSCNodeDataPoolContent::eSINT32);
      c_Max.SetArray(false);
      c_Value.SetType(C_OSCNodeDataPoolContent::eSINT32);
      c_Value.SetArray(false);
      //lint -e{530,10,1015,1013}  c++11 feature
      c_Min.SetValueS32(std::numeric_limits<sint32>::lowest());
      c_Max.SetValueS32(std::numeric_limits<sint32>::max());
      this->mpc_SpinBoxWidget->Init(c_Min, c_Max);
      c_Value.SetValueS32(os32_Value);
      this->mpc_SpinBoxWidget->SetValue(QVariant(static_cast<sint64>(c_Value.GetValueS32())));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set unit

   Warning: Only use for preview

   \param[in]  orc_Value   New unit
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaSpinBoxBase::SetUnitPe(const QString & orc_Value)
{
   if (this->mpc_SpinBoxWidget != NULL)
   {
      this->mpc_SpinBoxWidget->SetUnit(orc_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaSpinBoxBase::m_UpdateStaticValues(void)
{
   if (this->mpc_SpinBoxWidget != NULL)
   {
      const C_PuiSvDashboard * const pc_Dashboard = this->GetSvDashboard();
      C_PuiSvDbNodeDataPoolListElementId c_ID;

      if ((this->GetDataPoolElementIndex(0, c_ID) == C_NO_ERR) && (pc_Dashboard != NULL))
      {
         const C_PuiSvDbSpinBox * const pc_Box = pc_Dashboard->GetSpinBox(static_cast<uint32>(this->ms32_Index));
         tgl_assert(pc_Box != NULL);
         const C_OSCNodeDataPoolListElement * const pc_Element =
            C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(c_ID.u32_NodeIndex, c_ID.u32_DataPoolIndex,
                                                                       c_ID.u32_ListIndex, c_ID.u32_ElementIndex);
         if ((pc_Element != NULL) && (pc_Box != NULL))
         {
            QVariant c_QValue;
            C_PuiSvDbDataElementScaling c_Scaling;

            tgl_assert(this->GetDataPoolElementScaling(0, c_Scaling) == C_NO_ERR);
            this->mpc_SpinBoxWidget->Init(pc_Element->c_MinValue, pc_Element->c_MaxValue, c_Scaling.f64_Factor,
                                          c_Scaling.f64_Offset);
            this->mpc_SpinBoxWidget->SetShowUnit(pc_Box->q_ShowUnit);
            this->mpc_SpinBoxWidget->SetUnit(c_Scaling.c_Unit);
            tgl_assert(C_SdNdeDpContentUtil::h_SimpleConvertToVariant(pc_Box->c_Value, c_QValue) == C_NO_ERR);
            this->mpc_SpinBoxWidget->SetValue(c_QValue);
         }
      }
   }
}
