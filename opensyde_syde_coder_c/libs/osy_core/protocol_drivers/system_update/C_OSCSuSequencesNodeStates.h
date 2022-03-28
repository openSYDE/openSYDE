//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node state for C_OSCSuSequences and its sequences

   See cpp file for detailed description

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSUSEQUENCESNODESTATES_H
#define C_OSCSUSEQUENCESNODESTATES_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "CSCLString.h"
#include "C_OSCProtocolDriverOsy.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

enum E_OSCSuSequencesNodeState
{
   eSUSEQ_STATE_NOT_EXECUTED,
   eSUSEQ_STATE_NOT_NEEDED,
   eSUSEQ_STATE_NO_ERR,
   eSUSEQ_STATE_ERROR
};

class C_OSCSuSequencesNodeFileStates
{
public:
   C_OSCSuSequencesNodeFileStates(void);

   stw_scl::C_SCLString c_FileName;

   E_OSCSuSequencesNodeState e_FileExists;
   E_OSCSuSequencesNodeState e_FileLoaded;
};

class C_OSCSuSequencesNodeHexFileStates :
   public C_OSCSuSequencesNodeFileStates
{
public:
   C_OSCSuSequencesNodeHexFileStates(void);

   // Preconditions
   E_OSCSuSequencesNodeState e_SignatureBlockAddressFromFileRead;
   E_OSCSuSequencesNodeState e_NodeNameCompared;
   E_OSCSuSequencesNodeState e_DataDumpFromFileRead;
   E_OSCSuSequencesNodeState e_AvailableFlashMemoryChecked;

   // Transferring data
   E_OSCSuSequencesNodeState e_RequestDownloadSent;
   E_OSCSuSequencesNodeState e_AllTransferDataSent;
   E_OSCSuSequencesNodeState e_RequestTransferAddressExitSent;
};

class C_OSCSuSequencesNodeOtherFileStates :
   public C_OSCSuSequencesNodeFileStates
{
public:
   C_OSCSuSequencesNodeOtherFileStates(void);

   E_OSCSuSequencesNodeState e_RequestFileTransferSent;
   E_OSCSuSequencesNodeState e_AllTransferDataSent;
   E_OSCSuSequencesNodeState e_RequestTransferFileExitSent;
   E_OSCSuSequencesNodeState e_RequestTransferFileExitResultSent;
};

class C_OSCSuSequencesNodePsiFileStates :
   public C_OSCSuSequencesNodeFileStates
{
public:
   C_OSCSuSequencesNodePsiFileStates(void);

   E_OSCSuSequencesNodeState e_PsiFileWrote;
};

class C_OSCSuSequencesNodePemFileStates :
   public C_OSCSuSequencesNodeFileStates
{
public:
   C_OSCSuSequencesNodePemFileStates(void);

   // PEM file
   E_OSCSuSequencesNodeState e_PemFileExtracted;
   E_OSCSuSequencesNodeState e_SecurityKeySent;

   // Related node states
   E_OSCSuSequencesNodeState e_SecurityFlagSent;
   E_OSCSuSequencesNodeState e_DebuggerFlagSent;
};

class C_OSCSuSequencesNodeStwFlHexFileStates :
   public C_OSCSuSequencesNodeFileStates
{
public:
   C_OSCSuSequencesNodeStwFlHexFileStates(void);

   E_OSCSuSequencesNodeState e_FileSent;
};

// Generic states of node
class C_OSCSuSequencesNodeStates
{
public:
   C_OSCSuSequencesNodeStates(void);

   // Node caused a timeout
   bool q_Timeout;

   // Node caused an authentication error
   bool q_AuthenticationError;

   // Node has at least one node as router on its node which caused a problem
   bool q_CouldNotReachedDueToRoutingError;

   // Node is necessary for routing to an other node and caused a problem for building up the route
   bool q_RoutingNodeError;
};

// States of node of steps ActivateFlashloader and ReadDeviceInformation ("Enter Update Mode")
class C_OSCSuSequencesNodeConnectStates :
   public C_OSCSuSequencesNodeStates
{
public:
   C_OSCSuSequencesNodeConnectStates(void);

   E_OSCSuSequencesNodeState e_InformationRead;
   bool q_AuthenticationNecessarySet; // Flag if the check for the need of security was made
   bool q_AuthenticationNecessary;    // Flag if security is necessary for this node

   // Feature compatibility flags
   C_OSCProtocolDriverOsy::C_ListOfFeatures c_AvailableFeatures;
};

// States of node of step UpdateSystem
class C_OSCSuSequencesNodeUpdateStates :
   public C_OSCSuSequencesNodeStates
{
public:
   C_OSCSuSequencesNodeUpdateStates(void);

   // openSYDE specific
   std::vector<C_OSCSuSequencesNodeHexFileStates> c_StateHexFiles;
   std::vector<C_OSCSuSequencesNodeOtherFileStates> c_StateOtherFiles;
   std::vector<C_OSCSuSequencesNodePsiFileStates> c_StatePsiFiles;
   C_OSCSuSequencesNodePemFileStates c_StatePemFile;

   // STW flashloader specific
   std::vector<C_OSCSuSequencesNodeStwFlHexFileStates> c_StateStwFlHexFiles;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
