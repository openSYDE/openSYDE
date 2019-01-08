//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Export code of a openSYDE node. (implementation)

   Export code of a openSYDE node

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.10.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "TGLFile.h"
#include "stwerrors.h"
#include "C_OSCExportNode.h"
#include "C_OSCExportDataPool.h"
#include "C_OSCExportCommunicationStack.h"
#include "C_OSCExportOsyInit.h"
#include "C_OSCLoggingHandler.h"
#include "C_OSCUtils.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_scl;
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     16.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OSCExportNode::C_OSCExportNode(void)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Create code for one application

   Creates code for one programmable application.

   If the target path orc_Path does not exist the function can create it (also recursively).
   The function will store all files in the folder specified by orc_Path.

   Creates:
   * data pools
   * comm definition
   * openSYDE server initialization wrapper

   \param[in]  orc_Node                Node as information source for exported code
   \param[in]  ou16_ApplicationIndex   Index of programmable application within orc_Node.c_Applications
   \param[in]  orc_Path                Storage path for created files
   \param[out] orc_Files               List of all exported file names (with absolute or relative path)

   \return
   C_NO_ERR  Operation success
   C_RD_WR   Operation failure: cannot store files
   C_NOACT   Application is not of type ePROGRAMMABLE_APPLICATION
   C_RANGE   Information which application runs the DPD is invalid or refers to an invalid application
             Data pool does not provide information about owning application or refers to an invalid application
             ApplicationIndex references invalid application

   \created     06.09.2018  STW/A.Stangl (refactored from existing code)
