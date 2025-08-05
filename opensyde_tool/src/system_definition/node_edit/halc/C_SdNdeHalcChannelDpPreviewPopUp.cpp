//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Pop up widget for DP preview

   Pop up widget for DP preview

   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_OscHalcMagicianUtil.hpp"
#include "C_SdNdeHalcChannelDpPreviewPopUp.hpp"
#include "ui_C_SdNdeHalcChannelDpPreviewPopUp.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const int32_t C_SdNdeHalcChannelDpPreviewPopUp::mhs32_TABLE_SPACING = 50;

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
   stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent, const uint32_t ou32_NodeIndex,
   const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNdeHalcChannelDpPreviewPopUp),
   mrc_ParentDialog(orc_Parent)
{
   this->mpc_Ui->setupUi(this);

   InitStaticNames();

   //Dynamic title
   {
      const C_OscHalcConfigChannel * const pc_Channel =
         C_PuiSdHandler::h_GetInstance()->GetHalcDomainChannelConfigData(ou32_NodeIndex, ou32_DomainIndex,
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
void C_SdNdeHalcChannelDpPreviewPopUp::m_UpdateContent(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                                       const uint32_t ou32_ChannelIndex,
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
void C_SdNdeHalcChannelDpPreviewPopUp::mh_AddDpSection(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                                       const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                                       QString & orc_Text)
{
   const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_NodeIndex);
   const C_OscHalcConfigChannel * const pc_Channel =
      C_PuiSdHandler::h_GetInstance()->GetHalcDomainChannelConfigData(ou32_NodeIndex, ou32_DomainIndex,
                                                                      ou32_ChannelIndex, oq_UseChannelIndex);

   orc_Text += "<h3>";
   orc_Text += C_GtGetText::h_GetText("Datapool Information");
   orc_Text += "</h3>";
   if ((pc_Channel != NULL) && (pc_Node != NULL))
   {
      const C_OscNodeDataPool * const pc_Datapool =
         C_PuiSdHandler::h_GetInstance()->GetHalcDatapool(ou32_NodeIndex, pc_Channel->q_SafetyRelevant);
      const C_OscNodeApplication * pc_Appl = NULL;

      orc_Text += "<table style=\"width:100%;\">";
      orc_Text += "<tr>";
      orc_Text += "<td>" + static_cast<QString>(C_GtGetText::h_GetText("Datapool:")) + "</td>";
      orc_Text += "<td>" +
                  static_cast<QString>(C_OscHalcMagicianUtil::h_GetDatapoolName(pc_Channel->q_SafetyRelevant).c_str()) +
                  "</td>";
      orc_Text += "</tr>";
      orc_Text += "<tr>";
      orc_Text += "<td>" + static_cast<QString>(C_GtGetText::h_GetText("Mapped Data Block: ")) + "</td>";

      if ((pc_Datapool != NULL) && (pc_Datapool->s32_RelatedDataBlockIndex >= 0))
      {
         pc_Appl =
            C_PuiSdHandler::h_GetInstance()->GetApplication(ou32_NodeIndex,
                                                            static_cast<uint32_t>(pc_Datapool->s32_RelatedDataBlockIndex));
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
void C_SdNdeHalcChannelDpPreviewPopUp::mh_AddDeSection(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                                       const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                                       QString & orc_Text)
{
   const QString c_Column1HeadingTagStart =
      static_cast<QString>("<td style=\"padding: 30px %1 6px 0px;font-weight: bold;\">").
      arg(C_SdNdeHalcChannelDpPreviewPopUp::mhs32_TABLE_SPACING);
   const QString c_ColumnContentHeadingTagStart =
      static_cast<QString>("<td style=\"padding: 30px %1 6px %1;font-weight: bold;\">").
      arg(C_SdNdeHalcChannelDpPreviewPopUp::mhs32_TABLE_SPACING);
   const C_OscNode * const pc_OscNode = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(ou32_NodeIndex);
   const C_OscHalcConfigChannel * const pc_Channel =
      C_PuiSdHandler::h_GetInstance()->GetHalcDomainChannelConfigData(ou32_NodeIndex, ou32_DomainIndex,
                                                                      ou32_ChannelIndex, oq_UseChannelIndex);

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
   if ((pc_Channel != NULL) && (pc_OscNode != NULL))
   {
      const C_OscHalcMagicianDatapoolListHandler c_DpHandlerListParam(pc_OscNode->c_HalcConfig,
                                                                      C_OscHalcDefDomain::eVA_PARAM,
                                                                      pc_Channel->q_SafetyRelevant);
      const C_OscHalcMagicianDatapoolListHandler c_DpHandlerListInput(pc_OscNode->c_HalcConfig,
                                                                      C_OscHalcDefDomain::eVA_INPUT,
                                                                      pc_Channel->q_SafetyRelevant);
      const C_OscHalcMagicianDatapoolListHandler c_DpHandlerListOutput(pc_OscNode->c_HalcConfig,
                                                                       C_OscHalcDefDomain::eVA_OUTPUT,
                                                                       pc_Channel->q_SafetyRelevant);
      const C_OscHalcMagicianDatapoolListHandler c_DpHandlerListStatus(pc_OscNode->c_HalcConfig,
                                                                       C_OscHalcDefDomain::eVA_STATUS,
                                                                       pc_Channel->q_SafetyRelevant);
      std::vector<uint32_t> c_ParameterIndices;
      std::vector<uint32_t> c_InputIndices;
      std::vector<uint32_t> c_OutputIndices;
      std::vector<uint32_t> c_StatusIndices;
      if (C_PuiSdHandler::h_GetInstance()->GetHalcRelevantIndicesForSelectedUseCase(ou32_NodeIndex, ou32_DomainIndex,
                                                                                    ou32_ChannelIndex,
                                                                                    oq_UseChannelIndex,
                                                                                    &c_ParameterIndices,
                                                                                    &c_InputIndices,
                                                                                    &c_OutputIndices,
                                                                                    &c_StatusIndices) == C_NO_ERR)
      {
         const C_OscHalcConfig * const pc_HalcConfig = C_PuiSdHandler::h_GetInstance()->GetHalcConfig(ou32_NodeIndex);

         if (pc_HalcConfig != NULL)
         {
            const C_OscHalcDefDomain * const pc_Domain = pc_HalcConfig->GetDomainDefDataConst(ou32_DomainIndex);
            const C_OscHalcConfigDomain * const pc_DomainConfig = pc_HalcConfig->GetDomainConfigDataConst(
               ou32_DomainIndex);
            if ((pc_Domain != NULL) && (pc_DomainConfig != NULL))
            {
               const stw::scl::C_SclString c_ListName1 = C_OscHalcMagicianUtil::h_GetListName(
                  C_OscHalcDefDomain::eVA_PARAM);
               const stw::scl::C_SclString c_ListName2 = C_OscHalcMagicianUtil::h_GetListName(
                  C_OscHalcDefDomain::eVA_INPUT);
               const stw::scl::C_SclString c_ListName3 = C_OscHalcMagicianUtil::h_GetListName(
                  C_OscHalcDefDomain::eVA_OUTPUT);
               const stw::scl::C_SclString c_ListName4 = C_OscHalcMagicianUtil::h_GetListName(
                  C_OscHalcDefDomain::eVA_STATUS);
               if (oq_UseChannelIndex)
               {
                  C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListSection(pc_Domain->c_ChannelValues.c_Parameters,
                                                                      c_ParameterIndices, c_ListName1,
                                                                      pc_Domain->c_SingularName, orc_Text,
                                                                      *pc_Domain, *pc_DomainConfig,
                                                                      c_DpHandlerListParam, true);
                  C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListSection(pc_Domain->c_ChannelValues.c_InputValues,
                                                                      c_InputIndices, c_ListName2,
                                                                      pc_Domain->c_SingularName, orc_Text,
                                                                      *pc_Domain, *pc_DomainConfig,
                                                                      c_DpHandlerListInput, false);
                  C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListSection(pc_Domain->c_ChannelValues.c_OutputValues,
                                                                      c_OutputIndices, c_ListName3,
                                                                      pc_Domain->c_SingularName, orc_Text,
                                                                      *pc_Domain, *pc_DomainConfig,
                                                                      c_DpHandlerListOutput, false);
                  C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListSection(pc_Domain->c_ChannelValues.c_StatusValues,
                                                                      c_StatusIndices, c_ListName4,
                                                                      pc_Domain->c_SingularName, orc_Text,
                                                                      *pc_Domain, *pc_DomainConfig,
                                                                      c_DpHandlerListStatus, false);
               }
               else
               {
                  C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListSection(pc_Domain->c_DomainValues.c_Parameters,
                                                                      c_ParameterIndices, c_ListName1,
                                                                      pc_Domain->c_SingularName, orc_Text,
                                                                      *pc_Domain, *pc_DomainConfig,
                                                                      c_DpHandlerListParam, true);
                  C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListSection(pc_Domain->c_DomainValues.c_InputValues,
                                                                      c_InputIndices, c_ListName2,
                                                                      pc_Domain->c_SingularName, orc_Text,
                                                                      *pc_Domain, *pc_DomainConfig,
                                                                      c_DpHandlerListInput, false);
                  C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListSection(pc_Domain->c_DomainValues.c_OutputValues,
                                                                      c_OutputIndices, c_ListName3,
                                                                      pc_Domain->c_SingularName, orc_Text,
                                                                      *pc_Domain, *pc_DomainConfig,
                                                                      c_DpHandlerListOutput, false);
                  C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListSection(pc_Domain->c_DomainValues.c_StatusValues,
                                                                      c_StatusIndices, c_ListName4,
                                                                      pc_Domain->c_SingularName, orc_Text,
                                                                      *pc_Domain, *pc_DomainConfig,
                                                                      c_DpHandlerListStatus, false);
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
void C_SdNdeHalcChannelDpPreviewPopUp::mh_AddUseCase(const uint32_t ou32_NodeIndex, const uint32_t ou32_DomainIndex,
                                                     const uint32_t ou32_ChannelIndex, const bool oq_UseChannelIndex,
                                                     QString & orc_Text)
{
   const QString c_UseCase = "";

   if (oq_UseChannelIndex == true)
   {
      const C_OscHalcConfigChannel * const pc_Channel =
         C_PuiSdHandler::h_GetInstance()->GetHalcDomainChannelConfigData(ou32_NodeIndex, ou32_DomainIndex,
                                                                         ou32_ChannelIndex, oq_UseChannelIndex);
      const C_OscHalcConfigDomain * const pc_Domain =
         C_PuiSdHandler::h_GetInstance()->GetHalcDomainConfigDataConst(ou32_NodeIndex, ou32_DomainIndex);
      if ((pc_Domain != NULL) && (pc_Channel != NULL) &&
          (pc_Channel->u32_UseCaseIndex < pc_Domain->c_ChannelUseCases.size()))
      {
         const C_OscHalcDefChannelUseCase & rc_UseCase = pc_Domain->c_ChannelUseCases[pc_Channel->u32_UseCaseIndex];
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
   \param[in]      orc_DomainConfig          Domain config
   \param[in]      orc_DpHandler             Datapool handler
   \param[in]      oq_AddSpecialVars         Add special vars
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListSection(const std::vector<C_OscHalcDefStruct> & orc_Definition,
                                                         const std::vector<uint32_t> & orc_Indices,
                                                         const stw::scl::C_SclString & orc_ListName,
                                                         const stw::scl::C_SclString & orc_DomainSingularName,
                                                         QString & orc_Text,
                                                         const stw::opensyde_core::C_OscHalcDefDomain & orc_Domain,
                                                         const stw::opensyde_core::C_OscHalcConfigDomain & orc_DomainConfig, const C_OscHalcMagicianDatapoolListHandler & orc_DpHandler,
                                                         const bool oq_AddSpecialVars)
{
   bool q_AddedList = false;

   if (orc_Indices.size() > 0UL)
   {
      const std::vector<uint32_t> c_RelevantChannels = orc_DpHandler.GetRelevantChannels(
         orc_DomainConfig.c_ChannelConfigs, orc_DomainConfig.c_DomainConfig);
      if (oq_AddSpecialVars)
      {
         if (orc_DpHandler.CheckChanNumVariableNecessary(orc_Domain))
         {
            const C_OscNodeDataPoolListElement c_Tmp = C_OscHalcMagicianUtil::h_GetChanNumVariable(
               orc_DomainSingularName, true /*irrelevant here*/, 1UL, false);
            C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListEntry(orc_ListName, q_AddedList, c_Tmp.c_Name, c_Tmp.c_Comment,
                                                              orc_Text);
         }
         if (orc_DpHandler.CheckUseCaseVariableNecessary(orc_Domain))
         {
            const C_OscNodeDataPoolListElement c_Tmp = C_OscHalcMagicianUtil::h_GetUseCaseVariable(
               orc_DomainSingularName, true /*irrelevant here*/, 1UL, false);
            C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListEntry(orc_ListName, q_AddedList, c_Tmp.c_Name, c_Tmp.c_Comment,
                                                              orc_Text);
         }
      }
      for (uint32_t u32_ItIndex = 0UL; u32_ItIndex < orc_Indices.size(); ++u32_ItIndex)
      {
         if (orc_Indices[u32_ItIndex] < orc_Definition.size())
         {
            const C_OscHalcDefStruct & rc_Def = orc_Definition[orc_Indices[u32_ItIndex]];
            //Multiple
            for (uint32_t u32_ItElem = 0UL; u32_ItElem < rc_Def.c_StructElements.size(); ++u32_ItElem)
            {
               const C_OscHalcDefElement & rc_Elem = rc_Def.c_StructElements[u32_ItElem];
               mh_AddElementSection(rc_Elem, orc_Definition, orc_Indices[u32_ItIndex], orc_DomainSingularName,
                                    u32_ItElem,
                                    c_RelevantChannels, orc_ListName, q_AddedList, orc_Text);
            }
            //Single
            if (rc_Def.c_StructElements.size() == 0UL)
            {
               mh_AddElementSection(rc_Def, orc_Definition, orc_Indices[u32_ItIndex], orc_DomainSingularName, 0UL,
                                    c_RelevantChannels, orc_ListName, q_AddedList, orc_Text);
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
} //lint !e438   only care about final value of q_AddedList

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add element section

   \param[in]      orc_HalDefElement         Hal definition element
   \param[in]      orc_Definition            Definition
   \param[in]      ou32_Index                Index
   \param[in]      orc_DomainSingularName    Domain singular name
   \param[in]      ou32_ElementIndex         Element index
   \param[in]      orc_RelevantChannels      Relevant channels
   \param[in]      orc_ListName              List name
   \param[out]     orq_AddedList             Added list
   \param[in,out]  orc_Text                  Text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeHalcChannelDpPreviewPopUp::mh_AddElementSection(const C_OscHalcDefElement & orc_HalDefElement,
                                                            const std::vector<C_OscHalcDefStruct> & orc_Definition,
                                                            const uint32_t ou32_Index,
                                                            const stw::scl::C_SclString & orc_DomainSingularName,
                                                            const uint32_t ou32_ElementIndex,
                                                            const std::vector<uint32_t> & orc_RelevantChannels,
                                                            const stw::scl::C_SclString & orc_ListName,
                                                            bool & orq_AddedList, QString & orc_Text)
{
   stw::scl::C_SclString c_Name;
   if (orc_HalDefElement.GetComplexType() == C_OscHalcDefContent::eCT_STRING)
   {
      for (uint32_t u32_ItCh = 0UL; u32_ItCh < orc_RelevantChannels.size(); ++u32_ItCh)
      {
         C_OscHalcMagicianUtil::h_GetVariableName(orc_Definition, ou32_Index, ou32_ElementIndex,
                                                  orc_DomainSingularName, c_Name,
                                                  orc_RelevantChannels[u32_ItCh]);
         C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListEntry(orc_ListName, orq_AddedList, c_Name,
                                                           orc_HalDefElement.c_Comment,
                                                           orc_Text);
      }
   }
   else
   {
      C_OscHalcMagicianUtil::h_GetVariableName(orc_Definition, ou32_Index, ou32_ElementIndex,
                                               orc_DomainSingularName, c_Name);
      C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListEntry(orc_ListName, orq_AddedList, c_Name,
                                                        orc_HalDefElement.c_Comment,
                                                        orc_Text);
   }
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
void C_SdNdeHalcChannelDpPreviewPopUp::mh_AddListEntry(const stw::scl::C_SclString & orc_ListName, bool & orq_ListAdded,
                                                       const stw::scl::C_SclString & orc_VarName,
                                                       const stw::scl::C_SclString & orc_VarComment, QString & orc_Text)
{
   const QString c_Column1TagStart = static_cast<QString>("<td style=\"padding: 0 %1 0 0px;\">").arg(
      C_SdNdeHalcChannelDpPreviewPopUp::mhs32_TABLE_SPACING);
   const QString c_ColumnContentTagStart = static_cast<QString>(
      "<td valign=\"middle\" style=\"padding: 0 %1 0 %1;\">").arg(C_SdNdeHalcChannelDpPreviewPopUp::mhs32_TABLE_SPACING);

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
