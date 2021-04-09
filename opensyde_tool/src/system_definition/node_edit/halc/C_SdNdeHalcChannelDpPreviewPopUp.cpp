//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Pop up widget for DP preview

   Pop up widget for DP preview

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"
#include "C_OSCHALCMagicianUtil.h"
#include "C_SdNdeHalcChannelDpPreviewPopUp.h"
#include "ui_C_SdNdeHalcChannelDpPreviewPopUp.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const sintn C_SdNdeHalcChannelDpPreviewPopUp::mhsn_TableSpacing = 50;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  orc_Parent          Reference to parent
   \param[in]      ou32_NodeIndex      Node index
   \param[in]      ou32_DomainIndex    Domain index
   \param[in]      ou32_ChannelIndex   Channel index
   \param[in]      oq_UseChannelIndex  Use channel index
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcChannelDpPreviewPopUp::C_SdNdeHalcChannelDpPreviewPopUp(
   stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent, const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex,
   const uint32 ou32_ChannelIndex, const bool oq_UseChannelIndex) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNdeHalcChannelDpPreviewPopUp),
   mrc_ParentDialog(orc_Parent)
{
   this->mpc_Ui->setupUi(this);

   InitStaticNames();

   //Dynamic title
   {
      const C_OSCHalcConfigChannel * const pc_Channel =
         C_PuiSdHandler::h_GetInstance()->GetHALCDomainChannelConfigData(ou32_NodeIndex, ou32_DomainIndex,
                                                                         ou32_ChannelIndex, oq_UseChannelIndex);
      if (pc_Channel != NULL)
      {
         this->mrc_ParentDialog.SetTitle(pc_Channel->c_Name.c_str());
      }
   }
   //RO
   this->mpc_Ui->pc_TextEditContent->setReadOnly(true);

   //Content
   this->m_UpdateContent(ou32_NodeIndex, ou32_DomainIndex,
                         ou32_ChannelIndex, oq_UseChannelIndex);

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   connect(this->mpc_Ui->pc_PushButtonOk, &QPushButton::clicked, this, &C_SdNdeHalcChannelDpPreviewPopUp::m_OkClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcChannelDpPreviewPopUp::~C_SdNdeHalcChannelDpPreviewPopUp(void)
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelDpPreviewPopUp::InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Datapool Details"));
   this->mpc_Ui->pc_LabelHeadingPreview->setText(C_GtGetText::h_GetText("Details"));
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("OK"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out]  opc_KeyEvent  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelDpPreviewPopUp::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Return)))
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
void C_SdNdeHalcChannelDpPreviewPopUp::m_OkClicked(void)
{
   this->mrc_ParentDialog.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update content

   \param[in]  ou32_NodeIndex       Node index
   \param[in]  ou32_DomainIndex     Domain index
   \param[in]  ou32_ChannelIndex    Channel index
   \param[in]  oq_UseChannelIndex   Use channel index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelDpPreviewPopUp::m_UpdateContent(const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex,
                                                       const uint32 ou32_ChannelIndex,
                                                       const bool oq_UseChannelIndex) const
{
   QString c_Content = "<html><body>";

   C_SdNdeHalcChannelDpPreviewPopUp::mh_AddDpSection(ou32_NodeIndex, ou32_DomainIndex, ou32_ChannelIndex,
                                                     oq_UseChannelIndex, c_Content);
   C_SdNdeHalcChannelDpPreviewPopUp::mh_AddDeSection(ou32_NodeIndex, ou32_DomainIndex, ou32_ChannelIndex,
                                                     oq_UseChannelIndex, c_Content);

   c_Content += "</body></html>";
   this->mpc_Ui->pc_TextEditContent->setHtml(c_Content);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add dp section

   \param[in]      ou32_NodeIndex      Node index
   \param[in]      ou32_DomainIndex    Domain index
   \param[in]      ou32_ChannelIndex   Channel index
   \param[in]      oq_UseChannelIndex  Use channel index
   \param[in,out]  orc_Text            Text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelDpPreviewPopUp::mh_AddDpSection(const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex,
                                                       const uint32 ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                                       QString & orc_Text)
{
   const C_OSCNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOSCNodeConst(ou32_NodeIndex);
   const C_OSCHalcConfigChannel * const pc_Channel =
      C_PuiSdHandler::h_GetInstance()->GetHALCDomainChannelConfigData(ou32_NodeIndex, ou32_DomainIndex,
                                                                      ou32_ChannelIndex, oq_UseChannelIndex);

   orc_Text += "<h3>";
   orc_Text += C_GtGetText::h_GetText("Datapool Information");
   orc_Text += "</h3>";
   if ((pc_Channel != NULL) && (pc_Node != NULL))
   {
      const C_OSCNodeDataPool * const pc_Datapool =
         C_PuiSdHandler::h_GetInstance()->GetHALCDatapool(ou32_NodeIndex, pc_Channel->q_SafetyRelevant);
      const C_OSCNodeApplication * pc_Appl = NULL;

      orc_Text += "<table style=\"width:100%;\">";
      orc_Text += "<tr>";
      orc_Text += "<td>" + static_cast<QString>(C_GtGetText::h_GetText("Datapool:")) + "</td>";
      orc_Text += "<td>" +
                  static_cast<QString>(C_OSCHALCMagicianUtil::h_GetDatapoolName(pc_Channel->q_SafetyRelevant).c_str()) +
                  "</td>";
      orc_Text += "</tr>";
      orc_Text += "<tr>";
      orc_Text += "<td>" + static_cast<QString>(C_GtGetText::h_GetText("Mapped Datablock:")) + "</td>";

      if ((pc_Datapool != NULL) && (pc_Datapool->s32_RelatedDataBlockIndex >= 0))
      {
         pc_Appl =
            C_PuiSdHandler::h_GetInstance()->GetApplication(ou32_NodeIndex,
                                                            static_cast<uint32>(pc_Datapool->s32_RelatedDataBlockIndex));
      }
      if (pc_Appl != NULL)
      {
         orc_Text += "<td>" + static_cast<QString>(pc_Appl->c_Name.c_str()) + "</td>";
      }
      else
      {
         orc_Text += "<td>" + static_cast<QString>("-") + "</td>";
      }
      orc_Text += "</tr>";
      orc_Text += "</table>";
      //Spacing
      orc_Text += "<br/>";
      orc_Text += "<br/>";
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add data element section

   \param[in]      ou32_NodeIndex      Node index
   \param[in]      ou32_DomainIndex    Domain index
   \param[in]      ou32_ChannelIndex   Channel index
   \param[in]      oq_UseChannelIndex  Use channel index
   \param[in,out]  orc_Text            Text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelDpPreviewPopUp::mh_AddDeSection(const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex,
                                                       const uint32 ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                                       QString & orc_Text)
{
   const QString c_Column1HeadingTagStart =
      static_cast<QString>("<td style=\"padding: 30px %1 6px 0px;font-weight: bold;\">").
      arg(C_SdNdeHalcChannelDpPreviewPopUp::mhsn_TableSpacing);
   const QString c_ColumnContentHeadingTagStart =
      static_cast<QString>("<td style=\"padding: 30px %1 6px %1;font-weight: bold;\">").
      arg(C_SdNdeHalcChannelDpPreviewPopUp::mhsn_TableSpacing);

   orc_Text += "<h3>";
   orc_Text += C_GtGetText::h_GetText("Used Datapool Elements");
   orc_Text += "</h3>";
   orc_Text += "<p>";
   orc_Text += C_GtGetText::h_GetText("Used Datapool elements according to current ");
   orc_Text += oq_UseChannelIndex ? C_GtGetText::h_GetText("channel") : C_GtGetText::h_GetText("domain");
   orc_Text += C_GtGetText::h_GetText(" configuration");
   mh_AddUseCase(ou32_NodeIndex, ou32_DomainIndex, ou32_ChannelIndex, oq_UseChannelIndex, orc_Text);
   orc_Text += ":";
   orc_Text += "</p>";

   orc_Text += "<table style=\"width:100%;\">";
   orc_Text += "<tr>";
   orc_Text += c_Column1HeadingTagStart;
   orc_Text += C_GtGetText::h_GetText("List");
   orc_Text += "</td>";
   orc_Text += c_ColumnContentHeadingTagStart;
   orc_Text += C_GtGetText::h_GetText("Data Element");
   orc_Text += "</td>";
   orc_Text += c_ColumnContentHeadingTagStart;
   orc_Text += C_GtGetText::h_GetText("Description");
   orc_Text += "</td>";
   orc_Text += "</tr>";
   {
      std::vector<uint32> c_ParameterIndices;
      std::vector<uint32> c_InputIndices;
      std::vector<uint32> c_OutputIndices;
      std::vector<uint32> c_StatusIndices;
      if (C_PuiSdHandler::h_GetInstance()->GetHALCRelevantIndicesForSelectedUseCase(ou32_NodeIndex, ou32_DomainIndex,
                                                                                    ou32_ChannelIndex,
                                                                                    oq_UseChannelIndex,
                                                                                    &c_ParameterIndices,
                                                                                    &c_InputIndices,
                                                                                    &c_OutputIndices,
                                                                                    &c_StatusIndices) == C_NO_ERR)
      {
         const C_OSCHalcConfig * const pc_HALCConfig = C_PuiSdHandler::h_GetInstance()->GetHALCConfig(ou32_NodeIndex);

         if (pc_HALCConfig != NULL)
         {
            const C_OSCHalcDefDomain * const pc_Domain = pc_HALCConfig->GetDomainDefDataConst(ou32_DomainIndex);
            if (pc_Domain != NULL)
            {
               const stw_scl::C_SCLString c_ListName1 = C_OSCHALCMagicianUtil::h_GetListName(
                  C_OSCHalcDefDomain::eVA_PARAM);
               const stw_scl::C_SCLString c_ListName2 = C_OSCHALCMagicianUtil::h_GetListName(
                  C_OSCHalcDefDomain::eVA_INPUT);
               const stw_scl::C_SCLString c_ListName3 = C_OSCHALCMagicianUtil::h_GetListName(
                  C_OSCHalcDefDomain::eVA_OUTPUT);
               const stw_scl::C_SCLString c_ListName4 = C_OSCHALCMagicianUtil::h_GetListName(
                  C_OSCHalcDefDomain::eVA_STATUS);
               if (oq_UseChannelIndex)
               {
                  C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListSection(pc_Domain->c_ChannelValues.c_Parameters,
                                                                      c_ParameterIndices, c_ListName1,
                                                                      pc_Domain->c_SingularName, orc_Text,
                                                                      *pc_Domain, true);
                  C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListSection(pc_Domain->c_ChannelValues.c_InputValues,
                                                                      c_InputIndices, c_ListName2,
                                                                      pc_Domain->c_SingularName, orc_Text,
                                                                      *pc_Domain, false);
                  C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListSection(pc_Domain->c_ChannelValues.c_OutputValues,
                                                                      c_OutputIndices, c_ListName3,
                                                                      pc_Domain->c_SingularName, orc_Text,
                                                                      *pc_Domain, false);
                  C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListSection(pc_Domain->c_ChannelValues.c_StatusValues,
                                                                      c_StatusIndices, c_ListName4,
                                                                      pc_Domain->c_SingularName, orc_Text,
                                                                      *pc_Domain, false);
               }
               else
               {
                  C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListSection(pc_Domain->c_DomainValues.c_Parameters,
                                                                      c_ParameterIndices, c_ListName1,
                                                                      pc_Domain->c_SingularName, orc_Text,
                                                                      *pc_Domain, true);
                  C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListSection(pc_Domain->c_DomainValues.c_InputValues,
                                                                      c_InputIndices, c_ListName2,
                                                                      pc_Domain->c_SingularName, orc_Text,
                                                                      *pc_Domain, false);
                  C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListSection(pc_Domain->c_DomainValues.c_OutputValues,
                                                                      c_OutputIndices, c_ListName3,
                                                                      pc_Domain->c_SingularName, orc_Text,
                                                                      *pc_Domain, false);
                  C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListSection(pc_Domain->c_DomainValues.c_StatusValues,
                                                                      c_StatusIndices, c_ListName4,
                                                                      pc_Domain->c_SingularName, orc_Text,
                                                                      *pc_Domain, false);
               }
            }
         }
      }
   }
   orc_Text += "</table>";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add use case in channel case (nothing in domain case)

   \param[in]      ou32_NodeIndex      Node index
   \param[in]      ou32_DomainIndex    Domain index
   \param[in]      ou32_ChannelIndex   Channel index
   \param[in]      oq_UseChannelIndex  Use channel index
   \param[in,out]  orc_Text            Text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelDpPreviewPopUp::mh_AddUseCase(const uint32 ou32_NodeIndex, const uint32 ou32_DomainIndex,
                                                     const uint32 ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                                     QString & orc_Text)
{
   const QString c_UseCase = "";

   if (oq_UseChannelIndex == true)
   {
      const C_OSCHalcConfigChannel * const pc_Channel =
         C_PuiSdHandler::h_GetInstance()->GetHALCDomainChannelConfigData(ou32_NodeIndex, ou32_DomainIndex,
                                                                         ou32_ChannelIndex, oq_UseChannelIndex);
      const C_OSCHalcConfigDomain * const pc_Domain =
         C_PuiSdHandler::h_GetInstance()->GetHALCDomainConfigDataConst(ou32_NodeIndex, ou32_DomainIndex);
      if ((pc_Domain != NULL) && (pc_Channel != NULL) &&
          (pc_Channel->u32_UseCaseIndex < pc_Domain->c_ChannelUseCases.size()))
      {
         const C_OSCHalcDefChannelUseCase & rc_UseCase = pc_Domain->c_ChannelUseCases[pc_Channel->u32_UseCaseIndex];
         orc_Text += C_GtGetText::h_GetText(" (use case \'");
         orc_Text += rc_UseCase.c_Display.c_str();
         orc_Text += c_UseCase;
         orc_Text += "\')";
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add list section

   \param[in]      orc_Definition            Definition
   \param[in]      orc_Indices               Indices
   \param[in]      orc_ListName              List name
   \param[in]      orc_DomainSingularName    Domain singular name
   \param[in,out]  orc_Text                  Text
   \param[in]      orc_Domain                Domain
   \param[in]      oq_AddSpecialVars         Add special vars
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListSection(const std::vector<C_OSCHalcDefStruct> & orc_Definition,
                                                         const std::vector<uint32> & orc_Indices,
                                                         const stw_scl::C_SCLString & orc_ListName,
                                                         const stw_scl::C_SCLString & orc_DomainSingularName,
                                                         QString & orc_Text,
                                                         const stw_opensyde_core::C_OSCHalcDefDomain & orc_Domain,
                                                         const bool oq_AddSpecialVars)
{
   bool q_AddedList = false;

   if (orc_Indices.size() > 0UL)
   {
      if (oq_AddSpecialVars)
      {
         if (C_OSCHALCMagicianUtil::h_CheckChanNumVariableNecessary(orc_Domain))
         {
            const C_OSCNodeDataPoolListElement c_Tmp = C_OSCHALCMagicianUtil::h_GetChanNumVariable(
               orc_DomainSingularName, true /*irrelevant here*/, 1UL, false);
            C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListEntry(orc_ListName, q_AddedList, c_Tmp.c_Name, c_Tmp.c_Comment,
                                                              orc_Text);
         }
         if (C_OSCHALCMagicianUtil::h_CheckUseCaseVariableNecessary(orc_Domain))
         {
            const C_OSCNodeDataPoolListElement c_Tmp = C_OSCHALCMagicianUtil::h_GetUseCaseVariable(
               orc_DomainSingularName, true /*irrelevant here*/, 1UL, false);
            C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListEntry(orc_ListName, q_AddedList, c_Tmp.c_Name, c_Tmp.c_Comment,
                                                              orc_Text);
         }
      }
      for (uint32 u32_ItIndex = 0UL; u32_ItIndex < orc_Indices.size(); ++u32_ItIndex)
      {
         if (orc_Indices[u32_ItIndex] < orc_Definition.size())
         {
            stw_scl::C_SCLString c_Name;
            const C_OSCHalcDefStruct & rc_Def = orc_Definition[orc_Indices[u32_ItIndex]];
            //Multiple
            for (uint32 u32_ItElem = 0UL; u32_ItElem < rc_Def.c_StructElements.size(); ++u32_ItElem)
            {
               const C_OSCHalcDefElement & rc_Elem = rc_Def.c_StructElements[u32_ItElem];
               C_OSCHALCMagicianUtil::h_GetVariableName(orc_Definition, orc_Indices[u32_ItIndex], u32_ItElem,
                                                        orc_DomainSingularName, c_Name);
               C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListEntry(orc_ListName, q_AddedList, c_Name, rc_Elem.c_Comment,
                                                                 orc_Text);
            }
            //Single
            if (rc_Def.c_StructElements.size() == 0UL)
            {
               C_OSCHALCMagicianUtil::h_GetVariableName(orc_Definition, orc_Indices[u32_ItIndex], 0UL,
                                                        orc_DomainSingularName, c_Name);
               C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListEntry(orc_ListName, q_AddedList, c_Name, rc_Def.c_Comment,
                                                                 orc_Text);
            }
         }
      }
   }
   else
   {
      C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListEntry(orc_ListName, q_AddedList, "-", "-", orc_Text);
   }
   //Add empty row
   C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListEntry(orc_ListName, q_AddedList, "", "", orc_Text);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add list entry

   \param[in]      orc_ListName     List name
   \param[in,out]  orq_ListAdded    List added
   \param[in]      orc_VarName      Variable name
   \param[in]      orc_VarComment   Variable comment
   \param[in,out]  orc_Text         Text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListEntry(const stw_scl::C_SCLString & orc_ListName, bool & orq_ListAdded,
                                                       const stw_scl::C_SCLString & orc_VarName,
                                                       const stw_scl::C_SCLString & orc_VarComment, QString & orc_Text)
{
   const QString c_Column1TagStart = static_cast<QString>("<td style=\"padding: 0 %1 0 0px;\">").arg(
      C_SdNdeHalcChannelDpPreviewPopUp::mhsn_TableSpacing);
   const QString c_ColumnContentTagStart = static_cast<QString>(
      "<td valign=\"middle\" style=\"padding: 0 %1 0 %1;\">").arg(C_SdNdeHalcChannelDpPreviewPopUp::mhsn_TableSpacing);

   orc_Text += "<tr>";
   orc_Text += c_Column1TagStart;
   if (orq_ListAdded == false)
   {
      orc_Text += orc_ListName.c_str();
      orq_ListAdded = true;
   }
   orc_Text += "</td>";
   orc_Text += c_ColumnContentTagStart;
   orc_Text += orc_VarName.c_str();
   orc_Text += "</td>";
   orc_Text += c_ColumnContentTagStart;
   orc_Text += orc_VarComment.c_str();
   orc_Text += "</td>";
   orc_Text += "</tr>";
}
