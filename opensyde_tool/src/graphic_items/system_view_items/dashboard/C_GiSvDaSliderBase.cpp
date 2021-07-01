//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for system view dashboard slider item (implementation)

   Class for system view dashboard slider item

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QGraphicsView>
#include <limits>
#include "gitypes.h"
#include "stwtypes.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_OSCUtils.h"
#include "C_PuiSvHandler.h"
#include "C_GiSvDaSliderBase.h"
#include "C_PuiSvDbSlider.h"
#include "C_OgePopUpDialog.h"
#include "C_SyvDaPeBase.h"
#include "C_SyvDaPeSlider.h"
#include "C_PuiSdHandler.h"
#include "C_SdNdeDpContentUtil.h"
#include "C_OSCNodeDataPoolListElement.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_core;

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
C_GiSvDaSliderBase::C_GiSvDaSliderBase(const uint32 & oru32_ViewIndex, const uint32 & oru32_DashboardIndex,
                                       const sint32 & ors32_DataIndex, const uint64 & oru64_ID,
                                       QGraphicsItem * const opc_Parent) :
   C_GiSvDaRectBaseGroup(oru32_ViewIndex, oru32_DashboardIndex, ors32_DataIndex, C_PuiSvDbDataElement::eSLIDER, 1,
                         oru64_ID, 102.2, 15.2, 550.0 * (7.0 / 9.0), 55.0, false, false, opc_Parent),
   ms32_SliderMin(0),
   mf64_SliderFactor(1.0),
   mf64_UnscaledMinValue(0.0)
{
   mpc_SliderWidget = new C_SyvDaItDashboardSliderWidget();
   this->mpc_Widget->SetWidget(this->mpc_SliderWidget);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_Slider and the Qt memory management
C_GiSvDaSliderBase::~C_GiSvDaSliderBase(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the type of this item

   \return  ID
*/
//----------------------------------------------------------------------------------------------------------------------
sintn C_GiSvDaSliderBase::type(void) const
{
   return msn_GRAPHICS_ITEM_DB_SLIDER;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Apply style

   \param[in]  oe_Style       New style type
   \param[in]  oq_DarkMode    Flag if dark mode is active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaSliderBase::SetDisplayStyle(const C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode)
{
   C_GiSvDaRectBaseGroup::SetDisplayStyle(oe_Style, oq_DarkMode);

   this->mpc_SliderWidget->SetCurrentStyle(oe_Style, oq_DarkMode);
   if (this->ms32_Index >= 0)
   {
      const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();

      if (pc_Dashboard != NULL)
      {
         const C_PuiSvDbSlider * const pc_Box = pc_Dashboard->GetSlider(static_cast<uint32>(this->ms32_Index));
         tgl_assert(pc_Box != NULL);
         if (pc_Box != NULL)
         {
            this->mpc_SliderWidget->SetDisplayStyle(pc_Box->e_Type);
            this->m_UpdateStaticValues();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adjust font to current size
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaSliderBase::ReInitializeSize(void)
{
   if (this->mpc_SliderWidget != NULL)
   {
      this->mpc_SliderWidget->AdjustFontToSize();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load data from system view dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaSliderBase::LoadData(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbSlider * const pc_Box = pc_Dashboard->GetSlider(static_cast<uint32>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         this->LoadSvBasicData(*pc_Box);
         if (this->mpc_SliderWidget != NULL)
         {
            this->mpc_SliderWidget->SetDisplayStyle(pc_Box->e_Type);
            this->mpc_SliderWidget->SetShowMinMax(pc_Box->q_ShowMinMax);
         }
         this->m_UpdateStaticValues();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update data in system view dashboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaSliderBase::UpdateData(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbSlider * const pc_Box = pc_Dashboard->GetSlider(static_cast<uint32>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         C_PuiSvDbSlider c_Box = *pc_Box;
         this->UpdateSvBasicData(c_Box);
         if (this->mpc_SliderWidget != NULL)
         {
            c_Box.s32_Value = this->mpc_SliderWidget->GetValue();
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
void C_GiSvDaSliderBase::DeleteData(void)
{
   if (this->ms32_Index >= 0)
   {
      tgl_assert(C_PuiSvHandler::h_GetInstance()->DeleteDashboardWidget(this->mu32_ViewIndex, this->mu32_DashboardIndex,
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
void C_GiSvDaSliderBase::ConnectionActiveChanged(const bool oq_Active)
{
   if (oq_Active == true)
   {
      this->m_UpdateStaticValues();
      if (this->m_IsOnChange() == true)
      {
         connect(this->mpc_SliderWidget, &C_SyvDaItDashboardSliderWidget::SigOnChange, this,
                 &C_GiSvDaSliderBase::SendCurrentValue);
      }
   }
   else
   {
      if (this->m_IsOnChange() == true)
      {
         disconnect(this->mpc_SliderWidget, &C_SyvDaItDashboardSliderWidget::SigOnChange, this,
                    &C_GiSvDaSliderBase::SendCurrentValue);
      }
   }

   C_GiSvDaRectBaseGroup::ConnectionActiveChanged(oq_Active);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sends the current set value of the element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaSliderBase::SendCurrentValue(void)
{
   if (this->mpc_SliderWidget != NULL)
   {
      C_PuiSvDbDataElementScaling c_Scaling;
      tgl_assert(this->GetDataPoolElementScaling(0, c_Scaling) == C_NO_ERR);
      // Prepare the value
      //We use unscaled values in the original range to have the number of steps the original range would have
      if (C_OSCUtils::h_IsFloat64NearlyEqual(this->mf64_SliderFactor, 1.0) == true)
      {
         this->mf64_WriteValue = this->mf64_UnscaledMinValue  +
                                 (static_cast<float64>(this->mpc_SliderWidget->GetValue()) -
                                  static_cast<float64>(this->ms32_SliderMin));
      }
      else
      {
         //In this case we have to skip a few steps (determined by this->mf64_SliderFactor)
         this->mf64_WriteValue =
            this->mf64_UnscaledMinValue  +
            ((static_cast<float64>(this->mpc_SliderWidget->GetValue()) - static_cast<float64>(this->ms32_SliderMin)) *
             this->mf64_SliderFactor);
      }
      //For precision reasons we only use valid values (in range of actual type),
      // but this means we have to scale the actual value,
      // so we can apply the generic transmission method (which expects a scaled value)
      this->mf64_WriteValue = C_OSCUtils::h_GetValueScaled(this->mf64_WriteValue, c_Scaling.f64_Factor,
                                                           c_Scaling.f64_Offset);
      // Send the value
      C_GiSvDaRectBaseGroup::SendCurrentValue();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call properties for widgets

   \return true (configurable properties called)
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaSliderBase::CallProperties(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbSlider * const pc_Box = pc_Dashboard->GetSlider(static_cast<uint32>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         C_PuiSvDbNodeDataPoolListElementId c_ElementId;
         C_PuiSvDbDataElementScaling c_Scaling;
         QGraphicsView * const pc_View = this->scene()->views().at(0);
         QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(pc_View, pc_View);
         C_SyvDaPeBase * pc_Dialog;
         C_SyvDaPeSlider * pc_PropertiesWidget;

         if (pc_Box->c_DataPoolElementsConfig.size() > 0)
         {
            c_ElementId = pc_Box->c_DataPoolElementsConfig[0].c_ElementId;
            c_Scaling = pc_Box->c_DataPoolElementsConfig[0].c_ElementScaling;
         }
         else
         {
            c_ElementId = C_PuiSvDbNodeDataPoolListElementId(0, 0, 0, 0,
                                                             C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT,
                                                             false, 0UL,
                                                             false);
            c_Scaling = C_PuiSvDbDataElementScaling();
         }

         pc_Dialog = new C_SyvDaPeBase(*c_New, this->mu32_ViewIndex, this->mu32_DashboardIndex, "Slider", c_ElementId,
                                       c_Scaling, false, this->mq_DarkMode);
         pc_PropertiesWidget = new C_SyvDaPeSlider(*pc_Dialog, this->mq_DarkMode);

         pc_Dialog->SetWidget(pc_PropertiesWidget);
         pc_Dialog->SetTheme(pc_Box->e_DisplayStyle);

         //Resize
         c_New->SetSize(QSize(800, 800));

         pc_PropertiesWidget->SetType(pc_Box->e_Type);
         pc_PropertiesWidget->SetShowMinMax(pc_Box->q_ShowMinMax);
         pc_Dialog->SetWriteMode(pc_Box->e_ElementWriteMode);

         if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
         {
            C_PuiSvDbSlider c_Box = *pc_Box;
            C_PuiSvDbNodeDataElementConfig c_Tmp;
            //Save
            this->me_Style = pc_Dialog->GetTheme();
            c_Box.e_DisplayStyle = pc_Dialog->GetTheme();
            c_Box.e_Type = pc_PropertiesWidget->GetType();
            c_Box.q_ShowMinMax = pc_PropertiesWidget->GetShowMinMax();

            c_Tmp.c_ElementId = pc_Dialog->GetDataElementId();
            c_Tmp.c_ElementScaling = pc_Dialog->GetScalingInformation();
            c_Box.c_DataPoolElementsConfig.clear();
            if (c_Tmp.c_ElementId.GetIsValid())
            {
               c_Box.c_DataPoolElementsConfig.push_back(c_Tmp);
            }
            c_Box.e_ElementWriteMode = pc_Dialog->GetWriteMode();

            //Force update
            this->mq_InitialStyleCall = true;
            //Apply
            this->me_WriteMode = pc_Dialog->GetWriteMode();
            this->SetDisplayStyle(this->me_Style, this->mq_DarkMode);
            this->UpdateTypePe(c_Box.e_Type, c_Box.q_ShowMinMax);
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
      } //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management
   }
   return true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update type

   Warning: Only use for preview

   \param[in]  oe_Type        Type
   \param[in]  oq_ShowMinMax  Show minimum / maximum
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaSliderBase::UpdateTypePe(const C_PuiSvDbSlider::E_Type oe_Type, const bool oq_ShowMinMax)
{
   if (this->mpc_SliderWidget != NULL)
   {
      this->mpc_SliderWidget->SetDisplayStyle(oe_Type);
      this->mpc_SliderWidget->SetShowMinMax(oq_ShowMinMax);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set value

   Warning: Only use for preview

   \param[in]  osn_Value   New value
   \param[in]  osn_Min     New min value
   \param[in]  osn_Max     New max value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaSliderBase::SetValuePe(const sintn osn_Value, const sintn osn_Min, const sintn osn_Max)
{
   if (this->mpc_SliderWidget != NULL)
   {
      this->mpc_SliderWidget->SetMinMax(osn_Min, QString::number(osn_Min), osn_Max, QString::number(osn_Max));
      this->mpc_SliderWidget->SetValue(osn_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaSliderBase::m_UpdateStaticValues(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbSlider * const pc_Box = pc_Dashboard->GetSlider(static_cast<uint32>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         if (pc_Box->c_DataPoolElementsConfig.size() > 0)
         {
            const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Box->c_DataPoolElementsConfig[0];
            if (rc_Config.c_ElementId.GetIsValid() == true)
            {
               const C_OSCNodeDataPoolListElement * const pc_Element =
                  C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(rc_Config.c_ElementId.u32_NodeIndex,
                                                                             rc_Config.c_ElementId.u32_DataPoolIndex,
                                                                             rc_Config.c_ElementId.u32_ListIndex,
                                                                             rc_Config.c_ElementId.u32_ElementIndex);
               if ((pc_Element != NULL) &&
                   (this->mpc_SliderWidget != NULL))
               {
                  uint64 u64_Steps;
                  float64 f64_Min;
                  float64 f64_ScaledMin;
                  float64 f64_Max;
                  C_SdNdeDpContentUtil::h_GetValueAsFloat64(pc_Element->c_MinValue, f64_Min, 0UL);
                  this->mf64_UnscaledMinValue = f64_Min;
                  C_SdNdeDpContentUtil::h_GetValueAsFloat64(pc_Element->c_MaxValue, f64_Max, 0UL);
                  f64_ScaledMin = C_OSCUtils::h_GetValueScaled(f64_Min,
                                                               rc_Config.c_ElementScaling.f64_Factor,
                                                               rc_Config.c_ElementScaling.f64_Offset);
                  if (C_SdNdeDpContentUtil::h_GetNumberOfAvailableSteps(pc_Element->c_MinValue,
                                                                        pc_Element->c_MaxValue,
                                                                        u64_Steps, 0) == C_NO_ERR)
                  {
                     if (u64_Steps > 0)
                     {
                        if (u64_Steps <= static_cast<uint64>(std::numeric_limits<uint32>::max()))
                        {
                           //Standard
                           const sint32 s32_Max =
                              static_cast<sint32>(static_cast<sint64>(std::numeric_limits<sint32>::lowest()) +
                                                  static_cast<sint64>(u64_Steps));
                           const QString c_MinText = this->GetUnscaledValueAsScaledString(f64_Min);
                           const QString c_MaxText = this->GetUnscaledValueAsScaledString(f64_Max);
                           this->mpc_SliderWidget->SetMinMax(
                              std::numeric_limits<sint32>::lowest(), c_MinText, s32_Max, c_MaxText);
                           this->ms32_SliderMin = std::numeric_limits<sint32>::lowest();
                           //Default
                           this->mf64_SliderFactor = 1.0;
                           //Tool tip (BEFORE start value)
                           this->mpc_SliderWidget->SetToolTipParameters(static_cast<float64>(this->ms32_SliderMin),
                                                                        f64_ScaledMin,
                                                                        rc_Config.c_ElementScaling.f64_Factor,
                                                                        pc_Element->GetType());
                           //Start value
                           this->mpc_SliderWidget->SetValue(pc_Box->s32_Value);
                        }
                        else
                        {
                           //Start with half steps
                           //Step reduction necessary
                           //In this case we have to skip a few steps (determined by this->mf64_SliderFactor)
                           for (this->mf64_SliderFactor = 2.0;
                                (static_cast<float64>(u64_Steps) / this->mf64_SliderFactor) >
                                static_cast<float64>(std::numeric_limits<uint32>::max());
                                this->mf64_SliderFactor *= 2.0)
                           {
                              //All in for :)
                           }
                           const float64 f64_Temp = static_cast<float64>(u64_Steps) / this->mf64_SliderFactor;
                           const sint32 s32_Max =
                              static_cast<sint32>(static_cast<sint64>(std::numeric_limits<sint32>::lowest()) +
                                                  (static_cast<sint64>(f64_Temp)));
                           const QString c_MinText = this->GetUnscaledValueAsScaledString(f64_Min);
                           const QString c_MaxText = this->GetUnscaledValueAsScaledString(f64_Max);
                           this->mpc_SliderWidget->SetMinMax(
                              std::numeric_limits<sint32>::lowest(), c_MinText, s32_Max, c_MaxText);
                           this->ms32_SliderMin = std::numeric_limits<sint32>::lowest();
                           //Tool tip (BEFORE start value)
                           this->mpc_SliderWidget->SetToolTipParameters(static_cast<float64>(this->ms32_SliderMin),
                                                                        f64_ScaledMin,
                                                                        rc_Config.c_ElementScaling.f64_Factor *
                                                                        this->mf64_SliderFactor, pc_Element->GetType());
                           //Start value
                           this->mpc_SliderWidget->SetValue(pc_Box->s32_Value);
                        }
                     }
                     else
                     {
                        //No range at all
                        this->mpc_SliderWidget->SetMinMax(0, QString::number(f64_Min), 0, QString::number(f64_Min));
                        this->ms32_SliderMin = 0;
                        //Default
                        this->mf64_SliderFactor = 1.0;
                        //Tool tip (BEFORE start value)
                        this->mpc_SliderWidget->SetToolTipParameters(0.0, f64_ScaledMin, 1.0, pc_Element->GetType());
                        //Start value
                        this->mpc_SliderWidget->SetValue(0);
                     }
                  }
                  else
                  {
                     const QString c_MinText = this->GetUnscaledValueAsScaledString(f64_Min);
                     const QString c_MaxText = this->GetUnscaledValueAsScaledString(f64_Max);
                     this->mpc_SliderWidget->SetMinMax(
                        std::numeric_limits<sint32>::lowest(), c_MinText, std::numeric_limits<sint32>::max(),
                        c_MaxText);
                     this->ms32_SliderMin = std::numeric_limits<sint32>::lowest();
                     //factor for uint32::max steps
                     this->mf64_SliderFactor = (f64_Max - f64_Min) /
                                               static_cast<float64>(std::numeric_limits<uint32>::max());
                     //Tool tip (BEFORE start value)
                     this->mpc_SliderWidget->SetToolTipParameters(0.0, f64_ScaledMin,
                                                                  rc_Config.c_ElementScaling.f64_Factor *
                                                                  this->mf64_SliderFactor, pc_Element->GetType());
                     //Start value
                     this->mpc_SliderWidget->SetValue(pc_Box->s32_Value);
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get flag if this element was configured to react on change

   \return
   true  On change
   false Manual (default)
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvDaSliderBase::m_IsOnChange(void) const
{
   bool q_Retval = false;
   const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbSlider * const pc_Box = pc_Dashboard->GetSlider(static_cast<uint32>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         if (pc_Box->e_ElementWriteMode == C_PuiSvDbWidgetBase::eWM_ON_CHANGE)
         {
            q_Retval = true;
         }
      }
   }
   return q_Retval;
}