*/
//-----------------------------------------------------------------------------
sint32 C_OSCExportNode::h_CreateSourceCode(const C_OSCNode & orc_Node, const stw_types::uint16 ou16_ApplicationIndex,
                                           const C_SCLString & orc_Path, std::vector<C_SCLString> & orc_Files)
{
   sint32 s32_Retval = C_NO_ERR;

   orc_Files.clear();

   if (ou16_ApplicationIndex >= orc_Node.c_Applications.size())
   {
      s32_Retval = C_RANGE;
   }
   else
   {
      s32_Retval = mh_CheckPrerequisites(orc_Node);
   }
   if (s32_Retval == C_NO_ERR)
   {
      //we only need to create code for programmable applications
      if (orc_Node.c_Applications[ou16_ApplicationIndex].e_Type != C_OSCNodeApplication::ePROGRAMMABLE_APPLICATION)
      {
         s32_Retval = C_NOACT;
      }
   }

   if (s32_Retval == C_NO_ERR)
   {
      //create target folder:
      s32_Retval = C_OSCUtils::h_CreateFolderRecursively(orc_Path);
      if (s32_Retval != C_NO_ERR)
      {
         osc_write_log_error("Creating source code", "Could not create target directory \"" + orc_Path + "\".");
         s32_Retval = C_RD_WR;
      }
   }

   if (s32_Retval == C_NO_ERR)
   {
      //index of data pool within this application (as there can be data pools owned by other applications
      // this value is not identical to the data pool index within the whole list)
      uint8 u8_DataPoolIndexWithinApplication = 0U;
      bool q_CreateDpdInit;

      const C_SCLString c_FileBase = TGL_FileIncludeTrailingDelimiter(orc_Path) + "osy_init";
      orc_Files.push_back(c_FileBase + ".c");
      orc_Files.push_back(c_FileBase + ".h");
      if (ou16_ApplicationIndex == orc_Node.c_Properties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex)
      {
         //create DPD and DPH init functions
         q_CreateDpdInit = true;
      }
      else
      {
         //create DPH init functions
         q_CreateDpdInit = false;
      }
      s32_Retval = C_OSCExportOsyInit::h_CreateSourceCode(c_FileBase + ".c", orc_Node, q_CreateDpdInit,
                                                          ou16_ApplicationIndex);
      if (s32_Retval != C_NO_ERR)
      {
         osc_write_log_error("Creating source code",
                             "Could not write osy_init file to target directory \"" + orc_Path + "\".");
      }
      else
      {
         //Export data pools
         for (uint32 u32_ItDataPool = 0U;
              (u32_ItDataPool < orc_Node.c_DataPools.size()) && (s32_Retval == C_NO_ERR); ++u32_ItDataPool)
         {
            bool q_Create = false;
            bool q_IsRemote = false;

            //owned by this application ?
            if (orc_Node.c_DataPools[u32_ItDataPool].s32_RelatedDataBlockIndex == ou16_ApplicationIndex)
            {
               q_IsRemote = false;
               q_Create = true;
            }
            //if the data pool is not owned by this application, but that application runs the protocol driver,
            // then
            // make the data pool known as remote data pool
            else if (ou16_ApplicationIndex == orc_Node.c_Properties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex)
            {
               q_IsRemote = true;
               q_Create = true;
            }
            else
            {
               //finished here ...
            }

            if (q_Create == true)
            {
               //create source code
               const C_OSCNodeDataPool & rc_DataPool = orc_Node.c_DataPools[u32_ItDataPool];
               uint8 u8_ProcessId;
               uint8 u8_DataPoolIndexRemote;

               //we need the process ID of the owner process and the data pool index within the owning application
               if (q_IsRemote == false)
               {
                  //all your base are belong to us
                  u8_ProcessId = orc_Node.c_Applications[ou16_ApplicationIndex].u8_ProcessId;
                  u8_DataPoolIndexRemote = 0U; //does not really matter
               }
               else
               {
                  u8_ProcessId = orc_Node.c_Applications[rc_DataPool.s32_RelatedDataBlockIndex].u8_ProcessId;
                  //get index of data pool within the remote process:
                  u8_DataPoolIndexRemote = 0U;
                  for (uint8 u8_DataPool = 0U; u8_DataPool < u32_ItDataPool; u8_DataPool++)
                  {
                     if (orc_Node.c_DataPools[u8_DataPool].s32_RelatedDataBlockIndex ==
                         rc_DataPool.s32_RelatedDataBlockIndex)
                     {
                        u8_DataPoolIndexRemote++;
                     }
                  }
               }

               if (rc_DataPool.e_Type == C_OSCNodeDataPool::eCOM)
               {
                  C_OSCNodeDataPool c_DataPool;
                  //Patch data pool to include message names
                  s32_Retval = C_OSCExportNode::mh_GetAdaptedComDataPool(orc_Node, u32_ItDataPool, c_DataPool);
                  tgl_assert(s32_Retval == C_NO_ERR);
                  //Export
                  s32_Retval =
                     C_OSCExportDataPool::h_CreateSourceCode(orc_Path, c_DataPool,
                                                             u8_DataPoolIndexWithinApplication,
                                                             q_IsRemote, u8_DataPoolIndexRemote,
                                                             u8_ProcessId);
               }
               else
               {
                  //Export
                  s32_Retval =
                     C_OSCExportDataPool::h_CreateSourceCode(orc_Path, rc_DataPool,
                                                             u8_DataPoolIndexWithinApplication,
                                                             q_IsRemote, u8_DataPoolIndexRemote,
                                                             u8_ProcessId);
               }
               //Handle file names
               if (s32_Retval == C_NO_ERR)
               {
                  C_SCLString c_FileName;
                  C_OSCExportDataPool::h_GetFileName(rc_DataPool, c_FileName);
                  c_FileName = TGL_FileIncludeTrailingDelimiter(orc_Path) + c_FileName;
                  orc_Files.push_back(c_FileName + ".c");
                  orc_Files.push_back(c_FileName + ".h");
               }
               else
               {
                  osc_write_log_error("Creating source code",
                                      "Could not write data pool file to target directory \"" + orc_Path +
                                      "\".");
                  break;
               }
               u8_DataPoolIndexWithinApplication++;
            }
         }
      }
      if (s32_Retval == C_NO_ERR)
      {
         //Export COMM definition
         //create COMM configuration if the application owns that protocol
         for (uint32 u32_ItProtocol = 0U;
              (u32_ItProtocol < orc_Node.c_ComProtocols.size()) && (s32_Retval == C_NO_ERR);
              ++u32_ItProtocol)
         {
            const C_OSCCanProtocol & rc_Protocol = orc_Node.c_ComProtocols[u32_ItProtocol];

            //logic: C_OSCCanProtocol refers to a data pool; if that data pool is owned by the
            // C_OSCNodeApplication then create it
            if (orc_Node.c_DataPools[rc_Protocol.u32_DataPoolIndex].s32_RelatedDataBlockIndex == ou16_ApplicationIndex)
            {
               for (uint32 u32_ItInterface = 0U; u32_ItInterface < rc_Protocol.c_ComMessages.size();
                    ++u32_ItInterface)
               {
                  const C_OSCCanMessageContainer & rc_ComMessageContainer =
                     rc_Protocol.c_ComMessages[u32_ItInterface];
                  if ((rc_ComMessageContainer.c_TxMessages.size() > 0) ||
                      (rc_ComMessageContainer.c_RxMessages.size() > 0))
                  {
                     s32_Retval =
                        C_OSCExportCommunicationStack::h_CreateSourceCode(orc_Path, orc_Node,
                                                                          static_cast<uint8>(u32_ItInterface),
                                                                          rc_Protocol.e_Type);
                     //Handle file names
                     if (s32_Retval == C_NO_ERR)
                     {
                        C_SCLString c_FileName;
                        C_OSCExportCommunicationStack::h_GetFileName(static_cast<uint8>(u32_ItInterface),
                                                                     rc_Protocol.e_Type, c_FileName);
                        c_FileName = TGL_FileIncludeTrailingDelimiter(orc_Path) + c_FileName;
                        orc_Files.push_back(c_FileName + ".h");
                        orc_Files.push_back(c_FileName + ".c");
                     }
                     else
                     {
                        osc_write_log_error("Creating source code",
                                            "Could not write COMM definition file to target directory \"" +
                                            orc_Path + "\".");
                        break;
                     }
                  }
               }
            }
            if (s32_Retval != C_NO_ERR)
            {
               break;
            }
         }
      }
   }

   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check for correct preconditions for creating source code

   \param[in]  orc_Node   Node as information source for exported code
   \param[in]  orc_Path   Storage path for created files
   \param[out] orc_Files  List of all exported file names (with absolute or relative path)

   \return
   C_NO_ERR  Node configuration is OK
   C_RANGE   Information which application runs the DPD is invalid or refers to an invalid application
             Data pool does not provide information about owning application or refers to an invalid application

   \created     06.09.2018  STW/A.Stangl (refactored from existing code)
*/
//-----------------------------------------------------------------------------
sint32 C_OSCExportNode::mh_CheckPrerequisites(const C_OSCNode & orc_Node)
{
   sint32 s32_Retval = C_NO_ERR;

   //valid application running DPD ?
   if ((orc_Node.c_Properties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex == -1) ||
       (orc_Node.c_Properties.c_OpenSYDEServerSettings.s16_DPDDataBlockIndex >=
        static_cast<sint16>(orc_Node.c_Applications.size())))
   {
      osc_write_log_error("Creating source code",
                          "Invalid definition of application running the diagnostic protocol driver.");
      s32_Retval = C_RANGE;
   }

   if (s32_Retval == C_NO_ERR)
   {
      //Valid related application index ?
      for (uint8 u8_DataPool = 0U; u8_DataPool < orc_Node.c_DataPools.size(); u8_DataPool++)
      {
         if ((orc_Node.c_DataPools[u8_DataPool].s32_RelatedDataBlockIndex == -1) ||
             (orc_Node.c_DataPools[u8_DataPool].s32_RelatedDataBlockIndex >=
              static_cast<sint32>(orc_Node.c_Applications.size())))
         {
            osc_write_log_error("Creating source code",
                                "Invalid definition of owner application for data pool " +
                                orc_Node.c_DataPools[u8_DataPool].c_Name + ".");
            s32_Retval = C_RANGE;
            break;
         }
      }
   }
   return s32_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt com data pool elements

   \param[in]     orc_Node           Current node containing data pools
   \param[in]     ou32_DataPoolIndex Data pool index within data pools owned by node
   \param[out]    orc_DataPool       Copy of data pool with adapted names

   \return
   C_NO_ERR Operation successful
   C_RANGE  Data pool index or type invalid
   C_CONFIG Protocol not found

   \created     19.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
sint32 C_OSCExportNode::mh_GetAdaptedComDataPool(const C_OSCNode & orc_Node, const uint32 ou32_DataPoolIndex,
                                                 C_OSCNodeDataPool & orc_DataPool)
{
   sint32 s32_Retval = C_NO_ERR;

   if (ou32_DataPoolIndex < orc_Node.c_DataPools.size())
   {
      orc_DataPool = orc_Node.c_DataPools[ou32_DataPoolIndex];
      if (orc_DataPool.e_Type == C_OSCNodeDataPool::eCOM)
      {
         bool q_Found = false;
         //Search protocol
         for (uint32 u32_ItProtocol = 0; u32_ItProtocol < orc_Node.c_ComProtocols.size(); ++u32_ItProtocol)
         {
            const C_OSCCanProtocol & rc_Protocol = orc_Node.c_ComProtocols[u32_ItProtocol];
            if (rc_Protocol.u32_DataPoolIndex == ou32_DataPoolIndex)
            {
               q_Found = true;
               //For each interface
               for (uint32 u32_ItMessageContainer = 0; u32_ItMessageContainer < rc_Protocol.c_ComMessages.size();
                    ++u32_ItMessageContainer)
               {
                  const C_OSCCanMessageContainer & rc_MessageContainer =
                     rc_Protocol.c_ComMessages[u32_ItMessageContainer];
                  //For each direction
                  for (uint8 u8_Toggle = 0; u8_Toggle < 2; ++u8_Toggle)
                  {
                     const bool q_IsTx = (u8_Toggle == 0) ? true : false;
                     //Get matching list
                     C_OSCNodeDataPoolList * const pc_List =
                        C_OSCCanProtocol::h_GetComList(orc_DataPool, u32_ItMessageContainer, q_IsTx);
                     const std::vector<C_OSCCanMessage> & rc_Messages = rc_MessageContainer.GetMessagesConst(q_IsTx);
                     //For each message
                     for (uint32 u32_ItMessage = 0; u32_ItMessage < rc_Messages.size(); ++u32_ItMessage)
                     {
                        const C_OSCCanMessage & rc_Message = rc_Messages[u32_ItMessage];
                        //For each signal
                        for (uint32 u32_ItSignal = 0; u32_ItSignal < rc_Message.c_Signals.size(); ++u32_ItSignal)
                        {
                           const C_OSCCanSignal & rc_Signal = rc_Message.c_Signals[u32_ItSignal];
                           if (rc_Signal.u32_ComDataElementIndex < pc_List->c_Elements.size())
                           {
                              //Combine message and signal name
                              C_OSCNodeDataPoolListElement & rc_Element =
                                 pc_List->c_Elements[rc_Signal.u32_ComDataElementIndex];
                              rc_Element.c_Name = rc_Message.c_Name + '_' + rc_Element.c_Name;
                           }
                        }
                     }
                  }
               }
               break;
            }
         }
         if (q_Found == false)
         {
            s32_Retval = C_CONFIG;
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
