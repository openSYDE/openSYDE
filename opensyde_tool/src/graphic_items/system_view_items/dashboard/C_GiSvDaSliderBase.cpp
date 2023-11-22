//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Class for system view dashboard slider item (implementation)

   Class for system view dashboard slider item

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QGraphicsView>
#include <limits>
#include "gitypes.hpp"
#include "stwtypes.hpp"
#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "C_OscUtils.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_GiSvDaSliderBase.hpp"
#include "C_PuiSvDbSlider.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_SyvDaPeBase.hpp"
#include "C_SyvDaPeSlider.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SdNdeDpContentUtil.hpp"
#include "C_OscNodeDataPoolListElement.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_core;

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
C_GiSvDaSliderBase::C_GiSvDaSliderBase(const uint32_t & oru32_ViewIndex, const uint32_t & oru32_DashboardIndex,
                                       const int32_t & ors32_DataIndex, const uint64_t & oru64_Id,
                                       QGraphicsItem * const opc_Parent) :
   C_GiSvDaRectBaseGroup(oru32_ViewIndex, oru32_DashboardIndex, ors32_DataIndex, C_PuiSvDbDataElement::eSLIDER, 1,
                         oru64_Id, 102.2, 15.2, 550.0 * (7.0 / 9.0), 55.0, false, false, opc_Parent),
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
int32_t C_GiSvDaSliderBase::type(void) const
{
   return ms32_GRAPHICS_ITEM_DB_SLIDER;
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
         const C_PuiSvDbSlider * const pc_Box = pc_Dashboard->GetSlider(static_cast<uint32_t>(this->ms32_Index));
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
      const C_PuiSvDbSlider * const pc_Box = pc_Dashboard->GetSlider(static_cast<uint32_t>(this->ms32_Index));
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
      const C_PuiSvDbSlider * const pc_Box = pc_Dashboard->GetSlider(static_cast<uint32_t>(this->ms32_Index));
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
                                                                        static_cast<uint32_t>(this->ms32_Index),
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
                                                                        static_cast<uint32_t>(this->ms32_Index),
                                                                        this->me_Type) ==
                 C_NO_ERR);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates the shown value of the element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaSliderBase::UpdateShowValue(void)
{
   // Poll only when something is expected
   if (this->mq_ManualReadStarted == true)
   {
      QString c_Value;
      float64_t f64_UnscaledValue;
      if (this->m_GetLastValue(0UL, c_Value, &f64_UnscaledValue, NULL) == C_NO_ERR)
      {
         this->m_SetUnscaledValueToSliderWidget(f64_UnscaledValue);
         this->mq_ManualReadStarted = false;
      }
   }

   // For write widget calling base class is not necessary
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
      const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();
      const C_PuiSvDbSlider * const pc_Box = pc_Dashboard->GetSlider(static_cast<uint32_t>(this->ms32_Index));

      if ((pc_Box != NULL) &&
          (pc_Box->e_InitialValueMode == C_PuiSvDbWriteWidgetBase::eIVM_SET_CONSTANT_VALUE))
      {
         // Special case: Defined constant value as start value is set
         // Update before calling base class implementation
         float64_t f64_UnscaledValue;
         tgl_assert(C_SdNdeDpContentUtil::h_GetValueAsFloat64(pc_Box->c_InitialValue, f64_UnscaledValue,
                                                              0UL) == C_NO_ERR);
         this->m_SetUnscaledValueToSliderWidget(f64_UnscaledValue);
      }

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
      if (C_OscUtils::h_IsFloat64NearlyEqual(this->mf64_SliderFactor, 1.0) == true)
      {
         this->mf64_WriteValue = this->mf64_UnscaledMinValue  +
                                 (static_cast<float64_t>(this->mpc_SliderWidget->GetValue()) -
                                  static_cast<float64_t>(this->mpc_SliderWidget->GetMinValue()));
      }
      else
      {
         //In this case we have to skip a few steps (determined by this->mf64_SliderFactor)
         this->mf64_WriteValue =
            this->mf64_UnscaledMinValue  +
            ((static_cast<float64_t>(this->mpc_SliderWidget->GetValue()) -
              static_cast<float64_t>(this->mpc_SliderWidget->GetMinValue())) *
             this->mf64_SliderFactor);
      }
      //For precision reasons we only use valid values (in range of actual type),
      // but this means we have to scale the actual value,
      // so we can apply the generic transmission method (which expects a scaled value)
      this->mf64_WriteValue = C_OscUtils::h_GetValueScaled(this->mf64_WriteValue, c_Scaling.f64_Factor,
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
      const C_PuiSvDbSlider * const pc_Box = pc_Dashboard->GetSlider(static_cast<uint32_t>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         C_PuiSvDbNodeDataPoolListElementId c_ElementId;
         C_PuiSvDbDataElementScaling c_Scaling;
         C_PuiSvDbDataElementDisplayFormatter c_FormatterConfig;
         QGraphicsView * const pc_View = this->scene()->views().at(0);
         const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(pc_View, pc_View);
         C_SyvDaPeBase * pc_Dialog;
         C_SyvDaPeSlider * pc_PropertiesWidget;

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
                                                             false, 0UL,
                                                             false);
            c_Scaling = C_PuiSvDbDataElementScaling();
         }

         pc_Dialog = new C_SyvDaPeBase(*c_New, this->mu32_ViewIndex, this->mu32_DashboardIndex, "Slider", c_ElementId,
                                       c_Scaling, true, c_FormatterConfig, false, this->mq_DarkMode);
         pc_PropertiesWidget = new C_SyvDaPeSlider(*pc_Dialog, this->mq_DarkMode);

         pc_Dialog->SetWidget(pc_PropertiesWidget);
         pc_Dialog->SetTheme(pc_Box->e_DisplayStyle);

         //Resize
         c_New->SetSize(C_SyvDaPeBase::h_GetSliderPopupSizeWithDashboardConnect());

         pc_PropertiesWidget->SetType(pc_Box->e_Type);
         pc_PropertiesWidget->SetShowMinMax(pc_Box->q_ShowMinMax);
         pc_Dialog->SetWriteMode(pc_Box->e_ElementWriteMode);
         pc_Dialog->SetAutoWriteOnConnect(pc_Box->q_AutoWriteOnConnect);
         pc_Dialog->SetDashboardConnectInitialValue(pc_Box->e_InitialValueMode, pc_Box->c_InitialValue, false);

         if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
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

            //Force update
            this->mq_InitialStyleCall = true;
            //Apply
            this->me_WriteMode = pc_Dialog->GetWriteMode();
            this->me_WriteInitialValueMode = c_Box.e_InitialValueMode;
            this->mq_AutoWriteOnConnect = c_Box.q_AutoWriteOnConnect;

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

            //Update style after data update
            {
               this->SetDisplayStyle(this->me_Style, this->mq_DarkMode);
               this->mpc_SliderWidget->SetShowMinMax(c_Box.q_ShowMinMax);
               this->ReInitializeSize();
            }
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

   \param[in]  os32_Value   New value
   \param[in]  os32_Min     New min value
   \param[in]  os32_Max     New max value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaSliderBase::SetValuePe(const int32_t os32_Value, const int32_t os32_Min, const int32_t os32_Max)
{
   if (this->mpc_SliderWidget != NULL)
   {
      this->mpc_SliderWidget->SetMinMax(os32_Min, QString::number(os32_Min), os32_Max, QString::number(os32_Max));
      this->mpc_SliderWidget->SetValue(os32_Value);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function for handling a new registered data element

   Called at the end of RegisterDataPoolElement after registration of a specific Datapool element

   \param[in]  ou32_WidgetDataPoolElementIndex  Index of new registered element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaSliderBase::m_OnDataElementRegistered(const uint32_t ou32_WidgetDataPoolElementIndex)
{
   Q_UNUSED(ou32_WidgetDataPoolElementIndex)
   this->m_UpdateStaticValues();
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaSliderBase::m_UpdateStaticValues(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->m_GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbSlider * const pc_Box = pc_Dashboard->GetSlider(static_cast<uint32_t>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         if (pc_Box->c_DataPoolElementsConfig.size() > 0)
         {
            const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Box->c_DataPoolElementsConfig[0];
            if (rc_Config.c_ElementId.GetIsValid() == true)
            {
               const C_OscNodeDataPoolListElement * const pc_Element =
                  C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(rc_Config.c_ElementId.u32_NodeIndex,
                                                                             rc_Config.c_ElementId.u32_DataPoolIndex,
                                                                             rc_Config.c_ElementId.u32_ListIndex,
                                                                             rc_Config.c_ElementId.u32_ElementIndex);
               if ((pc_Element != NULL) &&
                   (this->mpc_SliderWidget != NULL))
               {
                  uint64_t u64_Steps;
                  float64_t f64_UnscaledMax;
                  C_PuiSvDbDataElementDisplayFormatterConfig c_Formatter;
                  tgl_assert(this->GetDataPoolElementFormatterConfig(0UL, c_Formatter) == C_NO_ERR);
                  C_SdNdeDpContentUtil::h_GetValueAsFloat64(pc_Element->c_MinValue, this->mf64_UnscaledMinValue, 0UL);
                  C_SdNdeDpContentUtil::h_GetValueAsFloat64(pc_Element->c_MaxValue, f64_UnscaledMax, 0UL);
                  if (C_SdNdeDpContentUtil::h_GetNumberOfAvailableSteps(pc_Element->c_MinValue,
                                                                        pc_Element->c_MaxValue,
                                                                        u64_Steps, 0) == C_NO_ERR)
                  {
                     if (u64_Steps > 0)
                     {
                        if (u64_Steps <= static_cast<uint64_t>(std::numeric_limits<uint32_t>::max()))
                        {
                           //Standard
                           const int32_t s32_Max =
                              static_cast<int32_t>(static_cast<int64_t>(std::numeric_limits<int32_t>::lowest()) +
                                                   static_cast<int64_t>(u64_Steps));
                           const QString c_MinText = this->GetUnscaledValueAsScaledString(this->mf64_UnscaledMinValue);
                           const QString c_MaxText = this->GetUnscaledValueAsScaledString(f64_UnscaledMax);
                           this->mpc_SliderWidget->SetMinMax(
                              std::numeric_limits<int32_t>::lowest(), c_MinText, s32_Max, c_MaxText);
                           //Default
                           this->mf64_SliderFactor = 1.0;
                           //Tool tip (BEFORE start value)
                           this->mpc_SliderWidget->SetToolTipParameters(this->mf64_SliderFactor,
                                                                        this->mf64_UnscaledMinValue,
                                                                        rc_Config.c_ElementScaling,
                                                                        pc_Element->GetType(), c_Formatter);
                           //Start value
                           this->mpc_SliderWidget->SetValue(pc_Box->s32_Value);
                        }
                        else
                        {
                           //Start with half steps
                           //Step reduction necessary
                           //In this case we have to skip a few steps (determined by this->mf64_SliderFactor)
                           for (this->mf64_SliderFactor = 2.0;
                                (static_cast<float64_t>(u64_Steps) / this->mf64_SliderFactor) >
                                static_cast<float64_t>(std::numeric_limits<uint32_t>::max());
                                this->mf64_SliderFactor *= 2.0)
                           {
                              //All in for :)
                           }
                           const float64_t f64_Temp = static_cast<float64_t>(u64_Steps) / this->mf64_SliderFactor;
                           const int32_t s32_Max =
                              static_cast<int32_t>(static_cast<int64_t>(std::numeric_limits<int32_t>::lowest()) +
                                                   (static_cast<int64_t>(f64_Temp)));
                           const QString c_MinText = this->GetUnscaledValueAsScaledString(this->mf64_UnscaledMinValue);
                           const QString c_MaxText = this->GetUnscaledValueAsScaledString(f64_UnscaledMax);
                           this->mpc_SliderWidget->SetMinMax(
                              std::numeric_limits<int32_t>::lowest(), c_MinText, s32_Max, c_MaxText);
                           //Tool tip (BEFORE start value)
                           this->mpc_SliderWidget->SetToolTipParameters(this->mf64_SliderFactor,
                                                                        this->mf64_UnscaledMinValue,
                                                                        rc_Config.c_ElementScaling,
                                                                        pc_Element->GetType(), c_Formatter);
                           //Start value
                           this->mpc_SliderWidget->SetValue(pc_Box->s32_Value);
                        }
                     }
                     else
                     {
                        const QString c_MinText = this->GetUnscaledValueAsScaledString(this->mf64_UnscaledMinValue);
                        //No range at all
                        this->mpc_SliderWidget->SetMinMax(0, c_MinText, 0, c_MinText);
                        //Default
                        this->mf64_SliderFactor = 1.0;
                        //Tool tip (BEFORE start value)
                        this->mpc_SliderWidget->SetToolTipParameters(this->mf64_SliderFactor,
                                                                     this->mf64_UnscaledMinValue,
                                                                     rc_Config.c_ElementScaling,
                                                                     pc_Element->GetType(), c_Formatter);
                        //Start value
                        this->mpc_SliderWidget->SetValue(0);
                     }
                  }
                  else
                  {
                     const QString c_MinText = this->GetUnscaledValueAsScaledString(this->mf64_UnscaledMinValue);
                     const QString c_MaxText = this->GetUnscaledValueAsScaledString(f64_UnscaledMax);
                     this->mpc_SliderWidget->SetMinMax(
                        std::numeric_limits<int32_t>::lowest(), c_MinText, std::numeric_limits<int32_t>::max(),
                        c_MaxText);
                     //factor for uint32_t::max steps
                     this->mf64_SliderFactor = (f64_UnscaledMax - this->mf64_UnscaledMinValue) /
                                               static_cast<float64_t>(std::numeric_limits<uint32_t>::max());
                     //Tool tip (BEFORE start value)
                     this->mpc_SliderWidget->SetToolTipParameters(this->mf64_SliderFactor, this->mf64_UnscaledMinValue,
                                                                  rc_Config.c_ElementScaling,
                                                                  pc_Element->GetType(), c_Formatter);
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
      const C_PuiSvDbSlider * const pc_Box = pc_Dashboard->GetSlider(static_cast<uint32_t>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         if (pc_Box->e_ElementWriteMode == C_PuiSvDbWriteWidgetBase::eWM_ON_CHANGE)
         {
            q_Retval = true;
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set unscaled value to slider widget

   \param[in]  of64_NewValue  New value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvDaSliderBase::m_SetUnscaledValueToSliderWidget(const float64_t of64_NewValue) const
{
   if (this->mpc_SliderWidget != NULL)
   {
      //Scale value to slider range
      const float64_t f64_SliderValue = ((of64_NewValue - this->mf64_UnscaledMinValue) /
                                         this->mf64_SliderFactor) +
                                        static_cast<float64_t>(this->mpc_SliderWidget->GetMinValue());
      this->mpc_SliderWidget->SetValue(static_cast<int32_t>(f64_SliderValue));
   }
}
