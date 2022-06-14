//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Export initialization module for DPD and DPH.

   Create a .c and .h file providing initialization structures for the OSS DPD and DPH init functions.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
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
#include "C_OSCExportUti.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Return filename (without extension)

   \return filename
*/
//----------------------------------------------------------------------------------------------------------------------
C_SCLString C_OSCExportOsyInit::h_GetFileName(void)
{
   return "osy_init";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Create source files

   Create .c and .h files with DPD / DPH initialization.
   The specified file path is used for the ".c" file to create.
   The name of the header file is composed by replacing the extension by ".h".
   Existing files will be replaced if possible.

   \param[in] orc_FilePath             path to file to create
   \param[in] orc_Node                 node definition
   \param[in] oq_RunsDpd               true: create DPD init code and init code for all local, public and remote DPs
                                       false: only create init code for local and public DPs
   \param[in] ou16_ApplicationIndex    index of application we create code for (to identify local DPs)
   \param[in] orc_ExportToolInfo       information about calling executable (name + version)

   \return
   C_NO_ERR Operation success
   C_RD_WR  Operation failure: cannot store files
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_OSCExportOsyInit::h_CreateSourceCode(const C_SCLString & orc_FilePath, const C_OSCNode & orc_Node,
                                              const bool oq_RunsDpd, const uint16 ou16_ApplicationIndex,
                                              const C_SCLString & orc_ExportToolInfo)
{
   C_SCLStringList c_Lines;
   sint32 s32_Return;
   uint8 u8_DataPoolsKnownInThisApplication = 0U;
   uint8 u8_CommDefinitionsKnownInThisApplication = 0U;
   uint8 u8_NumCanChannels = 0U;
   uint8 u8_NumEthChannels = 0U;
   uint32 u32_BufferSize;
   uint32 u32_CommProtocolCnt = 0U;

   //header file: quite simple (constant only as long as we always create DPD and DPH structures):
   c_Lines.Add(C_OSCExportUti::h_GetHeaderSeparator());
   c_Lines.Add("/*!");
   c_Lines.Add("   \\file");
   c_Lines.Add("   \\brief       Application specific openSYDE initialization (Header file with interface)");
   c_Lines.Add("");
   c_Lines.Add(C_OSCExportUti::h_GetCreationToolInfo(orc_ExportToolInfo));
   c_Lines.Add("*/");
   c_Lines.Add(C_OSCExportUti::h_GetHeaderSeparator());
   c_Lines.Add("#ifndef " + h_GetFileName().UpperCase() + "H");
   c_Lines.Add("#define " + h_GetFileName().UpperCase() + "H");
   c_Lines.Add("");
   c_Lines.Add(C_OSCExportUti::h_GetSectionSeparator("Includes"));
   c_Lines.Add("#include \"stwtypes.h\"");
   if (oq_RunsDpd == true)
   {
      c_Lines.Add("#include \"osy_dpd_driver.h\"");
   }
   c_Lines.Add("#include \"osy_dpa_data_pool.h\"");
   //includes for the Datapool definitions
   //adding them to this central header allows the application to just include one central file and access all data
   // pools
   c_Lines.Add("//Header files exporting application specific Datapools:");
   for (uint8 u8_DataPool = 0U; u8_DataPool < orc_Node.c_DataPools.size(); u8_DataPool++)
   {
      if (mh_IsDpKnownToApp(u8_DataPool, ou16_ApplicationIndex, orc_Node, oq_RunsDpd) == true)
      {
         C_SCLString c_HeaderName;
         c_HeaderName = C_OSCExportDataPool::h_GetFileName(orc_Node.c_DataPools[u8_DataPool]);
         c_Lines.Add("#include \"" + c_HeaderName + ".h\"");
         u8_DataPoolsKnownInThisApplication++;
      }
   }
   //count how many node protocols are NOT CANopen and how many ARE CANopen
   for (uint32 u32_ProtIt = 0U; u32_ProtIt < orc_Node.c_ComProtocols.size(); ++u32_ProtIt)
   {
      if (orc_Node.c_ComProtocols[u32_ProtIt].e_Type != C_OSCCanProtocol::eCAN_OPEN)
      {
         u32_CommProtocolCnt++;
      }
   }
   //includes for comm stack configuration
   //these are not needed by this module at all; they are only provided for application convenience
   //only add includes if there is at least one COMM Protocol which is NOT CANopen
   if ((orc_Node.c_ComProtocols.size() > 0U) && (u32_CommProtocolCnt > 0U))
   {
      c_Lines.Add("//Header files exporting comm stack configuration and status:");

      for (uint32 u32_ItProtocol = 0U; u32_ItProtocol < orc_Node.c_ComProtocols.size(); u32_ItProtocol++)
      {
         bool oq_Skip = false;
         const C_OSCCanProtocol & rc_Protocol = orc_Node.c_ComProtocols[u32_ItProtocol];

         //skip ay CANopen protocol
         if (rc_Protocol.e_Type == C_OSCCanProtocol::eCAN_OPEN)
         {
            oq_Skip = true;
         }

         if (oq_Skip == false)
         {
            //logic: C_OSCCanProtocol refers to a Datapool; if that Datapool is owned by the
            // C_OSCNodeApplication then create it
            if (orc_Node.c_DataPools[rc_Protocol.u32_DataPoolIndex].s32_RelatedDataBlockIndex == ou16_ApplicationIndex)
            {
               for (uint32 u32_ItInterface = 0U; u32_ItInterface < rc_Protocol.c_ComMessages.size();
                    u32_ItInterface++)
               {
                  //at least one message defined ?
                  if (rc_Protocol.c_ComMessages[u32_ItInterface].ContainsAtLeastOneMessage() == true)
                  {
                     const C_SCLString c_HeaderName =
                        C_OSCExportCommunicationStack::h_GetFileName(static_cast<uint8>(u32_ItInterface),
                                                                     rc_Protocol.e_Type);
                     c_Lines.Add("#include \"" + c_HeaderName + ".h\"");
                     u8_CommDefinitionsKnownInThisApplication++;
                  }
               }
            }
         }
      }
   }

   c_Lines.Add("");
   C_OSCExportUti::h_AddExternCStart(c_Lines);
   c_Lines.Add(C_OSCExportUti::h_GetSectionSeparator("Defines"));

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
      //add an additional 2 bytes; compensates for an buffer size issue with older server implementations; #62305
      u32_BufferSize += 2U;
      //consider minimum for non-DP services
      if (u32_BufferSize < hu8_MIN_SIZE_DPD_BUF_INSTANCE)
      {
         u32_BufferSize = hu8_MIN_SIZE_DPD_BUF_INSTANCE;
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

   c_Lines.Add("#define OSY_INIT_DPH_NUM_DATA_POOLS                 " +
               C_SCLString::IntToStr(u8_DataPoolsKnownInThisApplication) + "U");
   c_Lines.Add("");
   //add the next constant even if there are no COMM protocols; just placing the define does not create an external
   // dependency
   c_Lines.Add("#define OSY_INIT_COM_NUM_PROTOCOL_CONFIGURATIONS    " +
               C_SCLString::IntToStr(u8_CommDefinitionsKnownInThisApplication) + "U");
   c_Lines.Add("");
   c_Lines.Add(C_OSCExportUti::h_GetSectionSeparator("Types"));
   c_Lines.Add("");
   c_Lines.Add(C_OSCExportUti::h_GetSectionSeparator("Global Variables"));
   c_Lines.Add("");
   c_Lines.Add(C_OSCExportUti::h_GetSectionSeparator("Function Prototypes"));
   if (oq_RunsDpd == true)
   {
      c_Lines.Add("extern const T_osy_dpd_data * osy_dpd_get_init_config(void);");
   }
   c_Lines.Add("extern const T_osy_dpa_data_pool * const * osy_dph_get_init_config(void);");
   c_Lines.Add("extern uint8 osy_dph_get_num_data_pools(void);");
   c_Lines.Add("");

   //prototypes for COMM utility functions; only place if there are COMM protocols to prevent external dependencies
   // (on type definitions)
   if (u8_CommDefinitionsKnownInThisApplication > 0U) //TODO PTH min 1 config
   {
      c_Lines.Add("extern const T_osy_com_protocol_configuration * const * osy_com_get_protocol_configs(void);");
      c_Lines.Add("extern uint8 osy_com_get_num_protocol_configs(void);");
      c_Lines.Add("");
   }

   c_Lines.Add(C_OSCExportUti::h_GetSectionSeparator("Implementation"));
   c_Lines.Add("");
   C_OSCExportUti::h_AddExternCEnd(c_Lines);
   c_Lines.Add("#endif");

   // finally save all stuff into the file
   s32_Return = C_OSCExportUti::h_SaveToFile(c_Lines, orc_FilePath, h_GetFileName(), true);

   if (s32_Return == C_NO_ERR)
   {
      //now for the c file:
      c_Lines.Clear();

      //constant header part:
      c_Lines.Add(C_OSCExportUti::h_GetHeaderSeparator());
      c_Lines.Add("/*!");
      c_Lines.Add("   \\file");
      c_Lines.Add("   \\brief       Application specific openSYDE initialization (Source file with implementation)");
      c_Lines.Add("");
      c_Lines.Add(C_OSCExportUti::h_GetCreationToolInfo(orc_ExportToolInfo));
      c_Lines.Add("*/");
      c_Lines.Add(C_OSCExportUti::h_GetHeaderSeparator());
      c_Lines.Add("");
      c_Lines.Add(C_OSCExportUti::h_GetSectionSeparator("Includes"));
      c_Lines.Add("#include <stddef.h> //for NULL");
      c_Lines.Add("#include \"stwtypes.h\"");
      c_Lines.Add("#include \"" + h_GetFileName() + ".h\"");
      c_Lines.Add("#include \"osy_dpa_data_pool.h\"");
      c_Lines.Add("");
      c_Lines.Add(C_OSCExportUti::h_GetSectionSeparator("Defines"));
      c_Lines.Add("");
      c_Lines.Add(C_OSCExportUti::h_GetSectionSeparator("Types"));
      c_Lines.Add("");
      c_Lines.Add(C_OSCExportUti::h_GetSectionSeparator("Global Variables"));
      c_Lines.Add("");
      c_Lines.Add(C_OSCExportUti::h_GetSectionSeparator("Module Global Variables"));
      c_Lines.Add("");
      c_Lines.Add(C_OSCExportUti::h_GetSectionSeparator("Module Global Function Prototypes"));
      c_Lines.Add("");
      c_Lines.Add(C_OSCExportUti::h_GetSectionSeparator("Implementation"));

      if (oq_RunsDpd == true)
      {
         c_Lines.Add("");
         c_Lines.Add(C_OSCExportUti::h_GetHeaderSeparator());
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
         c_Lines.Add(C_OSCExportUti::h_GetHeaderSeparator());
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
                     "                       OSY_INIT_DPD_CAN_ROUTING_FIFO_SIZE_RX, OSY_INIT_DPD_CAN_FIFO_SIZE_TX)");
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
      c_Lines.Add(C_OSCExportUti::h_GetHeaderSeparator());
      c_Lines.Add("/*! \\brief   Set up and provide openSYDE Datapool handler configuration");
      c_Lines.Add("");
      c_Lines.Add("   Sets up:");
      c_Lines.Add("   * initialization structure listing all Datapools in correct sequence");
      c_Lines.Add("");
      c_Lines.Add("   \\return");
      c_Lines.Add(
         "   pointer to initialization structure (statically available; can be used for the DPH initialization function)");
      c_Lines.Add("   NULL: no Datapools defined");
      c_Lines.Add("*/");
      c_Lines.Add(C_OSCExportUti::h_GetHeaderSeparator());
      c_Lines.Add("const T_osy_dpa_data_pool * const * osy_dph_get_init_config(void)");
      c_Lines.Add("{");

      //add table of Datapools
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
            if (mh_IsDpKnownToApp(u8_DataPool, ou16_ApplicationIndex, orc_Node, oq_RunsDpd) == true)
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
      c_Lines.Add(C_OSCExportUti::h_GetHeaderSeparator());
      c_Lines.Add("/*! \\brief   Get number of defined openSYDE Datapools");
      c_Lines.Add("");
      c_Lines.Add("   \\return");
      c_Lines.Add("   number of openSYDE Datapools");
      c_Lines.Add("*/");
      c_Lines.Add(C_OSCExportUti::h_GetHeaderSeparator());
      c_Lines.Add("uint8 osy_dph_get_num_data_pools(void)");
      c_Lines.Add("{");
      c_Lines.Add("   return OSY_INIT_DPH_NUM_DATA_POOLS;");
      c_Lines.Add("}");

      //only add function implementations if there is at least one COMM Protocol which is NOT CANopen
      if ((u8_CommDefinitionsKnownInThisApplication > 0) && (u32_CommProtocolCnt > 0))
      {
         c_Lines.Add("");
         c_Lines.Add(C_OSCExportUti::h_GetHeaderSeparator());
         c_Lines.Add("/*! \\brief   Set up and provide a list of openSYDE COMM protocol configurations");
         c_Lines.Add("");
         c_Lines.Add("   Sets up a table with pointers to all defined COMM protocol configurations");
         c_Lines.Add("");
         c_Lines.Add("   \\return");
         c_Lines.Add("   pointer to table of configurations (statically available)");
         c_Lines.Add("*/");
         c_Lines.Add(C_OSCExportUti::h_GetHeaderSeparator());
         c_Lines.Add("const T_osy_com_protocol_configuration * const * osy_com_get_protocol_configs(void)");
         c_Lines.Add("{");
         c_Lines.Add("   static const T_osy_com_protocol_configuration * const");
         c_Lines.Add("      hapt_CommConfigurations[OSY_INIT_COM_NUM_PROTOCOL_CONFIGURATIONS] =");
         c_Lines.Add("   {");

         for (uint32 u32_Protocol = 0U; u32_Protocol < orc_Node.c_ComProtocols.size(); u32_Protocol++)
         {
            bool oq_Skip = false;
            const C_OSCCanProtocol & rc_Protocol = orc_Node.c_ComProtocols[u32_Protocol];

            //skip any CANopen protocol
            if (rc_Protocol.e_Type == C_OSCCanProtocol::eCAN_OPEN)
            {
               oq_Skip = true;
            }

            if (oq_Skip == false)
            {
               //logic: C_OSCCanProtocol refers to a Datapool; if that Datapool is owned by the
               // C_OSCNodeApplication then this node knows about the protocol
               if (orc_Node.c_DataPools[rc_Protocol.u32_DataPoolIndex].s32_RelatedDataBlockIndex ==
                   ou16_ApplicationIndex)
               {
                  for (uint8 u8_Interface = 0U; u8_Interface < rc_Protocol.c_ComMessages.size(); u8_Interface++)
                  {
                     //at least one message defined ?
                     if (rc_Protocol.c_ComMessages[u8_Interface].ContainsAtLeastOneMessage() == true)
                     {
                        //finally we have a winner ...
                        const C_SCLString c_Text = "      &" + C_OSCExportCommunicationStack::h_GetConfigurationName(
                           u8_Interface, rc_Protocol.e_Type) + ",";

                        c_Lines.Add(c_Text);
                     }
                  }
               }
            }
         }
         //remove final ",":
         c_Lines.Strings[static_cast<sint32>(c_Lines.GetCount()) - 1].Delete(
            c_Lines.Strings[static_cast<sint32>(c_Lines.GetCount()) - 1].Length(), 1U);
         c_Lines.Add("   };");
         c_Lines.Add("");
         c_Lines.Add("   return &hapt_CommConfigurations[0];");
         c_Lines.Add("}");
         c_Lines.Add("");
         c_Lines.Add(C_OSCExportUti::h_GetHeaderSeparator());
         c_Lines.Add("/*! \\brief   Get number of defined openSYDE COMM protocol configurations");
         c_Lines.Add("");
         c_Lines.Add(
            "   The returned value matches the number of elements in the table returned by osy_com_get_protocol_configs.");
         c_Lines.Add("");
         c_Lines.Add("   \\return");
         c_Lines.Add("   number of openSYDE Datapools");
         c_Lines.Add("*/");
         c_Lines.Add(C_OSCExportUti::h_GetHeaderSeparator());
         c_Lines.Add("uint8 osy_com_get_num_protocol_configs(void)");
         c_Lines.Add("{");
         c_Lines.Add("   return OSY_INIT_COM_NUM_PROTOCOL_CONFIGURATIONS;");
         c_Lines.Add("}");
      }

      // finally save all stuff into the file
      s32_Return = C_OSCExportUti::h_SaveToFile(c_Lines, orc_FilePath, h_GetFileName(), false);
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: check whether DPD initialization needs to be performed

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
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCExportOsyInit::mh_IsDpdInitRequired(const C_OSCNodeComInterfaceSettings & orc_Settings)
{
   return ((orc_Settings.GetBusConnected() == true) &&
           ((orc_Settings.q_IsDiagnosisEnabled == true) ||
            (orc_Settings.q_IsRoutingEnabled == true) ||
            (orc_Settings.q_IsUpdateEnabled == true)));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Utility: get size of greatest Datapool element or list

   For NVM Datapools:
   *  Go through vector of Datapools and check which is the greatest list
   For other Datapools:
   *  Go through vector of Datapools and check which is the greatest element

   This is used for defining the buffer size for the protocol driver. So the function considers local and remote
    Datapools.

   \param[in] orc_DataPools            Datapools to scan

   \return
   size of the greatest Datapool element/list in bytes
*/
//----------------------------------------------------------------------------------------------------------------------
uint32 C_OSCExportOsyInit::mh_GetSizeOfLargestDataPoolElement(const std::vector<C_OSCNodeDataPool> & orc_DataPools)
{
   uint32 u32_GreatestSize = 0U;

   for (uint32 u32_DataPool = 0U; u32_DataPool < orc_DataPools.size(); u32_DataPool++)
   {
      for (uint32 u32_List = 0U; u32_List < orc_DataPools[u32_DataPool].c_Lists.size(); u32_List++)
      {
         if ((orc_DataPools[u32_DataPool].e_Type == C_OSCNodeDataPool::eNVM) ||
             (orc_DataPools[u32_DataPool].e_Type == C_OSCNodeDataPool::eHALC_NVM))
         {
            const uint32 u32_NumBytesUsed = orc_DataPools[u32_DataPool].c_Lists[u32_List].GetNumBytesUsed();
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Utility: Check if a Datapool is known by a specific application.

   A Datapool is "known" by a specific application if it is
      - not empty i.e. has at least one list containing at least one element and
      - Datapool is owned by Application ("local Datapool") or Datapool runs DPD ("remote Datapool") or
        Datapool is public ("public remote Datapool")

   \param[in]       ou8_DataPoolIndex        datapool index
   \param[in]       ou16_ApplicationIndex    application index
   \param[in]       orc_Node                 system definition node
   \param[in]       oq_RunsDpd               application runs DPD

   \retval  true     Datapool is known to the specified app
   \retval  false    Datapool is not known to the specified app (e.g. owned by another application)
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCExportOsyInit::mh_IsDpKnownToApp(const uint8 ou8_DataPoolIndex, const uint16 ou16_ApplicationIndex,
                                           const C_OSCNode & orc_Node, const bool oq_RunsDpd)
{
   bool q_Return = false;

   if (ou8_DataPoolIndex < orc_Node.c_DataPools.size())
   {
      const C_OSCNodeDataPool & rc_DataPool = orc_Node.c_DataPools[ou8_DataPoolIndex];
      bool q_AtLeastOneElement = false;

      // check if datapool is non-empty (files only get generated in this case)
      for (uint16 u16_ListIndex = 0U; u16_ListIndex < rc_DataPool.c_Lists.size(); u16_ListIndex++)
      {
         const C_OSCNodeDataPoolList & rc_List = rc_DataPool.c_Lists[u16_ListIndex];
         if (rc_List.c_Elements.size() != 0)
         {
            q_AtLeastOneElement = true;
            break;
         }
      }

      if (q_AtLeastOneElement == true)
      {
         // check if application runs DPD or application owns this Datapool or Datapool is public
         if ((oq_RunsDpd == true) || (rc_DataPool.s32_RelatedDataBlockIndex == ou16_ApplicationIndex) ||
             ((orc_Node.c_Applications[ou16_ApplicationIndex].u16_GenCodeVersion >= 4U) &&
              (rc_DataPool.q_ScopeIsPrivate == false)))
         {
            q_Return = true;
         }
      }
   }

   return q_Return;
}
