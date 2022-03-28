//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node state for C_OSCSuSequences and its sequences

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_OSCSuSequencesNodeStates.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCSuSequencesNodeFileStates::C_OSCSuSequencesNodeFileStates(void) :
   c_FileName(""),
   e_FileExists(eSUSEQ_STATE_NOT_EXECUTED),
   e_FileLoaded(eSUSEQ_STATE_NOT_EXECUTED)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCSuSequencesNodeHexFileStates::C_OSCSuSequencesNodeHexFileStates(void) :
   C_OSCSuSequencesNodeFileStates(),
   e_SignatureBlockAddressFromFileRead(eSUSEQ_STATE_NOT_EXECUTED),
   e_NodeNameCompared(eSUSEQ_STATE_NOT_EXECUTED),
   e_DataDumpFromFileRead(eSUSEQ_STATE_NOT_EXECUTED),
   e_AvailableFlashMemoryChecked(eSUSEQ_STATE_NOT_EXECUTED),
   e_RequestDownloadSent(eSUSEQ_STATE_NOT_EXECUTED),
   e_AllTransferDataSent(eSUSEQ_STATE_NOT_EXECUTED),
   e_RequestTransferAddressExitSent(eSUSEQ_STATE_NOT_EXECUTED)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCSuSequencesNodeOtherFileStates::C_OSCSuSequencesNodeOtherFileStates(void) :
   C_OSCSuSequencesNodeFileStates(),
   e_RequestFileTransferSent(eSUSEQ_STATE_NOT_EXECUTED),
   e_AllTransferDataSent(eSUSEQ_STATE_NOT_EXECUTED),
   e_RequestTransferFileExitSent(eSUSEQ_STATE_NOT_EXECUTED),
   e_RequestTransferFileExitResultSent(eSUSEQ_STATE_NOT_EXECUTED)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCSuSequencesNodePsiFileStates::C_OSCSuSequencesNodePsiFileStates(void) :
   C_OSCSuSequencesNodeFileStates(),
   e_PsiFileWrote(eSUSEQ_STATE_NOT_EXECUTED)
{
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCSuSequencesNodePemFileStates::C_OSCSuSequencesNodePemFileStates(void) :
   C_OSCSuSequencesNodeFileStates(),
   e_PemFileExtracted(eSUSEQ_STATE_NOT_EXECUTED),
   e_SecurityKeySent(eSUSEQ_STATE_NOT_EXECUTED),
   e_SecurityFlagSent(eSUSEQ_STATE_NOT_EXECUTED),
   e_DebuggerFlagSent(eSUSEQ_STATE_NOT_EXECUTED)
{
}
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCSuSequencesNodeStwFlHexFileStates::C_OSCSuSequencesNodeStwFlHexFileStates(void) :
   C_OSCSuSequencesNodeFileStates(),
   e_FileSent(eSUSEQ_STATE_NOT_EXECUTED)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCSuSequencesNodeStates::C_OSCSuSequencesNodeStates(void) :
   q_Timeout(false),
   q_AuthenticationError(false),
   q_CouldNotReachedDueToRoutingError(false),
   q_RoutingNodeError(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCSuSequencesNodeConnectStates::C_OSCSuSequencesNodeConnectStates(void) :
   C_OSCSuSequencesNodeStates(),
   e_InformationRead(eSUSEQ_STATE_NOT_EXECUTED),
   q_AuthenticationNecessarySet(false),
   q_AuthenticationNecessary(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OSCSuSequencesNodeUpdateStates::C_OSCSuSequencesNodeUpdateStates(void) :
   C_OSCSuSequencesNodeStates()
{
}
