//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for system view dashboard spin box item (implementation)

   Class for system view dashboard spin box item

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QGraphicsView>

#include <cmath>
#include <limits>
#include "gitypes.hpp"
#include "stwtypes.hpp"
#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "C_OscUtils.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_GiSvDaSpinBoxBase.hpp"
#include "C_PuiSvDbSpinBox.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_SyvDaPeBase.hpp"
#include "C_SyvDaPeSpinBox.hpp"
#include "C_SdNdeDpContentUtil.hpp"
#include "C_OscNodeDataPoolListElement.hpp"
#include "C_PuiSdHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

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
   \param[in]      oru64_Id               Unique ID
   \param[in,out]  opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSvDaSpinBoxBase::C_GiSvDaSpinBoxBase(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                                         const int32_t & ors32_DataIndex, const uint64_t & oru64_Id,
                                         QGraphicsItem * const opc_Parent) :
   C_GiSvDaRectBaseGroup(oru32_ViewIndex, oru32_DashboardIndex, ors32_DataIndex, C_PuiSvDbDataElement::eSPIN_BOX, 1,
                         oru64_Id, 57.6, 20.0, 200.0, 60.0, false, false, opc_Parent),
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
//lint -e{1540}  no memory leak because of the parent of mpc_SpinBoxWidget and the Qt memory management
C_GiSvDaSpinBoxBase::~C_GiSvDaSpinBoxBase(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the type of this item

   \return  ID
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_GiSvDaSpinBoxBase::type(void) const
{
   return ms32_GRAPHICS_ITEM_DB_SPIN_BOX;
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
   const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbSpinBox * const pc_Box = pc_Dashboard->GetSpinBox(static_cast<uint32_t>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         //Load twice to fix initial size font issue
         C_PuiSvDbSpinBox c_Copy = *pc_Box;
         c_Copy.f64_Width += 1.0;
         this->LoadSvBasicData(c_Copy);
         c_Copy.f64_Width = pc_Box->f64_Width;
         this->LoadSvBasicData(c_Copy);
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
   const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbSpinBox * const pc_Box = pc_Dashboard->GetSpinBox(static_cast<uint32_t>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         C_PuiSvDbSpinBox c_Box = *pc_Box;
         this->UpdateSvBasicData(c_Box);
         if (this->mpc_SpinBoxWidget != NULL)
         {
            const QVariant c_VariantValue = this->mpc_SpinBoxWidget->GetValue();
            tgl_assert(C_SdNdeDpContentUtil::h_SimpleConvertFromVariant(c_VariantValue, c_Box.c_Value) == C_NO_ERR);
         }
         tgl_assert(C_PuiSvHandler::h_GetInstance()->SetDashboardWidget(this->mu32_ViewIndex,
                                                                        this->mu32_DashboardIndex,
                                                                        static_cast<uint32_t>(this->ms32_Index),
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
                                                                        static_cast<uint32_t>(this->ms32_Index),
                                                                        this->me_Type) ==
                 C_NO_ERR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the shown value of the element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaSpinBoxBase::UpdateShowValue(void)
{
   if (this->mpc_SpinBoxWidget != NULL)
   {
      // Poll only when something is expected
      if (this->mq_ManualReadStarted == true)
      {
         QString c_Value;
         float64_t f64_ScaledValue;
         if (this->m_GetLastValue(0UL, c_Value, NULL, &f64_ScaledValue) == C_NO_ERR)
         {
            this->mpc_SpinBoxWidget->SetValue(f64_ScaledValue);
            this->mq_ManualReadStarted = false;
         }
      }
   }

   // For write widget calling base class is not necessary
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Information about the start or stop of a connection

   \param[in]  oq_Active   Flag if connection is active or not active now
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaSpinBoxBase::ConnectionActiveChanged(const bool oq_Active)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbSpinBox * const pc_Box = pc_Dashboard->GetSpinBox(static_cast<uint32_t>(this->ms32_Index));
      if (pc_Box != NULL)
      {
         if ((oq_Active == true) &&
             (this->GetWidgetDataPoolElementCount() > 0U) &&
             (pc_Box->e_InitialValueMode == C_PuiSvDbWriteWidgetBase::eIVM_SET_CONSTANT_VALUE))
         {
            // Special case: Defined constant value as start value is set
            // Update before calling base class implementation
            // Scaling is here necessary
            C_PuiSvDbDataElementScaling c_Scaling;
            QVariant c_VariantValue;
            tgl_assert(this->GetDataPoolElementScaling(0, c_Scaling) == C_NO_ERR);
            c_VariantValue = C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(pc_Box->c_InitialValue,
                                                                                   c_Scaling.f64_Factor,
                                                                                   c_Scaling.f64_Offset,
                                                                                   0UL);
            this->mpc_SpinBoxWidget->SetValue(c_VariantValue);
         }

         C_GiSvDaRectBaseGroup::ConnectionActiveChanged(oq_Active);

         if (pc_Box->e_ElementWriteMode == C_PuiSvDbToggle::eWM_ON_CHANGE)
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
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Information about the start or stop of edit mode

   \param[in]  oq_Active   Flag if edit mode is active or not active now
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaSpinBoxBase::EditModeActiveChanged(const bool oq_Active)
{
   C_GiSvDaRectBaseGroup::EditModeActiveChanged(oq_Active);

   if (this->mpc_SpinBoxWidget != NULL)
   {
      this->mpc_SpinBoxWidget->setEnabled(!oq_Active);
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
      const QVariant c_Value = this->mpc_SpinBoxWidget->GetValue();
      bool q_Success = false;

      this->mf64_WriteValue = c_Value.toDouble(&q_Success);

      tgl_assert(q_Success);
      if (q_Success == true)
      {
         // Send the value
         C_GiSvDaRectBaseGroup::SendCurrentValue();
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
   const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbSpinBox * const pc_Box = pc_Dashboard->GetSpinBox(static_cast<uint32_t>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         C_PuiSvDbNodeDataPoolListElementId c_ElementId;
         C_PuiSvDbDataElementScaling c_Scaling;
         C_PuiSvDbDataElementDisplayFormatter c_FormatterConfig;
         QGraphicsView * const pc_View = this->scene()->views().at(0);
         const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(pc_View, pc_View);
         C_SyvDaPeBase * pc_Dialog;
         C_SyvDaPeSpinBox * pc_PropertiesWidget;

         if (pc_Box->c_DataPoolElementsConfig.size() > 0)
         {
            c_ElementId = pc_Box->c_DataPoolElementsConfig[0].c_ElementId;
            c_Scaling = pc_Box->c_DataPoolElementsConfig[0].c_ElementScaling;
            c_FormatterConfig = pc_Box->c_DataPoolElementsConfig[0].c_DisplayFormatter;
         }
         else
         {
            c_ElementId = C_PuiSvDbNodeDataPoolListElementId(0, 0, 0, 0,
                                                             C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT,
                                                             false, 0UL, false);
            c_Scaling = C_PuiSvDbDataElementScaling();
         }

         pc_Dialog = new C_SyvDaPeBase(*c_New, this->mu32_ViewIndex, this->mu32_DashboardIndex, "Spin Box",
                                       c_ElementId, c_Scaling, false, c_FormatterConfig, false, this->mq_DarkMode);
         pc_PropertiesWidget = new C_SyvDaPeSpinBox(*pc_Dialog, this->mq_DarkMode);

         pc_Dialog->SetWidget(pc_PropertiesWidget);
         pc_Dialog->SetTheme(pc_Box->e_DisplayStyle);

         //Resize
         c_New->SetSize(C_SyvDaPeBase::h_GetSpinBoxPopupSizeWithDashboardConnect());

         pc_PropertiesWidget->SetType(pc_Box->e_Type);
         pc_PropertiesWidget->SetShowUnit(pc_Box->q_ShowUnit);
         pc_Dialog->SetWriteMode(pc_Box->e_ElementWriteMode);
         pc_Dialog->SetAutoWriteOnConnect(pc_Box->q_AutoWriteOnConnect);
         pc_Dialog->SetDashboardConnectInitialValue(pc_Box->e_InitialValueMode, pc_Box->c_InitialValue, false);

         if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
         {
            C_PuiSvDbSpinBox c_Box = *pc_Box;
            C_PuiSvDbNodeDataElementConfig c_Tmp;
            //Save
            c_Box.e_DisplayStyle = pc_Dialog->GetTheme();
            c_Box.e_Type = pc_PropertiesWidget->GetType();
            c_Box.q_ShowUnit = pc_PropertiesWidget->GetShowUnit();

            c_Tmp.c_ElementId = pc_Dialog->GetDataElementId();
            c_Tmp.c_ElementScaling = pc_Dialog->GetScalingInformation();
            c_Tmp.c_DisplayFormatter = pc_Dialog->GetFormatterInformation();
            c_Box.c_DataPoolElementsConfig.clear();
            if (c_Tmp.c_ElementId.GetIsValid())
            {
               c_Box.c_DataPoolElementsConfig.push_back(c_Tmp);
            }
            c_Box.e_ElementWriteMode = pc_Dialog->GetWriteMode();
            c_Box.q_AutoWriteOnConnect = pc_Dialog->GetAutoWriteOnConnect();
            c_Box.e_InitialValueMode = pc_Dialog->GetDashboardConnectInitialValueMode();
            c_Box.c_InitialValue = pc_Dialog->GetDashboardConnectInitialValue();

            //Apply
            this->me_WriteMode = pc_Dialog->GetWriteMode();
            this->me_WriteInitialValueMode = c_Box.e_InitialValueMode;
            this->mq_AutoWriteOnConnect = c_Box.q_AutoWriteOnConnect;
            this->SetDisplayStyle(c_Box.e_DisplayStyle, this->mq_DarkMode);
            if (this->mpc_SpinBoxWidget != NULL)
            {
               this->mpc_SpinBoxWidget->SetDesignType(c_Box.e_Type);
               this->mpc_SpinBoxWidget->SetShowUnit(c_Box.q_ShowUnit);
            }
            this->ClearDataPoolElements();
            if (c_Tmp.c_ElementId.GetIsValid())
            {
               this->RegisterDataPoolElement(pc_Dialog->GetDataElementId(), pc_Dialog->GetScalingInformation(),
                                             pc_Dialog->GetFormatterInformation());
            }

            tgl_assert(C_PuiSvHandler::h_GetInstance()->CheckAndHandleNewElement(c_Tmp.c_ElementId) == C_NO_ERR);
            tgl_assert(C_PuiSvHandler::h_GetInstance()->SetDashboardWidget(this->mu32_ViewIndex,
                                                                           this->mu32_DashboardIndex,
                                                                           static_cast<uint32_t>(this->ms32_Index),
                                                                           &c_Box, this->me_Type) == C_NO_ERR);

            this->m_UpdateStaticValues();
         }
         Q_EMIT this->SigTriggerUpdateTransmissionConfiguration();
         if (c_New != NULL)
         {
            c_New->HideOverlay();
            c_New->deleteLater();
         }
      } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
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
void C_GiSvDaSpinBoxBase::SetValuePe(const int32_t os32_Value)
{
   if (this->mpc_SpinBoxWidget != NULL)
   {
      C_OscNodeDataPoolContent c_Min;
      C_OscNodeDataPoolContent c_Max;
      C_OscNodeDataPoolContent c_Value;
      c_Min.SetType(C_OscNodeDataPoolContent::eSINT32);
      c_Min.SetArray(false);
      c_Max.SetType(C_OscNodeDataPoolContent::eSINT32);
      c_Max.SetArray(false);
      c_Value.SetType(C_OscNodeDataPoolContent::eSINT32);
      c_Value.SetArray(false);
      c_Min.SetValueS32(std::numeric_limits<int32_t>::lowest());
      c_Max.SetValueS32(std::numeric_limits<int32_t>::max());
      this->mpc_SpinBoxWidget->Init(c_Min, c_Max);
      c_Value.SetValueS32(os32_Value);
      this->mpc_SpinBoxWidget->SetValue(static_cast<QVariant>(static_cast<int64_t>(c_Value.GetValueS32())));
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
      const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();
      C_PuiSvDbNodeDataPoolListElementId c_Id;

      if ((this->GetDataPoolElementIndex(0, c_Id) == C_NO_ERR) && (pc_Dashboard != NULL))
      {
         const C_PuiSvDbSpinBox * const pc_Box = pc_Dashboard->GetSpinBox(static_cast<uint32_t>(this->ms32_Index));
         tgl_assert(pc_Box != NULL);
         const C_OscNodeDataPoolListElement * const pc_Element =
            C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(c_Id.u32_NodeIndex, c_Id.u32_DataPoolIndex,
                                                                       c_Id.u32_ListIndex, c_Id.u32_ElementIndex);
         if ((pc_Element != NULL) && (pc_Box != NULL))
         {
            QVariant c_VariantValue;
            C_PuiSvDbDataElementScaling c_Scaling;

            tgl_assert(this->GetDataPoolElementScaling(0, c_Scaling) == C_NO_ERR);
            this->mpc_SpinBoxWidget->Init(pc_Element->c_MinValue, pc_Element->c_MaxValue, c_Scaling.f64_Factor,
                                          c_Scaling.f64_Offset);
            this->mpc_SpinBoxWidget->SetShowUnit(pc_Box->q_ShowUnit);
            this->mpc_SpinBoxWidget->SetUnit(c_Scaling.c_Unit);
            tgl_assert(C_SdNdeDpContentUtil::h_SimpleConvertToVariant(pc_Box->c_Value, c_VariantValue) == C_NO_ERR);

            float64_t f64_Test;
            pc_Box->c_Value.GetAnyValueAsFloat64(f64_Test, 0);
            this->mpc_SpinBoxWidget->SetValue(c_VariantValue);
         }
      }
   }
}
