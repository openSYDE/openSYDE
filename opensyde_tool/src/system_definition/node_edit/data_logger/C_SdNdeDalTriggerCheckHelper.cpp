//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Helper class to check trigger expressions

   Helper class to check trigger expressions

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "TglUtils.hpp"
#include "C_OscUtils.hpp"
#include "C_GtGetText.hpp"
#include "C_PuiSdUtil.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_OscHalcMagicianUtil.hpp"
#include "C_SdNdeDalTriggerCheckHelper.hpp"
#include "C_OscHalcMagicianDatapoolListHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
using namespace stw::tgl;
using namespace data::monitor;
using namespace data::container;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const uint32_t C_SdNdeDalTriggerCheckHelper::mhu32_DUMMY_CHANNEL_ID = 0UL;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check

   \param[in]      ou32_NodeIndex         Node index
   \param[in]      orc_Expression         Expression
   \param[in,out]  opc_ErrorDetails       Error details
   \param[out]     opq_AreVariablesValid  Are variables valid
   \param[out]     opq_IsSyntaxValid      Is syntax valid

   \return
   Flags

   \retval   True    Expression valid
   \retval   False   Expression not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalTriggerCheckHelper::h_Check(const uint32_t ou32_NodeIndex, const C_SclString & orc_Expression,
                                           std::string * const opc_ErrorDetails, bool * const opq_AreVariablesValid,
                                           bool * const opq_IsSyntaxValid)
{
   std::vector<Token> c_Tokens;
   C_SclString c_Expression = orc_Expression;
   bool q_IsValid;
   if (opq_AreVariablesValid != NULL)
   {
      *opq_AreVariablesValid = true;
   }
   if (opq_IsSyntaxValid != NULL)
   {
      *opq_IsSyntaxValid = true;
   }

   C_OscDataLoggerJobAdditionalTriggerExpertMode::h_ReplaceUiVarNames(c_Expression);

   q_IsValid = mh_ParseTokens(c_Expression, c_Tokens, opc_ErrorDetails);
   if (q_IsValid)
   {
      q_IsValid = mh_CheckTokens(ou32_NodeIndex, c_Tokens, opc_ErrorDetails);
   }
   else
   {
      if (opq_IsSyntaxValid != NULL)
      {
         *opq_IsSyntaxValid = false;
      }
   }
   if (q_IsValid)
   {
      q_IsValid = mh_ReplaceChannelTokens(c_Tokens, opc_ErrorDetails);
   }
   else
   {
      if (opq_AreVariablesValid != NULL)
      {
         *opq_AreVariablesValid = false;
      }
   }
   if (q_IsValid)
   {
      q_IsValid = mh_CheckSyntax(c_Tokens, opc_ErrorDetails);
   }
   else
   {
      if (opq_IsSyntaxValid != NULL)
      {
         *opq_IsSyntaxValid = false;
      }
   }
   if (!q_IsValid)
   {
      if (opq_IsSyntaxValid != NULL)
      {
         *opq_IsSyntaxValid = false;
      }
   }
   return q_IsValid;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Parse data elements

   \param[in]  orc_Condition  Condition

   \return
   Parsed data elements
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_OscNodeDataPoolListElementOptArrayId> C_SdNdeDalTriggerCheckHelper::h_ParseDataElements(
   const stw::scl::C_SclString & orc_Condition)
{
   std::vector<C_OscNodeDataPoolListElementOptArrayId> c_Retval;
   C_OscNodeDataPoolListElementOptArrayId c_ElementId;
   const std::vector<stw::scl::C_SclString> c_Strings = mh_ParseDataElementStrings(orc_Condition);
   c_Retval.reserve(c_Strings.size());
   for (uint32_t u32_It = 0UL; u32_It < c_Strings.size(); ++u32_It)
   {
      const bool q_IsVarValid = mh_TranslateDataElementStringToId(c_Strings[u32_It], c_ElementId);
      if (q_IsVarValid)
      {
         c_Retval.push_back(c_ElementId);
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDalTriggerCheckHelper::C_SdNdeDalTriggerCheckHelper()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Parse tokens

   \param[in]      orc_Expression      Expression
   \param[in,out]  orc_Tokens          Tokens
   \param[in,out]  opc_ErrorDetails    Error details

   \return
   Flags

   \retval   True    Expression valid
   \retval   False   Expression not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalTriggerCheckHelper::mh_ParseTokens(const scl::C_SclString & orc_Expression,
                                                  std::vector<data::monitor::Token> & orc_Tokens,
                                                  std::string * const opc_ErrorDetails)
{
   bool q_IsValid = true;

   try
   {
      orc_Tokens = Lexer().lex(orc_Expression.c_str());
   }
   catch (const std::runtime_error & rc_Error)
   {
      q_IsValid = false;
      if (opc_ErrorDetails != NULL)
      {
         *opc_ErrorDetails = rc_Error.what();
      }
   }
   catch (...)
   {
      q_IsValid = false;
      if (opc_ErrorDetails != NULL)
      {
         *opc_ErrorDetails = "Parsing expression: Unknown error";
      }
   }
   return q_IsValid;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Replace channel tokens

   \param[in,out]  orc_Tokens          Tokens
   \param[in,out]  opc_ErrorDetails    Error details

   \return
   Flags

   \retval   True    Expression valid
   \retval   False   Expression not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalTriggerCheckHelper::mh_ReplaceChannelTokens(std::vector<data::monitor::Token> & orc_Tokens,
                                                           std::string * const opc_ErrorDetails)
{
   bool q_IsValid = true;

   try
   {
      for (uint32_t u32_ItToken = 0UL; u32_ItToken < orc_Tokens.size(); ++u32_ItToken)
      {
         data::monitor::Token & rc_Token = orc_Tokens[u32_ItToken];
         if (rc_Token.type == data::monitor::Token::Type::Channel)
         {
            rc_Token.value.emplace(mhu32_DUMMY_CHANNEL_ID);
         }
         else if (rc_Token.type == data::monitor::Token::Type::ConstantValue)
         {
            const C_SclString c_Tmp(std::get<std::string>(rc_Token.value.value()));
            //lint -e{529} Variable is used
            const float32_t f32_Value = static_cast<float32_t>(c_Tmp.ToDouble());
            rc_Token.value.emplace(f32_Value);
         }
         else
         {
            //unchanged
         }
      }
   }
   catch (const std::runtime_error & rc_Error)
   {
      q_IsValid = false;
      if (opc_ErrorDetails != NULL)
      {
         *opc_ErrorDetails = rc_Error.what();
      }
   }
   catch (...)
   {
      q_IsValid = false;
      if (opc_ErrorDetails != NULL)
      {
         *opc_ErrorDetails = "Converting constant: Unknown error";
      }
   }
   return q_IsValid;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Initialize data container

   \return
   Initialized data container
*/
//----------------------------------------------------------------------------------------------------------------------
ChannelDataContainer C_SdNdeDalTriggerCheckHelper::mh_InitializeDataContainer(void)
{
   const Scalar<float32_t> c_Value(0.0F);
   ChannelDataContainer c_Container;

   c_Container.create_initialized_channel(mhu32_DUMMY_CHANNEL_ID, c_Value);
   return c_Container;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check syntax

   \param[in,out]  orc_Tokens          Tokens
   \param[in,out]  opc_ErrorDetails    Error details

   \return
   Flags

   \retval   True    Expression valid
   \retval   False   Expression not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalTriggerCheckHelper::mh_CheckSyntax(const std::vector<Token> & orc_Tokens,
                                                  std::string * const opc_ErrorDetails)
{
   bool q_IsValid = true;

   try
   {
      const ChannelDataContainer c_Context = mh_InitializeDataContainer();
      ExpressionBuilder c_Builder(orc_Tokens, c_Context);
      c_Builder.create();
   }
   catch (const std::runtime_error & rc_Error)
   {
      q_IsValid = false;
      if (opc_ErrorDetails != NULL)
      {
         *opc_ErrorDetails = rc_Error.what();
      }
   }
   catch (...)
   {
      q_IsValid = false;
      if (opc_ErrorDetails != NULL)
      {
         *opc_ErrorDetails = "Checking syntax: Unknown error";
      }
   }
   return q_IsValid;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check tokens

   \param[in]      ou32_NodeIndex      Node index
   \param[in]      orc_Tokens          Tokens
   \param[in,out]  opc_ErrorDetails    Error details

   \return
   Flags

   \retval   True    Expression valid
   \retval   False   Expression not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalTriggerCheckHelper::mh_CheckTokens(const uint32_t ou32_NodeIndex,
                                                  const std::vector<data::monitor::Token> & orc_Tokens,
                                                  std::string * const opc_ErrorDetails)
{
   bool q_IsValid = true;

   if (opc_ErrorDetails != NULL)
   {
      *opc_ErrorDetails = "";
   }

   for (uint32_t u32_ItToken = 0UL; (u32_ItToken < orc_Tokens.size()) && (q_IsValid == true); ++u32_ItToken)
   {
      const data::monitor::Token & rc_Token = orc_Tokens[u32_ItToken];
      if (rc_Token.type == data::monitor::Token::Type::Channel)
      {
         const std::vector<std::string> c_Components = std::get<std::vector<std::string> >(rc_Token.value.value());
         q_IsValid = mh_CheckChannel(ou32_NodeIndex, c_Components, opc_ErrorDetails);
      }
   }

   return q_IsValid;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check channel

   \param[in]      ou32_NodeIndex         Node index
   \param[in]      orc_ChannelComponents  Channel components
   \param[in,out]  opc_ErrorDetails       Error details

   \return
   Flags

   \retval   True    Expression valid
   \retval   False   Expression not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalTriggerCheckHelper::mh_CheckChannel(const uint32_t ou32_NodeIndex,
                                                   const std::vector<std::string> & orc_ChannelComponents,
                                                   std::string * const opc_ErrorDetails)
{
   bool q_IsValid = false;

   if (orc_ChannelComponents.size() > 5UL)
   {
      if (opc_ErrorDetails != NULL)
      {
         *opc_ErrorDetails = C_GtGetText::h_GetText("Variable invalid: too many components");
      }
   }
   else if (orc_ChannelComponents.size() > 3UL)
   {
      const std::string & rc_UserNodeName = orc_ChannelComponents[0UL];
      for (uint32_t u32_ItNode = 0UL; u32_ItNode < C_PuiSdHandler::h_GetInstance()->GetOscNodesSize();
           ++u32_ItNode)
      {
         const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_ItNode);
         tgl_assert(pc_Node != NULL);
         if (pc_Node != NULL)
         {
            if (pc_Node->c_Properties.c_Name == rc_UserNodeName)
            {
               if (C_PuiSdUtil::h_CheckXappNodeReachable(ou32_NodeIndex, u32_ItNode))
               {
                  q_IsValid = mh_CheckChannelDatapool(*pc_Node, orc_ChannelComponents, opc_ErrorDetails, NULL);
               }
               else
               {
                  if (opc_ErrorDetails != NULL)
                  {
                     *opc_ErrorDetails = C_GtGetText::h_GetText("Variable invalid: node not reachable");
                  }
               }
            }
         }
      }
      if (opc_ErrorDetails != NULL)
      {
         if ((q_IsValid == false) && (*opc_ErrorDetails == ""))
         {
            *opc_ErrorDetails = C_GtGetText::h_GetText("Variable invalid: variable not found");
         }
      }
   }
   else
   {
      if (opc_ErrorDetails != NULL)
      {
         *opc_ErrorDetails = C_GtGetText::h_GetText("Variable invalid: too few components");
      }
   }
   return q_IsValid;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check channel datapool

   \param[in]      orc_Node               Node
   \param[in]      orc_ChannelComponents  Channel components
   \param[in,out]  opc_ErrorDetails       Error details
   \param[in,out]  opc_ElementId          Element id

   \return
   Flags

   \retval   True    Expression valid
   \retval   False   Expression not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalTriggerCheckHelper::mh_CheckChannelDatapool(const opensyde_core::C_OscNode & orc_Node,
                                                           const std::vector<std::string> & orc_ChannelComponents,
                                                           std::string * const opc_ErrorDetails,
                                                           C_OscNodeDataPoolListElementOptArrayId * const opc_ElementId)
{
   bool q_IsValid = false;

   tgl_assert(orc_ChannelComponents.size() > 3UL);
   if (orc_ChannelComponents.size() > 3UL)
   {
      const std::string & rc_UserDatapoolName = orc_ChannelComponents[1UL];
      for (uint32_t u32_ItDatapool = 0UL; u32_ItDatapool < orc_Node.c_DataPools.size(); ++u32_ItDatapool)
      {
         const C_OscNodeDataPool & rc_Datapool = orc_Node.c_DataPools[u32_ItDatapool];
         if (rc_Datapool.c_Name == rc_UserDatapoolName)
         {
            if (opc_ElementId != NULL)
            {
               opc_ElementId->u32_DataPoolIndex = u32_ItDatapool;
            }
            if ((rc_Datapool.e_Type != C_OscNodeDataPool::eHALC) &&
                (rc_Datapool.e_Type != C_OscNodeDataPool::eHALC_NVM) &&
                (rc_Datapool.e_Type != C_OscNodeDataPool::eCOM))
            {
               q_IsValid = mh_CheckChannelList(rc_Datapool, orc_ChannelComponents, opc_ErrorDetails, opc_ElementId);
            }
            else if (rc_Datapool.e_Type == C_OscNodeDataPool::eCOM)
            {
               const C_OscCanProtocol * const pc_Protocol = orc_Node.GetRelatedCanProtocolConst(u32_ItDatapool);
               if (pc_Protocol != NULL)
               {
                  q_IsValid = mh_CheckChannelComm(rc_Datapool, *pc_Protocol, orc_ChannelComponents,
                                                  opc_ErrorDetails, opc_ElementId);
               }
               else
               {
                  if (opc_ErrorDetails != NULL)
                  {
                     *opc_ErrorDetails = C_GtGetText::h_GetText(
                        "Variable invalid: No matching communication protocol structure found");
                  }
               }
            }
            else
            {
               q_IsValid = mh_CheckChannelHalType(orc_Node, rc_Datapool, orc_ChannelComponents, opc_ErrorDetails,
                                                  opc_ElementId);
            }
         }
      }
   }

   return q_IsValid;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check channel hal

   \param[in]      orc_Node               Node
   \param[in]      orc_Datapool           Datapool
   \param[in]      orc_ChannelComponents  Channel components
   \param[in,out]  opc_ErrorDetails       Error details
   \param[in,out]  opc_ElementId          Element id

   \return
   Flags

   \retval   True    Expression valid
   \retval   False   Expression not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalTriggerCheckHelper::mh_CheckChannelHalType(const opensyde_core::C_OscNode & orc_Node,
                                                          const opensyde_core::C_OscNodeDataPool & orc_Datapool,
                                                          const std::vector<std::string> & orc_ChannelComponents,
                                                          std::string * const opc_ErrorDetails,
                                                          C_OscNodeDataPoolListElementOptArrayId * const opc_ElementId)
{
   bool q_IsValid = false;

   if (orc_ChannelComponents.size() == 5UL)
   {
      const std::string & rc_ListName = orc_ChannelComponents[3UL];
      C_OscHalcDefDomain::E_VariableSelector e_Type;
      const bool q_ListIsValid = mh_ConvertHalListToType(rc_ListName, e_Type);
      if (q_ListIsValid)
      {
         const std::string & rc_ChannelName = orc_ChannelComponents[2UL];
         bool q_UseArrayIndex = false;
         uint32_t u32_ArrayIndex = 0UL;
         bool q_Found = false;
         for (uint32_t u32_ItDomain = 0UL; (u32_ItDomain < orc_Node.c_HalcConfig.GetDomainSize()) && (q_Found == false);
              ++u32_ItDomain)
         {
            const C_OscHalcConfigDomain * const pc_Domain =
               orc_Node.c_HalcConfig.GetDomainConfigDataConst(u32_ItDomain);
            if (pc_Domain != NULL)
            {
               if (pc_Domain->c_ChannelConfigs.size() > 0UL)
               {
                  const C_OscHalcMagicianDatapoolListHandler c_Helper(orc_Node.c_HalcConfig, e_Type,
                                                                      orc_Datapool.q_IsSafety);
                  for (uint32_t u32_ItChan = 0UL; u32_ItChan < pc_Domain->c_ChannelConfigs.size(); ++u32_ItChan)
                  {
                     const C_OscHalcConfigChannel & rc_Chan = pc_Domain->c_ChannelConfigs[u32_ItChan];
                     if (c_Helper.CheckChanPresent(rc_Chan))
                     {
                        if (rc_Chan.c_Name == rc_ChannelName)
                        {
                           q_Found = true;
                           q_UseArrayIndex = true;
                           break;
                        }
                        ++u32_ArrayIndex;
                     }
                  }
               }
               else
               {
                  if (pc_Domain->c_Name == rc_ChannelName)
                  {
                     q_Found = true;
                  }
               }
               if (q_Found)
               {
                  const std::string & rc_VarName = orc_ChannelComponents[4UL];
                  const std::string c_CompleteVarName = C_OscHalcMagicianUtil::h_CombineVariableName(
                     pc_Domain->c_SingularName,
                     rc_VarName).c_str();
                  if (opc_ElementId != NULL)
                  {
                     *opc_ElementId = C_OscNodeDataPoolListElementOptArrayId(*opc_ElementId, q_UseArrayIndex,
                                                                             u32_ArrayIndex);
                     opc_ElementId->SetHalChannelName(rc_ChannelName);
                  }
                  q_IsValid = mh_CheckChannelList(orc_Datapool, rc_ListName, c_CompleteVarName, q_UseArrayIndex,
                                                  u32_ArrayIndex,
                                                  opc_ErrorDetails, opc_ElementId);
               }
            }
         }
      }
      else
      {
         if (opc_ErrorDetails != NULL)
         {
            *opc_ErrorDetails = C_GtGetText::h_GetText("Variable invalid: HAL unknown list type");
         }
      }
   }
   else
   {
      if (opc_ErrorDetails != NULL)
      {
         *opc_ErrorDetails = C_GtGetText::h_GetText("Variable invalid: too few components for HAL");
      }
   }

   return q_IsValid;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert hal list to type

   \param[in]   orc_ListName  List name
   \param[out]  ore_Type      Type

   \return
   Flags

   \retval   True    Expression valid
   \retval   False   Expression not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalTriggerCheckHelper::mh_ConvertHalListToType(const scl::C_SclString & orc_ListName,
                                                           C_OscHalcDefDomain::E_VariableSelector & ore_Type)
{
   bool q_IsValid = false;

   if (C_OscHalcMagicianUtil::h_GetListName(C_OscHalcDefDomain::eVA_PARAM) == orc_ListName)
   {
      ore_Type = C_OscHalcDefDomain::eVA_PARAM;
      q_IsValid = true;
   }
   else if (C_OscHalcMagicianUtil::h_GetListName(C_OscHalcDefDomain::eVA_INPUT) == orc_ListName)
   {
      ore_Type = C_OscHalcDefDomain::eVA_INPUT;
      q_IsValid = true;
   }
   else if (C_OscHalcMagicianUtil::h_GetListName(C_OscHalcDefDomain::eVA_OUTPUT) == orc_ListName)
   {
      ore_Type = C_OscHalcDefDomain::eVA_OUTPUT;
      q_IsValid = true;
   }
   else if (C_OscHalcMagicianUtil::h_GetListName(C_OscHalcDefDomain::eVA_STATUS) == orc_ListName)
   {
      ore_Type = C_OscHalcDefDomain::eVA_STATUS;
      q_IsValid = true;
   }
   else
   {
      //Unknown
   }
   return q_IsValid;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check channel list

   \param[in]      orc_Datapool           Datapool
   \param[in]      orc_ChannelComponents  Channel components
   \param[in,out]  opc_ErrorDetails       Error details
   \param[in,out]  opc_ElementId          Element id

   \return
   Flags

   \retval   True    Expression valid
   \retval   False   Expression not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalTriggerCheckHelper::mh_CheckChannelList(const opensyde_core::C_OscNodeDataPool & orc_Datapool,
                                                       const std::vector<std::string> & orc_ChannelComponents,
                                                       std::string * const opc_ErrorDetails,
                                                       C_OscNodeDataPoolListElementOptArrayId * const opc_ElementId)
{
   bool q_IsValid = false;

   tgl_assert(orc_ChannelComponents.size() > 3UL);
   if (orc_ChannelComponents.size() > 3UL)
   {
      bool q_Continue = true;
      bool q_UseArrayIndex = false;
      uint32_t u32_ArrayIndex = 0UL;
      if (orc_ChannelComponents.size() > 4UL)
      {
         const std::string & rc_ArrayIndex = orc_ChannelComponents[4UL];
         const C_SclString c_ArrayIndexScl = rc_ArrayIndex;
         q_Continue = false;
         q_UseArrayIndex = true;
         try
         {
            const int32_t s32_Index = c_ArrayIndexScl.ToInt();
            if (s32_Index >= 0)
            {
               u32_ArrayIndex = static_cast<uint32_t>(s32_Index);
               q_Continue = true;
            }
            else
            {
               if (opc_ErrorDetails != NULL)
               {
                  *opc_ErrorDetails = C_GtGetText::h_GetText("Variable index invalid: int negative");
               }
            }
         }
         catch (...)
         {
            if (opc_ErrorDetails != NULL)
            {
               *opc_ErrorDetails = C_GtGetText::h_GetText("Variable index invalid: could not parse int");
            }
         }
      }
      if (q_Continue)
      {
         const std::string & rc_UserListName = orc_ChannelComponents[2UL];
         const std::string & rc_UserElementName = orc_ChannelComponents[3UL];
         if (opc_ElementId != NULL)
         {
            *opc_ElementId = C_OscNodeDataPoolListElementOptArrayId(*opc_ElementId, q_UseArrayIndex,
                                                                    u32_ArrayIndex);
         }
         q_IsValid = mh_CheckChannelList(orc_Datapool, rc_UserListName, rc_UserElementName, q_UseArrayIndex,
                                         u32_ArrayIndex, opc_ErrorDetails, opc_ElementId);
      }
   }

   return q_IsValid;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check channel comm

   \param[in]      orc_Datapool           Datapool
   \param[in]      orc_ComProtocol        Com protocol
   \param[in]      orc_ChannelComponents  Channel components
   \param[in,out]  opc_ErrorDetails       Error details
   \param[in,out]  opc_ElementId          Element id

   \return
   Flags

   \retval   True    Expression valid
   \retval   False   Expression not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalTriggerCheckHelper::mh_CheckChannelComm(const C_OscNodeDataPool & orc_Datapool,
                                                       const C_OscCanProtocol & orc_ComProtocol,
                                                       const std::vector<std::string> & orc_ChannelComponents,
                                                       std::string * const opc_ErrorDetails,
                                                       C_OscNodeDataPoolListElementOptArrayId * const opc_ElementId)
{
   bool q_IsValid = false;

   tgl_assert(orc_ChannelComponents.size() > 3UL);
   if (orc_ChannelComponents.size() > 3UL)
   {
      if (orc_ChannelComponents.size() > 4UL)
      {
         if (opc_ErrorDetails != NULL)
         {
            *opc_ErrorDetails = C_GtGetText::h_GetText(
               "Variable invalid: No array types supported for communication");
         }
      }
      else
      {
         const std::string & rc_UserMessageName = orc_ChannelComponents[2UL];
         const std::string & rc_UserSignalName = orc_ChannelComponents[3UL];
         bool q_Found = false;
         for (uint32_t u32_ItContainer = 0UL;
              (u32_ItContainer < orc_ComProtocol.c_ComMessages.size()) && (q_Found == false); ++u32_ItContainer)
         {
            const C_OscCanMessageContainer & rc_Container = orc_ComProtocol.c_ComMessages[u32_ItContainer];
            q_IsValid = mh_CheckChannelMessages(orc_Datapool, rc_Container.c_TxMessages, rc_UserMessageName,
                                                rc_UserSignalName, true, q_Found, opc_ErrorDetails,
                                                opc_ElementId);
            if (q_Found == false)
            {
               q_IsValid = mh_CheckChannelMessages(orc_Datapool, rc_Container.c_RxMessages, rc_UserMessageName,
                                                   rc_UserSignalName, false, q_Found, opc_ErrorDetails,
                                                   opc_ElementId);
            }
         }
      }
   }

   return q_IsValid;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check channel messages

   \param[in]      orc_Datapool        Datapool
   \param[in]      orc_Messages        Messages
   \param[in]      orc_MessageName     Message name
   \param[in]      orc_SignalName      Signal name
   \param[in]      oq_IsTx             Is tx
   \param[out]     orq_Found           Found
   \param[in,out]  opc_ErrorDetails    Error details
   \param[in,out]  opc_ElementId       Element id

   \return
   Flags

   \retval   True    Expression valid
   \retval   False   Expression not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalTriggerCheckHelper::mh_CheckChannelMessages(const C_OscNodeDataPool & orc_Datapool,
                                                           const std::vector<C_OscCanMessage> & orc_Messages,
                                                           const std::string & orc_MessageName,
                                                           const std::string & orc_SignalName, const bool oq_IsTx,
                                                           bool & orq_Found, std::string * const opc_ErrorDetails,
                                                           C_OscNodeDataPoolListElementOptArrayId * const opc_ElementId)
{
   bool q_IsValid = false;

   orq_Found = false;

   for (uint32_t u32_ItMessage = 0UL; (u32_ItMessage < orc_Messages.size()) && (orq_Found == false); ++u32_ItMessage)
   {
      const C_OscCanMessage & rc_Message = orc_Messages[u32_ItMessage];
      if (rc_Message.c_Name == orc_MessageName)
      {
         orq_Found = true;
         for (uint32_t u32_ItSignal = 0UL; (u32_ItSignal < rc_Message.c_Signals.size()) && (q_IsValid == false);
              ++u32_ItSignal)
         {
            const C_OscCanSignal & rc_Signal = rc_Message.c_Signals[u32_ItSignal];
            q_IsValid = mh_CheckChannelSignal(orc_Datapool, rc_Signal, orc_SignalName, oq_IsTx, opc_ElementId);
         }
         if (opc_ErrorDetails != NULL)
         {
            if (q_IsValid == false)
            {
               *opc_ErrorDetails =
                  C_GtGetText::h_GetText("Variable index invalid: signal not found in message");
            }
         }
      }
   }

   return q_IsValid;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check channel signal

   \param[in]      orc_Datapool     Datapool
   \param[in]      orc_Signal       Signal
   \param[in]      orc_SignalName   Signal name
   \param[in]      oq_IsTx          Is tx
   \param[in,out]  opc_ElementId    Element id

   \return
   Flags

   \retval   True    Expression valid
   \retval   False   Expression not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalTriggerCheckHelper::mh_CheckChannelSignal(const opensyde_core::C_OscNodeDataPool & orc_Datapool,
                                                         const opensyde_core::C_OscCanSignal & orc_Signal,
                                                         const std::string & orc_SignalName, const bool oq_IsTx,
                                                         opensyde_core::C_OscNodeDataPoolListElementOptArrayId * const opc_ElementId)
{
   bool q_IsValid = false;

   for (uint32_t u32_ItList = 0UL; u32_ItList < orc_Datapool.c_Lists.size(); ++u32_ItList)
   {
      const C_OscNodeDataPoolList & rc_List = orc_Datapool.c_Lists[u32_ItList];
      if (C_OscCanProtocol::h_ListIsComTx(rc_List) == oq_IsTx)
      {
         if (orc_Signal.u32_ComDataElementIndex < rc_List.c_Elements.size())
         {
            const C_OscNodeDataPoolListElement & rc_Element =
               rc_List.c_Elements[orc_Signal.u32_ComDataElementIndex];
            if (rc_Element.c_Name == orc_SignalName)
            {
               q_IsValid = true;
               if (opc_ElementId != NULL)
               {
                  opc_ElementId->u32_ListIndex = u32_ItList;
                  opc_ElementId->u32_ElementIndex = orc_Signal.u32_ComDataElementIndex;
               }
            }
         }
      }
   }
   return q_IsValid;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check channel list

   \param[in]      orc_Datapool        Datapool
   \param[in]      orc_ListName        List name
   \param[in]      orc_ElementName     Element name
   \param[in]      oq_UseArrayIndex    Use array index
   \param[in]      ou32_ArrayIndex     Array index
   \param[in,out]  opc_ErrorDetails    Error details
   \param[in,out]  opc_ElementId       Element id

   \return
   Flags

   \retval   True    Expression valid
   \retval   False   Expression not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalTriggerCheckHelper::mh_CheckChannelList(const opensyde_core::C_OscNodeDataPool & orc_Datapool,
                                                       const std::string & orc_ListName,
                                                       const std::string & orc_ElementName, const bool oq_UseArrayIndex,
                                                       const uint32_t ou32_ArrayIndex,
                                                       std::string * const opc_ErrorDetails,
                                                       C_OscNodeDataPoolListElementOptArrayId * const opc_ElementId)
{
   bool q_IsValid = false;

   for (uint32_t u32_ItList = 0UL; u32_ItList < orc_Datapool.c_Lists.size(); ++u32_ItList)
   {
      const C_OscNodeDataPoolList & rc_List = orc_Datapool.c_Lists[u32_ItList];
      if (rc_List.c_Name == orc_ListName)
      {
         if (opc_ElementId != NULL)
         {
            opc_ElementId->u32_ListIndex = u32_ItList;
         }
         q_IsValid = mh_CheckChannelElement(rc_List, orc_ElementName, oq_UseArrayIndex, ou32_ArrayIndex,
                                            opc_ErrorDetails, opc_ElementId);
      }
   }

   return q_IsValid;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check channel element

   \param[in]      orc_List            List
   \param[in]      orc_ElementName     Element name
   \param[in]      oq_UseArrayIndex    Use array index
   \param[in]      ou32_ArrayIndex     Array index
   \param[in,out]  opc_ErrorDetails    Error details
   \param[in,out]  opc_ElementId       Element id

   \return
   Flags

   \retval   True    Expression valid
   \retval   False   Expression not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalTriggerCheckHelper::mh_CheckChannelElement(const opensyde_core::C_OscNodeDataPoolList & orc_List,
                                                          const std::string & orc_ElementName,
                                                          const bool oq_UseArrayIndex, const uint32_t ou32_ArrayIndex,
                                                          std::string * const opc_ErrorDetails,
                                                          C_OscNodeDataPoolListElementOptArrayId * const opc_ElementId)
{
   bool q_IsValid = false;

   for (uint32_t u32_ItElement = 0UL; u32_ItElement < orc_List.c_Elements.size(); ++u32_ItElement)
   {
      const C_OscNodeDataPoolListElement & rc_Element  = orc_List.c_Elements[u32_ItElement];
      if (rc_Element.c_Name == orc_ElementName)
      {
         if (opc_ElementId != NULL)
         {
            opc_ElementId->u32_ElementIndex = u32_ItElement;
         }
         if (oq_UseArrayIndex == false)
         {
            if ((rc_Element.GetArray()) && (rc_Element.q_InterpretAsString == false))
            {
               if (opc_ErrorDetails != NULL)
               {
                  *opc_ErrorDetails =
                     C_GtGetText::h_GetText("Variable index invalid: invalid array type usage");
               }
            }
            else
            {
               q_IsValid = true;
            }
         }
         else
         {
            q_IsValid = mh_CheckChannelElementArray(rc_Element, ou32_ArrayIndex, opc_ErrorDetails);
         }
      }
   }

   return q_IsValid;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check channel element array

   \param[in]      orc_Element         Element
   \param[in]      ou32_ArrayIndex     Array index
   \param[in,out]  opc_ErrorDetails    Error details

   \return
   Flags

   \retval   True    Expression valid
   \retval   False   Expression not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalTriggerCheckHelper::mh_CheckChannelElementArray(
   const opensyde_core::C_OscNodeDataPoolListElement & orc_Element, const uint32_t ou32_ArrayIndex,
   std::string * const opc_ErrorDetails)
{
   bool q_IsValid = false;

   if (orc_Element.GetArray())
   {
      if (ou32_ArrayIndex < orc_Element.GetArraySize())
      {
         q_IsValid = true;
      }
      else
      {
         if (opc_ErrorDetails != NULL)
         {
            *opc_ErrorDetails =
               C_GtGetText::h_GetText("Variable index invalid: index out of bounds");
         }
      }
   }
   else
   {
      if (opc_ErrorDetails != NULL)
      {
         *opc_ErrorDetails =
            C_GtGetText::h_GetText("Variable index invalid: variable is not of type array");
      }
   }

   return q_IsValid;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Parse data element strings

   \param[in]  orc_Condition  Condition

   \return
   Parsed data element strings
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<stw::scl::C_SclString> C_SdNdeDalTriggerCheckHelper::mh_ParseDataElementStrings(
   const stw::scl::C_SclString & orc_Condition)
{
   std::vector<stw::scl::C_SclString> c_Retval;
   stw::scl::C_SclString c_Copy = orc_Condition;
   const std::string * pc_NormalString;
   C_OscDataLoggerJobAdditionalTriggerExpertMode::h_ReplaceUiVarNames(c_Copy);
   pc_NormalString = c_Copy.AsStdString();

   tgl_assert(pc_NormalString != NULL);
   if (pc_NormalString != NULL)
   {
      std::string c_CurVariable;
      for (std::string::const_iterator c_ItChar = pc_NormalString->cbegin(); c_ItChar != pc_NormalString->cend();
           ++c_ItChar)
      {
         const bool q_IsPartOfVariable = C_OscUtils::h_CheckValidCeName(*c_ItChar, true) || (*c_ItChar == '.');
         if (q_IsPartOfVariable)
         {
            c_CurVariable += *c_ItChar;
         }
         else
         {
            if (c_CurVariable.size() > 0UL)
            {
               c_Retval.push_back(c_CurVariable);
               c_CurVariable = "";
            }
         }
      }
      if (c_CurVariable.size() > 0UL)
      {
         c_Retval.push_back(c_CurVariable);
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Translate data element string to id

   \param[in]      orc_Element      Element
   \param[in,out]  orc_ElementId    Element id

   \return
   Flags

   \retval   True    Expression valid
   \retval   False   Expression not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalTriggerCheckHelper::mh_TranslateDataElementStringToId(const C_SclString & orc_Element,
                                                                     opensyde_core::C_OscNodeDataPoolListElementOptArrayId & orc_ElementId)
{
   bool q_IsValid = false;

   std::vector<std::string> c_ChannelComponents;
   C_SclDynamicArray<C_SclString> c_Tokens;

   orc_Element.Tokenize(".", c_Tokens);
   c_ChannelComponents.reserve(c_Tokens.GetLength());
   for (int32_t s32_It = 0L; s32_It < c_Tokens.GetLength(); ++s32_It)
   {
      c_ChannelComponents.emplace_back(c_Tokens[s32_It].c_str());
   }

   if (c_ChannelComponents.size() > 5UL)
   {
      //Invalid
   }
   else if (c_ChannelComponents.size() > 3UL)
   {
      const std::string & rc_UserNodeName = c_ChannelComponents[0UL];
      for (uint32_t u32_ItNode = 0UL; u32_ItNode < C_PuiSdHandler::h_GetInstance()->GetOscNodesSize();
           ++u32_ItNode)
      {
         const C_OscNode * const pc_Node = C_PuiSdHandler::h_GetInstance()->GetOscNodeConst(u32_ItNode);
         tgl_assert(pc_Node != NULL);
         if (pc_Node != NULL)
         {
            if (pc_Node->c_Properties.c_Name == rc_UserNodeName)
            {
               orc_ElementId.u32_NodeIndex = u32_ItNode;
               q_IsValid = mh_CheckChannelDatapool(*pc_Node, c_ChannelComponents, NULL, &orc_ElementId);
            }
         }
      }
   }
   else
   {
      //Invalid
   }
   return q_IsValid;
}
