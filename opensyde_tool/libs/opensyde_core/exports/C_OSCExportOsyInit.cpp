//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Export initialization module for DPD and DPH.

   Create a .c and .h file providing initialization structures for the OSS DPD and DPH init functions.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     23.03.2018  STW/A.Stangl
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "stwerrors.h"

#include "TGLFile.h"
#include "TGLUtils.h"
#include "CSCLString.h"
#include "CSCLStringList.h"
#include "C_OSCExportOsyInit.h"
#include "C_OSCExportDataPool.h"
#include "C_OSCExportCommunicationStack.h"
#include "C_OSCProtocolDriverOsyTpBase.h"
#include "C_OSCLoggingHandler.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Constructor

   Initialize all elements with default values

   \created     23.03.2018  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_OSCExportOsyInit::C_OSCExportOsyInit(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     23.03.2018  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_OSCExportOsyInit::~C_OSCExportOsyInit(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Create source files

   Create .c and .h files with DPD / DPH initialization.
   The specified file path is used for the ".c" file to create.
   The name of the header file is composed by replacing the extension by ".h".
   Existing files will be replaced if possible.

   \param[in] orc_FilePath             path to file to create
   \param[in] orc_Node                 node definition
   \param[in] oq_RunsDpd               true: create DPD init code and init code for all local and remote DPs
                                       false: only create init code for local DPs
   \param[in] ou16_ApplicationIndex    index of application we create code for (to identify local DPs)

   \return
   C_NO_ERR Operation success
   C_RD_WR  Operation failure: cannot store files

   \created     23.03.2018  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_OSCExportOsyInit::h_CreateSourceCode(const C_SCLString & orc_FilePath, const C_OSCNode & orc_Node,
                                              const bool oq_RunsDpd, const uint16 ou16_ApplicationIndex)
{
   C_SCLStringList c_Lines;
   sint32 s32_Return = C_NO_ERR;
   uint8 u8_DataPoolsKnownInThisApplication = 0U;
   uint8 u8_NumCanChannels = 0U;
   uint8 u8_NumEthChannels = 0U;
   uint32 u32_BufferSize;

   //header file: quite simple (constant only as long as we always create DPD and DPH structures):
   c_Lines.Add(
      "//----------------------------------------------------------------------------------------------------------------------");
   c_Lines.Add("/*!");
   c_Lines.Add("   \\file");
   c_Lines.Add("   \\brief       Application specific openSYDE initialization (Header file with interface)");
   c_Lines.Add("*/");
   c_Lines.Add(
      "//----------------------------------------------------------------------------------------------------------------------");
   c_Lines.Add("#ifndef OSY_INITH");
   c_Lines.Add("#define OSY_INITH");
   c_Lines.Add("");
   c_Lines.Add(
      "/* -- Includes ------------------------------------------------------------------------------------------------------ */");
   c_Lines.Add("#include \"stwtypes.h\"");
   if (oq_RunsDpd == true)
   {
      c_Lines.Add("#include \"osy_dpd_driver.h\"");
   }
   c_Lines.Add("#include \"osy_dpa_data_pool.h\"");
   //includes for the data pool definitions
   //adding them to this central header allows the application to just include one central file and access all data
   // pools
   c_Lines.Add("//Header files exporting application specific data pools:");
   for (uint8 u8_DataPool = 0U; u8_DataPool < orc_Node.c_DataPools.size(); u8_DataPool++)
   {
      const C_OSCNodeDataPool & rc_DataPool = orc_Node.c_DataPools[u8_DataPool];

      //add data pool if application runs DPD or application owns this data pool
      if ((oq_RunsDpd == true) || (rc_DataPool.s32_RelatedDataBlockIndex == ou16_ApplicationIndex))
      {
         C_SCLString c_HeaderName;
         C_OSCExportDataPool::h_GetFileName(rc_DataPool, c_HeaderName);
         c_Lines.Add("#include \"" + c_HeaderName + ".h\"");
      }
   }
   //includes for comm stack configuration
   //these are not needed by this module at all; they are only provided for application convenience
   c_Lines.Add("//Header files exporting comm stack configuration and status:");
   for (uint32 u32_ItProtocol = 0U; u32_ItProtocol < orc_Node.c_ComProtocols.size(); u32_ItProtocol++)
   {
      const C_OSCCanProtocol & rc_Protocol = orc_Node.c_ComProtocols[u32_ItProtocol];
      //logic: C_OSCCanProtocol refers to a data pool; if that data pool is owned by the
      // C_OSCNodeApplication then create it
      if (orc_Node.c_DataPools[rc_Protocol.u32_DataPoolIndex].s32_RelatedDataBlockIndex == ou16_ApplicationIndex)
      {
         for (uint32 u32_ItInterface = 0U; u32_ItInterface < rc_Protocol.c_ComMessages.size();
              u32_ItInterface++)
         {
            //at least one message defined ?
            const C_OSCCanMessageContainer & rc_ComMessageContainer = rc_Protocol.c_ComMessages[u32_ItInterface];
            if ((rc_ComMessageContainer.c_TxMessages.size() > 0) || (rc_ComMessageContainer.c_RxMessages.size() > 0))
            {
               C_SCLString c_HeaderName;
               C_OSCExportCommunicationStack::h_GetFileName(static_cast<uint8>(u32_ItInterface),
                                                            rc_Protocol.e_Type, c_HeaderName);
               c_Lines.Add("#include \"" + c_HeaderName + ".h\"");
            }
         }
      }
   }
   c_Lines.Add("");

   c_Lines.Add("#ifdef __cplusplus");
   c_Lines.Add("extern \"C\"");
   c_Lines.Add("{");
   c_Lines.Add("#endif");
   c_Lines.Add("");
   c_Lines.Add(
      "/* -- Defines ------------------------------------------------------------------------------------------------------- */");

   if (oq_RunsDpd == true)
   {
      //count number of active CAN and ETH channels:
      //CAN and Ethernet bus number
      for (uint8 u8_Channel = 0U; u8_Channel < orc_Node.c_Properties.c_ComInterfaces.size(); u8_Channel++)
      {
         const C_OSCNodeComInterfaceSettings & rc_ComIf = orc_Node.c_Properties.c_ComInterfaces[u8_Channel];

         if (C_OSCExportOsyInit::mh_IsDpdInitRequired(rc_ComIf) == true)
         {
            switch (rc_ComIf.e_InterfaceType)
            {
            case C_OSCSystemBus::eCAN:
               c_Lines.Add("#define OSY_INIT_DPD_BUS_NUMBER_CAN_CHANNEL_" +
                           C_SCLString::IntToStr(u8_NumCanChannels) + "       " +
                           C_SCLString::IntToStr(rc_ComIf.u8_InterfaceNumber) + "U");
               u8_NumCanChannels++;
               break;
            case C_OSCSystemBus::eETHERNET:
               c_Lines.Add("#define OSY_INIT_DPD_BUS_NUMBER_ETHERNET_CHANNEL_" +
                           C_SCLString::IntToStr(u8_NumEthChannels) + "  " +
                           C_SCLString::IntToStr(rc_ComIf.u8_InterfaceNumber) + "U");
               u8_NumEthChannels++;
               break;
            default:
               tgl_assert(false); //oh for %$%$"§% sake
               break;
            }
         }
      }

      c_Lines.Add("#define OSY_INIT_DPD_NUMBER_OF_CAN_CHANNELS         " + C_SCLString::IntToStr(u8_NumCanChannels) +
                  "U");
      c_Lines.Add("#define OSY_INIT_DPD_NUMBER_OF_ETHERNET_CHANNELS    " + C_SCLString::IntToStr(u8_NumEthChannels) +
                  "U");
      c_Lines.Add("");
      c_Lines.Add("#define OSY_INIT_DPD_NUMBER_OF_PARALLEL_CONNECTIONS " +
                  C_SCLString::IntToStr(orc_Node.c_Properties.c_OpenSYDEServerSettings.u8_MaxClients) + "U");
      c_Lines.Add("#define OSY_INIT_DPD_CAN_FIFO_SIZE_TX               " +
                  C_SCLString::IntToStr(orc_Node.c_Properties.c_OpenSYDEServerSettings.u16_MaxMessageBufferTx) + "U");
      c_Lines.Add("#define OSY_INIT_DPD_CAN_ROUTING_FIFO_SIZE_RX       " +
                  C_SCLString::IntToStr(orc_Node.c_Properties.c_OpenSYDEServerSettings.u16_MaxRoutingMessageBufferRx) +
                  "U");
      //get size of greatest element so we know how to set up the buffers
      //add protocol overhead for DPD and NVM access services (greatest overhead: write_memory_by_address)
      u32_BufferSize = mh_GetSizeOfLargestDataPoolElement(orc_Node.c_DataPools) + 11U;
      //consider minimum for non-DPD services (greatest size for osy server: routine control DP meta data)
      if (u32_BufferSize < 43U)
      {
         u32_BufferSize = 43U;
      }
      //limit to maximum service size; larger access must be segmented by protocol driver
      if (u32_BufferSize > C_OSCProtocolDriverOsyTpBase::hu16_OSY_MAXIMUM_SERVICE_SIZE)
      {
         u32_BufferSize = C_OSCProtocolDriverOsyTpBase::hu16_OSY_MAXIMUM_SERVICE_SIZE;
      }

      c_Lines.Add("#define OSY_INIT_DPD_BUF_SIZE_INSTANCE              " + C_SCLString::IntToStr(u32_BufferSize) + "U");
      c_Lines.Add("#define OSY_INIT_DPD_MAX_NUM_CYCLIC_TRANSMISSIONS   " +
                  C_SCLString::IntToStr(orc_Node.c_Properties.c_OpenSYDEServerSettings.u8_MaxParallelTransmissions) +
                  "U");
      c_Lines.Add("");
   }

   //how many data pools does this application know ?
   for (uint8 u8_DataPool = 0U; u8_DataPool < orc_Node.c_DataPools.size(); u8_DataPool++)
   {
      const C_OSCNodeDataPool & rc_DataPool = orc_Node.c_DataPools[u8_DataPool];
      //add data pool if application runs DPD or application owns this data pool
      if ((oq_RunsDpd == true) || (rc_DataPool.s32_RelatedDataBlockIndex == ou16_ApplicationIndex))
      {
         u8_DataPoolsKnownInThisApplication++;
      }
   }

   c_Lines.Add("#define OSY_INIT_DPH_NUM_DATA_POOLS                 " +
               C_SCLString::IntToStr(u8_DataPoolsKnownInThisApplication) + "U");
   c_Lines.Add("");
   c_Lines.Add(
      "/* -- Types --------------------------------------------------------------------------------------------------------- */");
   c_Lines.Add("");
   c_Lines.Add(
      "/* -- Global Variables ---------------------------------------------------------------------------------------------- */");
   c_Lines.Add("");
   c_Lines.Add(
      "/* -- Function Prototypes ------------------------------------------------------------------------------------------- */");
   if (oq_RunsDpd == true)
   {
      c_Lines.Add("extern const T_osy_dpd_data * osy_dpd_get_init_config(void);");
   }
   c_Lines.Add("extern const T_osy_dpa_data_pool * const * osy_dph_get_init_config(void);");
   c_Lines.Add("extern uint8 osy_dph_get_num_data_pools(void);");
   c_Lines.Add("");
   c_Lines.Add(
      "/* -- Implementation ------------------------------------------------------------------------------------------------ */");
   c_Lines.Add("");
   c_Lines.Add("#ifdef __cplusplus");
   c_Lines.Add("}");
   c_Lines.Add("#endif");
   c_Lines.Add("");
   c_Lines.Add("#endif");

   try
   {
      c_Lines.SaveToFile(TGL_ChangeFileExtension(orc_FilePath, ".h"));
   }
   catch (...)
   {
      osc_write_log_error("Creating source code",
                          "Could not write to file \"" + TGL_ChangeFileExtension(orc_FilePath, ".h") + "\"");
      s32_Return = C_RD_WR;
   }

   if (s32_Return == C_NO_ERR)
   {
      //now for the c file:
      c_Lines.Clear();

      //constant header part:
      c_Lines.Add(
         "//----------------------------------------------------------------------------------------------------------------------");
      c_Lines.Add("/*!");
      c_Lines.Add("   \\file");
      c_Lines.Add("   \\brief       Application specific openSYDE initialization (Source file with implementation)");
      c_Lines.Add("");
      c_Lines.Add("   Auto-coded by openSYDE");
      c_Lines.Add("*/");
      c_Lines.Add(
         "//----------------------------------------------------------------------------------------------------------------------");
      c_Lines.Add("");
      c_Lines.Add(
         "/* -- Includes ------------------------------------------------------------------------------------------------------ */");
      c_Lines.Add("#include <stddef.h> //for NULL");
      c_Lines.Add("#include \"stwtypes.h\"");
      c_Lines.Add("#include \"osy_init.h\"");
      c_Lines.Add("#include \"osy_dpa_data_pool.h\"");
      c_Lines.Add("");
      c_Lines.Add(
         "/* -- Defines ------------------------------------------------------------------------------------------------------- */");
      c_Lines.Add("");
      c_Lines.Add(
         "/* -- Types --------------------------------------------------------------------------------------------------------- */");
      c_Lines.Add("");
      c_Lines.Add(
         "/* -- Global Variables ---------------------------------------------------------------------------------------------- */");
      c_Lines.Add("");
      c_Lines.Add(
         "/* -- Module Global Variables --------------------------------------------------------------------------------------- */");
      c_Lines.Add("");
      c_Lines.Add(
         "/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */");
      c_Lines.Add("");
      c_Lines.Add(
         "/* -- Implementation ------------------------------------------------------------------------------------------------ */");

      if (oq_RunsDpd == true)
      {
         c_Lines.Add("");
         c_Lines.Add(
            "//----------------------------------------------------------------------------------------------------------------------");
         c_Lines.Add("/*! \\brief   Set up and provide openSYDE protocol driver configuration");
         c_Lines.Add("");
         c_Lines.Add("   Sets up:");
         c_Lines.Add("   * CAN channel configuration");
         c_Lines.Add("   * Ethernet channel configuration");
         c_Lines.Add("   * Connection buffer definition");
         c_Lines.Add("   * Main initialization structure");
         c_Lines.Add("");
         c_Lines.Add("   \\return");
         c_Lines.Add(
            "   pointer to configuration structure (statically available; can be used for the DPD initialization function)");
         c_Lines.Add("*/");
         c_Lines.Add(
            "//----------------------------------------------------------------------------------------------------------------------");
         c_Lines.Add("const T_osy_dpd_data * osy_dpd_get_init_config(void)");
         c_Lines.Add("{");

         //channel instances
         u8_NumCanChannels = 0U;
         u8_NumEthChannels = 0U;
         for (uint8 u8_Channel = 0U; u8_Channel < orc_Node.c_Properties.c_ComInterfaces.size(); u8_Channel++)
         {
            const C_OSCNodeComInterfaceSettings & rc_ComIf = orc_Node.c_Properties.c_ComInterfaces[u8_Channel];
            //create initialization if
            //* the bus is connected and
            //** routing is enabled (in this case we need to be able to route on application level as well)
            //** or: update is enabled (in this case we need to be able to perform the reset from application level
            //       as well)
            //** or: diagnostic is enabled
            if (C_OSCExportOsyInit::mh_IsDpdInitRequired(rc_ComIf) == true)
            {
               if (rc_ComIf.e_InterfaceType == C_OSCSystemBus::eCAN)
               {
                  c_Lines.Add("   OSY_DPD_CAN_CHANNEL(ht_CanInitConfiguration" +
                              C_SCLString::IntToStr(u8_NumCanChannels) +
                              ", OSY_INIT_DPD_BUS_NUMBER_CAN_CHANNEL_" + C_SCLString::IntToStr(u8_NumCanChannels) +
                              ",");
                  c_Lines.Add(
                     "                       OSY_INIT_DPD_NUMBER_OF_PARALLEL_CONNECTIONS, OSY_INIT_DPD_NUMBER_OF_PARALLEL_CONNECTIONS,");
                  c_Lines.Add("                       OSY_INIT_DPD_BUF_SIZE_INSTANCE,");
                  c_Lines.Add(
                     "                       OSY_INIT_DPD_CAN_FIFO_SIZE_TX, OSY_INIT_DPD_CAN_ROUTING_FIFO_SIZE_RX)");
                  u8_NumCanChannels++;
               }
               else
               {
                  c_Lines.Add("   OSY_DPD_ETH_CHANNEL(ht_EthernetInitConfiguration" +
                              C_SCLString::IntToStr(u8_NumEthChannels) + ", OSY_INIT_DPD_BUS_NUMBER_ETHERNET_CHANNEL_" +
                              C_SCLString::IntToStr(u8_NumEthChannels) + ",");
                  c_Lines.Add(
                     "                       OSY_INIT_DPD_NUMBER_OF_PARALLEL_CONNECTIONS, OSY_INIT_DPD_NUMBER_OF_PARALLEL_CONNECTIONS,");
                  c_Lines.Add("                       OSY_INIT_DPD_BUF_SIZE_INSTANCE)");
                  u8_NumEthChannels++;
               }
            }
         }

         //channel lists
         c_Lines.Add("");
         if (u8_NumCanChannels > 0)
         {
            c_Lines.Add("   static const T_osy_udc_global_cantp_init_configuration * const");
            c_Lines.Add("      hapt_CanInitConfigurations[OSY_INIT_DPD_NUMBER_OF_CAN_CHANNELS] =");
            c_Lines.Add("   {");
            for (uint8 u8_Channel = 0U; u8_Channel < u8_NumCanChannels; u8_Channel++)
            {
               C_SCLString c_Text = "      &ht_CanInitConfiguration" + C_SCLString::IntToStr(u8_Channel);
               if (u8_Channel != (u8_NumCanChannels - 1))
               {
                  c_Text += ",";
               }
               c_Lines.Add(c_Text);
            }
            c_Lines.Add("   };");
         }
         c_Lines.Add("");
         if (u8_NumEthChannels > 0)
         {
            c_Lines.Add("   static const T_osy_udc_global_ethertp_init_configuration * const");
            c_Lines.Add("      hapt_EthernetInitConfigurations[OSY_INIT_DPD_NUMBER_OF_ETHERNET_CHANNELS] =");
            c_Lines.Add("   {");
            for (uint8 u8_Channel = 0U; u8_Channel < u8_NumEthChannels; u8_Channel++)
            {
               C_SCLString c_Text = "      &ht_EthernetInitConfiguration" + C_SCLString::IntToStr(u8_Channel);
               if (u8_Channel != (u8_NumEthChannels - 1))
               {
                  c_Text += ",";
               }
               c_Lines.Add(c_Text);
            }
            c_Lines.Add("   };");
         }
         c_Lines.Add("");
         //connection instances
         for (uint8 u8_Instance = 0U; u8_Instance < orc_Node.c_Properties.c_OpenSYDEServerSettings.u8_MaxClients;
              u8_Instance++)
         {
            c_Lines.Add("   OSY_DPD_CONNECTION_INSTANCE_INIT(ht_DpdConnectionInstance" +
                        C_SCLString::IntToStr(u8_Instance) + ", " + C_SCLString::IntToStr(u8_Instance) + "U, " +
                        "OSY_INIT_DPD_MAX_NUM_CYCLIC_TRANSMISSIONS)");
         }
         c_Lines.Add("");
         c_Lines.Add(
            "   static T_osy_dpd_connection_instance * const hapt_DpdConnections[OSY_INIT_DPD_NUMBER_OF_PARALLEL_CONNECTIONS] =");
         c_Lines.Add("   {");
         for (uint8 u8_Instance = 0U; u8_Instance < orc_Node.c_Properties.c_OpenSYDEServerSettings.u8_MaxClients;
              u8_Instance++)
         {
            C_SCLString c_Text = "      &ht_DpdConnectionInstance" + C_SCLString::IntToStr(u8_Instance);
            if (u8_Instance != (orc_Node.c_Properties.c_OpenSYDEServerSettings.u8_MaxClients - 1))
            {
               c_Text += ",";
            }
            c_Lines.Add(c_Text);
         }
         c_Lines.Add("   };");
         c_Lines.Add("");

         c_Lines.Add("   OSY_DPD_GLOBAL_DATA_INIT(ht_DpdDataInstance,");
         c_Lines.Add("                            OSY_INIT_DPD_NUMBER_OF_CAN_CHANNELS,");
         c_Lines.Add("                            OSY_INIT_DPD_NUMBER_OF_ETHERNET_CHANNELS,");
         c_Lines.Add("                            OSY_INIT_DPD_NUMBER_OF_PARALLEL_CONNECTIONS,");
         c_Lines.Add("                            &hapt_DpdConnections[0],");
         if (u8_NumCanChannels > 0)
         {
            c_Lines.Add("                            &hapt_CanInitConfigurations[0],");
         }
         else
         {
            c_Lines.Add("                            NULL,");
         }
         if (u8_NumEthChannels > 0)
         {
            c_Lines.Add("                            &hapt_EthernetInitConfigurations[0])");
         }
         else
         {
            c_Lines.Add("                            NULL)");
         }
         c_Lines.Add("");
         c_Lines.Add("   return &ht_DpdDataInstance;");
         c_Lines.Add("}");
      }
      c_Lines.Add("");
      c_Lines.Add(
         "//----------------------------------------------------------------------------------------------------------------------");
      c_Lines.Add("/*! \\brief   Set up and provide openSYDE data pool handler configuration");
      c_Lines.Add("");
      c_Lines.Add("   Sets up:");
      c_Lines.Add("   * initialization structure listing all data pools in correct sequence");
      c_Lines.Add("");
      c_Lines.Add("   \\return");
      c_Lines.Add(
         "   pointer to initialization structure (statically available; can be used for the DPH initialization function)");
      c_Lines.Add("   NULL: no data pools defined");
      c_Lines.Add("*/");
      c_Lines.Add(
         "//----------------------------------------------------------------------------------------------------------------------");
      c_Lines.Add("const T_osy_dpa_data_pool * const * osy_dph_get_init_config(void)");
      c_Lines.Add("{");

      //add table of data pools
      if (u8_DataPoolsKnownInThisApplication == 0U)
      {
         c_Lines.Add("   return NULL;");
      }
      else
      {
         c_Lines.Add("   static const T_osy_dpa_data_pool * const hapt_Datapools[OSY_INIT_DPH_NUM_DATA_POOLS] =");
         c_Lines.Add("   {");
         for (uint8 u8_DataPool = 0U; u8_DataPool < orc_Node.c_DataPools.size(); u8_DataPool++)
         {
            //add data pool if application runs DPD or application owns this data pool
            if ((oq_RunsDpd == true) ||
                (orc_Node.c_DataPools[u8_DataPool].s32_RelatedDataBlockIndex == ou16_ApplicationIndex))
            {
               const C_SCLString c_Text = "      &gt_" + orc_Node.c_DataPools[u8_DataPool].c_Name + "_DataPool,";
               c_Lines.Add(c_Text);
            }
         }
         //remove final ",":
         c_Lines.Strings[static_cast<sint32>(c_Lines.GetCount()) - 1].Delete(
            c_Lines.Strings[static_cast<sint32>(c_Lines.GetCount()) - 1].Length(), 1U);

         c_Lines.Add("   };");
         c_Lines.Add("");
         c_Lines.Add("   return &hapt_Datapools[0];");
      }
      c_Lines.Add("}");
      c_Lines.Add("");
      c_Lines.Add(
         "//----------------------------------------------------------------------------------------------------------------------");
      c_Lines.Add("/*! \\brief   Get number of defined openSYDE data pools");
      c_Lines.Add("");
      c_Lines.Add("   \\return");
      c_Lines.Add("   number of openSYDE data pools");
      c_Lines.Add("*/");
      c_Lines.Add(
         "//----------------------------------------------------------------------------------------------------------------------");
      c_Lines.Add("uint8 osy_dph_get_num_data_pools(void)");
      c_Lines.Add("{");
      c_Lines.Add("   return OSY_INIT_DPH_NUM_DATA_POOLS;");
      c_Lines.Add("}");

      try
      {
         c_Lines.SaveToFile(orc_FilePath);
      }
      catch (...)
      {
         osc_write_log_error("Creating source code", "Could not write to file \"" + orc_FilePath + "\"");
         s32_Return = C_RD_WR;
      }
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Utility: check whether DPD initialization needs to be performed

   Logic:
   Initialization is required if
   * the bus is connected and (
   ** routing is enabled (in this case we need to be able to route on application level as well)
   ** or: update is enabled (in this case we need to be able to perform the reset from application level as well)
   ** or: diagnostic is enabled )

   \param[in] orc_Settings            interface configuration

   \return
   true: DPD needs to be initialized on interface
   false: DPD does not need to be initialized on interface

   \created     25.05.2018  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
bool C_OSCExportOsyInit::mh_IsDpdInitRequired(const C_OSCNodeComInterfaceSettings & orc_Settings)
{
   return ((orc_Settings.q_IsBusConnected == true) &&
           ((orc_Settings.q_IsDiagnosisEnabled == true) ||
            (orc_Settings.q_IsRoutingEnabled == true) ||
            (orc_Settings.q_IsUpdateEnabled == true)));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Utility: get size of greatest data pool element or list

   For NVM data pools:
   *  Go through vector of data pools and check which is the greatest list
   For other data pools:
   *  Go through vector of data pools and check which is the greatest element

   This is used for defining the buffer size for the protocol driver. So the function considers local and remote
    data pools.

   \param[in] orc_DataPools            data pools to scan

   \return
   size of the greatest data pool element/list in bytes

   \created     26.03.2018  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
uint32 C_OSCExportOsyInit::mh_GetSizeOfLargestDataPoolElement(const std::vector<C_OSCNodeDataPool> & orc_DataPools)
{
   uint32 u32_GreatestSize = 0U;

   for (uint32 u32_DataPool = 0U; u32_DataPool < orc_DataPools.size(); u32_DataPool++)
   {
      for (uint32 u32_List = 0U; u32_List < orc_DataPools[u32_DataPool].c_Lists.size(); u32_List++)
      {
         if (orc_DataPools[u32_DataPool].e_Type == C_OSCNodeDataPool::eNVM)
         {
            const uint32 u32_NumBytesUsed = orc_DataPools[u32_DataPool].c_Lists[u32_List].GetNumBytesUsed() +
                                            ((orc_DataPools[u32_DataPool].c_Lists[u32_List].q_NvMCRCActive ==
                                              true) ? 2U : 0U);
            if (u32_NumBytesUsed > u32_GreatestSize)
            {
               u32_GreatestSize = u32_NumBytesUsed;
            }
         }
         else
         {
            for (uint32 u32_Element = 0U; u32_Element < orc_DataPools[u32_DataPool].c_Lists[u32_List].c_Elements.size();
                 u32_Element++)
            {
               const uint32 u32_Size =
                  orc_DataPools[u32_DataPool].c_Lists[u32_List].c_Elements[u32_Element].GetSizeByte();
               if (u32_Size > u32_GreatestSize)
               {
                  u32_GreatestSize = u32_Size;
               }
            }
         }
      }
   }
   return u32_GreatestSize;
}
