//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System view dashboard (implementation)

   System view dashboard

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "TGLUtils.h"
#include "C_CieUtil.h"
#include "C_GtGetText.h"
#include "CSCLChecksums.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "C_PuiSvDashboard.h"
#include "C_OSCNodeDataPoolContentUtil.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDashboard::C_PuiSvDashboard(void) :
   C_PuiBsElements(),
   mc_Name("Dashboard"),
   mc_Comment(""),
   mq_Active(true),
   ms32_TabIndex(-1),
   me_Type(eSCENE)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.

   \param[in,out]  oru32_HashValue  Hash value with init [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::CalcHash(uint32 & oru32_HashValue) const
{
   stw_scl::C_SCLChecksums::CalcCRC32(this->mc_Name.toStdString().c_str(),
                                      this->mc_Name.length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(this->mc_Comment.toStdString().c_str(),
                                      this->mc_Comment.length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->mq_Active, sizeof(this->mq_Active), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->ms32_TabIndex, sizeof(this->ms32_TabIndex), oru32_HashValue);
   for (uint32 u32_ItWidget = 0; u32_ItWidget < this->mc_Charts.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbChart & rc_Widget = this->mc_Charts[u32_ItWidget];
      rc_Widget.CalcHash(oru32_HashValue);
   }
   for (uint32 u32_ItWidget = 0; u32_ItWidget < this->mc_Labels.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbLabel & rc_Label = this->mc_Labels[u32_ItWidget];
      rc_Label.CalcHash(oru32_HashValue);
   }
   for (uint32 u32_ItWidget = 0; u32_ItWidget < this->mc_PieCharts.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbPieChart & rc_Widget = this->mc_PieCharts[u32_ItWidget];
      rc_Widget.CalcHash(oru32_HashValue);
   }
   for (uint32 u32_ItWidget = 0; u32_ItWidget < this->mc_ProgressBars.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbProgressBar & rc_Widget = this->mc_ProgressBars[u32_ItWidget];
      rc_Widget.CalcHash(oru32_HashValue);
   }
   for (uint32 u32_ItWidget = 0; u32_ItWidget < this->mc_SpinBoxes.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbSpinBox & rc_Widget = this->mc_SpinBoxes[u32_ItWidget];
      rc_Widget.CalcHash(oru32_HashValue);
   }
   for (uint32 u32_ItWidget = 0; u32_ItWidget < this->mc_Sliders.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbSlider & rc_Widget = this->mc_Sliders[u32_ItWidget];
      rc_Widget.CalcHash(oru32_HashValue);
   }
   for (uint32 u32_ItWidget = 0; u32_ItWidget < this->mc_Tables.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbTable & rc_Widget = this->mc_Tables[u32_ItWidget];
      rc_Widget.CalcHash(oru32_HashValue);
   }
   for (uint32 u32_ItWidget = 0; u32_ItWidget < this->mc_Toggles.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbToggle & rc_Widget = this->mc_Toggles[u32_ItWidget];
      rc_Widget.CalcHash(oru32_HashValue);
   }
   for (uint32 u32_ItWidget = 0; u32_ItWidget < this->mc_ParamWidgets.size(); ++u32_ItWidget)
   {
      const C_PuiSvDbParam & rc_Widget = this->mc_ParamWidgets[u32_ItWidget];
      rc_Widget.CalcHash(oru32_HashValue);
   }
   this->mc_TabChart.CalcHash(oru32_HashValue);
   C_PuiBsElements::CalcHash(oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get name

   \return
   Current name
*/
//----------------------------------------------------------------------------------------------------------------------
const QString & C_PuiSvDashboard::GetName(void) const
{
   return this->mc_Name;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set name

   \param[in]  orc_Value   New name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::SetName(const QString & orc_Value)
{
   this->mc_Name = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get comment

   \return
   Current comment
*/
//----------------------------------------------------------------------------------------------------------------------
const QString & C_PuiSvDashboard::GetComment(void) const
{
   return this->mc_Comment;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set comment

   \param[in]  orc_Value   New comment
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::SetComment(const QString & orc_Value)
{
   this->mc_Comment = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get type

   \return
   Current type
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDashboard::E_TabType C_PuiSvDashboard::GetType(void) const
{
   return this->me_Type;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set type

   \param[in]  oe_Type  Type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::SetType(const C_PuiSvDashboard::E_TabType oe_Type)
{
   this->me_Type = oe_Type;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get active flag

   \return
   Current active flag
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDashboard::GetActive(void) const
{
   return this->mq_Active;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set active flag

   \param[in]  oq_Value    New active flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::SetActive(const bool oq_Value)
{
   this->mq_Active = oq_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current tab index

   \return
   Current current tab index
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvDashboard::GetTabIndex(void) const
{
   return this->ms32_TabIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set current tab index

   \param[in]  os32_Value  New current tab index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::SetTabIndex(const sint32 os32_Value)
{
   this->ms32_TabIndex = os32_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get widgets

   \return
   Current widgets
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<C_PuiSvDbChart> & C_PuiSvDashboard::GetCharts(void) const
{
   return this->mc_Charts;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set widgets

   \param[in]  orc_Value   New widgets
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::SetCharts(const std::vector<C_PuiSvDbChart> & orc_Value)
{
   this->mc_Charts = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get tab chart

   \return
   Current tab chart
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSvDbTabChart & C_PuiSvDashboard::GetTabChart() const
{
   return this->mc_TabChart;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set tab chart

   \param[in]  orc_Value   New tab chart
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::SetTabChart(const C_PuiSvDbTabChart & orc_Value)
{
   this->mc_TabChart = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get widget

   \param[in]  ou32_Index  Widget index

   \return
   NULL Widget not found
   Else Valid widget
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSvDbChart * C_PuiSvDashboard::GetChart(const uint32 ou32_Index) const
{
   const C_PuiSvDbChart * pc_Retval = NULL;

   if (ou32_Index < this->mc_Charts.size())
   {
      pc_Retval = &this->mc_Charts[ou32_Index];
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get widgets

   \return
   Current widgets
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<C_PuiSvDbLabel> & C_PuiSvDashboard::GetLabels(void) const
{
   return this->mc_Labels;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set widgets

   \param[in]  orc_Value   New widgets
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::SetLabels(const std::vector<C_PuiSvDbLabel> & orc_Value)
{
   this->mc_Labels = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get widget

   \param[in]  ou32_Index  Widget index

   \return
   NULL Widget not found
   Else Valid widget
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSvDbLabel * C_PuiSvDashboard::GetLabel(const uint32 ou32_Index) const
{
   const C_PuiSvDbLabel * pc_Retval = NULL;

   if (ou32_Index < this->mc_Labels.size())
   {
      pc_Retval = &this->mc_Labels[ou32_Index];
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get widgets

   \return
   Current widgets
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<C_PuiSvDbParam> & C_PuiSvDashboard::GetParams(void) const
{
   return this->mc_ParamWidgets;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set widgets

   \param[in]  orc_Value   New widgets
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::SetParams(const std::vector<C_PuiSvDbParam> & orc_Value)
{
   this->mc_ParamWidgets = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get widget

   \param[in]  ou32_Index  Widget index

   \return
   NULL Widget not found
   Else Valid widget
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSvDbParam * C_PuiSvDashboard::GetParam(const uint32 ou32_Index) const
{
   const C_PuiSvDbParam * pc_Retval = NULL;

   if (ou32_Index < this->mc_ParamWidgets.size())
   {
      pc_Retval = &this->mc_ParamWidgets[ou32_Index];
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get widgets

   \return
   Current widgets
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<C_PuiSvDbPieChart> & C_PuiSvDashboard::GetPieCharts(void) const
{
   return this->mc_PieCharts;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set widgets

   \param[in]  orc_Value   New widgets
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::SetPieCharts(const std::vector<C_PuiSvDbPieChart> & orc_Value)
{
   this->mc_PieCharts = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get widget

   \param[in]  ou32_Index  Widget index

   \return
   NULL Widget not found
   Else Valid widget
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSvDbPieChart * C_PuiSvDashboard::GetPieChart(const uint32 ou32_Index) const
{
   const C_PuiSvDbPieChart * pc_Retval = NULL;

   if (ou32_Index < this->mc_PieCharts.size())
   {
      pc_Retval = &this->mc_PieCharts[ou32_Index];
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get widgets

   \return
   Current widgets
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<C_PuiSvDbSpinBox> & C_PuiSvDashboard::GetSpinBoxes(void) const
{
   return this->mc_SpinBoxes;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set widgets

   \param[in]  orc_Value   New widgets
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::SetSpinBoxes(const std::vector<C_PuiSvDbSpinBox> & orc_Value)
{
   this->mc_SpinBoxes = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get widget

   \param[in]  ou32_Index  Widget index

   \return
   NULL Widget not found
   Else Valid widget
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSvDbSpinBox * C_PuiSvDashboard::GetSpinBox(const uint32 ou32_Index) const
{
   const C_PuiSvDbSpinBox * pc_Retval = NULL;

   if (ou32_Index < this->mc_SpinBoxes.size())
   {
      pc_Retval = &this->mc_SpinBoxes[ou32_Index];
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get widgets

   \return
   Current widgets
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<C_PuiSvDbSlider> & C_PuiSvDashboard::GetSliders(void) const
{
   return this->mc_Sliders;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set widgets

   \param[in]  orc_Value   New widgets
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::SetSliders(const std::vector<C_PuiSvDbSlider> & orc_Value)
{
   this->mc_Sliders = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get widget

   \param[in]  ou32_Index  Widget index

   \return
   NULL Widget not found
   Else Valid widget
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSvDbSlider * C_PuiSvDashboard::GetSlider(const uint32 ou32_Index) const
{
   const C_PuiSvDbSlider * pc_Retval = NULL;

   if (ou32_Index < this->mc_Sliders.size())
   {
      pc_Retval = &this->mc_Sliders[ou32_Index];
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get widgets

   \return
   Current widgets
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<C_PuiSvDbProgressBar> & C_PuiSvDashboard::GetProgressBars(void) const
{
   return this->mc_ProgressBars;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set widgets

   \param[in]  orc_Value   New widgets
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::SetProgressBars(const std::vector<C_PuiSvDbProgressBar> & orc_Value)
{
   this->mc_ProgressBars = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get widget

   \param[in]  ou32_Index  Widget index

   \return
   NULL Widget not found
   Else Valid widget
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSvDbProgressBar * C_PuiSvDashboard::GetProgressBar(const uint32 ou32_Index) const
{
   const C_PuiSvDbProgressBar * pc_Retval = NULL;

   if (ou32_Index < this->mc_ProgressBars.size())
   {
      pc_Retval = &this->mc_ProgressBars[ou32_Index];
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get widgets

   \return
   Current widgets
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<C_PuiSvDbTable> & C_PuiSvDashboard::GetTables(void) const
{
   return this->mc_Tables;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set widgets

   \param[in]  orc_Value   New widgets
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::SetTables(const std::vector<C_PuiSvDbTable> & orc_Value)
{
   this->mc_Tables = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get widget

   \param[in]  ou32_Index  Widget index

   \return
   NULL Widget not found
   Else Valid widget
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSvDbTable * C_PuiSvDashboard::GetTable(const uint32 ou32_Index) const
{
   const C_PuiSvDbTable * pc_Retval = NULL;

   if (ou32_Index < this->mc_Tables.size())
   {
      pc_Retval = &this->mc_Tables[ou32_Index];
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get widgets

   \return
   Current widgets
*/
//----------------------------------------------------------------------------------------------------------------------
const std::vector<C_PuiSvDbToggle> & C_PuiSvDashboard::GetToggles(void) const
{
   return this->mc_Toggles;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set widgets

   \param[in]  orc_Value   New widgets
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::SetToggles(const std::vector<C_PuiSvDbToggle> & orc_Value)
{
   this->mc_Toggles = orc_Value;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get widget

   \param[in]  ou32_Index  Widget index

   \return
   NULL Widget not found
   Else Valid widget
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSvDbToggle * C_PuiSvDashboard::GetToggle(const uint32 ou32_Index) const
{
   const C_PuiSvDbToggle * pc_Retval = NULL;

   if (ou32_Index < this->mc_Toggles.size())
   {
      pc_Retval = &this->mc_Toggles[ou32_Index];
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get widget base item

   \param[in]  oe_Type     Widget type
   \param[in]  ou32_Index  Widget index

   \return
   NULL Widget base item not found
   Else Valid widget base item
*/
//----------------------------------------------------------------------------------------------------------------------
const C_PuiSvDbWidgetBase * C_PuiSvDashboard::GetWidgetBase(const C_PuiSvDbDataElement::E_Type oe_Type,
                                                            const uint32 ou32_Index) const
{
   const C_PuiSvDbWidgetBase * pc_Retval = NULL;

   switch (oe_Type)
   {
   case C_PuiSvDbDataElement::eTAB_CHART:
      pc_Retval = &this->mc_TabChart;
      break;
   case C_PuiSvDbDataElement::eLABEL:
      if (ou32_Index < this->mc_Labels.size())
      {
         pc_Retval = &this->mc_Labels[ou32_Index];
      }
      break;
   case C_PuiSvDbDataElement::ePIE_CHART:
      if (ou32_Index < this->mc_PieCharts.size())
      {
         pc_Retval = &this->mc_PieCharts[ou32_Index];
      }
      break;
   case C_PuiSvDbDataElement::eSPIN_BOX:
      if (ou32_Index < this->mc_SpinBoxes.size())
      {
         pc_Retval = &this->mc_SpinBoxes[ou32_Index];
      }
      break;
   case C_PuiSvDbDataElement::eSLIDER:
      if (ou32_Index < this->mc_Sliders.size())
      {
         pc_Retval = &this->mc_Sliders[ou32_Index];
      }
      break;
   case C_PuiSvDbDataElement::eTABLE:
      if (ou32_Index < this->mc_Tables.size())
      {
         pc_Retval = &this->mc_Tables[ou32_Index];
      }
      break;
   case C_PuiSvDbDataElement::eTOGGLE:
      if (ou32_Index < this->mc_Toggles.size())
      {
         pc_Retval = &this->mc_Toggles[ou32_Index];
      }
      break;
   case C_PuiSvDbDataElement::ePROGRESS_BAR:
      if (ou32_Index < this->mc_ProgressBars.size())
      {
         pc_Retval = &this->mc_ProgressBars[ou32_Index];
      }
      break;
   case C_PuiSvDbDataElement::ePARAM:
      if (ou32_Index < this->mc_ParamWidgets.size())
      {
         pc_Retval = &this->mc_ParamWidgets[ou32_Index];
      }
      break;
   case C_PuiSvDbDataElement::eCHART: // handle deprecated chart as unacceptable
   case C_PuiSvDbDataElement::eLINE_ARROW:
   case C_PuiSvDbDataElement::eBOUNDARY:
   case C_PuiSvDbDataElement::eTEXT_ELEMENT:
   case C_PuiSvDbDataElement::eIMAGE:
   case C_PuiSvDbDataElement::eUNKNOWN:
   default:
      //No handling possible
      break;
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all widget items in one single vector

   Warning: output not cleared

   \param[in,out]  orc_Output    Output
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::GetAllWidgetItems(std::vector<const C_PuiSvDbWidgetBase *> & orc_Output) const
{
   const uintn un_Size = this->mc_Charts.size() + this->mc_Labels.size() + this->mc_PieCharts.size() +
                         this->mc_ProgressBars.size() + this->mc_SpinBoxes.size() + this->mc_Sliders.size() +
                         this->mc_Tables.size() + this->mc_Toggles.size() + this->mc_ParamWidgets.size() + 1U;

   //Improve performance
   orc_Output.reserve(un_Size);
   for (uint32 u32_ItItem = 0; u32_ItItem < this->mc_Charts.size(); ++u32_ItItem)
   {
      orc_Output.push_back(&this->mc_Charts[u32_ItItem]);
   }
   for (uint32 u32_ItItem = 0; u32_ItItem < this->mc_Labels.size(); ++u32_ItItem)
   {
      orc_Output.push_back(&this->mc_Labels[u32_ItItem]);
   }
   for (uint32 u32_ItItem = 0; u32_ItItem < this->mc_ParamWidgets.size(); ++u32_ItItem)
   {
      orc_Output.push_back(&this->mc_ParamWidgets[u32_ItItem]);
   }
   for (uint32 u32_ItItem = 0; u32_ItItem < this->mc_PieCharts.size(); ++u32_ItItem)
   {
      orc_Output.push_back(&this->mc_PieCharts[u32_ItItem]);
   }
   for (uint32 u32_ItItem = 0; u32_ItItem < this->mc_ProgressBars.size(); ++u32_ItItem)
   {
      orc_Output.push_back(&this->mc_ProgressBars[u32_ItItem]);
   }
   for (uint32 u32_ItItem = 0; u32_ItItem < this->mc_SpinBoxes.size(); ++u32_ItItem)
   {
      orc_Output.push_back(&this->mc_SpinBoxes[u32_ItItem]);
   }
   for (uint32 u32_ItItem = 0; u32_ItItem < this->mc_Sliders.size(); ++u32_ItItem)
   {
      orc_Output.push_back(&this->mc_Sliders[u32_ItItem]);
   }
   for (uint32 u32_ItItem = 0; u32_ItItem < this->mc_Tables.size(); ++u32_ItItem)
   {
      orc_Output.push_back(&this->mc_Tables[u32_ItItem]);
   }
   for (uint32 u32_ItItem = 0; u32_ItItem < this->mc_Toggles.size(); ++u32_ItItem)
   {
      orc_Output.push_back(&this->mc_Toggles[u32_ItItem]);
   }
   orc_Output.push_back(&this->mc_TabChart);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets all registered dashboard elements of all dashboard tabs

   \param[in,out]  orc_Ids    Set with all registered elements. Will not be cleared when called
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::GetAllRegisteredDashboardElements(std::set<C_OSCNodeDataPoolListElementId> & orc_Ids) const
{
   std::vector<const C_PuiSvDbWidgetBase *> c_Widgets;
   uint32 u32_WidgetCounter;

   this->GetAllWidgetItems(c_Widgets);

   for (u32_WidgetCounter = 0U; u32_WidgetCounter < c_Widgets.size(); ++u32_WidgetCounter)
   {
      const C_PuiSvDbWidgetBase * const pc_Widget = c_Widgets[u32_WidgetCounter];
      uint32 u32_ConfigCounter;

      for (u32_ConfigCounter = 0U; u32_ConfigCounter < pc_Widget->c_DataPoolElementsConfig.size(); ++u32_ConfigCounter)
      {
         orc_Ids.insert(pc_Widget->c_DataPoolElementsConfig[u32_ConfigCounter].c_ElementId);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set widget

   \param[in]  ou32_Index  Widget index
   \param[in]  opc_Value   New widget value
   \param[in]  oe_Type     New widget type

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvDashboard::SetWidget(const uint32 ou32_Index, const C_PuiSvDbWidgetBase * const opc_Value,
                                   const C_PuiSvDbDataElement::E_Type oe_Type)
{
   sint32 s32_Retval = C_NO_ERR;
   const C_PuiSvDbTabChart * opc_TabChart;
   const C_PuiSvDbLabel * opc_Label;
   const C_PuiSvDbParam * opc_ParamWidget;
   const C_PuiSvDbPieChart * opc_PieChart;
   const C_PuiSvDbProgressBar * opc_ProgressBar;
   const C_PuiSvDbSlider * opc_Slider;
   const C_PuiSvDbSpinBox * opc_SpinBox;
   const C_PuiSvDbTable * opc_Table;
   const C_PuiSvDbToggle * opc_Toggle;

   switch (oe_Type)
   {
   case C_PuiSvDbDataElement::eTAB_CHART:
      opc_TabChart = dynamic_cast<const C_PuiSvDbTabChart *>(opc_Value);
      if (opc_TabChart != NULL)
      {
         this->mc_TabChart = *opc_TabChart;
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      break;
   case C_PuiSvDbDataElement::eLABEL:
      opc_Label = dynamic_cast<const C_PuiSvDbLabel *>(opc_Value);
      if (opc_Label != NULL)
      {
         if (ou32_Index < this->mc_Labels.size())
         {
            this->mc_Labels[ou32_Index] = *opc_Label;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      break;
   case C_PuiSvDbDataElement::ePARAM:
      opc_ParamWidget = dynamic_cast<const C_PuiSvDbParam *>(opc_Value);
      if (opc_ParamWidget != NULL)
      {
         if (ou32_Index < this->mc_ParamWidgets.size())
         {
            this->mc_ParamWidgets[ou32_Index] = *opc_ParamWidget;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      break;
   case C_PuiSvDbDataElement::ePIE_CHART:
      opc_PieChart = dynamic_cast<const C_PuiSvDbPieChart *>(opc_Value);
      if (opc_PieChart != NULL)
      {
         if (ou32_Index < this->mc_PieCharts.size())
         {
            this->mc_PieCharts[ou32_Index] = *opc_PieChart;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      break;
   case C_PuiSvDbDataElement::ePROGRESS_BAR:
      opc_ProgressBar = dynamic_cast<const C_PuiSvDbProgressBar *>(opc_Value);
      if (opc_ProgressBar != NULL)
      {
         if (ou32_Index < this->mc_ProgressBars.size())
         {
            this->mc_ProgressBars[ou32_Index] = *opc_ProgressBar;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      break;
   case C_PuiSvDbDataElement::eSLIDER:
      opc_Slider = dynamic_cast<const C_PuiSvDbSlider *>(opc_Value);
      if (opc_Slider != NULL)
      {
         if (ou32_Index < this->mc_Sliders.size())
         {
            this->mc_Sliders[ou32_Index] = *opc_Slider;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      break;
   case C_PuiSvDbDataElement::eSPIN_BOX:
      opc_SpinBox = dynamic_cast<const C_PuiSvDbSpinBox *>(opc_Value);
      if (opc_SpinBox != NULL)
      {
         if (ou32_Index < this->mc_SpinBoxes.size())
         {
            this->mc_SpinBoxes[ou32_Index] = *opc_SpinBox;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      break;
   case C_PuiSvDbDataElement::eTABLE:
      opc_Table = dynamic_cast<const C_PuiSvDbTable *>(opc_Value);
      if (opc_Table != NULL)
      {
         if (ou32_Index < this->mc_Tables.size())
         {
            this->mc_Tables[ou32_Index] = *opc_Table;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      break;
   case C_PuiSvDbDataElement::eTOGGLE:
      opc_Toggle = dynamic_cast<const C_PuiSvDbToggle *>(opc_Value);
      if (opc_Toggle != NULL)
      {
         if (ou32_Index < this->mc_Toggles.size())
         {
            this->mc_Toggles[ou32_Index] = *opc_Toggle;
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      break;
   case C_PuiSvDbDataElement::eCHART: // setting new deprecated chart is forbidden
   case C_PuiSvDbDataElement::eLINE_ARROW:
   case C_PuiSvDbDataElement::eBOUNDARY:
   case C_PuiSvDbDataElement::eTEXT_ELEMENT:
   case C_PuiSvDbDataElement::eIMAGE:
   case C_PuiSvDbDataElement::eUNKNOWN:
   default:
      s32_Retval = C_RANGE;
      break;
   }

   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_Index  Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::OnSyncNodeAdded(const uint32 ou32_Index)
{
   std::vector<C_PuiSvDbWidgetBase *> c_Widgets;
   m_GetAllWidgetItems(c_Widgets);
   for (uint32 u32_ItWidget = 0; u32_ItWidget < c_Widgets.size(); ++u32_ItWidget)
   {
      C_PuiSvDbWidgetBase * const pc_Widget = c_Widgets[u32_ItWidget];
      C_PuiSvDbParam * const pc_Param = dynamic_cast<C_PuiSvDbParam * const>(c_Widgets[u32_ItWidget]);
      if (pc_Widget != NULL)
      {
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_Widget->c_DataPoolElementsConfig.size(); ++u32_ItElement)
         {
            C_PuiSvDbNodeDataElementConfig & rc_DataElementConfig = pc_Widget->c_DataPoolElementsConfig[u32_ItElement];
            C_PuiSvDbNodeDataPoolListElementId & rc_DataElementId = rc_DataElementConfig.c_ElementId;
            h_OnSyncNodeAdded(rc_DataElementId, ou32_Index);
         }
      }
      if (pc_Param != NULL)
      {
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_Param->c_ExpandedItems.size(); ++u32_ItElement)
         {
            C_PuiSvDbExpandedTreeIndex & rc_DataElementConfig = pc_Param->c_ExpandedItems[u32_ItElement];
            h_OnSyncNodeAdded(rc_DataElementConfig.c_ExpandedId, ou32_Index);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapt to system definition change

   \param[in]  ou32_Index        Index
   \param[in]  orc_MapCurToNew   Map cur to new
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::OnSyncNodeHALC(const uint32 ou32_Index, const std::map<C_PuiSvDbNodeDataPoolListElementId,
                                                                              C_PuiSvDbNodeDataPoolListElementId> & orc_MapCurToNew)
{
   std::vector<C_PuiSvDbWidgetBase *> c_Widgets;
   m_GetAllWidgetItems(c_Widgets);
   for (uint32 u32_ItWidget = 0; u32_ItWidget < c_Widgets.size(); ++u32_ItWidget)
   {
      C_PuiSvDbWidgetBase * const pc_Widget = c_Widgets[u32_ItWidget];
      C_PuiSvDbParam * const pc_Param = dynamic_cast<C_PuiSvDbParam * const>(c_Widgets[u32_ItWidget]);
      if (pc_Widget != NULL)
      {
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_Widget->c_DataPoolElementsConfig.size(); ++u32_ItElement)
         {
            C_PuiSvDbNodeDataElementConfig & rc_DataElementConfig = pc_Widget->c_DataPoolElementsConfig[u32_ItElement];
            C_PuiSvDbNodeDataPoolListElementId & rc_DataElementId = rc_DataElementConfig.c_ElementId;
            h_OnSyncNodeHALC(rc_DataElementId, ou32_Index, orc_MapCurToNew);
         }
      }
      if (pc_Param != NULL)
      {
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_Param->c_ExpandedItems.size(); ++u32_ItElement)
         {
            C_PuiSvDbExpandedTreeIndex & rc_DataElementConfig = pc_Param->c_ExpandedItems[u32_ItElement];
            h_OnSyncNodeHALC(rc_DataElementConfig.c_ExpandedId, ou32_Index, orc_MapCurToNew);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_Index  Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::OnSyncNodeAboutToBeDeleted(const uint32 ou32_Index)
{
   std::vector<C_PuiSvDbWidgetBase *> c_Widgets;
   m_GetAllWidgetItems(c_Widgets);
   for (uint32 u32_ItWidget = 0; u32_ItWidget < c_Widgets.size(); ++u32_ItWidget)
   {
      C_PuiSvDbWidgetBase * const pc_Widget = c_Widgets[u32_ItWidget];
      C_PuiSvDbParam * const pc_Param = dynamic_cast<C_PuiSvDbParam * const>(c_Widgets[u32_ItWidget]);
      if (pc_Widget != NULL)
      {
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_Widget->c_DataPoolElementsConfig.size(); ++u32_ItElement)
         {
            C_PuiSvDbNodeDataElementConfig & rc_DataElementConfig = pc_Widget->c_DataPoolElementsConfig[u32_ItElement];
            C_PuiSvDbNodeDataPoolListElementId & rc_DataElementId = rc_DataElementConfig.c_ElementId;
            h_OnSyncNodeAboutToBeDeleted(rc_DataElementId, ou32_Index);
         }
      }
      if (pc_Param != NULL)
      {
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_Param->c_ExpandedItems.size(); ++u32_ItElement)
         {
            C_PuiSvDbExpandedTreeIndex & rc_DataElementConfig = pc_Param->c_ExpandedItems[u32_ItElement];
            h_OnSyncNodeAboutToBeDeleted(rc_DataElementConfig.c_ExpandedId, ou32_Index);
         }
      }
   }
   m_SyncCleanUpParams();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::OnSyncNodeDataPoolAdded(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex)
{
   std::vector<C_PuiSvDbWidgetBase *> c_Widgets;
   m_GetAllWidgetItems(c_Widgets);
   for (uint32 u32_ItWidget = 0; u32_ItWidget < c_Widgets.size(); ++u32_ItWidget)
   {
      C_PuiSvDbWidgetBase * const pc_Widget = c_Widgets[u32_ItWidget];
      C_PuiSvDbParam * const pc_Param = dynamic_cast<C_PuiSvDbParam * const>(c_Widgets[u32_ItWidget]);
      if (pc_Widget != NULL)
      {
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_Widget->c_DataPoolElementsConfig.size(); ++u32_ItElement)
         {
            C_PuiSvDbNodeDataElementConfig & rc_DataElementConfig = pc_Widget->c_DataPoolElementsConfig[u32_ItElement];
            C_PuiSvDbNodeDataPoolListElementId & rc_DataElementId = rc_DataElementConfig.c_ElementId;
            h_OnSyncNodeDataPoolAdded(rc_DataElementId, ou32_NodeIndex, ou32_DataPoolIndex);
         }
      }
      if (pc_Param != NULL)
      {
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_Param->c_ExpandedItems.size(); ++u32_ItElement)
         {
            C_PuiSvDbExpandedTreeIndex & rc_DataElementConfig = pc_Param->c_ExpandedItems[u32_ItElement];
            h_OnSyncNodeDataPoolAdded(rc_DataElementConfig.c_ExpandedId, ou32_NodeIndex, ou32_DataPoolIndex);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex             Node index
   \param[in]  ou32_DataPoolSourceIndex   Source data pool index
   \param[in]  ou32_DataPoolTargetIndex   Target data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::OnSyncNodeDataPoolMoved(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolSourceIndex,
                                               const uint32 ou32_DataPoolTargetIndex)
{
   std::vector<C_PuiSvDbWidgetBase *> c_Widgets;
   m_GetAllWidgetItems(c_Widgets);
   for (uint32 u32_ItWidget = 0; u32_ItWidget < c_Widgets.size(); ++u32_ItWidget)
   {
      C_PuiSvDbWidgetBase * const pc_Widget = c_Widgets[u32_ItWidget];
      C_PuiSvDbParam * const pc_Param = dynamic_cast<C_PuiSvDbParam * const>(c_Widgets[u32_ItWidget]);
      if (pc_Widget != NULL)
      {
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_Widget->c_DataPoolElementsConfig.size(); ++u32_ItElement)
         {
            C_PuiSvDbNodeDataElementConfig & rc_DataElementConfig = pc_Widget->c_DataPoolElementsConfig[u32_ItElement];
            C_PuiSvDbNodeDataPoolListElementId & rc_DataElementId = rc_DataElementConfig.c_ElementId;
            h_OnSyncNodeDataPoolMoved(rc_DataElementId, ou32_NodeIndex, ou32_DataPoolSourceIndex,
                                      ou32_DataPoolTargetIndex);
         }
      }
      if (pc_Param != NULL)
      {
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_Param->c_ExpandedItems.size(); ++u32_ItElement)
         {
            C_PuiSvDbExpandedTreeIndex & rc_DataElementConfig = pc_Param->c_ExpandedItems[u32_ItElement];
            h_OnSyncNodeDataPoolMoved(rc_DataElementConfig.c_ExpandedId, ou32_NodeIndex, ou32_DataPoolSourceIndex,
                                      ou32_DataPoolTargetIndex);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::OnSyncNodeDataPoolAboutToBeDeleted(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex)
{
   std::vector<C_PuiSvDbWidgetBase *> c_Widgets;
   m_GetAllWidgetItems(c_Widgets);
   for (uint32 u32_ItWidget = 0; u32_ItWidget < c_Widgets.size(); ++u32_ItWidget)
   {
      C_PuiSvDbWidgetBase * const pc_Widget = c_Widgets[u32_ItWidget];
      C_PuiSvDbParam * const pc_Param = dynamic_cast<C_PuiSvDbParam * const>(c_Widgets[u32_ItWidget]);
      if (pc_Widget != NULL)
      {
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_Widget->c_DataPoolElementsConfig.size(); ++u32_ItElement)
         {
            C_PuiSvDbNodeDataElementConfig & rc_DataElementConfig = pc_Widget->c_DataPoolElementsConfig[u32_ItElement];
            C_PuiSvDbNodeDataPoolListElementId & rc_DataElementId = rc_DataElementConfig.c_ElementId;
            h_OnSyncNodeDataPoolAboutToBeDeleted(rc_DataElementId, ou32_NodeIndex, ou32_DataPoolIndex);
         }
      }
      if (pc_Param != NULL)
      {
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_Param->c_ExpandedItems.size(); ++u32_ItElement)
         {
            C_PuiSvDbExpandedTreeIndex & rc_DataElementConfig = pc_Param->c_ExpandedItems[u32_ItElement];
            h_OnSyncNodeDataPoolAboutToBeDeleted(rc_DataElementConfig.c_ExpandedId, ou32_NodeIndex, ou32_DataPoolIndex);
         }
      }
   }
   m_SyncCleanUpParams();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::OnSyncNodeDataPoolListAdded(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                   const uint32 ou32_ListIndex)
{
   std::vector<C_PuiSvDbWidgetBase *> c_Widgets;
   m_GetAllWidgetItems(c_Widgets);
   for (uint32 u32_ItWidget = 0; u32_ItWidget < c_Widgets.size(); ++u32_ItWidget)
   {
      C_PuiSvDbWidgetBase * const pc_Widget = c_Widgets[u32_ItWidget];
      C_PuiSvDbParam * const pc_Param = dynamic_cast<C_PuiSvDbParam * const>(c_Widgets[u32_ItWidget]);
      if (pc_Widget != NULL)
      {
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_Widget->c_DataPoolElementsConfig.size(); ++u32_ItElement)
         {
            C_PuiSvDbNodeDataElementConfig & rc_DataElementConfig = pc_Widget->c_DataPoolElementsConfig[u32_ItElement];
            C_PuiSvDbNodeDataPoolListElementId & rc_DataElementId = rc_DataElementConfig.c_ElementId;
            h_OnSyncNodeDataPoolListAdded(rc_DataElementId, ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex);
         }
      }
      if (pc_Param != NULL)
      {
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_Param->c_ExpandedItems.size(); ++u32_ItElement)
         {
            C_PuiSvDbExpandedTreeIndex & rc_DataElementConfig = pc_Param->c_ExpandedItems[u32_ItElement];
            h_OnSyncNodeDataPoolListAdded(rc_DataElementConfig.c_ExpandedId, ou32_NodeIndex, ou32_DataPoolIndex,
                                          ou32_ListIndex);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex          Node index
   \param[in]  ou32_DataPoolIndex      Data pool index
   \param[in]  ou32_ListSourceIndex    Source list index
   \param[in]  ou32_ListTargetIndex    Target list index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::OnSyncNodeDataPoolListMoved(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                   const uint32 ou32_ListSourceIndex, const uint32 ou32_ListTargetIndex)
{
   std::vector<C_PuiSvDbWidgetBase *> c_Widgets;
   m_GetAllWidgetItems(c_Widgets);
   for (uint32 u32_ItWidget = 0; u32_ItWidget < c_Widgets.size(); ++u32_ItWidget)
   {
      C_PuiSvDbWidgetBase * const pc_Widget = c_Widgets[u32_ItWidget];
      C_PuiSvDbParam * const pc_Param = dynamic_cast<C_PuiSvDbParam * const>(c_Widgets[u32_ItWidget]);
      if (pc_Widget != NULL)
      {
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_Widget->c_DataPoolElementsConfig.size(); ++u32_ItElement)
         {
            C_PuiSvDbNodeDataElementConfig & rc_DataElementConfig = pc_Widget->c_DataPoolElementsConfig[u32_ItElement];
            C_PuiSvDbNodeDataPoolListElementId & rc_DataElementId = rc_DataElementConfig.c_ElementId;
            h_OnSyncNodeDataPoolListMoved(rc_DataElementId, ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListSourceIndex,
                                          ou32_ListTargetIndex);
         }
      }
      if (pc_Param != NULL)
      {
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_Param->c_ExpandedItems.size(); ++u32_ItElement)
         {
            C_PuiSvDbExpandedTreeIndex & rc_DataElementConfig = pc_Param->c_ExpandedItems[u32_ItElement];
            h_OnSyncNodeDataPoolListMoved(rc_DataElementConfig.c_ExpandedId, ou32_NodeIndex, ou32_DataPoolIndex,
                                          ou32_ListSourceIndex, ou32_ListTargetIndex);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::OnSyncNodeDataPoolListAboutToBeDeleted(const uint32 ou32_NodeIndex,
                                                              const uint32 ou32_DataPoolIndex,
                                                              const uint32 ou32_ListIndex)
{
   std::vector<C_PuiSvDbWidgetBase *> c_Widgets;
   m_GetAllWidgetItems(c_Widgets);
   for (uint32 u32_ItWidget = 0; u32_ItWidget < c_Widgets.size(); ++u32_ItWidget)
   {
      C_PuiSvDbWidgetBase * const pc_Widget = c_Widgets[u32_ItWidget];
      C_PuiSvDbParam * const pc_Param = dynamic_cast<C_PuiSvDbParam * const>(c_Widgets[u32_ItWidget]);
      if (pc_Widget != NULL)
      {
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_Widget->c_DataPoolElementsConfig.size(); ++u32_ItElement)
         {
            C_PuiSvDbNodeDataElementConfig & rc_DataElementConfig = pc_Widget->c_DataPoolElementsConfig[u32_ItElement];
            C_PuiSvDbNodeDataPoolListElementId & rc_DataElementId = rc_DataElementConfig.c_ElementId;
            h_OnSyncNodeDataPoolListAboutToBeDeleted(rc_DataElementId, ou32_NodeIndex, ou32_DataPoolIndex,
                                                     ou32_ListIndex);
         }
      }
      if (pc_Param != NULL)
      {
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_Param->c_ExpandedItems.size(); ++u32_ItElement)
         {
            C_PuiSvDbExpandedTreeIndex & rc_DataElementConfig = pc_Param->c_ExpandedItems[u32_ItElement];
            h_OnSyncNodeDataPoolListAboutToBeDeleted(rc_DataElementConfig.c_ExpandedId, ou32_NodeIndex,
                                                     ou32_DataPoolIndex, ou32_ListIndex);
         }
      }
   }
   m_SyncCleanUpParams();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_DataSetIndex    Data set index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::OnSyncNodeDataPoolListDataSetAdded(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                          const uint32 ou32_ListIndex, const uint32 ou32_DataSetIndex)
{
   for (uint32 u32_ItParam = 0; u32_ItParam < this->mc_ParamWidgets.size(); ++u32_ItParam)
   {
      C_PuiSvDbParam & rc_ParamWidget = this->mc_ParamWidgets[u32_ItParam];
      for (uint32 u32_ItElement = 0; u32_ItElement < rc_ParamWidget.c_DataPoolElementsConfig.size();
           ++u32_ItElement)
      {
         const C_PuiSvDbNodeDataElementConfig & rc_DataElementConfig =
            rc_ParamWidget.c_DataPoolElementsConfig[u32_ItElement];
         if (((rc_DataElementConfig.c_ElementId.u32_NodeIndex == ou32_NodeIndex) &&
              (rc_DataElementConfig.c_ElementId.u32_DataPoolIndex == ou32_DataPoolIndex)) &&
             (rc_DataElementConfig.c_ElementId.u32_ListIndex == ou32_ListIndex))
         {
            //Should be synchronized
            tgl_assert(u32_ItElement < rc_ParamWidget.c_DataSetSelectionIndices.size());
            if (u32_ItElement < rc_ParamWidget.c_DataSetSelectionIndices.size())
            {
               sint32 & rs32_DataSetIndex = rc_ParamWidget.c_DataSetSelectionIndices[u32_ItElement];
               if (rs32_DataSetIndex >= 0)
               {
                  const uint32 u32_DataSetIndex = static_cast<uint32>(rs32_DataSetIndex);
                  if (u32_DataSetIndex >= ou32_DataSetIndex)
                  {
                     ++rs32_DataSetIndex;
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex             Node index
   \param[in]  ou32_DataPoolIndex         Data pool index
   \param[in]  ou32_ListIndex             List index
   \param[in]  ou32_DataSetSourceIndex    Source data set index
   \param[in]  ou32_DataSetTargetIndex    Target data set index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::OnSyncNodeDataPoolListDataSetMoved(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                          const uint32 ou32_ListIndex,
                                                          const uint32 ou32_DataSetSourceIndex,
                                                          const uint32 ou32_DataSetTargetIndex)
{
   for (uint32 u32_ItParam = 0; u32_ItParam < this->mc_ParamWidgets.size(); ++u32_ItParam)
   {
      C_PuiSvDbParam & rc_ParamWidget = this->mc_ParamWidgets[u32_ItParam];
      for (uint32 u32_ItElement = 0; u32_ItElement < rc_ParamWidget.c_DataPoolElementsConfig.size();
           ++u32_ItElement)
      {
         const C_PuiSvDbNodeDataElementConfig & rc_DataElementConfig =
            rc_ParamWidget.c_DataPoolElementsConfig[u32_ItElement];
         if (((rc_DataElementConfig.c_ElementId.u32_NodeIndex == ou32_NodeIndex) &&
              (rc_DataElementConfig.c_ElementId.u32_DataPoolIndex == ou32_DataPoolIndex)) &&
             (rc_DataElementConfig.c_ElementId.u32_ListIndex == ou32_ListIndex))
         {
            //Should be synchronized
            tgl_assert(u32_ItElement < rc_ParamWidget.c_DataSetSelectionIndices.size());
            if (u32_ItElement < rc_ParamWidget.c_DataSetSelectionIndices.size())
            {
               sint32 & rs32_DataSetIndex = rc_ParamWidget.c_DataSetSelectionIndices[u32_ItElement];
               if (rs32_DataSetIndex >= 0)
               {
                  const uint32 u32_DataSetIndex = static_cast<uint32>(rs32_DataSetIndex);
                  if (u32_DataSetIndex > ou32_DataSetSourceIndex)
                  {
                     --rs32_DataSetIndex;
                     if ((u32_DataSetIndex - 1) >= ou32_DataSetTargetIndex)
                     {
                        ++rs32_DataSetIndex;
                     }
                     else
                     {
                        //No adaptation necessary
                     }
                  }
                  else if (u32_DataSetIndex == ou32_DataSetSourceIndex)
                  {
                     rs32_DataSetIndex = ou32_DataSetTargetIndex;
                  }
                  else
                  {
                     if (u32_DataSetIndex >= ou32_DataSetTargetIndex)
                     {
                        ++rs32_DataSetIndex;
                     }
                     else
                     {
                        //No adaptation necessary
                     }
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_DataSetIndex    Data set index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::OnSyncNodeDataPoolListDataSetAboutToBeDeleted(const uint32 ou32_NodeIndex,
                                                                     const uint32 ou32_DataPoolIndex,
                                                                     const uint32 ou32_ListIndex,
                                                                     const uint32 ou32_DataSetIndex)
{
   for (uint32 u32_ItParam = 0; u32_ItParam < this->mc_ParamWidgets.size(); ++u32_ItParam)
   {
      C_PuiSvDbParam & rc_ParamWidget = this->mc_ParamWidgets[u32_ItParam];
      for (uint32 u32_ItElement = 0; u32_ItElement < rc_ParamWidget.c_DataPoolElementsConfig.size();
           ++u32_ItElement)
      {
         const C_PuiSvDbNodeDataElementConfig & rc_DataElementConfig =
            rc_ParamWidget.c_DataPoolElementsConfig[u32_ItElement];
         if (((rc_DataElementConfig.c_ElementId.u32_NodeIndex == ou32_NodeIndex) &&
              (rc_DataElementConfig.c_ElementId.u32_DataPoolIndex == ou32_DataPoolIndex)) &&
             (rc_DataElementConfig.c_ElementId.u32_ListIndex == ou32_ListIndex))
         {
            //Should be synchronized
            tgl_assert(u32_ItElement < rc_ParamWidget.c_DataSetSelectionIndices.size());
            if (u32_ItElement < rc_ParamWidget.c_DataSetSelectionIndices.size())
            {
               sint32 & rs32_DataSetIndex = rc_ParamWidget.c_DataSetSelectionIndices[u32_ItElement];
               if (rs32_DataSetIndex >= 0)
               {
                  const uint32 u32_DataSetIndex = static_cast<uint32>(rs32_DataSetIndex);
                  if (u32_DataSetIndex == ou32_DataSetIndex)
                  {
                     //Reset to no selection
                     rs32_DataSetIndex = -1;
                  }
                  else if (u32_DataSetIndex > ou32_DataSetIndex)
                  {
                     --rs32_DataSetIndex;
                  }
                  else
                  {
                     //All fine
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_ElementIndex    Element index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::OnSyncNodeDataPoolListElementAdded(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                          const uint32 ou32_ListIndex, const uint32 ou32_ElementIndex)
{
   std::vector<C_PuiSvDbWidgetBase *> c_Widgets;
   m_GetAllWidgetItems(c_Widgets);
   for (uint32 u32_ItWidget = 0; u32_ItWidget < c_Widgets.size(); ++u32_ItWidget)
   {
      C_PuiSvDbWidgetBase * const pc_Widget = c_Widgets[u32_ItWidget];
      C_PuiSvDbParam * const pc_Param = dynamic_cast<C_PuiSvDbParam * const>(c_Widgets[u32_ItWidget]);
      if (pc_Widget != NULL)
      {
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_Widget->c_DataPoolElementsConfig.size(); ++u32_ItElement)
         {
            C_PuiSvDbNodeDataElementConfig & rc_DataElementConfig = pc_Widget->c_DataPoolElementsConfig[u32_ItElement];
            C_PuiSvDbNodeDataPoolListElementId & rc_DataElementId = rc_DataElementConfig.c_ElementId;
            h_OnSyncNodeDataPoolListElementAdded(rc_DataElementId, ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                 ou32_ElementIndex);
         }
      }
      if (pc_Param != NULL)
      {
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_Param->c_ExpandedItems.size(); ++u32_ItElement)
         {
            C_PuiSvDbExpandedTreeIndex & rc_DataElementConfig = pc_Param->c_ExpandedItems[u32_ItElement];
            h_OnSyncNodeDataPoolListElementAdded(rc_DataElementConfig.c_ExpandedId, ou32_NodeIndex, ou32_DataPoolIndex,
                                                 ou32_ListIndex, ou32_ElementIndex);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex             Node index
   \param[in]  ou32_DataPoolIndex         Data pool index
   \param[in]  ou32_ListIndex             List index
   \param[in]  ou32_ElementSourceIndex    Source element index
   \param[in]  ou32_ElementTargetIndex    Target element index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::OnSyncNodeDataPoolListElementMoved(const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                          const uint32 ou32_ListIndex,
                                                          const uint32 ou32_ElementSourceIndex,
                                                          const uint32 ou32_ElementTargetIndex)
{
   std::vector<C_PuiSvDbWidgetBase *> c_Widgets;
   m_GetAllWidgetItems(c_Widgets);
   for (uint32 u32_ItWidget = 0; u32_ItWidget < c_Widgets.size(); ++u32_ItWidget)
   {
      C_PuiSvDbWidgetBase * const pc_Widget = c_Widgets[u32_ItWidget];
      C_PuiSvDbParam * const pc_Param = dynamic_cast<C_PuiSvDbParam * const>(c_Widgets[u32_ItWidget]);
      if (pc_Widget != NULL)
      {
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_Widget->c_DataPoolElementsConfig.size(); ++u32_ItElement)
         {
            C_PuiSvDbNodeDataElementConfig & rc_DataElementConfig = pc_Widget->c_DataPoolElementsConfig[u32_ItElement];
            C_PuiSvDbNodeDataPoolListElementId & rc_DataElementId = rc_DataElementConfig.c_ElementId;
            h_OnSyncNodeDataPoolListElementMoved(rc_DataElementId, ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                 ou32_ElementSourceIndex, ou32_ElementTargetIndex);
         }
      }
      if (pc_Param != NULL)
      {
         //Param widget
         C_PuiSvDbNodeDataElementConfig c_Config;
         C_OSCNodeDataPoolContent c_Value;
         sint32 s32_Dataset;
         bool q_Found = false;
         //Move moved item to better position
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_Param->c_DataPoolElementsConfig.size(); ++u32_ItElement)
         {
            const C_PuiSvDbNodeDataElementConfig & rc_DataElementConfig =
               pc_Param->c_DataPoolElementsConfig[u32_ItElement];
            const C_PuiSvDbNodeDataPoolListElementId & rc_DataElementId = rc_DataElementConfig.c_ElementId;
            if ((((rc_DataElementId.u32_NodeIndex == ou32_NodeIndex) &&
                  (rc_DataElementId.u32_DataPoolIndex == ou32_DataPoolIndex)) &&
                 (rc_DataElementId.u32_ListIndex == ou32_ListIndex)) &&
                (rc_DataElementId.u32_ElementIndex == ou32_ElementTargetIndex))
            {
               //Copy
               c_Config = rc_DataElementConfig;
               c_Value = pc_Param->c_ListValues[u32_ItElement];
               s32_Dataset = pc_Param->c_DataSetSelectionIndices[u32_ItElement];
               //Remove
               pc_Param->c_DataPoolElementsConfig.erase(pc_Param->c_DataPoolElementsConfig.begin() + u32_ItElement);
               pc_Param->c_ListValues.erase(pc_Param->c_ListValues.begin() + u32_ItElement);
               pc_Param->c_DataSetSelectionIndices.erase(pc_Param->c_DataSetSelectionIndices.begin() + u32_ItElement);
               //Signal found
               q_Found = true;
               break;
            }
         }
         //If this element was part of the parameter widget, insert it at a better place
         if (q_Found)
         {
            const C_OSCNodeDataPoolListElementId c_Id(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                      ou32_ElementTargetIndex);
            const uint32 u32_NewIndex = C_PuiSvDashboard::mh_GetBestIndexForNewItem(*pc_Param,
                                                                                    c_Id, NULL);
            pc_Param->c_DataPoolElementsConfig.insert(
               pc_Param->c_DataPoolElementsConfig.begin() + u32_NewIndex, c_Config);
            pc_Param->c_DataSetSelectionIndices.insert(
               pc_Param->c_DataSetSelectionIndices.begin() + u32_NewIndex, s32_Dataset);
            pc_Param->c_ListValues.insert(pc_Param->c_ListValues.begin() + u32_NewIndex, c_Value);
         }
         //Handle expanded indices
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_Param->c_ExpandedItems.size(); ++u32_ItElement)
         {
            C_PuiSvDbExpandedTreeIndex & rc_DataElementConfig = pc_Param->c_ExpandedItems[u32_ItElement];
            h_OnSyncNodeDataPoolListElementMoved(rc_DataElementConfig.c_ExpandedId, ou32_NodeIndex, ou32_DataPoolIndex,
                                                 ou32_ListIndex, ou32_ElementSourceIndex, ou32_ElementTargetIndex);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_ElementIndex    Element index
   \param[in]  oe_Type              New element type
   \param[in]  oq_IsArray           New array type
   \param[in]  ou32_ArraySize       New array size
   \param[in]  oq_IsString          Flag if new type is string

   \return
   True  Some elements invalidated
   False No elements were invalidated
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDashboard::OnSyncNodeDataPoolListElementArrayChanged(const uint32 ou32_NodeIndex,
                                                                 const uint32 ou32_DataPoolIndex,
                                                                 const uint32 ou32_ListIndex,
                                                                 const uint32 ou32_ElementIndex,
                                                                 const C_OSCNodeDataPoolContent::E_Type oe_Type,
                                                                 const bool oq_IsArray, const uint32 ou32_ArraySize,
                                                                 const bool oq_IsString)
{
   bool q_Retval = false;

   std::vector<C_PuiSvDbWidgetBase *> c_Widgets;
   m_GetAllWidgetItems(c_Widgets);

   for (uint32 u32_ItWidget = 0; u32_ItWidget < c_Widgets.size(); ++u32_ItWidget)
   {
      C_PuiSvDbWidgetBase * const pc_Widget = c_Widgets[u32_ItWidget];
      if (pc_Widget != NULL)
      {
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_Widget->c_DataPoolElementsConfig.size();
              ++u32_ItElement)
         {
            C_PuiSvDbNodeDataElementConfig & rc_DataElementConfig =
               pc_Widget->c_DataPoolElementsConfig[u32_ItElement];
            C_PuiSvDbNodeDataPoolListElementId & rc_DataElementId = rc_DataElementConfig.c_ElementId;
            //Check if this is the same data element
            if ((rc_DataElementId.CheckSameDataElement(
                    C_PuiSvDbNodeDataPoolListElementId(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                       ou32_ElementIndex,
                                                       C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT,
                                                       false,
                                                       0UL))) ||
                (rc_DataElementId.CheckSameDataElement(
                    C_PuiSvDbNodeDataPoolListElementId(
                       ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                       ou32_ElementIndex, C_PuiSvDbNodeDataPoolListElementId::eBUS_SIGNAL, false, 0UL))))
            {
               //Only critical if element was turned into an array
               if (oq_IsArray)
               {
                  //Skip widgets which support array type
                  if ((C_PuiSvDashboard::h_GetWidgetType(pc_Widget) != C_PuiSvDbDataElement::eTABLE) &&
                      (C_PuiSvDashboard::h_GetWidgetType(pc_Widget) != C_PuiSvDbDataElement::ePARAM))
                  {
                     mh_MarkInvalid(rc_DataElementId);
                     q_Retval = true;
                  }
               }
               //Handle indices
               if (rc_DataElementId.GetUseArrayElementIndex())
               {
                  //Char elements not supported
                  if (oq_IsString)
                  {
                     mh_MarkInvalid(rc_DataElementId);
                     q_Retval = true;
                  }
                  else
                  {
                     //Array element index can no longer be used
                     if (oq_IsArray == false)
                     {
                        mh_MarkInvalid(rc_DataElementId);
                        q_Retval = true;
                     }
                     else
                     {
                        //Check index out of range
                        if (ou32_ArraySize <= rc_DataElementId.GetArrayElementIndex())
                        {
                           mh_MarkInvalid(rc_DataElementId);
                           q_Retval = true;
                        }
                     }
                  }
               }
            }
         }
      }
   }
   //Probably not necessary
   m_SyncCleanUpParams();

   //For param widgets we need to sync the array size
   for (uint32 u32_ItWidget = 0; u32_ItWidget < c_Widgets.size(); ++u32_ItWidget)
   {
      C_PuiSvDbWidgetBase * const pc_Widget = c_Widgets[u32_ItWidget];
      if (pc_Widget != NULL)
      {
         C_PuiSvDbParam * const pc_ParamWidgets = dynamic_cast<C_PuiSvDbParam * const>(pc_Widget);
         if (pc_ParamWidgets != NULL)
         {
            for (uint32 u32_ItElement = 0; u32_ItElement < pc_ParamWidgets->c_DataPoolElementsConfig.size();
                 ++u32_ItElement)
            {
               const C_PuiSvDbNodeDataElementConfig & rc_DataElementConfig =
                  pc_ParamWidgets->c_DataPoolElementsConfig[u32_ItElement];
               const C_PuiSvDbNodeDataPoolListElementId & rc_DataElementId = rc_DataElementConfig.c_ElementId;
               if (rc_DataElementId ==
                   C_PuiSvDbNodeDataPoolListElementId(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                      ou32_ElementIndex,
                                                      C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT, false,
                                                      0UL))
               {
                  C_OSCNodeDataPoolContent & rc_CurElement = pc_ParamWidgets->c_ListValues[u32_ItElement];

                  rc_CurElement.SetType(oe_Type);
                  rc_CurElement.SetArray(oq_IsArray);
                  if (oq_IsArray == true)
                  {
                     rc_CurElement.SetArraySize(ou32_ArraySize);
                  }
               }
            }
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_ElementIndex    Element index
   \param[in]  oe_Access            New access type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::OnSyncNodeDataPoolListElementAccessChanged(const uint32 ou32_NodeIndex,
                                                                  const uint32 ou32_DataPoolIndex,
                                                                  const uint32 ou32_ListIndex,
                                                                  const uint32 ou32_ElementIndex,
                                                                  const C_OSCNodeDataPoolListElement::E_Access oe_Access)
{
   std::vector<C_PuiSvDbWidgetBase *> c_Widgets;
   m_GetAllWidgetItems(c_Widgets);

   //Only critical if RO
   if (oe_Access == C_OSCNodeDataPoolListElement::eACCESS_RO)
   {
      for (uint32 u32_ItWidget = 0; u32_ItWidget < c_Widgets.size(); ++u32_ItWidget)
      {
         C_PuiSvDbWidgetBase * const pc_Widget = c_Widgets[u32_ItWidget];
         if (pc_Widget != NULL)
         {
            if (pc_Widget->IsReadElement() == false)
            {
               for (uint32 u32_ItElement = 0; u32_ItElement < pc_Widget->c_DataPoolElementsConfig.size();
                    ++u32_ItElement)
               {
                  C_PuiSvDbNodeDataElementConfig & rc_DataElementConfig =
                     pc_Widget->c_DataPoolElementsConfig[u32_ItElement];
                  C_PuiSvDbNodeDataPoolListElementId & rc_DataElementId = rc_DataElementConfig.c_ElementId;
                  if ((rc_DataElementId ==
                       C_PuiSvDbNodeDataPoolListElementId(ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                                                          ou32_ElementIndex,
                                                          C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT, false,
                                                          0UL)) ||
                      (rc_DataElementId ==
                       C_PuiSvDbNodeDataPoolListElementId(
                          ou32_NodeIndex, ou32_DataPoolIndex, ou32_ListIndex,
                          ou32_ElementIndex, C_PuiSvDbNodeDataPoolListElementId::eBUS_SIGNAL, false, 0UL)))
                  {
                     mh_MarkInvalid(rc_DataElementId);
                  }
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DataPoolIndex   Data pool index
   \param[in]  ou32_ListIndex       List index
   \param[in]  ou32_ElementIndex    Element index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::OnSyncNodeDataPoolListElementAboutToBeDeleted(const uint32 ou32_NodeIndex,
                                                                     const uint32 ou32_DataPoolIndex,
                                                                     const uint32 ou32_ListIndex,
                                                                     const uint32 ou32_ElementIndex)
{
   std::vector<C_PuiSvDbWidgetBase *> c_Widgets;
   m_GetAllWidgetItems(c_Widgets);
   for (uint32 u32_ItWidget = 0; u32_ItWidget < c_Widgets.size(); ++u32_ItWidget)
   {
      C_PuiSvDbWidgetBase * const pc_Widget = c_Widgets[u32_ItWidget];
      C_PuiSvDbParam * const pc_Param = dynamic_cast<C_PuiSvDbParam * const>(c_Widgets[u32_ItWidget]);
      if (pc_Widget != NULL)
      {
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_Widget->c_DataPoolElementsConfig.size(); ++u32_ItElement)
         {
            C_PuiSvDbNodeDataElementConfig & rc_DataElementConfig = pc_Widget->c_DataPoolElementsConfig[u32_ItElement];
            C_PuiSvDbNodeDataPoolListElementId & rc_DataElementId = rc_DataElementConfig.c_ElementId;
            h_OnSyncNodeDataPoolListElementAboutToBeDeleted(rc_DataElementId, ou32_NodeIndex, ou32_DataPoolIndex,
                                                            ou32_ListIndex, ou32_ElementIndex);
         }
      }
      if (pc_Param != NULL)
      {
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_Param->c_ExpandedItems.size(); ++u32_ItElement)
         {
            C_PuiSvDbExpandedTreeIndex & rc_DataElementConfig = pc_Param->c_ExpandedItems[u32_ItElement];
            h_OnSyncNodeDataPoolListElementAboutToBeDeleted(rc_DataElementConfig.c_ExpandedId, ou32_NodeIndex,
                                                            ou32_DataPoolIndex, ou32_ListIndex, ou32_ElementIndex);
         }
      }
   }
   m_SyncCleanUpParams();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in,out]  orc_DataElementId   Data element ID
   \param[in]      ou32_Index          Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::h_OnSyncNodeAdded(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                         const uint32 ou32_Index)
{
   if (orc_DataElementId.GetIsValid() == true)
   {
      if (orc_DataElementId.u32_NodeIndex >= ou32_Index)
      {
         ++orc_DataElementId.u32_NodeIndex;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Adapt to system definition change

   \param[in,out]  orc_DataElementId   Data element id
   \param[in]      ou32_Index          Index
   \param[in]      orc_MapCurToNew     Map cur to new
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::h_OnSyncNodeHALC(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId, const uint32 ou32_Index,
                                        const std::map<C_PuiSvDbNodeDataPoolListElementId,
                                                       C_PuiSvDbNodeDataPoolListElementId> & orc_MapCurToNew)
{
   if (orc_DataElementId.GetIsValid() == true)
   {
      if (orc_DataElementId.u32_NodeIndex == ou32_Index)
      {
         const std::map<C_PuiSvDbNodeDataPoolListElementId,
                        C_PuiSvDbNodeDataPoolListElementId>::const_iterator c_It = orc_MapCurToNew.find(
            orc_DataElementId);
         if (c_It != orc_MapCurToNew.end())
         {
            //Replace
            orc_DataElementId = c_It->second;
         }
         else
         {
            if (C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(orc_DataElementId.u32_NodeIndex,
                                                                orc_DataElementId.u32_DataPoolIndex) != NULL)
            {
               C_OSCNodeDataPool::E_Type e_Type;
               if (C_PuiSdHandler::h_GetInstance()->GetDataPoolType(ou32_Index, orc_DataElementId.u32_DataPoolIndex,
                                                                    e_Type) == C_NO_ERR)
               {
                  if (e_Type == C_OSCNodeDataPool::eHALC)
                  {
                     //Delete
                     orc_DataElementId.MarkInvalid(C_OSCNodeDataPool::eHALC,
                                                   C_GtGetText::h_GetText("Unknown HAL data element"));
                  }
               }
            }
            else
            {
               //Delete
               orc_DataElementId.MarkInvalid(C_OSCNodeDataPool::eHALC,
                                             C_GtGetText::h_GetText("Unknown HAL data element"));
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in,out]  orc_DataElementId   Data element ID
   \param[in]      ou32_Index          Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::h_OnSyncNodeAboutToBeDeleted(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                                    const uint32 ou32_Index)
{
   if (orc_DataElementId.GetIsValid() == true)
   {
      if (orc_DataElementId.u32_NodeIndex > ou32_Index)
      {
         --orc_DataElementId.u32_NodeIndex;
      }
      else if (orc_DataElementId.u32_NodeIndex == ou32_Index)
      {
         mh_MarkInvalid(orc_DataElementId);
      }
      else
      {
         //No adaptation necessary
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in,out]  orc_DataElementId   Data element ID
   \param[in]      ou32_NodeIndex      Node index
   \param[in]      ou32_DataPoolIndex  Data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::h_OnSyncNodeDataPoolAdded(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                                 const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex)
{
   if (orc_DataElementId.GetIsValid() == true)
   {
      if (orc_DataElementId.u32_NodeIndex == ou32_NodeIndex)
      {
         if (orc_DataElementId.u32_DataPoolIndex >= ou32_DataPoolIndex)
         {
            ++orc_DataElementId.u32_DataPoolIndex;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in,out]  orc_DataElementId         Data element ID
   \param[in]      ou32_NodeIndex            Node index
   \param[in]      ou32_DataPoolSourceIndex  Source data pool index
   \param[in]      ou32_DataPoolTargetIndex  Target data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::h_OnSyncNodeDataPoolMoved(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                                 const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolSourceIndex,
                                                 const uint32 ou32_DataPoolTargetIndex)
{
   if (orc_DataElementId.GetIsValid() == true)
   {
      if (orc_DataElementId.u32_NodeIndex == ou32_NodeIndex)
      {
         if (orc_DataElementId.u32_DataPoolIndex > ou32_DataPoolSourceIndex)
         {
            --orc_DataElementId.u32_DataPoolIndex;
            if (orc_DataElementId.u32_DataPoolIndex >= ou32_DataPoolTargetIndex)
            {
               ++orc_DataElementId.u32_DataPoolIndex;
            }
            else
            {
               //No adaptation necessary
            }
         }
         else if (orc_DataElementId.u32_DataPoolIndex == ou32_DataPoolSourceIndex)
         {
            orc_DataElementId.u32_DataPoolIndex = ou32_DataPoolTargetIndex;
         }
         else
         {
            if (orc_DataElementId.u32_DataPoolIndex >= ou32_DataPoolTargetIndex)
            {
               ++orc_DataElementId.u32_DataPoolIndex;
            }
            else
            {
               //No adaptation necessary
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in,out]  orc_DataElementId   Data element ID
   \param[in]      ou32_NodeIndex      Node index
   \param[in]      ou32_DataPoolIndex  Data pool index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::h_OnSyncNodeDataPoolAboutToBeDeleted(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                                            const uint32 ou32_NodeIndex,
                                                            const uint32 ou32_DataPoolIndex)
{
   if (orc_DataElementId.GetIsValid() == true)
   {
      if (orc_DataElementId.u32_NodeIndex == ou32_NodeIndex)
      {
         if (orc_DataElementId.u32_DataPoolIndex > ou32_DataPoolIndex)
         {
            --orc_DataElementId.u32_DataPoolIndex;
         }
         else if (orc_DataElementId.u32_DataPoolIndex == ou32_DataPoolIndex)
         {
            mh_MarkInvalid(orc_DataElementId);
         }
         else
         {
            //No adaptation necessary
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in,out]  orc_DataElementId   Data element ID
   \param[in]      ou32_NodeIndex      Node index
   \param[in]      ou32_DataPoolIndex  Data pool index
   \param[in]      ou32_ListIndex      List index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::h_OnSyncNodeDataPoolListAdded(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                                     const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                     const uint32 ou32_ListIndex)
{
   if (orc_DataElementId.GetIsValid() == true)
   {
      if ((orc_DataElementId.u32_NodeIndex == ou32_NodeIndex) &&
          (orc_DataElementId.u32_DataPoolIndex == ou32_DataPoolIndex))
      {
         if (orc_DataElementId.u32_ListIndex >= ou32_ListIndex)
         {
            ++orc_DataElementId.u32_ListIndex;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in,out]  orc_DataElementId      Data element ID
   \param[in]      ou32_NodeIndex         Node index
   \param[in]      ou32_DataPoolIndex     Data pool index
   \param[in]      ou32_ListSourceIndex   Source list index
   \param[in]      ou32_ListTargetIndex   Target list index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::h_OnSyncNodeDataPoolListMoved(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                                     const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
                                                     const uint32 ou32_ListSourceIndex,
                                                     const uint32 ou32_ListTargetIndex)
{
   if (orc_DataElementId.GetIsValid() == true)
   {
      if ((orc_DataElementId.u32_NodeIndex == ou32_NodeIndex) &&
          (orc_DataElementId.u32_DataPoolIndex == ou32_DataPoolIndex))
      {
         if (orc_DataElementId.u32_ListIndex > ou32_ListSourceIndex)
         {
            --orc_DataElementId.u32_ListIndex;
            if (orc_DataElementId.u32_ListIndex >= ou32_ListTargetIndex)
            {
               ++orc_DataElementId.u32_ListIndex;
            }
            else
            {
               //No adaptation necessary
            }
         }
         else if (orc_DataElementId.u32_ListIndex == ou32_ListSourceIndex)
         {
            orc_DataElementId.u32_ListIndex = ou32_ListTargetIndex;
         }
         else
         {
            if (orc_DataElementId.u32_ListIndex >= ou32_ListTargetIndex)
            {
               ++orc_DataElementId.u32_ListIndex;
            }
            else
            {
               //No adaptation necessary
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in,out]  orc_DataElementId   Data element ID
   \param[in]      ou32_NodeIndex      Node index
   \param[in]      ou32_DataPoolIndex  Data pool index
   \param[in]      ou32_ListIndex      List index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::h_OnSyncNodeDataPoolListAboutToBeDeleted(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                                                const uint32 ou32_NodeIndex,
                                                                const uint32 ou32_DataPoolIndex,
                                                                const uint32 ou32_ListIndex)
{
   if (orc_DataElementId.GetIsValid() == true)
   {
      if ((orc_DataElementId.u32_NodeIndex == ou32_NodeIndex) &&
          (orc_DataElementId.u32_DataPoolIndex == ou32_DataPoolIndex))
      {
         if (orc_DataElementId.u32_ListIndex > ou32_ListIndex)
         {
            --orc_DataElementId.u32_ListIndex;
         }
         else if (orc_DataElementId.u32_ListIndex == ou32_ListIndex)
         {
            mh_MarkInvalid(orc_DataElementId);
         }
         else
         {
            //No adaptation necessary
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in,out]  orc_DataElementId   Data element ID
   \param[in]      ou32_NodeIndex      Node index
   \param[in]      ou32_DataPoolIndex  Data pool index
   \param[in]      ou32_ListIndex      List index
   \param[in]      ou32_ElementIndex   Element index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::h_OnSyncNodeDataPoolListElementAdded(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                                            const uint32 ou32_NodeIndex,
                                                            const uint32 ou32_DataPoolIndex,
                                                            const uint32 ou32_ListIndex, const uint32 ou32_ElementIndex)
{
   if (orc_DataElementId.GetIsValid() == true)
   {
      if (((orc_DataElementId.u32_NodeIndex == ou32_NodeIndex) &&
           (orc_DataElementId.u32_DataPoolIndex == ou32_DataPoolIndex)) &&
          (orc_DataElementId.u32_ListIndex == ou32_ListIndex))
      {
         if (orc_DataElementId.u32_ElementIndex >= ou32_ElementIndex)
         {
            ++orc_DataElementId.u32_ElementIndex;
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in,out]  orc_DataElementId         Data element ID
   \param[in]      ou32_NodeIndex            Node index
   \param[in]      ou32_DataPoolIndex        Data pool index
   \param[in]      ou32_ListIndex            List index
   \param[in]      ou32_ElementSourceIndex   Source element index
   \param[in]      ou32_ElementTargetIndex   Target element index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::h_OnSyncNodeDataPoolListElementMoved(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId,
                                                            const uint32 ou32_NodeIndex,
                                                            const uint32 ou32_DataPoolIndex,
                                                            const uint32 ou32_ListIndex,
                                                            const uint32 ou32_ElementSourceIndex,
                                                            const uint32 ou32_ElementTargetIndex)
{
   if (orc_DataElementId.GetIsValid() == true)
   {
      if (((orc_DataElementId.u32_NodeIndex == ou32_NodeIndex) &&
           (orc_DataElementId.u32_DataPoolIndex == ou32_DataPoolIndex)) &&
          (orc_DataElementId.u32_ListIndex == ou32_ListIndex))
      {
         if (orc_DataElementId.u32_ElementIndex > ou32_ElementSourceIndex)
         {
            --orc_DataElementId.u32_ElementIndex;
            if (orc_DataElementId.u32_ElementIndex >= ou32_ElementTargetIndex)
            {
               ++orc_DataElementId.u32_ElementIndex;
            }
            else
            {
               //No adaptation necessary
            }
         }
         else if (orc_DataElementId.u32_ElementIndex == ou32_ElementSourceIndex)
         {
            orc_DataElementId.u32_ElementIndex = ou32_ElementTargetIndex;
         }
         else
         {
            if (orc_DataElementId.u32_ElementIndex >= ou32_ElementTargetIndex)
            {
               ++orc_DataElementId.u32_ElementIndex;
            }
            else
            {
               //No adaptation necessary
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt to system definition change

   \param[in,out]  orc_DataElementId   Data element ID
   \param[in]      ou32_NodeIndex      Node index
   \param[in]      ou32_DataPoolIndex  Data pool index
   \param[in]      ou32_ListIndex      List index
   \param[in]      ou32_ElementIndex   Element index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::h_OnSyncNodeDataPoolListElementAboutToBeDeleted(
   C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId, const uint32 ou32_NodeIndex, const uint32 ou32_DataPoolIndex,
   const uint32 ou32_ListIndex, const uint32 ou32_ElementIndex)
{
   if (orc_DataElementId.GetIsValid() == true)
   {
      if (((orc_DataElementId.u32_NodeIndex == ou32_NodeIndex) &&
           (orc_DataElementId.u32_DataPoolIndex == ou32_DataPoolIndex)) &&
          (orc_DataElementId.u32_ListIndex == ou32_ListIndex))
      {
         if (orc_DataElementId.u32_ElementIndex > ou32_ElementIndex)
         {
            --orc_DataElementId.u32_ElementIndex;
         }
         else if (orc_DataElementId.u32_ElementIndex == ou32_ElementIndex)
         {
            mh_MarkInvalid(orc_DataElementId);
         }
         else
         {
            //No adaptation necessary
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Synchronise internally stored scaling information with current system definition
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::SyncScalingInformation(void)
{
   std::vector<C_PuiSvDbWidgetBase *> c_Widgets;
   this->m_GetAllWidgetItems(c_Widgets);
   for (uint32 u32_ItWidget = 0; u32_ItWidget < c_Widgets.size(); ++u32_ItWidget)
   {
      C_PuiSvDbWidgetBase * const pc_Widget = c_Widgets[u32_ItWidget];
      if (pc_Widget != NULL)
      {
         for (uint32 u32_ItData = 0; u32_ItData < pc_Widget->c_DataPoolElementsConfig.size(); ++u32_ItData)
         {
            C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Widget->c_DataPoolElementsConfig[u32_ItData];
            if ((rc_Config.c_ElementId.GetIsValid() == true) && (rc_Config.c_ElementScaling.q_UseDefault == true))
            {
               const C_OSCNodeDataPoolListElement * const pc_Element =
                  C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(rc_Config.c_ElementId.u32_NodeIndex,
                                                                             rc_Config.c_ElementId.u32_DataPoolIndex,
                                                                             rc_Config.c_ElementId.u32_ListIndex,
                                                                             rc_Config.c_ElementId.u32_ElementIndex);
               //Check with system definition
               if (pc_Element != NULL)
               {
                  rc_Config.c_ElementScaling.c_Unit = pc_Element->c_Unit.c_str();
                  rc_Config.c_ElementScaling.f64_Factor = pc_Element->f64_Factor;
                  rc_Config.c_ElementScaling.f64_Offset = pc_Element->f64_Offset;
               }
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add widget to view dashboard

   \param[in]  opc_Box  New widget value
   \param[in]  oe_Type  New widget type

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvDashboard::AddWidget(const C_PuiSvDbWidgetBase * const opc_Box,
                                   const C_PuiSvDbDataElement::E_Type oe_Type)
{
   sint32 s32_Retval = C_NO_ERR;

   switch (oe_Type)
   {
   case C_PuiSvDbDataElement::eLABEL:
      s32_Retval = this->InsertWidget(this->mc_Labels.size(), opc_Box, oe_Type);
      break;
   case C_PuiSvDbDataElement::ePARAM:
      s32_Retval = this->InsertWidget(this->mc_ParamWidgets.size(), opc_Box, oe_Type);
      break;
   case C_PuiSvDbDataElement::ePIE_CHART:
      s32_Retval = this->InsertWidget(this->mc_PieCharts.size(), opc_Box, oe_Type);
      break;
   case C_PuiSvDbDataElement::ePROGRESS_BAR:
      s32_Retval = this->InsertWidget(this->mc_ProgressBars.size(), opc_Box, oe_Type);
      break;
   case C_PuiSvDbDataElement::eSLIDER:
      s32_Retval = this->InsertWidget(this->mc_Sliders.size(), opc_Box, oe_Type);
      break;
   case C_PuiSvDbDataElement::eSPIN_BOX:
      s32_Retval = this->InsertWidget(this->mc_SpinBoxes.size(), opc_Box, oe_Type);
      break;
   case C_PuiSvDbDataElement::eTABLE:
      s32_Retval = this->InsertWidget(this->mc_Tables.size(), opc_Box, oe_Type);
      break;
   case C_PuiSvDbDataElement::eTOGGLE:
      s32_Retval = this->InsertWidget(this->mc_Toggles.size(), opc_Box, oe_Type);
      break;
   case C_PuiSvDbDataElement::eTAB_CHART:
      s32_Retval = this->InsertWidget(0UL, opc_Box, oe_Type);
      break;
   case C_PuiSvDbDataElement::eCHART: // adding deprecated chart is forbidden
   case C_PuiSvDbDataElement::eLINE_ARROW:
   case C_PuiSvDbDataElement::eBOUNDARY:
   case C_PuiSvDbDataElement::eTEXT_ELEMENT:
   case C_PuiSvDbDataElement::eIMAGE:
   case C_PuiSvDbDataElement::eUNKNOWN:
   default:
      s32_Retval = C_RANGE;
      break;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Insert widget to view dashboard

   \param[in]  ou32_WidgetIndex  Widget index
   \param[in]  opc_Box           New widget value
   \param[in]  oe_Type           New widget type

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvDashboard::InsertWidget(const uint32 ou32_WidgetIndex, const C_PuiSvDbWidgetBase * const opc_Box,
                                      const C_PuiSvDbDataElement::E_Type oe_Type)
{
   sint32 s32_Retval = C_NO_ERR;
   const C_PuiSvDbTabChart * opc_TabChart;
   const C_PuiSvDbLabel * opc_Label;
   const C_PuiSvDbParam * opc_ParamWidget;
   const C_PuiSvDbPieChart * opc_PieChart;
   const C_PuiSvDbProgressBar * opc_ProgressBar;
   const C_PuiSvDbSlider * opc_Slider;
   const C_PuiSvDbSpinBox * opc_SpinBox;
   const C_PuiSvDbTable * opc_Table;
   const C_PuiSvDbToggle * opc_Toggle;

   switch (oe_Type)
   {
   case C_PuiSvDbDataElement::eLABEL:
      opc_Label = dynamic_cast<const C_PuiSvDbLabel *>(opc_Box);
      if (opc_Label != NULL)
      {
         if (ou32_WidgetIndex <= this->mc_Labels.size())
         {
            this->mc_Labels.insert(this->mc_Labels.begin() + ou32_WidgetIndex, *opc_Label);
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      break;
   case C_PuiSvDbDataElement::ePARAM:
      opc_ParamWidget = dynamic_cast<const C_PuiSvDbParam *>(opc_Box);
      if (opc_ParamWidget != NULL)
      {
         if (ou32_WidgetIndex <= this->mc_ParamWidgets.size())
         {
            this->mc_ParamWidgets.insert(this->mc_ParamWidgets.begin() + ou32_WidgetIndex, *opc_ParamWidget);
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      break;
   case C_PuiSvDbDataElement::ePIE_CHART:
      opc_PieChart = dynamic_cast<const C_PuiSvDbPieChart *>(opc_Box);
      if (opc_PieChart != NULL)
      {
         if (ou32_WidgetIndex <= this->mc_PieCharts.size())
         {
            this->mc_PieCharts.insert(this->mc_PieCharts.begin() + ou32_WidgetIndex, *opc_PieChart);
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      break;
   case C_PuiSvDbDataElement::ePROGRESS_BAR:
      opc_ProgressBar = dynamic_cast<const C_PuiSvDbProgressBar *>(opc_Box);
      if (opc_ProgressBar != NULL)
      {
         if (ou32_WidgetIndex <= this->mc_ProgressBars.size())
         {
            this->mc_ProgressBars.insert(this->mc_ProgressBars.begin() + ou32_WidgetIndex, *opc_ProgressBar);
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      break;
   case C_PuiSvDbDataElement::eSLIDER:
      opc_Slider = dynamic_cast<const C_PuiSvDbSlider *>(opc_Box);
      if (opc_Slider != NULL)
      {
         if (ou32_WidgetIndex <= this->mc_Sliders.size())
         {
            this->mc_Sliders.insert(this->mc_Sliders.begin() + ou32_WidgetIndex, *opc_Slider);
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      break;
   case C_PuiSvDbDataElement::eSPIN_BOX:
      opc_SpinBox = dynamic_cast<const C_PuiSvDbSpinBox *>(opc_Box);
      if (opc_SpinBox != NULL)
      {
         if (ou32_WidgetIndex <= this->mc_SpinBoxes.size())
         {
            this->mc_SpinBoxes.insert(this->mc_SpinBoxes.begin() + ou32_WidgetIndex, *opc_SpinBox);
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      break;
   case C_PuiSvDbDataElement::eTABLE:
      opc_Table = dynamic_cast<const C_PuiSvDbTable *>(opc_Box);
      if (opc_Table != NULL)
      {
         if (ou32_WidgetIndex <= this->mc_Tables.size())
         {
            this->mc_Tables.insert(this->mc_Tables.begin() + ou32_WidgetIndex, *opc_Table);
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      break;
   case C_PuiSvDbDataElement::eTOGGLE:
      opc_Toggle = dynamic_cast<const C_PuiSvDbToggle *>(opc_Box);
      if (opc_Toggle != NULL)
      {
         if (ou32_WidgetIndex <= this->mc_Toggles.size())
         {
            this->mc_Toggles.insert(this->mc_Toggles.begin() + ou32_WidgetIndex, *opc_Toggle);
         }
         else
         {
            s32_Retval = C_RANGE;
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      break;
   case C_PuiSvDbDataElement::eTAB_CHART:
      opc_TabChart = dynamic_cast<const C_PuiSvDbTabChart *>(opc_Box);
      if (opc_TabChart != NULL)
      {
         this->mc_TabChart = *opc_TabChart;
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      break;
   case C_PuiSvDbDataElement::eCHART: // inserting deprecated chart is forbidden
   case C_PuiSvDbDataElement::eLINE_ARROW:
   case C_PuiSvDbDataElement::eBOUNDARY:
   case C_PuiSvDbDataElement::eTEXT_ELEMENT:
   case C_PuiSvDbDataElement::eIMAGE:
   case C_PuiSvDbDataElement::eUNKNOWN:
   default:
      s32_Retval = C_RANGE;
      break;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Param widget clear all data pool elements

   \param[in]  ou32_ParamWidgetIndex   Param index

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvDashboard::ClearParamDataPoolElements(const uint32 ou32_ParamWidgetIndex)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ParamWidgetIndex < this->mc_ParamWidgets.size())
   {
      C_PuiSvDbParam & rc_ParamWidget = this->mc_ParamWidgets[ou32_ParamWidgetIndex];
      rc_ParamWidget.c_DataPoolElementsConfig.clear();
      rc_ParamWidget.c_DataSetSelectionIndices.clear();
      rc_ParamWidget.c_ListValues.clear();
      rc_ParamWidget.c_ColWidth.clear();
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Param widget add new data pool element

   \param[in]  ou32_ParamWidgetIndex   Param index
   \param[in]  orc_NewId               New ID
   \param[in]  opc_Content             Optional init value

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvDashboard::AddParamNewDataPoolElement(const uint32 ou32_ParamWidgetIndex,
                                                    const C_OSCNodeDataPoolListElementId & orc_NewId,
                                                    const C_OSCNodeDataPoolContent * const opc_Content)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_ParamWidgetIndex < this->mc_ParamWidgets.size())
   {
      const C_OSCNodeDataPoolListElement * const pc_Element =
         C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(orc_NewId.u32_NodeIndex,
                                                                    orc_NewId.u32_DataPoolIndex,
                                                                    orc_NewId.u32_ListIndex,
                                                                    orc_NewId.u32_ElementIndex);

      if (pc_Element != NULL)
      {
         C_PuiSvDbParam & rc_ParamWidget = this->mc_ParamWidgets[ou32_ParamWidgetIndex];
         C_PuiSvDbNodeDataElementConfig c_NewConfig;
         sint32 s32_SelectedDataset = -1;
         const uint32 u32_NewIndex = C_PuiSvDashboard::mh_GetBestIndexForNewItem(
            rc_ParamWidget, orc_NewId, &s32_SelectedDataset);
         c_NewConfig.c_ElementId = C_PuiSvDbNodeDataPoolListElementId(orc_NewId,
                                                                      C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT, false,
                                                                      0UL);
         //Add all necessary elements
         rc_ParamWidget.c_DataPoolElementsConfig.insert(
            rc_ParamWidget.c_DataPoolElementsConfig.begin() + u32_NewIndex, c_NewConfig);
         rc_ParamWidget.c_DataSetSelectionIndices.insert(
            rc_ParamWidget.c_DataSetSelectionIndices.begin() + u32_NewIndex, s32_SelectedDataset);
         if ((((opc_Content != NULL) && (opc_Content->GetType() == pc_Element->GetType())) &&
              (opc_Content->GetArray() == pc_Element->GetArray())) &&
             (opc_Content->GetArraySize() == pc_Element->GetArraySize()))
         {
            rc_ParamWidget.c_ListValues.insert(rc_ParamWidget.c_ListValues.begin() + u32_NewIndex, *opc_Content);
         }
         else
         {
            C_OSCNodeDataPoolContentUtil::E_ValueChangedTo e_Tmp;
            C_OSCNodeDataPoolContent c_Content;
            //Init content
            c_Content = pc_Element->c_MinValue;
            tgl_assert(C_OSCNodeDataPoolContentUtil::h_SetValueInMinMaxRange(
                          pc_Element->c_MinValue, pc_Element->c_MaxValue, c_Content, e_Tmp,
                          C_OSCNodeDataPoolContentUtil::eTO_ZERO) == C_NO_ERR);
            rc_ParamWidget.c_ListValues.insert(rc_ParamWidget.c_ListValues.begin() + u32_NewIndex, c_Content);
         }
         //Expand up to new item
         for (uint32 u32_ItRelevantLayer = 0UL; u32_ItRelevantLayer < 3UL; ++u32_ItRelevantLayer)
         {
            bool q_Found = false;
            for (std::vector<C_PuiSvDbExpandedTreeIndex>::const_iterator c_ExistingItems =
                    rc_ParamWidget.c_ExpandedItems.begin();
                 (c_ExistingItems != rc_ParamWidget.c_ExpandedItems.end()) && (q_Found == false); ++c_ExistingItems)
            {
               const C_PuiSvDbExpandedTreeIndex & rc_CurItem = *c_ExistingItems;
               if (rc_CurItem.u32_Layer == u32_ItRelevantLayer)
               {
                  switch (u32_ItRelevantLayer)
                  {
                  case 0UL:
                     q_Found = true;
                     break;
                  case 1UL:
                     if (rc_CurItem.c_ExpandedId.u32_NodeIndex == orc_NewId.u32_NodeIndex)
                     {
                        q_Found = true;
                     }
                     break;
                  case 2UL:
                     if ((rc_CurItem.c_ExpandedId.u32_NodeIndex == orc_NewId.u32_NodeIndex) &&
                         (rc_CurItem.c_ExpandedId.u32_DataPoolIndex == orc_NewId.u32_DataPoolIndex))
                     {
                        q_Found = true;
                     }
                     break;
                  default:
                     //Unexpected
                     break;
                  }
               }
            }
            if (q_Found == false)
            {
               //Append new item if not already existing
               C_PuiSvDbExpandedTreeIndex c_NewItem;
               c_NewItem.c_ExpandedId = C_PuiSvDbNodeDataPoolListElementId(orc_NewId,
                                                                           C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT, false,
                                                                           0UL);
               c_NewItem.u32_Layer = u32_ItRelevantLayer;
               rc_ParamWidget.c_ExpandedItems.push_back(c_NewItem);
            }
         }
      }
      else
      {
         s32_Retval = C_RANGE;
      }
   }
   else
   {
      s32_Retval = C_RANGE;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete widget from view dashboard

   \param[in]  ou32_WidgetIndex  Widget index
   \param[in]  oe_Type           New widget type

   \return
   C_NO_ERR Operation success
   C_RANGE  Operation failure: parameter invalid
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_PuiSvDashboard::DeleteWidget(const uint32 ou32_WidgetIndex, const C_PuiSvDbDataElement::E_Type oe_Type)
{
   sint32 s32_Retval = C_NO_ERR;

   switch (oe_Type)
   {
   case C_PuiSvDbDataElement::eCHART: // keep this for compatibility update in existing projects
      if (ou32_WidgetIndex < this->mc_Charts.size())
      {
         this->mc_Charts.erase(this->mc_Charts.begin() + ou32_WidgetIndex);
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      break;
   case C_PuiSvDbDataElement::eLABEL:
      if (ou32_WidgetIndex < this->mc_Labels.size())
      {
         this->mc_Labels.erase(this->mc_Labels.begin() + ou32_WidgetIndex);
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      break;
   case C_PuiSvDbDataElement::ePARAM:
      if (ou32_WidgetIndex < this->mc_ParamWidgets.size())
      {
         this->mc_ParamWidgets.erase(this->mc_ParamWidgets.begin() + ou32_WidgetIndex);
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      break;
   case C_PuiSvDbDataElement::ePIE_CHART:
      if (ou32_WidgetIndex < this->mc_PieCharts.size())
      {
         this->mc_PieCharts.erase(this->mc_PieCharts.begin() + ou32_WidgetIndex);
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      break;
   case C_PuiSvDbDataElement::ePROGRESS_BAR:
      if (ou32_WidgetIndex < this->mc_ProgressBars.size())
      {
         this->mc_ProgressBars.erase(this->mc_ProgressBars.begin() + ou32_WidgetIndex);
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      break;
   case C_PuiSvDbDataElement::eSLIDER:
      if (ou32_WidgetIndex < this->mc_Sliders.size())
      {
         this->mc_Sliders.erase(this->mc_Sliders.begin() + ou32_WidgetIndex);
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      break;
   case C_PuiSvDbDataElement::eSPIN_BOX:
      if (ou32_WidgetIndex < this->mc_SpinBoxes.size())
      {
         this->mc_SpinBoxes.erase(this->mc_SpinBoxes.begin() + ou32_WidgetIndex);
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      break;
   case C_PuiSvDbDataElement::eTABLE:
      if (ou32_WidgetIndex < this->mc_Tables.size())
      {
         this->mc_Tables.erase(this->mc_Tables.begin() + ou32_WidgetIndex);
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      break;
   case C_PuiSvDbDataElement::eTOGGLE:
      if (ou32_WidgetIndex < this->mc_Toggles.size())
      {
         this->mc_Toggles.erase(this->mc_Toggles.begin() + ou32_WidgetIndex);
      }
      else
      {
         s32_Retval = C_RANGE;
      }
      break;
   case C_PuiSvDbDataElement::eTAB_CHART: // tab chart delete is handled like dashboard tab delete
   case C_PuiSvDbDataElement::eLINE_ARROW:
   case C_PuiSvDbDataElement::eBOUNDARY:
   case C_PuiSvDbDataElement::eTEXT_ELEMENT:
   case C_PuiSvDbDataElement::eIMAGE:
   case C_PuiSvDbDataElement::eUNKNOWN:
   default:
      s32_Retval = C_RANGE;
      break;
   }
   return s32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check error for dashboard

   \param[in,out]  opq_InvalidDataElements   Error result for check of invalid data elements
   \param[in,out]  opq_MissingDataElements   Error result for check of missing data elements
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::CheckError(bool * const opq_InvalidDataElements, bool * const opq_MissingDataElements) const
{
   if ((opq_InvalidDataElements != NULL) || (opq_MissingDataElements != NULL))
   {
      std::vector<const C_PuiSvDbWidgetBase *> c_Widgets;
      if (opq_MissingDataElements != NULL)
      {
         *opq_MissingDataElements = false;
      }
      if (opq_InvalidDataElements != NULL)
      {
         *opq_InvalidDataElements = false;
      }
      this->GetAllWidgetItems(c_Widgets);
      //For all widgets
      for (std::vector<const C_PuiSvDbWidgetBase *>::const_iterator c_ItWidget = c_Widgets.begin();
           c_ItWidget != c_Widgets.end(); ++c_ItWidget)
      {
         const C_PuiSvDbWidgetBase * const pc_Widget = *c_ItWidget;
         if (pc_Widget != NULL)
         {
            //For all data elements
            for (std::vector<C_PuiSvDbNodeDataElementConfig>::const_iterator c_ItConfig =
                    pc_Widget->c_DataPoolElementsConfig.begin();
                 c_ItConfig != pc_Widget->c_DataPoolElementsConfig.end(); ++c_ItConfig)
            {
               const C_PuiSvDbNodeDataPoolListElementId & rc_ElementId = c_ItConfig->c_ElementId;
               //Check ID valid
               if (rc_ElementId.GetIsValid() == false)
               {
                  if (opq_InvalidDataElements != NULL)
                  {
                     *opq_InvalidDataElements = true;
                  }
               }
            }
            if ((pc_Widget->c_DataPoolElementsConfig.size() == 0) && (opq_MissingDataElements != NULL))
            {
               *opq_MissingDataElements = true;
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if list already in use by param widget

   \param[in]  orc_Id   Node data pool list ID

   \return
   True  Used
   False False unused
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDashboard::CheckNvmParamListUsage(const C_OSCNodeDataPoolListId & orc_Id) const
{
   bool q_Retval = false;

   //For all param widgets
   for (uint32 u32_ItParam = 0; (u32_ItParam < this->mc_ParamWidgets.size()) && (q_Retval == false); ++u32_ItParam)
   {
      const C_PuiSvDbParam & rc_Param = this->mc_ParamWidgets[u32_ItParam];
      //For each list and value item
      for (uint32 u32_ItElement = 0; (u32_ItElement < rc_Param.c_DataPoolElementsConfig.size()) && (q_Retval == false);
           ++u32_ItElement)
      {
         const C_PuiSvDbNodeDataElementConfig & rc_Config = rc_Param.c_DataPoolElementsConfig[u32_ItElement];
         if (rc_Config.c_ElementId.GetIsValid() == true)
         {
            if (((rc_Config.c_ElementId.u32_NodeIndex == orc_Id.u32_NodeIndex) &&
                 (rc_Config.c_ElementId.u32_DataPoolIndex == orc_Id.u32_DataPoolIndex)) &&
                (rc_Config.c_ElementId.u32_ListIndex == orc_Id.u32_ListIndex))
            {
               q_Retval = true;
            }
         }
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check if all contained indices are still valid and remove any not found indices

   \retval   true    At least one item was changed
   \retval   false   No adaptations
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_PuiSvDashboard::DiscardInvalidIndices(void)
{
   bool q_Retval = false;

   std::vector<C_PuiSvDbWidgetBase *> c_AllWidgets;
   m_GetAllWidgetItems(c_AllWidgets);
   for (uint32 u32_ItWidget = 0; u32_ItWidget < c_AllWidgets.size(); ++u32_ItWidget)
   {
      C_PuiSvDbWidgetBase * const pc_Widget = c_AllWidgets[u32_ItWidget];
      if (pc_Widget != NULL)
      {
         //Parse all current elements
         for (uint32 u32_ItElement = 0; u32_ItElement < pc_Widget->c_DataPoolElementsConfig.size();)
         {
            bool q_KickIt;
            const C_PuiSvDbNodeDataElementConfig & rc_Config = pc_Widget->c_DataPoolElementsConfig[u32_ItElement];
            if (rc_Config.c_ElementId.GetIsValid() == true)
            {
               if (C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(rc_Config.c_ElementId.u32_NodeIndex,
                                                                              rc_Config.c_ElementId.
                                                                              u32_DataPoolIndex,
                                                                              rc_Config.c_ElementId.u32_ListIndex,
                                                                              rc_Config.c_ElementId.
                                                                              u32_ElementIndex) == NULL)
               {
                  q_KickIt = true;
               }
               else
               {
                  q_KickIt = false;
               }
            }
            else
            {
               q_KickIt = true;
            }
            if (q_KickIt)
            {
               //Allow the widget to clean up all synchronized vectors
               tgl_assert(pc_Widget->RemoveElement(u32_ItElement) == C_NO_ERR);
               //Signal change
               q_Retval = true;
               //New element at current index (no iteration necessary)
            }
            else
            {
               //Important iteration step
               ++u32_ItElement;
            }
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle compatibility chart

   \param[in,out]  orc_NewCharts    New charts
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::HandleCompatibilityChart(std::vector<C_PuiSvDashboard> & orc_NewCharts)
{
   uint32 u32_Counter = 0;

   for (const C_PuiSvDbChart & rc_Chart : this->GetCharts())
   {
      C_PuiSvDashboard c_NewDashboard;
      const QString c_NewTabName = QString("%1_Chart_%2").arg(this->GetName()).arg(u32_Counter + 1UL);
      C_PuiSvDbTabChart c_NewChart;
      C_PuiBsTextElement c_NewTextElement;
      uint8 u8_ColorCounter;
      //Text
      c_NewTextElement.c_UIText =
         QString(C_GtGetText::h_GetText(
                    "This version of the chart is no longer supported, and was replaced by tab \"%1\"")).arg(
            c_NewTabName);
      //New text element
      c_NewTextElement.f64_ZOrder = rc_Chart.f64_ZOrder;
      c_NewTextElement.f64_Height = rc_Chart.f64_Height;
      c_NewTextElement.f64_Width = rc_Chart.f64_Width;
      c_NewTextElement.c_UIPosition = rc_Chart.c_UIPosition;
      this->AddTextElement(c_NewTextElement);
      //New chart
      c_NewChart.c_DataPoolElementsActive = rc_Chart.c_DataPoolElementsActive;
      c_NewChart.c_DataPoolElementsConfig = rc_Chart.c_DataPoolElementsConfig;
      for (u8_ColorCounter = 0U; u8_ColorCounter < c_NewChart.c_DataPoolElementsActive.size(); ++u8_ColorCounter)
      {
         // Color index is necessary now but does not exist for the old chart
         c_NewChart.c_DataPoolElementsColorIndex.push_back(u8_ColorCounter);
      }
      //New dashboard
      c_NewDashboard.SetTabChart(c_NewChart);
      c_NewDashboard.SetName(c_NewTabName);
      c_NewDashboard.SetComment(C_GtGetText::h_GetText("Autogenerated for compatibility reasons"));
      c_NewDashboard.SetType(C_PuiSvDashboard::eCHART);
      orc_NewCharts.push_back(c_NewDashboard);
      //Iterate
      ++u32_Counter;
   }
   for (uint32 u32_It = this->GetCharts().size(); u32_It > 0UL; --u32_It)
   {
      Q_UNUSED(u32_It)
      tgl_assert(this->DeleteWidget(0UL, C_PuiSvDbDataElement::eCHART) == C_NO_ERR);
   }
   tgl_assert(this->GetCharts().size() == 0UL);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get widget type

   \param[in]  opc_Box  Basic widget item

   \return
   Widget type, else widget type unknown
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbDataElement::E_Type C_PuiSvDashboard::h_GetWidgetType(const C_PuiSvDbWidgetBase * const opc_Box)
{
   C_PuiSvDbDataElement::E_Type e_Retval = C_PuiSvDbDataElement::eUNKNOWN;
   if (opc_Box != NULL)
   {
      const C_PuiSvDbChart * const pc_Charts = dynamic_cast<const C_PuiSvDbChart * const>(opc_Box);
      const C_PuiSvDbLabel * const pc_Labels = dynamic_cast<const C_PuiSvDbLabel * const>(opc_Box);
      const C_PuiSvDbParam * const pc_ParamWidgets = dynamic_cast<const C_PuiSvDbParam * const>(opc_Box);
      const C_PuiSvDbPieChart * const pc_PieCharts = dynamic_cast<const C_PuiSvDbPieChart * const>(opc_Box);
      const C_PuiSvDbProgressBar * const pc_ProgressBars = dynamic_cast<const C_PuiSvDbProgressBar * const>(opc_Box);
      const C_PuiSvDbSpinBox * const pc_SpinBoxes = dynamic_cast<const C_PuiSvDbSpinBox * const>(opc_Box);
      const C_PuiSvDbSlider * const pc_Sliders = dynamic_cast<const C_PuiSvDbSlider * const>(opc_Box);
      const C_PuiSvDbTable * const pc_Tables = dynamic_cast<const C_PuiSvDbTable * const>(opc_Box);
      const C_PuiSvDbToggle * const pc_Toggles = dynamic_cast<const C_PuiSvDbToggle * const>(opc_Box);
      const C_PuiSvDbTabChart * const pc_TabChart = dynamic_cast<const C_PuiSvDbTabChart * const>(opc_Box);

      if (pc_Charts != NULL)
      {
         e_Retval = C_PuiSvDbDataElement::eCHART;
      }
      if (pc_Labels != NULL)
      {
         e_Retval = C_PuiSvDbDataElement::eLABEL;
      }
      if (pc_ParamWidgets != NULL)
      {
         e_Retval = C_PuiSvDbDataElement::ePARAM;
      }
      if (pc_PieCharts != NULL)
      {
         e_Retval = C_PuiSvDbDataElement::ePIE_CHART;
      }
      if (pc_ProgressBars != NULL)
      {
         e_Retval = C_PuiSvDbDataElement::ePROGRESS_BAR;
      }
      if (pc_SpinBoxes != NULL)
      {
         e_Retval = C_PuiSvDbDataElement::eSPIN_BOX;
      }
      if (pc_Sliders != NULL)
      {
         e_Retval = C_PuiSvDbDataElement::eSLIDER;
      }
      if (pc_Tables != NULL)
      {
         e_Retval = C_PuiSvDbDataElement::eTABLE;
      }
      if (pc_Toggles != NULL)
      {
         e_Retval = C_PuiSvDbDataElement::eTOGGLE;
      }
      if (pc_TabChart != NULL)
      {
         e_Retval = C_PuiSvDbDataElement::eTAB_CHART;
      }
      tgl_assert(e_Retval != C_PuiSvDbDataElement::eUNKNOWN);
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear stored content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::Clear(void)
{
   this->mc_Charts.clear();
   this->mc_Labels.clear();
   this->mc_ParamWidgets.clear();
   this->mc_PieCharts.clear();
   this->mc_ProgressBars.clear();
   this->mc_SpinBoxes.clear();
   this->mc_Sliders.clear();
   this->mc_Tables.clear();
   this->mc_Toggles.clear();
   C_PuiBsElements::Clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Count sum of all items

   \return
   Total number of dashboard elements
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_PuiSvDashboard::Count(void) const
{
   uint32 u32_Retval = C_PuiBsElements::Count();

   u32_Retval += this->mc_Charts.size();
   u32_Retval += this->mc_Labels.size();
   u32_Retval += this->mc_ParamWidgets.size();
   u32_Retval += this->mc_PieCharts.size();
   u32_Retval += this->mc_ProgressBars.size();
   u32_Retval += this->mc_SpinBoxes.size();
   u32_Retval += this->mc_Sliders.size();
   u32_Retval += this->mc_Tables.size();
   u32_Retval += this->mc_Toggles.size();
   return u32_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get all widget items in one single vector

   Warning: output not cleared

   \param[in,out]  orc_Output    Output
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::m_GetAllWidgetItems(std::vector<C_PuiSvDbWidgetBase *> & orc_Output)
{
   const uintn un_Size = this->mc_Charts.size() + this->mc_Labels.size() + this->mc_PieCharts.size() +
                         this->mc_ProgressBars.size() + this->mc_SpinBoxes.size() + this->mc_Sliders.size() +
                         this->mc_Tables.size() + this->mc_Toggles.size() + this->mc_ParamWidgets.size() + 1U;

   //Improve performance
   orc_Output.reserve(un_Size);
   for (uint32 u32_ItItem = 0; u32_ItItem < this->mc_Charts.size(); ++u32_ItItem)
   {
      orc_Output.push_back(&this->mc_Charts[u32_ItItem]);
   }
   for (uint32 u32_ItItem = 0; u32_ItItem < this->mc_Labels.size(); ++u32_ItItem)
   {
      orc_Output.push_back(&this->mc_Labels[u32_ItItem]);
   }
   for (uint32 u32_ItItem = 0; u32_ItItem < this->mc_ParamWidgets.size(); ++u32_ItItem)
   {
      orc_Output.push_back(&this->mc_ParamWidgets[u32_ItItem]);
   }
   for (uint32 u32_ItItem = 0; u32_ItItem < this->mc_PieCharts.size(); ++u32_ItItem)
   {
      orc_Output.push_back(&this->mc_PieCharts[u32_ItItem]);
   }
   for (uint32 u32_ItItem = 0; u32_ItItem < this->mc_ProgressBars.size(); ++u32_ItItem)
   {
      orc_Output.push_back(&this->mc_ProgressBars[u32_ItItem]);
   }
   for (uint32 u32_ItItem = 0; u32_ItItem < this->mc_SpinBoxes.size(); ++u32_ItItem)
   {
      orc_Output.push_back(&this->mc_SpinBoxes[u32_ItItem]);
   }
   for (uint32 u32_ItItem = 0; u32_ItItem < this->mc_Sliders.size(); ++u32_ItItem)
   {
      orc_Output.push_back(&this->mc_Sliders[u32_ItItem]);
   }
   for (uint32 u32_ItItem = 0; u32_ItItem < this->mc_Tables.size(); ++u32_ItItem)
   {
      orc_Output.push_back(&this->mc_Tables[u32_ItItem]);
   }
   for (uint32 u32_ItItem = 0; u32_ItItem < this->mc_Toggles.size(); ++u32_ItItem)
   {
      orc_Output.push_back(&this->mc_Toggles[u32_ItItem]);
   }
   orc_Output.push_back(&this->mc_TabChart);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Mark data element id invalid

   Warning: expected to be called before data element deletion

   \param[in,out]  orc_DataElementId   Data element id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::mh_MarkInvalid(C_PuiSvDbNodeDataPoolListElementId & orc_DataElementId)
{
   const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(
      orc_DataElementId.u32_NodeIndex, orc_DataElementId.u32_DataPoolIndex);

   if (pc_DataPool != NULL)
   {
      const QString c_Namespace = C_PuiSvHandler::h_GetNamespace(orc_DataElementId);
      orc_DataElementId.MarkInvalid(pc_DataPool->e_Type, c_Namespace);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get best index for new item

   \param[in]   orc_ParamWidget     Param widget
   \param[in]   orc_NewId           New id
   \param[out]  ops32_DatasetIndex  Dataset index

   \return
   Best index for new item
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_PuiSvDashboard::mh_GetBestIndexForNewItem(const C_PuiSvDbParam & orc_ParamWidget,
                                                   const C_OSCNodeDataPoolListElementId & orc_NewId,
                                                   stw_types::sint32 * const ops32_DatasetIndex)
{
   uint32 u32_NewIndex = 0UL;

   //Search for best place of new item
   for (uint32 u32_ItItem = 0UL; u32_ItItem < orc_ParamWidget.c_DataPoolElementsConfig.size(); ++u32_ItItem)
   {
      const C_PuiSvDbNodeDataElementConfig & rc_CurId = orc_ParamWidget.c_DataPoolElementsConfig[u32_ItItem];
      if (rc_CurId.c_ElementId.GetIsValid())
      {
         //Check if relevant list
         if (((rc_CurId.c_ElementId.u32_NodeIndex == orc_NewId.u32_NodeIndex) &&
              (rc_CurId.c_ElementId.u32_DataPoolIndex == orc_NewId.u32_DataPoolIndex)) &&
             (rc_CurId.c_ElementId.u32_ListIndex == orc_NewId.u32_ListIndex))
         {
            if (ops32_DatasetIndex != NULL)
            {
               //Use the current selected dataset index
               *ops32_DatasetIndex = orc_ParamWidget.c_DataSetSelectionIndices[u32_ItItem];
            }
            //If next item should be after the new item: stop and use this index instead
            if (rc_CurId.c_ElementId.u32_ElementIndex >= orc_NewId.u32_ElementIndex)
            {
               break;
            }
         }
      }
      //Iterate
      ++u32_NewIndex;
   }
   return u32_NewIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clean up parameters after synchronisation step
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDashboard::m_SyncCleanUpParams(void)
{
   //For all param widgets
   for (uint32 u32_ItParam = 0; u32_ItParam < this->mc_ParamWidgets.size(); ++u32_ItParam)
   {
      C_PuiSvDbParam & rc_Param = this->mc_ParamWidgets[u32_ItParam];
      //For each list and value item
      tgl_assert(rc_Param.c_ListValues.size() == rc_Param.c_DataPoolElementsConfig.size());
      tgl_assert(rc_Param.c_DataSetSelectionIndices.size() == rc_Param.c_DataPoolElementsConfig.size());
      for (uint32 u32_ItElement = 0; u32_ItElement < rc_Param.c_DataPoolElementsConfig.size();)
      {
         const C_PuiSvDbNodeDataElementConfig & rc_Config = rc_Param.c_DataPoolElementsConfig[u32_ItElement];
         //Check if the item was invalidated
         if (rc_Config.c_ElementId.GetIsValid() == false)
         {
            //Erase current element
            tgl_assert(rc_Param.RemoveElement(u32_ItElement) == C_NO_ERR);
            //New element at current position (no iteration step necessary
         }
         else
         {
            //Iterate to next item
            ++u32_ItElement;
         }
      }
      for (std::vector<C_PuiSvDbExpandedTreeIndex>::iterator c_ItDataPoolElements = rc_Param.c_ExpandedItems.begin();
           c_ItDataPoolElements != rc_Param.c_ExpandedItems.end();)
      {
         const C_PuiSvDbExpandedTreeIndex & rc_Config = *c_ItDataPoolElements;
         //Check if the item was invalidated
         if (rc_Config.c_ExpandedId.GetIsValid() == false)
         {
            //Erase current element
            c_ItDataPoolElements = rc_Param.c_ExpandedItems.erase(c_ItDataPoolElements);
         }
         else
         {
            //Iterate to next item
            ++c_ItDataPoolElements;
         }
      }
   }
}
