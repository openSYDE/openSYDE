//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Class for system view dashboard toggle item (implementation)

   Class for system view dashboard toggle item

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QGraphicsView>

#include "gitypes.h"
#include "stwtypes.h"
#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_OSCUtils.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "C_GiSvDaToggleBase.h"
#include "C_PuiSvDbTable.h"
#include "C_OgePopUpDialog.h"
#include "C_SyvDaPeBase.h"
#include "C_SyvDaPeToggle.h"
#include "C_SdNdeDataPoolContentUtil.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in]     oru32_ViewIndex        Index of system view
   \param[in]     oru32_DashboardIndex   Index of dashboard in system view
   \param[in]     ors32_DataIndex        Index of data element in dashboard in system view
   \param[in]     oru64_ID               Unique ID
   \param[in,out] opc_Parent             Optional pointer to parent

   \created     25.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiSvDaToggleBase::C_GiSvDaToggleBase(const uint32 & oru32_ViewIndex, const uint32 & oru32_DashboardIndex,
                                       const sint32 & ors32_DataIndex, const uint64 & oru64_ID,
                                       QGraphicsItem * const opc_Parent) :
   C_GiSvDaRectBaseGroup(oru32_ViewIndex, oru32_DashboardIndex, ors32_DataIndex, C_PuiSvDbDataElement::eTOGGLE, 1,
                         oru64_ID, 36.5, 22.0, 83.0, 50.0, true, false, opc_Parent)
{
   mpc_CheckBoxWidget = new C_OgePubDashboard();
   this->mpc_CheckBoxWidget->setText("");
   this->mpc_Widget->SetWidget(this->mpc_CheckBoxWidget);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     25.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_GiSvDaToggleBase::~C_GiSvDaToggleBase(void)
{
   //lint -e{1540}  no memory leak because of the parent of mpc_CheckBoxWidget by calling SetWidget and
   // the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the type of this item

   \return  ID

   \created     25.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sintn C_GiSvDaToggleBase::type(void) const
{
   return msn_GRAPHICS_ITEM_DB_TOGGLE;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Apply style

   \param[in] oe_Style    New style type
   \param[in] oq_DarkMode Flag if dark mode is active

   \created     28.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaToggleBase::SetDisplayStyle(const C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode)
{
   C_GiSvDaRectBaseGroup::SetDisplayStyle(oe_Style, oq_DarkMode);

   if (this->ms32_Index >= 0)
   {
      const C_PuiSvDashboard * const pc_Dashboard = this->GetSvDashboard();

      if (pc_Dashboard != NULL)
      {
         const C_PuiSvDbToggle * const pc_Box =
            pc_Dashboard->GetToggle(static_cast<uint32>(this->ms32_Index));
         tgl_assert(pc_Box != NULL);
         if (pc_Box != NULL)
         {
            this->UpdateType(pc_Box->e_Type);
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adjust font to current size

   \created     25.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaToggleBase::ReInitializeSize(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Load data from system view dashboard

   \created     25.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaToggleBase::LoadData(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbToggle * const pc_Box = pc_Dashboard->GetToggle(static_cast<uint32>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         this->LoadSvBasicData(*pc_Box);
         this->UpdateType(pc_Box->e_Type);
         if (this->mpc_CheckBoxWidget != NULL)
         {
            this->mpc_CheckBoxWidget->setChecked(pc_Box->q_State);
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update data in system view dashboard

   \created     25.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaToggleBase::UpdateData(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbToggle * const pc_Box = pc_Dashboard->GetToggle(static_cast<uint32>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         C_PuiSvDbToggle c_Box = *pc_Box;
         this->UpdateSvBasicData(c_Box);
         if (this->mpc_CheckBoxWidget != NULL)
         {
            c_Box.q_State = this->mpc_CheckBoxWidget->isChecked();
         }
         tgl_assert(C_PuiSvHandler::h_GetInstance()->SetDashboardWidget(this->mu32_ViewIndex,
                                                                        this->mu32_DashboardIndex,
                                                                        static_cast<uint32>(this->ms32_Index),
                                                                        &c_Box, this->me_Type) == C_NO_ERR);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Delete data in system view dashboard

   \created     25.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaToggleBase::DeleteData(void)
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

//-----------------------------------------------------------------------------
/*!
   \brief   Sends the current set value of the element

   \created     28.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiSvDaToggleBase::SendCurrentValue(void)
{
   if (this->mpc_CheckBoxWidget != NULL)
   {
      // Prepare the value
      this->mf64_WriteValue = static_cast<float64>(this->mpc_CheckBoxWidget->isChecked());

      // Send the value
      C_GiSvDaRectBaseGroup::SendCurrentValue();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Call properties for widgets

   \created     12.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_GiSvDaToggleBase::CallProperties(void)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->GetSvDashboard();

   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbToggle * const pc_Box = pc_Dashboard->GetToggle(static_cast<uint32>(this->ms32_Index));
      tgl_assert(pc_Box != NULL);
      if (pc_Box != NULL)
      {
         sint32 s32_Return = C_NO_ERR;
         C_PuiSvDbNodeDataPoolListElementId c_ElementId;
         C_PuiSvDbDataElementScaling c_Scaling;

         // Fill the existing data
         if (this->GetWidgetDataPoolElementCount() > 0)
         {
            s32_Return = this->GetDataPoolElementIndex(0U, c_ElementId);

            if (s32_Return == C_NO_ERR)
            {
               s32_Return = this->GetDataPoolElementScaling(0U, c_Scaling);
            }
         }
         else
         {
            c_ElementId = C_PuiSvDbNodeDataPoolListElementId(0, 0, 0, 0,
                                                             C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT,
                                                             false);
            c_Scaling = C_PuiSvDbDataElementScaling();
         }

         if (s32_Return == C_NO_ERR)
         {
            QGraphicsView * const pc_View = this->scene()->views().at(0);
            QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(pc_View, pc_View);
            C_SyvDaPeBase * pc_Dialog;
            C_SyvDaPeToggle * pc_PropertiesWidget;

            pc_Dialog = new C_SyvDaPeBase(*c_New, this->mu32_ViewIndex, this->mu32_DashboardIndex, "Toggle",
                                          c_ElementId, c_Scaling, false, this->mq_DarkMode);
            pc_PropertiesWidget = new C_SyvDaPeToggle(*pc_Dialog, this->mq_DarkMode);

            pc_Dialog->SetWidget(pc_PropertiesWidget);
            pc_Dialog->SetTheme(pc_Box->e_DisplayStyle);

            //Resize
            c_New->SetSize(QSize(800, 800));

            pc_PropertiesWidget->SetType(pc_Box->e_Type);
            pc_Dialog->SetWriteMode(pc_Box->e_ElementWriteMode);

            if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
            {
               C_PuiSvDbToggle c_Box = *pc_Box;
               C_PuiSvDbNodeDataElementConfig c_Tmp;
               //Save
               this->me_Style = pc_Dialog->GetTheme();
               c_Box.e_DisplayStyle = pc_Dialog->GetTheme();
               c_Box.e_Type = pc_PropertiesWidget->GetType();

               c_Tmp.c_ElementId = pc_Dialog->GetDataElementId();
               c_Tmp.c_ElementScaling = pc_Dialog->GetScalingInformation();
               c_Box.c_DataPoolElementsConfig.clear();
               c_Box.c_DataPoolElementsConfig.push_back(c_Tmp);
               c_Box.e_ElementWriteMode = pc_Dialog->GetWriteMode();

               //Apply
               this->me_WriteMode = pc_Dialog->GetWriteMode();
               this->SetDisplayStyle(this->me_Style, this->mq_DarkMode);
               this->UpdateType(c_Box.e_Type);
               this->ClearDataPoolElements();
               this->RegisterDataPoolElement(pc_Dialog->GetDataElementId(), pc_Dialog->GetScalingInformation());

               tgl_assert(C_PuiSvHandler::h_GetInstance()->SetDashboardWidget(this->mu32_ViewIndex,
                                                                              this->mu32_DashboardIndex,
                                                                              static_cast<uint32>(this->ms32_Index),
                                                                              &c_Box, this->me_Type) == C_NO_ERR);
            }
            Q_EMIT this->SigTriggerUpdateTransmissionConfiguration();
            if (c_New != NULL)
            {
               c_New->HideOverlay();
            }
            //lint -e{429}  no memory leak because of the parent of pc_Dialog and the Qt memory management
         }
      }
   }
   return true;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Information about the start or stop of a connection

   \param[in]  oq_Active      Flag if connection is active or not active now

   \created     17.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_GiSvDaToggleBase::ConnectionActiveChanged(const bool oq_Active)
{
   const C_PuiSvDashboard * const pc_Dashboard = this->GetSvDashboard();

   C_GiSvDaRectBaseGroup::ConnectionActiveChanged(oq_Active);
   if (pc_Dashboard != NULL)
   {
      const C_PuiSvDbToggle * const pc_Box = pc_Dashboard->GetToggle(static_cast<uint32>(this->ms32_Index));
      if ((pc_Box != NULL) && (pc_Box->e_ElementWriteMode == C_PuiSvDbToggle::eWM_ON_CHANGE))
      {
         if (oq_Active == true)
         {
            connect(this->mpc_CheckBoxWidget, &C_OgePubDashboard::toggled, this, &C_GiSvDaToggleBase::SendCurrentValue);
         }
         else
         {
            disconnect(this->mpc_CheckBoxWidget, &C_OgePubDashboard::toggled, this,
                       &C_GiSvDaToggleBase::SendCurrentValue);
         }
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Update type

   Warning: Only use for preview

   \param[in] oe_Type        Type

   \created     12.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_GiSvDaToggleBase::UpdateType(const C_PuiSvDbToggle::E_Type oe_Type)
{
   if (this->mpc_CheckBoxWidget != NULL)
   {
      QString c_SvgUrlUnchecked;
      QString c_SvgUrlChecked;

      switch (this->me_Style)
      {
      //TBD by Karsten: in progress
      case C_PuiSvDbWidgetBase::eOPENSYDE:
         switch (oe_Type)
         {
         case C_PuiSvDbToggle::eTYPE1:
            if (this->mq_DarkMode == false)
            {
               c_SvgUrlUnchecked = "://images/system_views/dashboards/icons/ToggleStwOffT1Bright.svg";
               c_SvgUrlChecked = "://images/system_views/dashboards/icons/ToggleStwOnT1Bright.svg";
            }
            else
            {
               c_SvgUrlUnchecked = "://images/system_views/dashboards/icons/ToggleStwOffT1Dark.svg";
               c_SvgUrlChecked = "://images/system_views/dashboards/icons/ToggleStwOnT1Dark.svg";
            }
            break;
         case C_PuiSvDbToggle::eTYPE2:
            if (this->mq_DarkMode == false)
            {
               c_SvgUrlUnchecked = "://images/system_views/dashboards/icons/ToggleStwOffT2Bright.svg";
               c_SvgUrlChecked = "://images/system_views/dashboards/icons/ToggleStwOnT2Bright.svg";
            }
            else
            {
               c_SvgUrlUnchecked = "://images/system_views/dashboards/icons/ToggleStwOffT2Dark.svg";
               c_SvgUrlChecked = "://images/system_views/dashboards/icons/ToggleStwOnT2Dark.svg";
            }
            break;
         case C_PuiSvDbToggle::eTYPE3:
            if (this->mq_DarkMode == false)
            {
               c_SvgUrlUnchecked = "://images/system_views/dashboards/icons/ToggleStwOffT3Bright.svg";
               c_SvgUrlChecked = "://images/system_views/dashboards/icons/ToggleStwOnT3Bright.svg";
            }
            else
            {
               c_SvgUrlUnchecked = "://images/system_views/dashboards/icons/ToggleStwOffT3Dark.svg";
               c_SvgUrlChecked = "://images/system_views/dashboards/icons/ToggleStwOnT3Dark.svg";
            }
            break;
         default:
            break;
         }
         break;

      case C_PuiSvDbWidgetBase::eOPENSYDE_2:
         switch (oe_Type)
         {
         case C_PuiSvDbToggle::eTYPE1:
            if (this->mq_DarkMode == false)
            {
               c_SvgUrlUnchecked = "://images/system_views/dashboards/icons/ToggleOpenSyde2OffT1Bright.svg";
               c_SvgUrlChecked = "://images/system_views/dashboards/icons/ToggleOpenSyde2OnT1Bright.svg";
            }
            else
            {
               c_SvgUrlUnchecked = "://images/system_views/dashboards/icons/ToggleOpenSyde2OffT1Dark.svg";
               c_SvgUrlChecked = "://images/system_views/dashboards/icons/ToggleOpenSyde2OnT1Dark.svg";
            }
            break;
         case C_PuiSvDbToggle::eTYPE2:
            if (this->mq_DarkMode == false)
            {
               c_SvgUrlUnchecked = "://images/system_views/dashboards/icons/ToggleOpenSyde2OffT2Bright.svg";
               c_SvgUrlChecked = "://images/system_views/dashboards/icons/ToggleOpenSyde2OnT2Bright.svg";
            }
            else
            {
               c_SvgUrlUnchecked = "://images/system_views/dashboards/icons/ToggleOpenSyde2OffT2Dark.svg";
               c_SvgUrlChecked = "://images/system_views/dashboards/icons/ToggleOpenSyde2OnT2Dark.svg";
            }
            break;
         case C_PuiSvDbToggle::eTYPE3:
            if (this->mq_DarkMode == false)
            {
               c_SvgUrlUnchecked = "://images/system_views/dashboards/icons/ToggleOpenSyde2OffT3Bright.svg";
               c_SvgUrlChecked = "://images/system_views/dashboards/icons/ToggleOpenSyde2OnT3Bright.svg";
            }
            else
            {
               c_SvgUrlUnchecked = "://images/system_views/dashboards/icons/ToggleOpenSyde2OffT3Dark.svg";
               c_SvgUrlChecked = "://images/system_views/dashboards/icons/ToggleOpenSyde2OnT3Dark.svg";
            }
            break;
         default:
            break;
         }
         break;

      case C_PuiSvDbWidgetBase::eFLAT:
         switch (oe_Type)
         {
         case C_PuiSvDbToggle::eTYPE1:
            if (this->mq_DarkMode == false)
            {
               c_SvgUrlUnchecked = "://images/system_views/dashboards/icons/ToggleFlatOffT1Bright.svg";
               c_SvgUrlChecked = "://images/system_views/dashboards/icons/ToggleFlatOnT1Bright.svg";
            }
            else
            {
               c_SvgUrlUnchecked = "://images/system_views/dashboards/icons/ToggleFlatOffT1Dark.svg";
               c_SvgUrlChecked = "://images/system_views/dashboards/icons/ToggleFlatOnT1Dark.svg";
            }
            break;
         case C_PuiSvDbToggle::eTYPE2:
            if (this->mq_DarkMode == false)
            {
               c_SvgUrlUnchecked = "://images/system_views/dashboards/icons/ToggleFlatOffT2Bright.svg";
               c_SvgUrlChecked = "://images/system_views/dashboards/icons/ToggleFlatOnT2Bright.svg";
            }
            else
            {
               c_SvgUrlUnchecked = "://images/system_views/dashboards/icons/ToggleFlatOffT2Dark.svg";
               c_SvgUrlChecked = "://images/system_views/dashboards/icons/ToggleFlatOnT2Dark.svg";
            }
            break;
         case C_PuiSvDbToggle::eTYPE3:
            if (this->mq_DarkMode == false)
            {
               c_SvgUrlUnchecked = "://images/system_views/dashboards/icons/ToggleFlatOffT3Bright.svg";
               c_SvgUrlChecked = "://images/system_views/dashboards/icons/ToggleFlatOnT3Bright.svg";
            }
            else
            {
               c_SvgUrlUnchecked = "://images/system_views/dashboards/icons/ToggleFlatOffT3Dark.svg";
               c_SvgUrlChecked = "://images/system_views/dashboards/icons/ToggleFlatOnT3Dark.svg";
            }
            break;
         default:
            break;
         }
         break;

      case C_PuiSvDbWidgetBase::eSKEUOMORPH:
         switch (oe_Type)
         {
         case C_PuiSvDbToggle::eTYPE1:
            c_SvgUrlUnchecked = "://images/system_views/dashboards/icons/ToggleSkeuomorphOffT1Bright.svg";
            c_SvgUrlChecked = "://images/system_views/dashboards/icons/ToggleSkeuomorphOnT1Bright.svg";
            break;
         case C_PuiSvDbToggle::eTYPE2:
            c_SvgUrlUnchecked = "://images/system_views/dashboards/icons/ToggleSkeuomorphOffT2Dark.svg";
            c_SvgUrlChecked = "://images/system_views/dashboards/icons/ToggleSkeuomorphOnT2Dark.svg";
            break;
         case C_PuiSvDbToggle::eTYPE3:
            c_SvgUrlUnchecked = "://images/system_views/dashboards/icons/ToggleSkeuomorphOffT3Bright.svg";
            c_SvgUrlChecked = "://images/system_views/dashboards/icons/ToggleSkeuomorphOnT3Bright.svg";
            break;
         default:
            break;
         }
         break;

      default:
         break;
      }
      this->mpc_CheckBoxWidget->SetFiles(c_SvgUrlChecked, c_SvgUrlUnchecked);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check for any invalid elements

   \return
   true  Found
   false Not found

   \created     16.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_GiSvDaToggleBase::m_CheckInvalidElements(QString & orc_FirstInvalidElementName) const
{
   bool q_Retval = C_GiSvDaRectBaseGroup::m_CheckInvalidElements(orc_FirstInvalidElementName);

   if (q_Retval == false)
   {
      const QMap<C_PuiSvDbNodeDataPoolListElementId, uint32> & rc_Elements = this->GetMappingDpElementToDataSerie();

      for (QMap<C_PuiSvDbNodeDataPoolListElementId, uint32>::const_iterator c_ItElement = rc_Elements.begin();
           c_ItElement != rc_Elements.end(); ++c_ItElement)
      {
         const C_PuiSvDbNodeDataPoolListElementId c_ElementId = c_ItElement.key();

         // Is the data element valid?
         if (c_ElementId.GetIsValid() == true)
         {
            uint32 u32_Index;
            if (this->GetWidgetDataPoolElementIndex(c_ElementId, u32_Index) == C_NO_ERR)
            {
               C_PuiSvDbDataElementScaling c_Scaling;
               const C_OSCNodeDataPoolListElement * const pc_Element =
                  C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(c_ElementId.u32_NodeIndex,
                                                                             c_ElementId.u32_DataPoolIndex,
                                                                             c_ElementId.u32_ListIndex,
                                                                             c_ElementId.u32_ElementIndex);
               if ((pc_Element != NULL) && (this->GetDataPoolElementScaling(u32_Index, c_Scaling) == C_NO_ERR))
               {
                  std::vector<float64> c_Min;
                  std::vector<float64> c_Max;
                  C_SdNdeDataPoolContentUtil::h_GetValuesAsFloat64(pc_Element->c_MinValue, c_Min);
                  C_SdNdeDataPoolContentUtil::h_GetValuesAsFloat64(pc_Element->c_MaxValue, c_Max);
                  if ((c_Min.size() == c_Max.size()) && (c_Min.size() == 1UL))
                  {
                     const float64 f64_MinScaled = C_OSCUtils::h_GetValueScaled(c_Min[0], c_Scaling.f64_Factor,
                                                                                c_Scaling.f64_Offset);
                     const float64 f64_MaxScaled = C_OSCUtils::h_GetValueScaled(c_Max[0], c_Scaling.f64_Factor,
                                                                                c_Scaling.f64_Offset);
                     //Check if out of range
                     if ((f64_MinScaled > 0.0) || (f64_MaxScaled < 1.0))
                     {
                        q_Retval = true;
                        orc_FirstInvalidElementName = pc_Element->c_Name.c_str();
                     }
                  }
               }
            }
         }
      }
   }
   return q_Retval;
}
