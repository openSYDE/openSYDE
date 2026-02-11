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
   \param[in]      ou32_DataLoggerIndex   Data logger index
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
bool C_SdNdeDalTriggerCheckHelper::h_Check(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataLoggerIndex,
                                           const C_SclString & orc_Expression, std::string * const opc_ErrorDetails,
                                           bool * const opq_AreVariablesValid, bool * const opq_IsSyntaxValid)
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
      q_IsValid = mh_CheckTokens(ou32_NodeIndex, ou32_DataLoggerIndex, c_Tokens, opc_ErrorDetails);
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

   \param[in]      ou32_NodeIndex         Node index
   \param[in]      ou32_DataLoggerIndex   Data logger index
   \param[in]      orc_Tokens             Tokens
   \param[in,out]  opc_ErrorDetails       Error details

   \return
   Flags

   \retval   True    Expression valid
   \retval   False   Expression not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalTriggerCheckHelper::mh_CheckTokens(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataLoggerIndex,
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
         q_IsValid = mh_CheckChannelDataLogger(ou32_NodeIndex, ou32_DataLoggerIndex, c_Components, opc_ErrorDetails);
      }
   }

   return q_IsValid;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check channel data logger

   \param[in]      ou32_NodeIndex         Node index
   \param[in]      ou32_DataLoggerIndex   Data logger index
   \param[in]      orc_ChannelComponents  Channel components
   \param[in,out]  opc_ErrorDetails       Error details

   \return
   Flags

   \retval   True    Expression valid
   \retval   False   Expression not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalTriggerCheckHelper::mh_CheckChannelDataLogger(const uint32_t ou32_NodeIndex,
                                                             const uint32_t ou32_DataLoggerIndex,
                                                             const std::vector<std::string> & orc_ChannelComponents,
                                                             std::string * const opc_ErrorDetails)
{
   bool q_IsValid = false;
   const C_OscDataLoggerJob * const pc_DataLogger = C_PuiSdHandler::h_GetInstance()->GetDataLoggerJob(ou32_NodeIndex,
                                                                                                      ou32_DataLoggerIndex);

   if (pc_DataLogger != NULL)
   {
      const QString c_SearchNamespace = mh_GetNamespace(orc_ChannelComponents);
      for (std::vector<C_OscDataLoggerDataElementReference>::const_iterator c_It =
              pc_DataLogger->c_ConfiguredDataElements.cbegin();
           (c_It != pc_DataLogger->c_ConfiguredDataElements.cend()) && (q_IsValid == false); ++c_It)
      {
         const QString c_CurNamespace = C_PuiSdUtil::h_GetNamespaceDatapoolElement(c_It->c_ConfiguredElementId);
         if (c_CurNamespace == c_SearchNamespace)
         {
            q_IsValid = true;
         }
      }
   }
   if (q_IsValid == false)
   {
      if (opc_ErrorDetails != NULL)
      {
         *opc_ErrorDetails = C_GtGetText::h_GetText(
            "Variable invalid: variable not found as part of data logger configuration");
      }
   }
   return q_IsValid;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get namespace

   \param[in]  orc_ChannelComponents   Channel components

   \return
   Namespace
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SdNdeDalTriggerCheckHelper::mh_GetNamespace(const std::vector<std::string> & orc_ChannelComponents)
{
   QString c_Namespace;

   if (orc_ChannelComponents.size() > 0UL)
   {
      const QString c_Seperator = "::";
      const std::string c_LastComponent = orc_ChannelComponents[orc_ChannelComponents.size() - 1];
      const bool oq_IsNumAtEnd = mh_CheckOnlyNumbers(c_LastComponent);
      for (std::vector<std::string>::const_iterator c_ItPart = orc_ChannelComponents.cbegin();
           c_ItPart != orc_ChannelComponents.cend();
           ++c_ItPart)
      {
         const QString c_Part = static_cast<QString>(c_ItPart->c_str());
         if (c_Namespace.isEmpty())
         {
            c_Namespace = c_Part;
         }
         else
         {
            if (((c_ItPart + 1) == orc_ChannelComponents.cend()) && (oq_IsNumAtEnd))
            {
               c_Namespace += "[" + c_Part + "]";
            }
            else
            {
               c_Namespace += c_Seperator + c_Part;
            }
         }
      }
   }
   return c_Namespace;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check only numbers

   \param[in]  orc_String  String

   \return
   Flags

   \retval   True    Only numbers
   \retval   False   At least one non number
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalTriggerCheckHelper::mh_CheckOnlyNumbers(const std::string & orc_String)
{
   bool q_IsNum = true;

   for (std::string::const_iterator c_ItChar = orc_String.cbegin(); (c_ItChar != orc_String.cend()) && q_IsNum;
        ++c_ItChar)
   {
      if ((*c_ItChar < '0') || (*c_ItChar > '9'))
      {
         q_IsNum = false;
      }
   }
   return q_IsNum;
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
                  q_IsValid = mh_CheckChannelDatapool(*pc_Node, orc_ChannelComponents, opc_ErrorDetails);
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

   \return
   Flags

   \retval   True    Expression valid
   \retval   False   Expression not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalTriggerCheckHelper::mh_CheckChannelDatapool(const opensyde_core::C_OscNode & orc_Node,
                                                           const std::vector<std::string> & orc_ChannelComponents,
                                                           std::string * const opc_ErrorDetails)
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
            if ((rc_Datapool.e_Type != C_OscNodeDataPool::eHALC) &&
                (rc_Datapool.e_Type != C_OscNodeDataPool::eHALC_NVM))
            {
               q_IsValid = mh_CheckChannelList(rc_Datapool, orc_ChannelComponents, opc_ErrorDetails);
            }
            else
            {
               q_IsValid = mh_CheckChannelHalType(orc_Node, rc_Datapool, orc_ChannelComponents, opc_ErrorDetails);
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

   \return
   Flags

   \retval   True    Expression valid
   \retval   False   Expression not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalTriggerCheckHelper::mh_CheckChannelHalType(const opensyde_core::C_OscNode & orc_Node,
                                                          const opensyde_core::C_OscNodeDataPool & orc_Datapool,
                                                          const std::vector<std::string> & orc_ChannelComponents,
                                                          std::string * const opc_ErrorDetails)
{
   bool q_IsValid = false;

   if (orc_ChannelComponents.size() == 5UL)
   {
      const std::string & rc_ListName = orc_ChannelComponents[3UL];
      C_OscHalcDefDomain::E_VariableSelector e_Type;
      q_IsValid = mh_ConvertHalListToType(rc_ListName, e_Type);
      if (q_IsValid)
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
                  q_IsValid = mh_CheckChannelList(orc_Datapool, rc_ListName, c_CompleteVarName, q_UseArrayIndex,
                                                  u32_ArrayIndex,
                                                  opc_ErrorDetails);
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

   \return
   Flags

   \retval   True    Expression valid
   \retval   False   Expression not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalTriggerCheckHelper::mh_CheckChannelList(const opensyde_core::C_OscNodeDataPool & orc_Datapool,
                                                       const std::vector<std::string> & orc_ChannelComponents,
                                                       std::string * const opc_ErrorDetails)
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
         q_IsValid = mh_CheckChannelList(orc_Datapool, rc_UserListName, rc_UserElementName, q_UseArrayIndex,
                                         u32_ArrayIndex, opc_ErrorDetails);
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
                                                       std::string * const opc_ErrorDetails)
{
   bool q_IsValid = false;

   for (uint32_t u32_ItList = 0UL; u32_ItList < orc_Datapool.c_Lists.size(); ++u32_ItList)
   {
      const C_OscNodeDataPoolList & rc_List = orc_Datapool.c_Lists[u32_ItList];
      if (rc_List.c_Name == orc_ListName)
      {
         q_IsValid = mh_CheckChannelElement(rc_List, orc_ElementName, oq_UseArrayIndex, ou32_ArrayIndex,
                                            opc_ErrorDetails);
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

   \return
   Flags

   \retval   True    Expression valid
   \retval   False   Expression not valid
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDalTriggerCheckHelper::mh_CheckChannelElement(const opensyde_core::C_OscNodeDataPoolList & orc_List,
                                                          const std::string & orc_ElementName,
                                                          const bool oq_UseArrayIndex, const uint32_t ou32_ArrayIndex,
                                                          std::string * const opc_ErrorDetails)
{
   bool q_IsValid = false;

   for (uint32_t u32_ItElement = 0UL; u32_ItElement < orc_List.c_Elements.size(); ++u32_ItElement)
   {
      const C_OscNodeDataPoolListElement & rc_Element  = orc_List.c_Elements[u32_ItElement];
      if (rc_Element.c_Name == orc_ElementName)
      {
         if (oq_UseArrayIndex == false)
         {
            q_IsValid = true;
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
