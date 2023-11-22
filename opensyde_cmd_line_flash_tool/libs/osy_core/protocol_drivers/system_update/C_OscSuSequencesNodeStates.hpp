//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node state for C_OscSuSequences and its sequences

   See cpp file for detailed description

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSUSEQUENCESNODESTATES_HPP
#define C_OSCSUSEQUENCESNODESTATES_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SclString.hpp"
#include "C_OscProtocolDriverOsy.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

enum E_OscSuSequencesNodeState
{
   eSUSEQ_STATE_NOT_EXECUTED,
   eSUSEQ_STATE_NOT_NEEDED,
   eSUSEQ_STATE_NO_ERR,
   eSUSEQ_STATE_ERROR
};

class C_OscSuSequencesNodeFileStates
{
public:
   C_OscSuSequencesNodeFileStates(void);

   stw::scl::C_SclString c_FileName;

   E_OscSuSequencesNodeState e_FileExists;
   E_OscSuSequencesNodeState e_FileLoaded;
};

class C_OscSuSequencesNodeHexFileStates :
   public C_OscSuSequencesNodeFileStates
{
public:
   C_OscSuSequencesNodeHexFileStates(void);

   // Preconditions
   E_OscSuSequencesNodeState e_SignatureBlockAddressFromFileRead;
   E_OscSuSequencesNodeState e_NodeNameCompared;
   E_OscSuSequencesNodeState e_DataDumpFromFileRead;
   E_OscSuSequencesNodeState e_AvailableFlashMemoryChecked;

   // Transferring data
   E_OscSuSequencesNodeState e_RequestDownloadSent;
   E_OscSuSequencesNodeState e_AllTransferDataSent;
   E_OscSuSequencesNodeState e_RequestTransferAddressExitSent;
};

class C_OscSuSequencesNodeOtherFileStates :
   public C_OscSuSequencesNodeFileStates
{
public:
   C_OscSuSequencesNodeOtherFileStates(void);

   E_OscSuSequencesNodeState e_RequestFileTransferSent;
   E_OscSuSequencesNodeState e_AllTransferDataSent;
   E_OscSuSequencesNodeState e_RequestTransferFileExitSent;
   E_OscSuSequencesNodeState e_RequestTransferFileExitResultSent;
};

class C_OscSuSequencesNodePsiFileStates :
   public C_OscSuSequencesNodeFileStates
{
public:
   C_OscSuSequencesNodePsiFileStates(void);

   E_OscSuSequencesNodeState e_PsiFileWrote;
};

class C_OscSuSequencesNodePemFileStates :
   public C_OscSuSequencesNodeFileStates
{
public:
   C_OscSuSequencesNodePemFileStates(void);

   // PEM file
   E_OscSuSequencesNodeState e_PemFileExtracted;
   E_OscSuSequencesNodeState e_SecurityKeySent;

   // Related node states
   E_OscSuSequencesNodeState e_SecurityFlagSent;
   E_OscSuSequencesNodeState e_DebuggerFlagSent;
};

class C_OscSuSequencesNodeStwFlHexFileStates :
   public C_OscSuSequencesNodeFileStates
{
public:
   C_OscSuSequencesNodeStwFlHexFileStates(void);

   E_OscSuSequencesNodeState e_FileSent;
};

// Generic states of node
class C_OscSuSequencesNodeStates
{
public:
   C_OscSuSequencesNodeStates(void);

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
class C_OscSuSequencesNodeConnectStates :
   public C_OscSuSequencesNodeStates
{
public:
   C_OscSuSequencesNodeConnectStates(void);

   E_OscSuSequencesNodeState e_InformationRead;
   bool q_AuthenticationNecessarySet; // Flag if the check for the need of security was made
   bool q_AuthenticationNecessary;    // Flag if security is necessary for this node

   // Feature compatibility flags
   C_OscProtocolDriverOsy::C_ListOfFeatures c_AvailableFeatures;
};

// States of node of step UpdateSystem
class C_OscSuSequencesNodeUpdateStates :
   public C_OscSuSequencesNodeStates
{
public:
   C_OscSuSequencesNodeUpdateStates(void);

   // openSYDE specific
   std::vector<C_OscSuSequencesNodeHexFileStates> c_StateHexFiles;
   std::vector<C_OscSuSequencesNodeOtherFileStates> c_StateOtherFiles;
   std::vector<C_OscSuSequencesNodePsiFileStates> c_StatePsiFiles;
   C_OscSuSequencesNodePemFileStates c_StatePemFile;

   // STW flashloader specific
   std::vector<C_OscSuSequencesNodeStwFlHexFileStates> c_StateStwFlHexFiles;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
