//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for param widget configuration import report (implementation)

   Widget for param widget configuration import report

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "constants.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_SdNdeDpUtil.hpp"
#include "C_ImpUtil.hpp"
#include "C_SyvDaItPaTreeModel.hpp"
#include "C_SyvDaItPaImportReport.hpp"
#include "C_SdNdeDpContentUtil.hpp"
#include "ui_C_SyvDaItPaImportReport.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const QString C_SyvDaItPaImportReport::mhc_HTML_TABLE_HEADER_START =
   "<td align=\"left\" valign=\"top\" style=\"padding: 5px 18px 0px 0px;white-space:pre;font-weight:bold;\">";
const QString C_SyvDaItPaImportReport::mhc_HTML_TABLE_DATA_START =
   "<td align=\"left\" valign=\"top\" style=\"padding: 0px 18px 0px 0px;white-space:pre;\">";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  orc_Parent          Reference to parent
   \param[in]      orc_Data            Loaded data
   \param[in]      orc_ElementIds      Element IDs to overwrite
   \param[in]      orc_Id              Selected element ID
   \param[in]      ou32_ValidLayers    Number of valid layers in selected element ID
   \param[in]      orc_Path            Selected file path
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItPaImportReport::C_SyvDaItPaImportReport(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                 const std::vector<C_OscParamSetInterpretedNode> & orc_Data,
                                                 const std::vector<C_OscNodeDataPoolListElementId> & orc_ElementIds,
                                                 const C_OscNodeDataPoolListElementId & orc_Id,
                                                 const uint32_t ou32_ValidLayers, const QString & orc_Path) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SyvDaItPaImportReport),
   mrc_ParentDialog(orc_Parent),
   mrc_Data(orc_Data),
   mrc_ElementIds(orc_ElementIds),
   mrc_Path(orc_Path),
   mrc_Id(orc_Id),
   mu32_ValidLayers(ou32_ValidLayers)
{
   this->mpc_Ui->setupUi(this);

   InitStaticNames();
   m_SetReport();

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_SyvDaItPaImportReport::m_OkClicked);
   connect(this->mpc_Ui->pc_PushButtonCancel, &QPushButton::clicked, this, &C_SyvDaItPaImportReport::m_CancelClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItPaImportReport::~C_SyvDaItPaImportReport(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImportReport::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Load Parameter Set File"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Report"));
   this->mpc_Ui->pc_LabelHeadingPreview->setText(C_GtGetText::h_GetText("Details"));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("Apply"));
   this->mpc_Ui->pc_PushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get results

   \param[out]  orc_OutputListIds   Element IDs to replace
   \param[out]  orc_OutputContent   Content to replace items with
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImportReport::GetOutput(std::vector<C_OscNodeDataPoolListElementId> & orc_OutputListIds,
                                        std::vector<C_OscNodeDataPoolContent> & orc_OutputContent) const
{
   orc_OutputListIds = this->mc_OutputListIds;
   orc_OutputContent = this->mc_OutputContent;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get float range check results

   \param[in,out]  orc_InvalidValueIds    Invalid value ids
   \param[in,out]  orc_InvalidValues      Invalid values
   \param[in,out]  orc_NewValues          New values
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImportReport::GetFloatRangeCheckResults(
   std::vector<C_OscNodeDataPoolListElementId> & orc_InvalidValueIds, std::vector<QString> & orc_InvalidValues,
   std::vector<QString> & orc_NewValues) const
{
   orc_InvalidValueIds = this->mc_FloatRangeCheckInvalidValueIds;
   orc_InvalidValues = this->mc_FloatRangeCheckInvalidValues;
   orc_NewValues = this->mc_FloatRangeCheckNewValues;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out]  opc_KeyEvent  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImportReport::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Return)))
   {
      if (((opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == true) &&
           (opc_KeyEvent->modifiers().testFlag(Qt::AltModifier) == false)) &&
          (opc_KeyEvent->modifiers().testFlag(Qt::ShiftModifier) == false))
      {
         this->mrc_ParentDialog.accept();
      }
      else
      {
         q_CallOrg = false;
      }
   }
   if (q_CallOrg == true)
   {
      QWidget::keyPressEvent(opc_KeyEvent);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImportReport::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImportReport::m_CancelClicked(void)
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set report content
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImportReport::m_SetReport(void)
{
   QString c_CompleteLog;

   //Start
   c_CompleteLog += "<html>";
   c_CompleteLog += "<body>";
   m_HandleSourceFileInformation(c_CompleteLog);
   m_HandleParsing(c_CompleteLog);
   //End
   c_CompleteLog += "</body>";
   c_CompleteLog += "</html>";
   this->mpc_Ui->textBrowser->setHtml(c_CompleteLog);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle source file information

   \param[in,out]  orc_Text   Text to append to
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImportReport::m_HandleSourceFileInformation(QString & orc_Text) const
{
   QString c_ReadContent;
   uint32_t u32_NodeCount = 0UL;
   uint32_t u32_DatapoolCount = 0UL;
   uint32_t u32_ListCount = 0UL;
   uint32_t u32_ParameterCount = 0UL;

   for (uint32_t u32_ItNode = 0UL; u32_ItNode < this->mrc_Data.size(); ++u32_ItNode)
   {
      const C_OscParamSetInterpretedNode & rc_Node = this->mrc_Data[u32_ItNode];
      ++u32_NodeCount;
      for (uint32_t u32_ItDp = 0UL; u32_ItDp < rc_Node.c_DataPools.size(); ++u32_ItDp)
      {
         const C_OscParamSetInterpretedDataPool & rc_Dp = rc_Node.c_DataPools[u32_ItDp];
         ++u32_DatapoolCount;
         for (uint32_t u32_ItList = 0UL; u32_ItList < rc_Dp.c_Lists.size(); ++u32_ItList)
         {
            const C_OscParamSetInterpretedList & rc_List = rc_Dp.c_Lists[u32_ItList];
            ++u32_ListCount;
            u32_ParameterCount += rc_List.c_Elements.size();
         }
      }
   }
   c_ReadContent +=
      static_cast<QString>(C_GtGetText::h_GetText("Nodes: %1, Datapools: %2, Lists: %3, Parameters: %4")).arg(
         u32_NodeCount).arg(
         u32_DatapoolCount).arg(u32_ListCount).arg(u32_ParameterCount);
   orc_Text += C_ImpUtil::h_FormatSourceFileInfoForReport(this->mrc_Path, c_ReadContent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle parsing information

   \param[in,out]  orc_Text   Text to append to
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImportReport::m_HandleParsing(QString & orc_Text)
{
   QString c_TableApplyContent;
   QString c_TableMismatchContent;
   QString c_TableRemainContent;
   uint32_t u32_TableApplyContent = 0UL;
   uint32_t u32_TableMismatchContent = 0UL;
   uint32_t u32_TableRemainContent = 0UL;

   orc_Text += "<h3>";
   orc_Text += C_GtGetText::h_GetText("Import Preview");
   orc_Text += "</h3>";
   orc_Text += "<p>";
   orc_Text += static_cast<QString>(C_GtGetText::h_GetText("Filter: %1")).arg(m_GetFilter(this->mu32_ValidLayers));
   orc_Text += "</p>";
   m_PrepareTableContent(c_TableApplyContent, c_TableMismatchContent, c_TableRemainContent, u32_TableApplyContent,
                         u32_TableMismatchContent, u32_TableRemainContent);
   orc_Text += "<p><b>";
   //Translation: 1=Number of entries
   orc_Text += static_cast<QString>(C_GtGetText::h_GetText("Applied parameters (%1)")).arg(u32_TableApplyContent);
   orc_Text += "</b></p>";
   if (u32_TableApplyContent == 0UL)
   {
      orc_Text += "<p>";
      orc_Text += "No parameters";
      orc_Text += "</p>";
   }
   else
   {
      orc_Text += "<table>";
      orc_Text += "<tr>";
      orc_Text += C_SyvDaItPaImportReport::mhc_HTML_TABLE_HEADER_START;
      orc_Text += "#</td>";
      orc_Text += C_SyvDaItPaImportReport::mhc_HTML_TABLE_HEADER_START;
      orc_Text += C_GtGetText::h_GetText("Node");
      orc_Text += "</td>";
      orc_Text += C_SyvDaItPaImportReport::mhc_HTML_TABLE_HEADER_START;
      orc_Text += C_GtGetText::h_GetText("Datapool");
      orc_Text += "</td>";
      orc_Text += C_SyvDaItPaImportReport::mhc_HTML_TABLE_HEADER_START;
      orc_Text += C_GtGetText::h_GetText("List");
      orc_Text += "</td>";
      orc_Text += C_SyvDaItPaImportReport::mhc_HTML_TABLE_HEADER_START;
      orc_Text += C_GtGetText::h_GetText("Parameter");
      orc_Text += "</td>";
      orc_Text += C_SyvDaItPaImportReport::mhc_HTML_TABLE_HEADER_START;
      orc_Text += C_GtGetText::h_GetText("Value");
      orc_Text += "</td>";
      orc_Text += "</tr>";
      orc_Text += c_TableApplyContent;
      orc_Text += "</table>";
   }
   orc_Text += "<p><b>";
   //Translation: 1=Number of entries
   orc_Text += static_cast<QString>(C_GtGetText::h_GetText("Ignored parameters (%1)")).arg(u32_TableMismatchContent);
   orc_Text += "</b></p>";
   if (u32_TableMismatchContent == 0UL)
   {
      orc_Text += "<p>";
      orc_Text += "No parameters";
      orc_Text += "</p>";
   }
   else
   {
      orc_Text += "<table>";
      orc_Text += "<tr>";
      orc_Text += C_SyvDaItPaImportReport::mhc_HTML_TABLE_HEADER_START;
      orc_Text += "#</td>";
      orc_Text += C_SyvDaItPaImportReport::mhc_HTML_TABLE_HEADER_START;
      orc_Text += C_GtGetText::h_GetText("Node");
      orc_Text += "</td>";
      orc_Text += C_SyvDaItPaImportReport::mhc_HTML_TABLE_HEADER_START;
      orc_Text += C_GtGetText::h_GetText("Datapool");
      orc_Text += "</td>";
      orc_Text += C_SyvDaItPaImportReport::mhc_HTML_TABLE_HEADER_START;
      orc_Text += C_GtGetText::h_GetText("List");
      orc_Text += "</td>";
      orc_Text += C_SyvDaItPaImportReport::mhc_HTML_TABLE_HEADER_START;
      orc_Text += C_GtGetText::h_GetText("Parameter");
      orc_Text += "</td>";
      orc_Text += C_SyvDaItPaImportReport::mhc_HTML_TABLE_HEADER_START;
      orc_Text += C_GtGetText::h_GetText("Value");
      orc_Text += "</td>";
      orc_Text += C_SyvDaItPaImportReport::mhc_HTML_TABLE_HEADER_START;
      orc_Text += C_GtGetText::h_GetText("Reason");
      orc_Text += "</td>";
      orc_Text += "</tr>";
      orc_Text += c_TableMismatchContent;
      orc_Text += "</table>";
   }
   orc_Text += "<p><b>";
   //Translation: 1=Number of entries
   orc_Text += static_cast<QString>(C_GtGetText::h_GetText("Remaining untouched parameters (%1)")).arg(
      u32_TableRemainContent);
   orc_Text += "</b></p>";
   if (u32_TableRemainContent == 0UL)
   {
      orc_Text += "<p>";
      orc_Text += "No parameters";
      orc_Text += "</p>";
   }
   else
   {
      orc_Text += "<table>";
      orc_Text += "<tr>";
      orc_Text += C_SyvDaItPaImportReport::mhc_HTML_TABLE_HEADER_START;
      orc_Text += "#</td>";
      orc_Text += C_SyvDaItPaImportReport::mhc_HTML_TABLE_HEADER_START;
      orc_Text += C_GtGetText::h_GetText("Node");
      orc_Text += "</td>";
      orc_Text += C_SyvDaItPaImportReport::mhc_HTML_TABLE_HEADER_START;
      orc_Text += C_GtGetText::h_GetText("Datapool");
      orc_Text += "</td>";
      orc_Text += C_SyvDaItPaImportReport::mhc_HTML_TABLE_HEADER_START;
      orc_Text += C_GtGetText::h_GetText("List");
      orc_Text += "</td>";
      orc_Text += C_SyvDaItPaImportReport::mhc_HTML_TABLE_HEADER_START;
      orc_Text += C_GtGetText::h_GetText("Parameter");
      orc_Text += "</td>";
      orc_Text += "</tr>";
      orc_Text += c_TableRemainContent;
      orc_Text += "</table>";
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Parse for interpreted file content to get table content

   \param[in,out]  orc_TableApplyContent     Content for table in apply section
   \param[in,out]  orc_TableMismatchContent  Content for table in mismatch section
   \param[in,out]  orc_TableRemainContent    Content for table in remain section
   \param[in,out]  oru32_TableApplyCount     Number of entries in table in apply section
   \param[in,out]  oru32_TableMismatchCount  Number of entries in table in mismatch section
   \param[in,out]  oru32_TableRemainCount    Number of entries in table in remain section
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImportReport::m_PrepareTableContent(QString & orc_TableApplyContent, QString & orc_TableMismatchContent,
                                                    QString & orc_TableRemainContent, uint32_t & oru32_TableApplyCount,
                                                    uint32_t & oru32_TableMismatchCount,
                                                    uint32_t & oru32_TableRemainCount)
{
   for (uint32_t u32_ItParam = 0; u32_ItParam < this->mrc_ElementIds.size(); ++u32_ItParam)
   {
      bool q_Found = false;
      const C_OscNodeDataPoolListElementId & rc_ParamId = this->mrc_ElementIds[u32_ItParam];
      const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(rc_ParamId.u32_NodeIndex);
      const C_OscNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOscDataPool(
         rc_ParamId.u32_NodeIndex, rc_ParamId.u32_DataPoolIndex);
      const C_OscNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(
         rc_ParamId.u32_NodeIndex, rc_ParamId.u32_DataPoolIndex, rc_ParamId.u32_ListIndex);
      const C_OscNodeDataPoolListElementId c_ElementId(rc_ParamId.u32_NodeIndex,
                                                       rc_ParamId.u32_DataPoolIndex,
                                                       rc_ParamId.u32_ListIndex,
                                                       rc_ParamId.u32_ElementIndex);
      const C_OscNodeDataPoolListElement * const pc_Element =
         C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(c_ElementId);
      if ((((pc_Node != NULL) && (pc_DataPool != NULL)) && (pc_List != NULL)) && (pc_Element != NULL))
      {
         for (uint32_t u32_ItNode = 0UL; u32_ItNode < this->mrc_Data.size(); ++u32_ItNode)
         {
            const C_OscParamSetInterpretedNode & rc_Node = this->mrc_Data[u32_ItNode];
            if (pc_Node->c_Properties.c_Name == rc_Node.c_Name)
            {
               for (uint32_t u32_ItDp = 0UL; u32_ItDp < rc_Node.c_DataPools.size(); ++u32_ItDp)
               {
                  const C_OscParamSetInterpretedDataPool & rc_Dp = rc_Node.c_DataPools[u32_ItDp];
                  if (pc_DataPool->c_Name == rc_Dp.c_DataPoolInfo.c_Name)
                  {
                     for (uint32_t u32_ItList = 0UL; u32_ItList < rc_Dp.c_Lists.size(); ++u32_ItList)
                     {
                        const C_OscParamSetInterpretedList & rc_List = rc_Dp.c_Lists[u32_ItList];
                        if (pc_List->c_Name == rc_List.c_Name)
                        {
                           for (uint32_t u32_ItElement = 0UL; u32_ItElement < rc_List.c_Elements.size();
                                ++u32_ItElement)
                           {
                              const C_OscParamSetInterpretedElement & rc_Element = rc_List.c_Elements[u32_ItElement];
                              if (pc_Element->c_Name == rc_Element.c_Name)
                              {
                                 QString c_Value;
                                 C_SdNdeDpContentUtil::h_GetValuesAsScaledCombinedString(rc_Element.c_NvmValue,
                                                                                         pc_Element->f64_Factor,
                                                                                         pc_Element->f64_Offset,
                                                                                         c_Value, false);
                                 q_Found = true;
                                 if (pc_Element->GetType() != rc_Element.c_NvmValue.GetType())
                                 {
                                    const QString c_Description = static_cast<QString>(
                                       C_GtGetText::h_GetText("Specified type in project: %1, found type in file: %2")).
                                                                  arg(C_SdNdeDpUtil::h_ConvertContentTypeToString(
                                                                         pc_Element->GetType())).arg(C_SdNdeDpUtil::h_ConvertContentTypeToString(
                                                                                                        rc_Element
                                                                                                        .c_NvmValue.
                                                                                                        GetType()));
                                    m_AppendTableEntry(orc_TableMismatchContent, oru32_TableMismatchCount, rc_ParamId,
                                                       c_Value, c_Description);
                                 }
                                 else if (pc_Element->GetArray() != rc_Element.c_NvmValue.GetArray())
                                 {
                                    QString c_ElementArray;
                                    QString c_FoundArray;
                                    if (pc_Element->GetArray())
                                    {
                                       c_ElementArray += C_GtGetText::h_GetText("is array");
                                    }
                                    else
                                    {
                                       c_ElementArray += C_GtGetText::h_GetText("is not array");
                                    }
                                    if (rc_Element.c_NvmValue.GetArray())
                                    {
                                       c_FoundArray += C_GtGetText::h_GetText("is array");
                                    }
                                    else
                                    {
                                       c_FoundArray += C_GtGetText::h_GetText("is not array");
                                    }
                                    const QString c_Description = static_cast<QString>(
                                       C_GtGetText::h_GetText("Specified type in project: %1, found type in file: %2")).
                                                                  arg(c_ElementArray).arg(c_FoundArray);
                                    m_AppendTableEntry(orc_TableMismatchContent, oru32_TableMismatchCount, rc_ParamId,
                                                       c_Value, c_Description);
                                 }
                                 else if (pc_Element->GetArraySize() != rc_Element.c_NvmValue.GetArraySize())
                                 {
                                    const QString c_Description = static_cast<QString>(
                                       C_GtGetText::h_GetText(
                                          "Specified array size in project: %1, found array size in file: %2")).
                                                                  arg(pc_Element->GetArraySize()).arg(
                                       rc_Element.c_NvmValue.GetArraySize());
                                    m_AppendTableEntry(orc_TableMismatchContent, oru32_TableMismatchCount, rc_ParamId,
                                                       c_Value, c_Description);
                                 }
                                 else if (((pc_Element->c_MinValue <= rc_Element.c_NvmValue) &&
                                           (rc_Element.c_NvmValue <= pc_Element->c_MaxValue)) == false)
                                 {
                                    QString c_Min;
                                    QString c_Max;
                                    C_SdNdeDpContentUtil::h_GetValuesAsScaledCombinedString(
                                       pc_Element->c_MinValue, pc_Element->f64_Factor, pc_Element->f64_Offset, c_Min,
                                       false);
                                    C_SdNdeDpContentUtil::h_GetValuesAsScaledCombinedString(
                                       pc_Element->c_MaxValue, pc_Element->f64_Factor, pc_Element->f64_Offset, c_Max,
                                       false);
                                    const QString c_Description =
                                       static_cast<QString>(C_GtGetText::h_GetText(
                                                               "Specified value not in allowed range. Min: %1, Max: %2"))
                                       .arg(c_Min).
                                       arg(c_Max);
                                    m_AppendTableEntry(orc_TableMismatchContent, oru32_TableMismatchCount, rc_ParamId,
                                                       c_Value, c_Description);
                                 }
                                 else
                                 {
                                    {
                                       C_OscNodeDataPoolContent c_Content = rc_Element.c_NvmValue;
                                       C_SyvDaItPaTreeModel::h_AdaptFloatRangeOfValueAndAppendResults(c_Content,
                                                                                                      c_ElementId,
                                                                                                      this->mc_FloatRangeCheckInvalidValueIds, this->mc_FloatRangeCheckInvalidValues,
                                                                                                      this->mc_FloatRangeCheckNewValues);
                                       C_SdNdeDpContentUtil::h_GetValuesAsScaledCombinedString(c_Content,
                                                                                               pc_Element->f64_Factor,
                                                                                               pc_Element->f64_Offset,
                                                                                               c_Value, false);
                                    }
                                    m_AppendTableEntry(orc_TableApplyContent, oru32_TableApplyCount, rc_ParamId,
                                                       c_Value, "");
                                    //Remember as output
                                    mc_OutputListIds.push_back(rc_ParamId);
                                    mc_OutputContent.push_back(rc_Element.c_NvmValue);
                                 }
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }
      if (q_Found == false)
      {
         m_AppendTableEntry(orc_TableRemainContent, oru32_TableRemainCount, rc_ParamId, "", "");
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Append entry to table

   \param[in,out]  orc_TableContent    Table content to append entry to
   \param[in,out]  oru32_TableCount    Current number of entries in table
   \param[in]      orc_Id              ID of element to append to table
   \param[in]      orc_Value           Value as string
   \param[in]      orc_Reason          Optional reason for discarding
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaImportReport::m_AppendTableEntry(QString & orc_TableContent, uint32_t & oru32_TableCount,
                                                 const C_OscNodeDataPoolListElementId & orc_Id,
                                                 const QString & orc_Value, const QString & orc_Reason) const
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(orc_Id.u32_NodeIndex);
   const C_OscNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOscDataPool(orc_Id.u32_NodeIndex,
                                                                                                 orc_Id.u32_DataPoolIndex);
   const C_OscNodeDataPoolList * const pc_List = C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(
      orc_Id.u32_NodeIndex, orc_Id.u32_DataPoolIndex, orc_Id.u32_ListIndex);
   const C_OscNodeDataPoolListElement * const pc_Element = C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(
      orc_Id.u32_NodeIndex, orc_Id.u32_DataPoolIndex, orc_Id.u32_ListIndex, orc_Id.u32_ElementIndex);

   if ((((pc_Node != NULL) && (pc_DataPool != NULL)) && (pc_List != NULL)) && (pc_Element != NULL))
   {
      orc_TableContent += "<tr>";
      orc_TableContent += C_SyvDaItPaImportReport::mhc_HTML_TABLE_DATA_START;
      ++oru32_TableCount;
      orc_TableContent += QString::number(oru32_TableCount);
      orc_TableContent += "</td>";
      orc_TableContent += C_SyvDaItPaImportReport::mhc_HTML_TABLE_DATA_START;
      orc_TableContent += pc_Node->c_Properties.c_Name.c_str();
      orc_TableContent += "</td>";
      orc_TableContent += C_SyvDaItPaImportReport::mhc_HTML_TABLE_DATA_START;
      orc_TableContent += pc_DataPool->c_Name.c_str();
      orc_TableContent += "</td>";
      orc_TableContent += C_SyvDaItPaImportReport::mhc_HTML_TABLE_DATA_START;
      orc_TableContent += pc_List->c_Name.c_str();
      orc_TableContent += "</td>";
      orc_TableContent += C_SyvDaItPaImportReport::mhc_HTML_TABLE_DATA_START;
      orc_TableContent += pc_Element->c_Name.c_str();
      orc_TableContent += "</td>";
      if (orc_Value.isEmpty() == false)
      {
         orc_TableContent += C_SyvDaItPaImportReport::mhc_HTML_TABLE_DATA_START;
         orc_TableContent += orc_Value;
         orc_TableContent += "</td>";
      }
      if (orc_Reason.isEmpty() == false)
      {
         orc_TableContent += C_SyvDaItPaImportReport::mhc_HTML_TABLE_DATA_START;
         orc_TableContent += orc_Reason;
         orc_TableContent += "</td>";
      }
      orc_TableContent += "</tr>";
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get filter name for index

   \param[in]  ou32_Value  Index to get filter for

   \return
   Get Name for current filter
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDaItPaImportReport::m_GetFilter(const uint32_t ou32_Value) const
{
   QString c_Retval;

   //Special case: top layer
   if (ou32_Value == 0UL)
   {
      c_Retval = static_cast<QString>(C_GtGetText::h_GetText("All"));
   }
   else if (ou32_Value <= this->mu32_ValidLayers)
   {
      const C_OscNode * pc_Node;
      const C_OscNodeDataPool * pc_DataPool;
      const C_OscNodeDataPoolList * pc_List;
      const C_OscNodeDataPoolListElement * pc_Element;
      //Get specific name for current layer
      switch (ou32_Value)
      {
      case 4UL:
         pc_Element = C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(
            this->mrc_Id.u32_NodeIndex, this->mrc_Id.u32_DataPoolIndex, this->mrc_Id.u32_ListIndex,
            this->mrc_Id.u32_ElementIndex);
         if (pc_Element != NULL)
         {
            c_Retval = static_cast<QString>(C_GtGetText::h_GetText(" - Parameter \"%1\"")).arg(
               pc_Element->c_Name.c_str());
         }
         break;
      case 3UL:
         pc_List = C_PuiSdHandler::h_GetInstance()->GetOscDataPoolList(
            this->mrc_Id.u32_NodeIndex, this->mrc_Id.u32_DataPoolIndex, this->mrc_Id.u32_ListIndex);
         if (pc_List != NULL)
         {
            c_Retval = static_cast<QString>(C_GtGetText::h_GetText(" - List \"%1\"")).arg(pc_List->c_Name.c_str());
         }
         break;
      case 2UL:
         pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOscDataPool(this->mrc_Id.u32_NodeIndex,
                                                                       this->mrc_Id.u32_DataPoolIndex);
         if (pc_DataPool != NULL)
         {
            c_Retval = static_cast<QString>(C_GtGetText::h_GetText(" - Datapool \"%1\"")).arg(
               pc_DataPool->c_Name.c_str());
         }
         break;
      case 1UL:
         pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(this->mrc_Id.u32_NodeIndex);
         if (pc_Node != NULL)
         {
            c_Retval = static_cast<QString>(C_GtGetText::h_GetText("Node \"%1\"")).arg(
               pc_Node->c_Properties.c_Name.c_str());
         }
         break;
      default:
         break;
      }
      //If not at top prepend previous layer names
      if (ou32_Value > 1UL)
      {
         c_Retval = m_GetFilter(ou32_Value - 1UL) + c_Retval;
      }
   }
   else
   {
      //Invalid
   }
   return c_Retval;
}
